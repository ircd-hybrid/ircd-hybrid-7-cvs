/*
 *  ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd).
 *  s_auth.c: Functions for querying a users ident.
 *
 *  Copyright (C) 2002 by the past and present ircd coders, and others.
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307
 *  USA
 *
 *  $Id: s_auth.c,v 7.120 2003/05/11 22:27:45 joshk Exp $
 */

/*
 * Changes:
 *   July 6, 1999 - Rewrote most of the code here. When a client connects
 *     to the server and passes initial socket validation checks, it
 *     is owned by this module (auth) which returns it to the rest of the
 *     server when dns and auth queries are finished. Until the client is
 *     released, the server does not know it exists and does not process
 *     any messages from it.
 *     --Bleep  Thomas Helvey <tomh@inxpress.net>
 */
#include "stdinc.h"
#include "tools.h"
#include "list.h"
#include "s_auth.h"
#include "s_conf.h"
#include "client.h"
#include "common.h"
#include "event.h"
#include "fdlist.h"              /* fdlist_add */
#include "irc_string.h"
#include "ircd.h"
#include "ircdauth.h"
#include "numeric.h"
#include "packet.h"
#include "res.h"
#include "s_bsd.h"
#include "s_log.h"
#include "s_stats.h"
#include "send.h"
#include "memory.h"

/*
 * a bit different approach
 * this replaces the original sendheader macros
 */
static struct {
  const char *message;
  size_t length;
} HeaderMessages [] = {
  /* 123456789012345678901234567890123456789012345678901234567890 */
  { "NOTICE AUTH :*** Looking up your hostname...\r\n",    46 },
  { "NOTICE AUTH :*** Found your hostname\r\n",            38 },
  { "NOTICE AUTH :*** Couldn't look up your hostname\r\n", 49 },
  { "NOTICE AUTH :*** Checking Ident\r\n",                 33 },
  { "NOTICE AUTH :*** Got Ident response\r\n",             37 },
  { "NOTICE AUTH :*** No Ident response\r\n",              36 },
  { "NOTICE AUTH :*** Your forward and reverse DNS do not match, " \
    "ignoring hostname.\r\n",                              80 },
  { "NOTICE AUTH :*** Your hostname is too long, ignoring hostname\r\n", 63 }
};

typedef enum {
  REPORT_DO_DNS,
  REPORT_FIN_DNS,
  REPORT_FAIL_DNS,
  REPORT_DO_ID,
  REPORT_FIN_ID,
  REPORT_FAIL_ID,
  REPORT_IP_MISMATCH,
  REPORT_HOST_TOOLONG
} ReportType;

#define sendheader(c, r) \
   send((c)->localClient->fd, HeaderMessages[(r)].message, HeaderMessages[(r)].length, 0)

/*
 */
dlink_list auth_client_list = {NULL, NULL, 0};
dlink_list auth_poll_list   = {NULL, NULL, 0};

static EVH timeout_auth_queries_event;

static PF read_auth_reply;
static CNCB auth_connect_callback;

/* init_auth()
 *
 * Initialise the auth code
 */
void
init_auth(void)
{
  eventAddIsh("timeout_auth_queries_event", timeout_auth_queries_event, NULL, 1);
}

/*
 * make_auth_request - allocate a new auth request
 */
static struct AuthRequest*
make_auth_request(struct Client* client)
{
  struct AuthRequest *request = 
    (struct AuthRequest *)MyMalloc(sizeof(struct AuthRequest));
  request->fd      = -1;
  request->client  = client;
  request->timeout = CurrentTime + CONNECTTIMEOUT;
  return request;
}

/*
 * free_auth_request - cleanup auth request allocations
 */
static void
free_auth_request(struct AuthRequest *request)
{
  MyFree(request);
}

/*
 * unlink_auth_request - remove auth request from a list
 */
static void
unlink_auth_request(struct AuthRequest *request, dlink_list *list)
{
  dlink_node *ptr;

  if ((ptr = dlinkFindDelete(list, request)) != NULL)
    free_dlink_node(ptr);
}

/*
 * link_auth_request - add auth request to a list
 */
static void
link_auth_request(struct AuthRequest *request, dlink_list *list)
{
  dlinkAdd(request, make_dlink_node(), list);
}

