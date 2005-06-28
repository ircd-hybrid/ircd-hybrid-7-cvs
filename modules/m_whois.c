/*
 *  ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd).
 *  m_whois.c: Shows who a user is.
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
 *  $Id: m_whois.c,v 1.123 2005/06/28 19:47:11 adx Exp $
 */

#include "stdinc.h"
#include "tools.h"
#include "common.h"  
#include "handlers.h"
#include "client.h"
#include "hash.h"
#include "channel.h"
#include "channel_mode.h"
#include "ircd.h"
#include "numeric.h"
#include "s_conf.h"
#include "s_serv.h"
#include "send.h"
#include "list.h"
#include "irc_string.h"
#include "sprintf_irc.h"
#include "msg.h"
#include "parse.h"
#include "modules.h"
#include "hook.h"

static void do_whois(struct Client *client_p, struct Client *source_p, int parc, char *parv[]);
static int single_whois(struct Client *source_p, struct Client *target_p);
static void whois_person(struct Client *source_p, struct Client *target_p);
static int global_whois(struct Client *source_p, const char *nick);

static void m_whois(struct Client *, struct Client *, int, char **);
static void mo_whois(struct Client *, struct Client *, int, char **);

struct Message whois_msgtab = {
  "WHOIS", 0, 0, 0, 0, MFLG_SLOW, 0,
  {m_unregistered, m_whois, mo_whois, m_ignore, mo_whois, m_ignore}
};

#ifndef STATIC_MODULES
void
_modinit(void)
{
  hook_add_event("doing_whois");
  mod_add_cmd(&whois_msgtab);
}

void
_moddeinit(void)
{
  hook_del_event("doing_whois");
  mod_del_cmd(&whois_msgtab);
}

const char *_version = "$Revision: 1.123 $";
#endif

/*
** m_whois
**      parv[0] = sender prefix
**      parv[1] = nickname masklist
*/
static void
m_whois(struct Client *client_p, struct Client *source_p,
        int parc, char *parv[])
{
  static time_t last_used = 0;

  if (parc < 2 || EmptyString(parv[1]))
  {
    sendto_one(source_p, form_str(ERR_NONICKNAMEGIVEN),
               me.name, source_p->name);
    return;
  }

  if (parc > 2)
  {
    /* seeing as this is going across servers, we should limit it */
    if ((last_used + ConfigFileEntry.pace_wait_simple) > CurrentTime)
    {
      sendto_one(source_p, form_str(RPL_LOAD2HI),
                 me.name, source_p->name);
      return;
    }
    else
      last_used = CurrentTime;

    /* if we have serverhide enabled, they can either ask the clients
     * server, or our server.. I dont see why they would need to ask
     * anything else for info about the client.. --fl_
     */
    if (ConfigFileEntry.disable_remote)
      parv[1] = parv[2];

    if (hunt_server(client_p,source_p,":%s WHOIS %s :%s", 1, parc, parv) != HUNTED_ISME)
      return;

    parv[1] = parv[2];
  }

  do_whois(client_p, source_p, parc, parv);
}

/*
** mo_whois
**      parv[0] = sender prefix
**      parv[1] = nickname masklist
*/
static void
mo_whois(struct Client *client_p, struct Client *source_p,
         int parc, char *parv[])
{
  if (parc < 2 || EmptyString(parv[1]))
  {
    sendto_one(source_p, form_str(ERR_NONICKNAMEGIVEN),
               me.name, source_p->name);
    return;
  }

  if (parc > 2)
  {
    if (hunt_server(client_p,source_p,":%s WHOIS %s :%s", 1, parc, parv) != HUNTED_ISME)
      return;

    parv[1] = parv[2];
  }

  do_whois(client_p, source_p, parc, parv);
}

/* do_whois()
 *
 * inputs	- pointer to 
 * output	- 
 * side effects -
 */
static void
do_whois(struct Client *client_p, struct Client *source_p,
         int parc, char *parv[])
{
  struct Client *target_p;
  char *nick;
  char *p = NULL;
  int found = 0;

  nick = parv[1];
  while (*nick == ',')
    nick++;
  if ((p = strchr(nick,',')) != NULL)
    *p = '\0';

  if (*nick == '\0')
    return;

  collapse(nick);

  if (strpbrk(nick, "?#*") == NULL)
  {
    if ((target_p = find_client(nick)) != NULL)
    {
      if (IsServer(client_p))
	client_burst_if_needed(client_p, target_p);

      if (IsPerson(target_p))
      {
	single_whois(source_p, target_p);
	found = 1;
      }
    }
    else if (!ServerInfo.hub && uplink && IsCapable(uplink, CAP_LL))
    {
      if (parc > 2)
        sendto_one(uplink,":%s WHOIS %s :%s",
                   source_p->name, nick, nick);
      else
        sendto_one(uplink,":%s WHOIS %s",
                   source_p->name, nick);
      return;
    }
  }
  else /* wilds is true */
  {
    /* disallow wild card whois on lazylink leafs for now */
    if (!ServerInfo.hub && uplink && IsCapable(uplink, CAP_LL))
      return;

    /* Oh-oh wilds is true so have to do it the hard expensive way */
    if (MyClient(source_p))
      found = global_whois(source_p, nick);
  }

  if (!found)
  {
    if (IsDigit(*nick))
      sendto_one(source_p, form_str(ERR_NOTARGET),
		 me.name, source_p->name);
    else
      sendto_one(source_p, form_str(ERR_NOSUCHNICK),
		 me.name, source_p->name, nick);
  }
  sendto_one(source_p, form_str(RPL_ENDOFWHOIS),
             me.name, source_p->name, parv[1]);
}

