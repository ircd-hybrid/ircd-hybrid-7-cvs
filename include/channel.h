/************************************************************************
 *
 *   IRC - Internet Relay Chat, include/channel.h
 *   Copyright (C) 1990 Jarkko Oikarinen
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
 * $Id: channel.h,v 7.37 2000/12/01 22:17:49 db Exp $
 */

#ifndef INCLUDED_channel_h
#define INCLUDED_channel_h
#ifndef INCLUDED_config_h
#include "config.h"           /* config settings */
#endif
#ifndef INCLUDED_ircd_defs_h
#include "ircd_defs.h"        /* buffer sizes */
#endif
#ifndef INCLUDED_sys_types_h
#include <sys/types.h>        /* time_t */
#define INCLUDED_sys_types_h
#endif

struct Client;


/* mode structure for channels */

struct Mode
{
  unsigned int  mode;
  int   limit;
  char  key[KEYLEN + 1];
};

/* channel structure */

struct Channel
{
  struct Channel* nextch;
  struct Channel* prevch;
  struct Channel* hnextch;
  struct Mode     mode;
  char            topic[TOPICLEN + 1];
  char           *topic_info;
  time_t          topic_time;
  int             users;      /* user count */
  int             locusers;   /* local user count */
  int             opcount;    /* number of chanops */
  unsigned long   lazyLinkChannelExists;
  time_t          users_last;		/* when last user was in channel */
  time_t          last_knock;           /* don't allow knock to flood */
  
  struct Channel* next_vchan;           /* Link list of sub channels */
  struct Channel* prev_vchan;           /* Link list of sub channels */

  dlink_list      chanops;
  dlink_list      halfops;
  dlink_list      voiced;
  dlink_list      peons;	/* non ops, just members */

  dlink_list      invites;
  dlink_list      banlist;
  dlink_list      exceptlist;
  dlink_list      denylist;
  dlink_list      invexlist;

  int             num_bed;          /* number of bans+exceptions+denies */
  time_t          channelts;
  char            chname[1];
};

extern  struct  Channel *GlobalChannelList;

void cleanup_channels(void *);

#define CREATE 1        /* whether a channel should be
                           created or just tested for existance */

#define MODEBUFLEN      200

#define NullChn ((struct Channel *)0)

#define ChannelExists(n)        (hash_find_channel(n, NullChn) != NullChn)

/* Maximum mode changes allowed per client, per server is different */
/*
  arghhh ONE MORE TIME explaining this, read carefully...

From rfc1459.txt

   IRC messages are always lines of characters terminated with a CR-LF
   (Carriage Return - Line Feed) pair, and these messages shall not
   exceed 512 characters in length, counting all characters including
   the trailing CR-LF. Thus, there are 510 characters maximum allowed
   for the command and its parameters.  There is no provision for
   continuation message lines.  See section 7 for more details about
   current implementations.

   Now, from ircd_defs.h

#define USERLEN         10
#define HOSTLEN         63
#define NICKLEN         9
#define CHANNELLEN      200

so
:nick #channel_name +bbb ban1 ban2 ban3 ban4

Where nick == 10 chars
channel_name == 200
each ban is of form "nick!user@host"
thats 10 + 10 + 63 folks

thats a total of 10 + 200 + (4 * 73)
10 + 200 + 292
Thats 502

Now add "+bbbb" Thats 5 chars
now thats 507 

add 6 spaces
thats 513

  We could walk over the end of the buffer..... It would probably take
a kiddie trying to core the server to do it, but still ;)

  Yes, its keen having +4 but it slows us down to strlen() to check
that the buffer isn't being overflowed. I'd rather be comfortable at
3, and gain CPU speed at the expensive of more mode sends.

-db
*/

#define MAXMODEPARAMS   3

extern dlink_node *find_user_link (dlink_list *, struct Client *);
extern dlink_node *find_channel_link(dlink_list *lp,
				     struct Channel *chptr); 
extern void    add_user_to_channel(struct Channel *chptr,
				   struct Client *who, int flags);
extern void    remove_user_from_channel(struct Channel *chptr,
					struct Client *who);

extern int     can_send (struct Channel *chptr, struct Client *who);
extern int     is_banned (struct Channel *chptr, struct Client *who);

extern int     is_chan_op (struct Channel *chptr,struct Client *who);

extern void    send_channel_modes (struct Client *, struct Channel *);
extern int     check_channel_name(const char* name);
extern void    channel_modes(struct Channel *chptr, struct Client *who,
			     char *, char *);
extern void    set_channel_mode(struct Client *, struct Client *, 
                                struct Channel *, int, char **, char *);