/*
 * release_auth_client - release auth client from auth system
 * this adds the client into the local client lists so it can be read by
 * the main io processing loop
 */
static void
release_auth_client(struct Client *client)
{
  if (client->localClient->fd > highest_fd)
    highest_fd = client->localClient->fd;

  /*
   * When a client has auth'ed, we want to start reading what it sends
   * us. This is what read_packet() does.
   *     -- adrian
   */
  client->localClient->allow_read = MAX_FLOOD;
  comm_setflush(client->localClient->fd, 1000, flood_recalc, client);
  set_no_delay(client->localClient->fd);
  dlinkAdd(client, &client->node, &global_client_list);
  read_packet(client->localClient->fd, client);
}
 
/*
 * auth_dns_callback - called when resolver query finishes
 * if the query resulted in a successful search, hp will contain
 * a non-null pointer, otherwise hp will be null.
 * set the client on it's way to a connection completion, regardless
 * of success of failure
 */
static void
auth_dns_callback(void* vptr, adns_answer* reply)
{
  struct AuthRequest* auth = (struct AuthRequest*) vptr;
  ClearDNSPending(auth);
  if(reply && (reply->status == adns_s_ok))
    {
      if(strlen(*reply->rrs.str) <= HOSTLEN)
        {
          strlcpy(auth->client->host, *reply->rrs.str, sizeof(auth->client->host));
          sendheader(auth->client, REPORT_FIN_DNS);
        } else
          sendheader(auth->client, REPORT_HOST_TOOLONG);
    }
  else
    {
#ifdef IPV6
      if(auth->client->localClient->aftype == AF_INET6 && 
              ConfigFileEntry.fallback_to_ip6_int == 1 && auth->ip6_int == 0)
      {
        struct Client *client = auth->client;
        auth->ip6_int = 1;
	MyFree(reply);
	reply = NULL;

	/* Only set DNS pending if no error! -- Dianora */
	if (adns_getaddr(&client->localClient->ip, client->localClient->aftype,
			 client->localClient->dns_query, 0) == 0)
	{
	  SetDNSPending(auth);
	  return;
	}
      }
#endif
      sendheader(auth->client, REPORT_FAIL_DNS);
    }

  MyFree(reply);
  MyFree(auth->client->localClient->dns_query);

  auth->client->localClient->dns_query = NULL;
  if (!IsDoingAuth(auth))
    {
      struct Client *client_p = auth->client;
      unlink_auth_request(auth, &auth_poll_list);
#ifdef USE_IAUTH
      ilog(L_ERROR, "Linking to auth client list");
      link_auth_request(auth, &auth_client_list);
#else
      free_auth_request(auth);
#endif
      release_auth_client(client_p);
    }
}

/*
 * authsenderr - handle auth send errors
 */
static void
auth_error(struct AuthRequest* auth)
{
  ++ServerStats->is_abad;

  fd_close(auth->fd);
  auth->fd = -1;

  ClearAuth(auth);
  sendheader(auth->client, REPORT_FAIL_ID);

  if (!IsDNSPending(auth))
  {
    unlink_auth_request(auth, &auth_poll_list);
    release_auth_client(auth->client);
#ifdef USE_IAUTH
    ilog(L_ERROR, "linking to auth client list 2");
    link_auth_request(auth, &auth_client_list);
#else
    free_auth_request(auth);
#endif
  }
}

/*
 * start_auth_query - Flag the client to show that an attempt to 
 * contact the ident server on
 * the client's host.  The connect and subsequently the socket are all put
 * into 'non-blocking' mode.  Should the connect or any later phase of the
 * identifing process fail, it is aborted and the user is given a username
 * of "unknown".
 */
