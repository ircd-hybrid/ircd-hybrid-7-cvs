/************************************************************************
 *   IRC - Internet Relay Chat, src/m_version.c
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Computing Center
 *
 *   See file AUTHORS in IRC package for additional names of
 *   the programmers. 
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation; either version 1, or (at your option)
 *   any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program; if not, write to the Free Software
 *   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 *   $Id: m_ltrace.c,v 1.1 2000/11/08 23:57:31 ejb Exp $
 */
#include "handlers.h"
#include "class.h"
#include "client.h"
#include "common.h"
#include "hash.h"
#include "irc_string.h"
#include "ircd.h"
#include "numeric.h"
#include "s_bsd.h"
#include "s_serv.h"
#include "send.h"

#include <string.h>
#include <time.h>

/*
 * m_functions execute protocol messages on this server:
 *
 *      cptr    is always NON-NULL, pointing to a *LOCAL* client
 *              structure (with an open socket connected!). This
 *              identifies the physical socket where the message
 *              originated (or which caused the m_function to be
 *              executed--some m_functions may call others...).
 *
 *      sptr    is the source of the message, defined by the
 *              prefix part of the message if present. If not
 *              or prefix not found, then sptr==cptr.
 *
 *              (!IsServer(cptr)) => (cptr == sptr), because
 *              prefixes are taken *only* from servers...
 *
 *              (IsServer(cptr))
 *                      (sptr == cptr) => the message didn't
 *                      have the prefix.
 *
 *                      (sptr != cptr && IsServer(sptr) means
 *                      the prefix specified servername. (?)
 *
 *                      (sptr != cptr && !IsServer(sptr) means
 *                      that message originated from a remote
 *                      user (not local).
 *
 *              combining
 *
 *              (!IsServer(sptr)) means that, sptr can safely
 *              taken as defining the target structure of the
 *              message in this server.
 *
 *      *Always* true (if 'parse' and others are working correct):
 *
 *      1)      sptr->from == cptr  (note: cptr->from == cptr)
 *
 *      2)      MyConnect(sptr) <=> sptr == cptr (e.g. sptr
 *              *cannot* be a local connection, unless it's
 *              actually cptr!). [MyConnect(x) should probably
 *              be defined as (x == x->from) --msa ]
 *
 *      parc    number of variable parameter strings (if zero,
 *              parv is allowed to be NULL)
 *
 *      parv    a NULL terminated list of parameter pointers,
 *
 *                      parv[0], sender (prefix string), if not present
 *                              this points to an empty string.
 *                      parv[1]...parv[parc-1]
 *                              pointers to additional parameters
 *                      parv[parc] == NULL, *always*
 *
 *              note:   it is guaranteed that parv[0]..parv[parc-1] are all
 *                      non-NULL pointers.
 */


