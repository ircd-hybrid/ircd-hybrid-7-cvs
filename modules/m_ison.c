/************************************************************************
 *   IRC - Internet Relay Chat, src/m_ison.c
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
 *   $Id: m_ison.c,v 1.1 2000/11/08 23:57:28 ejb Exp $
 */
#include "handlers.h"
#include "client.h"
#include "irc_string.h"
#include "ircd.h"
#include "numeric.h"
#include "send.h"
#include "msg.h"

#include <string.h>

struct Message ison_msgtab = {
  MSG_ISON, 0, 1, MFLG_SLOW, 0,
  {m_unregistered, m_ison, m_ignore, m_ison}
};

void
_modinit(void)
{
  mod_add_cmd(MSG_ISON, &ison_msgtab);
}

static char buf[BUFSIZE];

/*
 * m_ison added by Darren Reed 13/8/91 to act as an efficent user indicator
 * with respect to cpu/bandwidth used. Implemented for NOTIFY feature in
 * clients. Designed to reduce number of whois requests. Can process
 * nicknames in batches as long as the maximum buffer length.
 *
 * format:
 * ISON :nicklist
 */
/*
 * Take care of potential nasty buffer overflow problem 
 * -Dianora
 *
 */


int m_ison(struct Client *cptr, struct Client *sptr, int parc, char *parv[])
{
  struct Client *acptr;
  char *nick;
  char *p;
  char *current_insert_point;
  int len;

  if (parc < 2)
    {
      sendto_one(sptr, form_str(ERR_NEEDMOREPARAMS),
                 me.name, parv[0], "ISON");
      return 0;
    }

  ircsprintf(buf, form_str(RPL_ISON), me.name, parv[0]);
  len = strlen(buf);
  current_insert_point = buf + len;

  if (!IsGlobalOper(cptr))
    cptr->priority +=20; /* this keeps it from moving to 'busy' list */

  for (nick = strtoken(&p, parv[1], ","); nick;
       nick = strtoken(&p, NULL, ","))
    {
      if ((acptr = find_person(nick, NULL)))
	{
	  len = strlen(acptr->name);
	  if( (current_insert_point + (len + 5)) < (buf + sizeof(buf)) )
	    {
	      memcpy((void *)current_insert_point,
		     (void *)acptr->name, len);
	      current_insert_point += len;
	      *current_insert_point++ = ' ';
	    }
	  else
	    break;
	}
    }

/*  current_insert_point--; Do NOT take out the trailing space, it breaks ircII --Rodder */
  *current_insert_point = '\0';
  sendto_one(sptr, "%s", buf);
  return 0;
}
