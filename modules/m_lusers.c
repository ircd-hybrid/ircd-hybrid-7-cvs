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
 *   $Id: m_lusers.c,v 1.1 2000/11/08 23:57:31 ejb Exp $
 */
#include "handlers.h"
#include "client.h"
#include "ircd.h"
#include "numeric.h"
#include "s_serv.h"    /* hunt_server */
#include "s_user.h"    /* show_lusers */
#include "send.h"
#include "s_conf.h"
#include "msg.h"

struct Message lusers_msgtab = {
  MSG_LUSERS, 0, 0, MFLG_SLOW, 0,
  {m_unregistered, m_lusers, ms_lusers, m_lusers}
};

void
_modinit(void)
{
  mod_add_cmd(MSG_LUSERS, &lusers_msgtab);
}

/*
 * m_lusers - LUSERS message handler
 * parv[0] = sender
 * parv[1] = host/server mask.
 * parv[2] = server to query
 * 
 * 199970918 JRL hacked to ignore parv[1] completely and require parc > 3
 * to cause a force
 */
int m_lusers(struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  static time_t last_used = 0;

  if (!IsAnyOper(sptr))
    {
      if ((last_used + ConfigFileEntry.pace_wait) > CurrentTime)
        {
          /* safe enough to give this on a local connect only */
          if (MyClient(sptr))
            sendto_one(sptr, form_str(RPL_LOAD2HI), me.name, parv[0]);
          return 0;
        }
      else
        {
          last_used = CurrentTime;
        }
    }

  if (parc > 2)
    {
      if(hunt_server(cptr, sptr, ":%s LUSERS %s :%s", 2, parc, parv)
       != HUNTED_ISME)
        {
          return 0;
        }
    }
  return show_lusers(cptr,sptr,parc,parv);
}

/*
 * ms_lusers - LUSERS message handler
 * parv[0] = sender
 * parv[1] = host/server mask.
 * parv[2] = server to query
 * 
 * 199970918 JRL hacked to ignore parv[1] completely and require parc > 3
 * to cause a force
 */
int ms_lusers(struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  static time_t last_used = 0;

  if (!IsAnyOper(sptr))
    {
      if ((last_used + ConfigFileEntry.pace_wait) > CurrentTime)
        {
          /* safe enough to give this on a local connect only */
          if (MyClient(sptr))
            sendto_one(sptr, form_str(RPL_LOAD2HI), me.name, parv[0]);
          return 0;
        }
      else
        {
          last_used = CurrentTime;
        }
    }

  if (parc > 2)
    {
      if(hunt_server(cptr, sptr, ":%s LUSERS %s :%s", 2, parc, parv)
       != HUNTED_ISME)
        {
          return 0;
        }
    }
  return show_lusers(cptr,sptr,parc,parv);
}