static int
start_auth_query(struct AuthRequest* auth)
{
  struct irc_ssaddr localaddr;
  socklen_t locallen = sizeof(struct irc_ssaddr);
  int                fd;
#ifdef IPV6
  struct sockaddr_in6 *v6;
#else
  struct sockaddr_in *v4;
#endif

  /* open a socket of the same type as the client socket */
  if ((fd = comm_open(auth->client->localClient->ip.ss.ss_family, SOCK_STREAM,
          0, "ident")) == -1)
  {
    report_error(L_ALL, "creating auth stream socket %s:%s", 
        get_client_name(auth->client, SHOW_IP), errno);
    ilog(L_ERROR, "Unable to create auth socket for %s:%m",
        get_client_name(auth->client, SHOW_IP));
    ++ServerStats->is_abad;
    return 0;
  }
  if ((MAXCONNECTIONS - 10) < fd)
  {
    sendto_realops_flags(UMODE_ALL, L_ALL,"Can't allocate fd for auth on %s",
        get_client_name(auth->client, SHOW_IP));
    fd_close(fd);
    return 0;
  }

  sendheader(auth->client, REPORT_DO_ID);
  if (!set_non_blocking(fd))
  {
    report_error(L_ALL, NONB_ERROR_MSG, get_client_name(auth->client, SHOW_IP), errno);
    fd_close(fd);
    return 0;
  }

  /* 
   * get the local address of the client and bind to that to
   * make the auth request.  This used to be done only for
   * ifdef VIRTUAL_HOST, but needs to be done for all clients
   * since the ident request must originate from that same address--
   * and machines with multiple IP addresses are common now
   */
  memset(&localaddr, 0, locallen);
  getsockname(auth->client->localClient->fd, (struct sockaddr*)&localaddr,
      &locallen);

#ifdef IPV6
  remove_ipv6_mapping(&localaddr);
  v6 = (struct sockaddr_in6 *)&localaddr;
  v6->sin6_port = htons(0);
#else
  localaddr.ss_len = locallen;
  v4 = (struct sockaddr_in *)&localaddr;
  v4->sin_port = htons(0);
#endif
  localaddr.ss_port = htons(0);

  auth->fd = fd;
  SetAuthConnect(auth);

  comm_connect_tcp(fd, auth->client->localClient->sockhost, 113, 
      (struct sockaddr *)&localaddr, localaddr.ss_len, auth_connect_callback, 
      auth, auth->client->localClient->ip.ss.ss_family, 
      GlobalSetOptions.ident_timeout);
  return 1; /* We suceed here for now */
}

/*
 * GetValidIdent - parse ident query reply from identd server
 * 
 * Inputs        - pointer to ident buf
 * Output        - NULL if no valid ident found, otherwise pointer to name
 * Side effects  -
 */
/*
 * A few questions have been asked about this mess, obviously
 * it should have been commented better the first time.
 * The original idea was to remove all references to libc from ircd-hybrid.
 * Instead of having to write a replacement for sscanf(), I did a
 * rather gruseome parser here so we could remove this function call.
 * Note, that I had also removed a few floating point printfs as well (though
 * now we are still stuck with a few...)
 * Remember, we have a replacement ircd sprintf, we have bleeps fputs lib
 * it would have been nice to remove some unneeded code.
 * Oh well. If we don't remove libc stuff totally, then it would be
 * far cleaner to use sscanf()
 *
 * - Dianora
 */
static char*
GetValidIdent(char *buf)
{
  int   remp = 0;
  int   locp = 0;
  char* colon1Ptr;
  char* colon2Ptr;
  char* colon3Ptr;
  char* commaPtr;
  char* remotePortString;

  /* All this to get rid of a sscanf() fun. */
  remotePortString = buf;
  
  if((colon1Ptr = strchr(remotePortString,':')) == NULL)
    return 0;
  *colon1Ptr = '\0';
  colon1Ptr++;

  if((colon2Ptr = strchr(colon1Ptr,':')) == NULL)
    return 0;
  *colon2Ptr = '\0';
  colon2Ptr++;
  
  if((commaPtr = strchr(remotePortString, ',')) == NULL)
    return 0;
  *commaPtr = '\0';
  commaPtr++;

  if((remp = atoi(remotePortString)) == 0)
    return 0;
              
  if((locp = atoi(commaPtr)) == 0)
    return 0;

  /* look for USERID bordered by first pair of colons */
  if(strstr(colon1Ptr, "USERID") == NULL)
    return 0;

  if((colon3Ptr = strchr(colon2Ptr,':')) == NULL)
    return 0;
  *colon3Ptr = '\0';
  colon3Ptr++;
  return(colon3Ptr);
}

/*
 * start_auth - starts auth (identd) and dns queries for a client
 */