#ifdef LTRACE
/*
** m_ltrace - LimitedTRACE... like m_trace() but doesn't return TRACEUSER, TRACEUNKNOWN, or TRACECLASS
**      parv[0] = sender prefix
**      parv[1] = servername
*/
int     m_ltrace(struct Client *cptr,
                struct Client *sptr,
                int parc,
                char *parv[])
{
  int   i;
  struct Client       *acptr = NULL;
  char  *tname;
  int   doall, link_s[MAXCONNECTIONS], link_u[MAXCONNECTIONS];
  int   cnt = 0, wilds, dow;
  static time_t now;
  
  if (check_registered(sptr))
    return 0;

  if (parc > 2)
    if (hunt_server(cptr, sptr, ":%s LTRACE %s :%s",
                    2, parc, parv))
      return 0;
  
  if (parc > 1)
    tname = parv[1];
  else
    tname = me.name;

  switch (hunt_server(cptr, sptr, ":%s LTRACE :%s", 1, parc, parv))
    {
    case HUNTED_PASS: /* note: gets here only if parv[1] exists */
      {
        struct Client *ac2ptr;
        
        ac2ptr = next_client(GlobalClientList, tname);
        if (ac2ptr)
          sendto_one(sptr, form_str(RPL_TRACELINK), me.name, parv[0],
                     version, debugmode, tname, ac2ptr->from->name);
        else
          sendto_one(sptr, form_str(RPL_TRACELINK), me.name, parv[0],
                     version, debugmode, tname, "ac2ptr_is_NULL!!");
        return 0;
      }
    case HUNTED_ISME:
      break;
    default:
      return 0;
    }


  if(MyClient(sptr))
    sendto_realops_flags(FLAGS_SPY, "ltrace requested by %s (%s@%s) [%s]",
                       sptr->name, sptr->username, sptr->host,
                       sptr->user->server);


  doall = (parv[1] && (parc > 1)) ? match(tname, me.name): TRUE;
  wilds = !parv[1] || strchr(tname, '*') || strchr(tname, '?');
  dow = wilds || doall;

  if(!IsAnyOper(sptr) || !dow) /* non-oper traces must be full nicks */
                              /* lets also do this for opers tracing nicks */
    { 
      const char* name;
      const char* ip;
      int         c_class;

      acptr = hash_find_client(tname,(struct Client *)NULL);
      if(!acptr || !IsPerson(acptr))
        { 
          /* this should only be reached if the matching
             target is this server */
          sendto_one(sptr, form_str(RPL_ENDOFTRACE),me.name,
                     parv[0], tname);
          return 0;
        }
      name = get_client_name(acptr, FALSE);
      ip = inetntoa((char*) &acptr->ip);

      c_class = get_client_class(acptr);

      if (IsAnyOper(acptr))
        { 
          sendto_one(sptr, form_str(RPL_TRACEOPERATOR),
                     me.name, parv[0], c_class,
                     name,
                     IsAnyOper(sptr)?ip:(IsIPHidden(acptr)?"127.0.0.1":ip),
                     now - acptr->lasttime,
                     (acptr->user)?(now - acptr->user->last):0);
        }
      sendto_one(sptr, form_str(RPL_ENDOFTRACE),me.name,
                 parv[0], tname);
      return 0;
    }
  
  for (i = 0; i < MAXCONNECTIONS; i++)
    link_s[i] = 0, link_u[i] = 0;
                        
  if (dow && GlobalSetOptions.lifesux && !IsGlobalOper(sptr))
    {
      sendto_one(sptr,form_str(RPL_LOAD2HI),me.name,parv[0]);
      return 0;
    }

  /*
   * Count up all the servers and clients in a downlink.
   */
  if (doall) {
    for (acptr = GlobalClientList; acptr; acptr = acptr->next) {
      if (IsServer(acptr)) 
        ++link_s[acptr->from->fd];
    }
  }

  /* report all direct connections */
  now = time(NULL);
  for (i = 0; i <= highest_fd; i++)
    {
      const char* name;
      const char* ip;
      int         c_class;
      
      if (!(acptr = local[i])) /* Local Connection? */
        continue;
      if (IsInvisible(acptr) && dow &&
          !(MyConnect(sptr) && IsAnyOper(sptr)) &&
          !IsAnyOper(acptr) && (acptr != sptr))
        continue;
      if (!doall && wilds && !match(tname, acptr->name))
        continue;
      if (!dow && irccmp(tname, acptr->name))
        continue;
      name = get_client_name(acptr, FALSE);
      ip = inetntoa((const char*) &acptr->ip);

      c_class = get_client_class(acptr);
      
      switch(acptr->status)
        {
        case STAT_HANDSHAKE:
          sendto_one(sptr, form_str(RPL_TRACEHANDSHAKE), me.name,
                     parv[0], c_class, name);
          cnt++;
          break;
        case STAT_ME:
          break;
        case STAT_CLIENT:
          /* Well, most servers don't have a LOT of OPERs... let's show them too */
          if ((IsAnyOper(sptr) &&
              (MyClient(sptr) || !(dow && IsInvisible(acptr))))
              || !dow || IsAnyOper(acptr))
            {
              if (IsAnyOper(acptr))
                sendto_one(sptr,
                           form_str(RPL_TRACEOPERATOR),
                           me.name, parv[0], c_class,
                           name, 
                           IsAnyOper(sptr)?ip:(IsIPHidden(acptr)?"127.0.0.1":ip), 
                           now - acptr->lasttime,
                           (acptr->user)?(now - acptr->user->last):0);
              cnt++;
            }
          break;
        case STAT_SERVER:
#if 0
          if (acptr->serv->user)
            sendto_one(sptr, form_str(RPL_TRACESERVER),
                       me.name, parv[0], c_class, link_s[i],
                       link_u[i], name, acptr->serv->by,
                       acptr->serv->user->username,
                       acptr->serv->user->host, now - acptr->lasttime);
          else
#endif
            sendto_one(sptr, form_str(RPL_TRACESERVER),
                       me.name, parv[0], c_class, link_s[i],
                       link_u[i], name, *(acptr->serv->by) ?
                       acptr->serv->by : "*", "*",
                       me.name, now - acptr->lasttime);
          cnt++;
          break;
        default: /* ...we actually shouldn't come here... --msa */
          sendto_one(sptr, form_str(RPL_TRACENEWTYPE), me.name,
                     parv[0], name);
          cnt++;
          break;
        }
    }
  /*
   * Add these lines to summarize the above which can get rather long
   * and messy when done remotely - Avalon
   */
  if (!IsAnyOper(sptr) || !cnt)
    {
      if (cnt)
          return 0;
      /* let the user have some idea that its at the end of the
       * trace
       */
      sendto_one(sptr, form_str(RPL_TRACESERVER),
                 me.name, parv[0], 0, link_s[me.fd],
                 link_u[me.fd], me.name, "*", "*", me.name);
      return 0;
    }
  return 0;
}
#endif /* LTRACE */

