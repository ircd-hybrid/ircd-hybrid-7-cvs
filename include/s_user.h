/************************************************************************
 *   IRC - Internet Relay Chat, include/s_user.h
 *   Copyright (C) 1992 Darren Reed
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
 *
 * "s_user.h". - Headers file.
 *
 * $Id: s_user.h,v 7.2 1999/09/01 04:28:00 tomh Exp $
 *
 */
#ifndef INCLUDED_s_user_h
#define INCLUDED_s_user_h
#ifndef INCLUDED_config_h
#include "config.h"
#endif
#ifndef INCLUDED_sys_types_h
#include <sys/types.h>      /* time_t */
#define INCLUDED_sys_types_h
#endif

struct Client;
#ifdef PACE_WALLOPS
extern time_t LastUsedWallops;
#endif


#ifdef BOTCHECK
extern int bot_check(const char* host);
#endif

extern int   user_mode(struct Client *, struct Client *, int, char **);
extern void  send_umode (struct Client *, struct Client *,
                         int, int, char *);
extern void  send_umode_out (struct Client*, struct Client *, int);
extern int   show_lusers(struct Client *, struct Client *, int, char **);
extern void  show_opers(struct Client* client);

extern int do_user(char* nick, struct Client* cptr, struct Client* sptr,
                   char* username, char *host, char *server, char *realname);

extern int clean_nick_name(char* nick);
extern int nickkilldone(struct Client *cptr, struct Client *sptr, int parc,
                        char *parv[], time_t newts,char *nick);

#endif /* INCLUDED_s_user_h */