void
start_auth(struct Client* client)
{
  struct AuthRequest *auth = 0;

  assert(client != 0);

  if(client == NULL)
    return;

  auth = make_auth_request(client);

  client->localClient->dns_query = MyMalloc(sizeof(struct DNSQuery));
  client->localClient->dns_query->ptr = auth;
  client->localClient->dns_query->callback = auth_dns_callback;

  sendheader(client, REPORT_DO_DNS);

  /* No DNS cache now, remember? -- adrian */
  /* Only set DNS pending if no error! -- Dianora */
  if (adns_getaddr(&client->localClient->ip, client->localClient->aftype,
		   client->localClient->dns_query, 0) == 0)
    SetDNSPending(auth);

  (void)start_auth_query(auth);
  link_auth_request(auth, &auth_poll_list);
}

/*
 * timeout_auth_queries - timeout resolver and identd requests
 * allow clients through if requests failed
 */
static void
timeout_auth_queries_event(void *notused)
{
  dlink_node *ptr;
  dlink_node *next_ptr;
  struct AuthRequest* auth;

  DLINK_FOREACH_SAFE(ptr, next_ptr, auth_poll_list.head)
  {
      auth = ptr->data;

      if (auth->timeout < CurrentTime)
	{
	  if (auth->fd >= 0)
	    fd_close(auth->fd);

	  if (IsDoingAuth(auth))
	    sendheader(auth->client, REPORT_FAIL_ID);
	  if (IsDNSPending(auth))
	    {
	      delete_adns_queries(auth->client->localClient->dns_query);
	      auth->client->localClient->dns_query->query = NULL;
	      sendheader(auth->client, REPORT_FAIL_DNS);
	    }
	  ilog(L_INFO, "DNS/AUTH timeout %s",
	      get_client_name(auth->client, SHOW_IP));

	  auth->client->since = CurrentTime;
	  dlinkDelete(ptr, &auth_poll_list);
	  free_dlink_node(ptr);
	  release_auth_client(auth->client);
#ifdef USE_IAUTH
    ilog(L_ERROR, "linking to auth client list 3");
	  link_auth_request(auth, &auth_client_list);
#else
	  free_auth_request(auth);
#endif
	}
    }
}

/*
 * auth_connect_callback() - deal with the result of comm_connect_tcp()
 *
 * If the connection failed, we simply close the auth fd and report
 * a failure. If the connection suceeded send the ident server a query
 * giving "theirport , ourport". The write is only attempted *once* so
 * it is deemed to be a fail if the entire write doesn't write all the
 * data given.  This shouldnt be a problem since the socket should have
 * a write buffer far greater than this message to store it in should
 * problems arise. -avalon
 */
static void
auth_connect_callback(int fd, int error, void *data)
{
  struct AuthRequest *auth = data;
  struct irc_ssaddr us;
  struct irc_ssaddr them;
  char authbuf[32];
  socklen_t ulen = sizeof(struct irc_ssaddr);
  socklen_t tlen = sizeof(struct irc_ssaddr);
  uint16_t uport, tport;
#ifdef IPV6
  struct sockaddr_in6 *v6;
#else
  struct sockaddr_in *v4;
#endif

  if (error != COMM_OK)
  {
    auth_error(auth);
    return;
  }

  if (getsockname(auth->client->localClient->fd, (struct sockaddr *)&us,   (socklen_t*)&ulen) ||
      getpeername(auth->client->localClient->fd, (struct sockaddr *)&them, (socklen_t*)&tlen))
  {
    ilog(L_INFO, "auth get{sock,peer}name error for %s:%m",
        get_client_name(auth->client, SHOW_IP));
    auth_error(auth);
    return;
  }

#ifdef IPV6
  v6 = (struct sockaddr_in6 *)&us;
  uport = ntohs(v6->sin6_port);
  v6 = (struct sockaddr_in6 *)&them;
  tport = ntohs(v6->sin6_port);
  remove_ipv6_mapping(&us);
  remove_ipv6_mapping(&them);
#else
  v4 = (struct sockaddr_in *)&us;
  uport = ntohs(v4->sin_port);
  v4 = (struct sockaddr_in *)&them;
  tport = ntohs(v4->sin_port);
  us.ss_len = ulen;
  them.ss_len = tlen;
#endif
  
  ircsprintf(authbuf, "%u , %u\r\n", tport, uport); 

  if (send(auth->fd, authbuf, strlen(authbuf), 0) == -1)
  {
    auth_error(auth);
    return;
  }
  ClearAuthConnect(auth);
  SetAuthPending(auth);
  read_auth_reply(auth->fd, auth);
}