/* global_whois()
 *
 * Inputs	- source_p client to report to
 *		- target_p client to report on
 * Output	- if found return 1
 * Side Effects	- do a single whois on given client
 * 		  writing results to source_p
 */
static int
global_whois(struct Client *source_p, const char *nick)
{
  dlink_node *ptr;
  struct Client *target_p;
  int found = 0;

  DLINK_FOREACH(ptr, global_client_list.head)
  {
    target_p = ptr->data;

    if (!IsPerson(target_p))
      continue;

    if (!match(nick, target_p->name))
      continue;

    assert(target_p->user->server != NULL);

    /* 'Rules' established for sending a WHOIS reply:
     *
     *
     * - if wildcards are being used dont send a reply if
     *   the querier isnt any common channels and the
     *   client in question is invisible and wildcards are
     *   in use (allow exact matches only);
     *
     * - only send replies about common or public channels
     *   the target user(s) are on;
     */

    found |= single_whois(source_p, target_p);
  }

  return (found);
}

/* single_whois()
 *
 * Inputs	- source_p client to report to
 *		- target_p client to report on
 * Output	- if found return 1
 * Side Effects	- do a single whois on given client
 * 		  writing results to source_p
 */
static int
single_whois(struct Client *source_p, struct Client *target_p)
{
  dlink_node *ptr;
  struct Channel *chptr;

  assert(target_p->user != NULL);

  if (!IsInvisible(target_p))
  {
    /* always show user if they are visible (no +i) or
     * were identified with their exact nickname */
    whois_person(source_p, target_p);
    return 1;
  }

  /* whois with wildcards, target_p is +i. Check if it is on
   * any common channels with source_p */
  DLINK_FOREACH(ptr, target_p->user->channel.head)
  {
    chptr = ((struct Membership *) ptr->data)->chptr;
    if (IsMember(source_p, chptr))
    {
      whois_person(source_p, target_p);
      return 1;
    }
  }

  return 0;
}

/* whois_person()
 *
 * inputs	- source_p client to report to
 *		- target_p client to report on
 * output	- NONE
 * side effects	- 
 */
static void
whois_person(struct Client *source_p, struct Client *target_p)
{
  char buf[BUFSIZE];
  dlink_node *lp;
  struct Client *server_p;
  struct Channel *chptr;
  struct Membership *ms;
  int cur_len = 0;
  int mlen;
  char *t;
  int tlen;
  int reply_to_send = NO;
  struct hook_mfunc_data hd;

  assert(target_p->user != NULL);

  server_p = target_p->user->server;

  sendto_one(source_p, form_str(RPL_WHOISUSER),
             me.name, source_p->name, target_p->name,
             target_p->username, target_p->host, target_p->info);

  cur_len = mlen = ircsprintf(buf, form_str(RPL_WHOISCHANNELS),
             me.name, source_p->name, target_p->name, "");
  t = buf + mlen;

  DLINK_FOREACH(lp, target_p->user->channel.head)
  {
    ms    = lp->data;
    chptr = ms->chptr;

    if (ShowChannel(source_p, chptr))
    {
      if ((cur_len + 3 + strlen(chptr->chname) + 1) > (BUFSIZE - 2))
      {
	sendto_one(source_p, "%s", buf);
	cur_len = mlen;
	t = buf + mlen;
      }
                             /* XXX -eeeek */
      tlen = ircsprintf(t, "%s%s ", get_member_status(ms, YES), chptr->chname);
      t += tlen;
      cur_len += tlen;
      reply_to_send = YES;
    }
  }

  if (reply_to_send)
    sendto_one(source_p, "%s", buf);

  if ((IsOper(source_p) || !ConfigServerHide.hide_servers) || target_p == source_p)
    sendto_one(source_p, form_str(RPL_WHOISSERVER),
               me.name, source_p->name, target_p->name,
               server_p->name, server_p->info);
  else
    sendto_one(source_p, form_str(RPL_WHOISSERVER),
	       me.name, source_p->name, target_p->name,
               ServerInfo.network_name,
	       ServerInfo.network_desc);

  if (target_p->user->away != NULL)
    sendto_one(source_p, form_str(RPL_AWAY),
               me.name, source_p->name, target_p->name,
               target_p->user->away);

  if (IsOper(target_p))
    sendto_one(source_p, form_str(IsAdmin(target_p) ? RPL_WHOISADMIN :
               RPL_WHOISOPERATOR), me.name, source_p->name, target_p->name);

  if (IsOper(source_p) && IsCaptured(target_p))
    sendto_one(source_p, form_str(RPL_ISCAPTURED),
               me.name, source_p->name, target_p->name);

  if ((target_p->sockhost[0] != '\0') &&
      !(target_p->sockhost[0] == '0' && target_p->sockhost[1] == '\0'))
  {
    if (IsAdmin(source_p) || source_p == target_p)
      sendto_one(source_p, form_str(RPL_WHOISACTUALLY),
                 me.name, source_p->name, target_p->name, target_p->sockhost);
    else
      sendto_one(source_p, form_str(RPL_WHOISACTUALLY),
                 me.name, source_p->name, target_p->name,
                 IsIPSpoof(target_p) ? "255.255.255.255" : target_p->sockhost);
  }

  if (MyConnect(target_p)) /* Can't do any of this if not local! db */
    sendto_one(source_p, form_str(RPL_WHOISIDLE),
               me.name, source_p->name, target_p->name,
               CurrentTime - target_p->user->last,
               target_p->firsttime);

  hd.client_p = target_p;
  hd.source_p = source_p;

  /* although we should fill in parc and parv, we don't ..
   * be careful of this when writing whois hooks
   */
  hook_call_event("doing_whois", &hd);
}