extern struct  Channel* get_channel(struct Client *,char*,int );
extern void    clear_bans_exceptions_denies(struct Client *,struct Channel *);

extern void channel_member_names( struct Client *sptr, struct Channel *chptr,
				  char *name_of_channel);
extern char *channel_pub_or_secret(struct Channel *chptr);
extern char *channel_chanop_or_voice(struct Channel *, struct Client *);

extern void add_invite(struct Channel *chptr, struct Client *who);
extern void del_invite(struct Channel *chptr, struct Client *who);

extern int list_continue(struct Client *sptr);
extern void list_one_channel(struct Client *sptr,struct Channel *chptr);

/*
** Channel Related macros follow
*/

/* Channel related flags */

#define CHFL_PEON	0x0000 /* normal member of channel */
#define CHFL_CHANOP     0x0001 /* Channel operator */
#define CHFL_VOICE      0x0002 /* the power to speak */
#define CHFL_DEOPPED    0x0004 /* deopped by us, modes need to be bounced */
#define CHFL_HALFOP     0x0008 /* deopped by us, modes need to be bounced */
#define CHFL_BAN        0x0010 /* ban channel flag */
#define CHFL_EXCEPTION  0x0020 /* exception to ban channel flag */
#define CHFL_DENY       0x0040 /* regular expression deny flag */
#define CHFL_INVEX      0x0080

/* Channel Visibility macros */

#define MODE_PEON	CHFL_PEON
#define MODE_CHANOP     CHFL_CHANOP
#define MODE_VOICE      CHFL_VOICE
#define MODE_HALFOP	CHFL_HALFOP
#define MODE_DEOPPED	CHFL_DEOPPED

/* channel modes ONLY */
#define MODE_PRIVATE    0x0008
#define MODE_SECRET     0x0010
#define MODE_MODERATED  0x0020
#define MODE_TOPICLIMIT 0x0040
#define MODE_INVITEONLY 0x0080
#define MODE_NOPRIVMSGS 0x0100
#define MODE_KEY        0x0200
#define MODE_BAN        0x0400
#define MODE_EXCEPTION  0x0800
#define MODE_DENY       0x1000
#define MODE_INVEX	0x2000

#define MODE_LIMIT      0x4000  /* was 0x2000 */
#define MODE_FLAGS      0x4fff  /* was 0x2fff */

/*
 * mode flags which take another parameter (With PARAmeterS)
 */
#define MODE_WPARAS (MODE_CHANOP|MODE_VOICE|MODE_BAN|\
                     MODE_EXCEPTION|MODE_DENY|MODE_KEY|MODE_LIMIT|MODE_INVEX)

/*
 * Undefined here, these are used in conjunction with the above modes in
 * the source.
#define MODE_QUERY     0x10000000
#define MODE_DEL       0x40000000
#define MODE_ADD       0x80000000
 */

/* used in SetMode() in channel.c and m_umode() in s_msg.c */

#define MODE_NULL      0
#define MODE_QUERY     0x10000000
#define MODE_ADD       0x40000000
#define MODE_DEL       0x20000000

#define HoldChannel(x)          (!(x))
/* name invisible */
#define SecretChannel(x)        ((x) && ((x)->mode.mode & MODE_SECRET))
/* channel not shown but names are */
#define HiddenChannel(x)        ((x) && ((x)->mode.mode & MODE_PRIVATE))
/* channel visible */
#define ShowChannel(v,c)        (PubChannel(c) || IsMember((v),(c)))
#define PubChannel(x)           ((!x) || ((x)->mode.mode &\
                                 (MODE_PRIVATE | MODE_SECRET)) == 0)

#define IsMember(blah,chan) ((blah && blah->user && \
                find_channel_link(&blah->user->channel, chan)) ? 1 : 0)

#define IsChannelName(name) ((name) && (*(name) == '#' || *(name) == '&'))

/*
 * Move BAN_INFO information out of the SLink struct
 * its _only_ used for bans, no use wasting the memory for it
 * in any other type of link. Keep in mind, doing this that
 * it makes it slower as more Malloc's/Free's have to be done, 
 * on the plus side bans are a smaller percentage of SLink usage.
 * Over all, the hybrid coding team came to the conclusion
 * it was worth the effort.
 *
 *  - Dianora
 */
typedef struct Ban      /* also used for exceptions -orabidoo */
{
  char *banstr;
  char *who;
  time_t when;
} aBan;

#define CLEANUP_CHANNELS_TIME (15*60)
#define MAX_VCHAN_TIME (60*60)

#endif  /* INCLUDED_channel_h */

