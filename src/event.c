/*
 * Event Processing
 *
 * This code was borrowed from the squid web cache by Adrian Chadd.
 *
 * $Id: event.c,v 7.22 2001/09/13 00:19:44 habeeb Exp $
 *
 * Original header follows:
 *
 * Id: event.c,v 1.30 2000/04/16 21:55:10 wessels Exp
 *
 * DEBUG: section 41    Event Processing
 * AUTHOR: Henrik Nordstrom
 *
 * SQUID Internet Object Cache  http://squid.nlanr.net/Squid/
 * ----------------------------------------------------------
 *
 *  Squid is the result of efforts by numerous individuals from the
 *  Internet community.  Development is led by Duane Wessels of the
 *  National Laboratory for Applied Network Research and funded by the
 *  National Science Foundation.  Squid is Copyrighted (C) 1998 by
 *  the Regents of the University of California.  Please see the
 *  COPYRIGHT file for full details.  Squid incorporates software
 *  developed and/or copyrighted by other sources.  Please see the
 *  CREDITS file for full details.
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
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111, USA.
 *
 */

/*
 * How its used:
 *
 * Should be pretty self-explanatory. Events are added to the static
 * array event_table with a frequency time telling eventRun how often
 * to execute it.
 */

#include <stdlib.h>

#include "ircd.h"
#include "event.h"
#include "client.h"
#include "send.h"
#include "memory.h"
#include "s_log.h"

static const char *last_event_ran = NULL;
struct ev_entry event_table[MAX_EVENTS];
static int event_max = 0;
static time_t event_time_min = -1;


void
eventAdd(const char *name, EVH *func, void *arg, time_t when)
{

  if (event_max > MAX_EVENTS)
    {
      ilog(L_ERROR, "event number has exceeded static maximum number of events (%d).  Please raise MAX_EVENTS!", MAX_EVENTS);
      exit(-1);
    }

  event_table[event_max].func = func;
  event_table[event_max].name = name;
  event_table[event_max].arg = arg;
  event_table[event_max].when = CurrentTime + when;
  event_table[event_max].frequency = when; 
  event_table[event_max].active = 1;

  if ((event_table[event_max].when < event_time_min) || (event_time_min == -1))
    event_time_min = event_table[event_max].when;

  ++event_max;
}

/* same as eventAdd but adds a random offset within +-1/3 of delta_ish */
void
eventAddIsh(const char *name, EVH *func, void *arg, time_t delta_ish)
{
  if (delta_ish >= 3.0)
    {
      const time_t two_third = (2 * delta_ish) / 3;
      delta_ish = two_third + ((random() % 1000) * two_third) / 1000;
      /*
       * XXX I hate the above magic, I don't even know if its right.
       * Grr. -- adrian
       */
    }
  eventAdd(name, func, arg, delta_ish);
}

void
eventRun(void)
{
  int i;

  if (event_max == 0)
    return;

  for (i = 0; i < event_max; i++)
    {
      if (event_table[i].active && (event_table[i].when <= CurrentTime))
        {
          last_event_ran = event_table[i].name;
          event_table[i].func(event_table[i].arg);
          event_table[i].when = CurrentTime + event_table[i].frequency;
          event_time_min = -1;
        }
    }
}


time_t
eventNextTime(void)
{
  int i;

  if (event_max == 0)
    return (CurrentTime+1);
  else if (event_time_min == -1)
    {
      for (i = 0; i < event_max; i++)
        {
          if (event_table[i].active && ((event_table[i].when < event_time_min) || (event_time_min == -1)))
            event_time_min = event_table[i].when;
        }
    }
  return event_time_min;
}

void
eventInit(void)
{
  last_event_ran = NULL;
}

int
eventFind(EVH *func, void *arg)
{
  int i;
  for (i = 0; i < event_max; i++)
    {
      if (event_table[i].func == func && event_table[i].arg == arg)
        return i;
    }
  return -1;
}

void
show_events(struct Client *source_p)
{
  int i;

  if (last_event_ran)
    sendto_one(source_p, ":%s NOTICE %s :*** Last event to run: %s",
               me.name, source_p->name,
               last_event_ran);

  sendto_one(source_p,
     ":%s NOTICE %s :*** Operation            Next Execution",
     me.name, source_p->name);

  for (i = 0; i < event_max; i++)
    {
      sendto_one(source_p,
                 ":%s NOTICE %s :*** %-20s %-3d seconds",
                 me.name, source_p->name,
                 event_table[i].name, (int)(event_table[i].when - CurrentTime));
    }
  sendto_one(source_p, ":%s NOTICE %s :*** Finished", me.name, source_p->name);
}

/* void set_back_events(time_t by)
 * Input: Time to set back events by.
 * Output: None.
 * Side-effects: Sets back all events by "by" seconds.
 */
void
set_back_events(time_t by)
{
  int i;
  for (i = 0; i < event_max; i++)
    if (event_table[i].when > by)
      event_table[i].when -= by;
    else
      event_table[i].when = 0;
}