/*
 * read_auth_reply - read the reply (if any) from the ident server 
 * we connected to.
 * We only give it one shot, if the reply isn't good the first time
 * fail the authentication entirely. --Bleep
 */
#define AUTH_BUFSIZ 128

static void
read_auth_reply(int fd, void *data)
{
  struct AuthRequest *auth = data;
  char* s= NULL;
  char* t= NULL;
  int   len;
  int   count;
  char  buf[AUTH_BUFSIZ + 1]; /* buffer to read auth reply into */

  len = recv(auth->fd, buf, AUTH_BUFSIZ, 0);
  
  if (len < 0 && ignoreErrno(errno))
  {
    comm_setselect(fd, FDLIST_IDLECLIENT, COMM_SELECT_READ,
                   read_auth_reply, auth, 0);
    return;
  }

  if (len > 0)
    {
      buf[len] = '\0';

      if( (s = GetValidIdent(buf)) )
	{
	  t = auth->client->username;

	  while(*s == '~' || *s == '^')
            s++;

	  for (count = USERLEN; *s && count; s++)
	    {
	      if(*s == '@')
		{
		  break;
		}
	      if ( !IsSpace(*s) && *s != ':' && *s != '[')
		{
		  *t++ = *s;
		  count--;
		}
	    }
	  *t = '\0';
	}
    }

  fd_close(auth->fd);
  auth->fd = -1;
  ClearAuth(auth);
  
  if (s == NULL)
    {
      ++ServerStats->is_abad;
      strcpy(auth->client->username, "unknown");
    }
  else
    {
      sendheader(auth->client, REPORT_FIN_ID);
      ++ServerStats->is_asuc;
      SetGotId(auth->client);
    }

  if (!IsDNSPending(auth))
    {
      unlink_auth_request(auth, &auth_poll_list);
      release_auth_client(auth->client);
#ifdef USE_IAUTH
    ilog(L_ERROR, "linking to auth client list 4");
      link_auth_request(auth, &auth_client_list);
#else
      free_auth_request(auth);
#endif
    }
}

/* remove_auth_request()
 *
 * Remove request 'auth' from AuthClientList, and free it.
 * There is no need to release auth->client since it has
 * already been done
 */
void
remove_auth_request(struct AuthRequest *auth)
{
  unlink_auth_request(auth, &auth_client_list);
  free_auth_request(auth);
} /* remove_auth_request() */

/*
 * FindAuthClient()
 * Find the client matching 'id' in the AuthClientList. The
 * id will match the memory address of the client structure.
 */

struct AuthRequest *
/* XXX This is just wrong, should be a *void at least */
FindAuthClient(long id)
{
  dlink_node *ptr;
  struct AuthRequest *auth;

  DLINK_FOREACH(ptr, auth_client_list.head)
  {
    auth = ptr->data;

    if (auth->client == (struct Client *)id)
      return auth;
  }

  return(NULL);
} /* FindAuthClient() */

/*
 * delete_identd_queries()
 *
 */
void 
delete_identd_queries(struct Client *target_p)
{
  dlink_node *ptr;
  dlink_node *next_ptr;
  struct AuthRequest *auth;

  DLINK_FOREACH_SAFE(ptr, next_ptr, auth_poll_list.head)
  {
    auth = ptr->data;

    if (auth->client == target_p)
    {
      if (auth->fd >= 0)
        fd_close(auth->fd);

      dlinkDelete(ptr, &auth_poll_list);
      free_auth_request(auth);
      free_dlink_node(ptr);
    }
  }

  DLINK_FOREACH_SAFE(ptr, next_ptr, auth_client_list.head)
  {
    auth = ptr->data;

    if (auth->client == target_p)
    {
      if (auth->fd >= 0)
        fd_close(auth->fd);

      dlinkDelete(ptr, &auth_client_list);
      free_auth_request(auth);
      free_dlink_node(ptr);
    }
  }
}
