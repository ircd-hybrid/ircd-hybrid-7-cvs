/*
 *  ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd).
 *  list.c: Various assorted functions for various structures.
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
 *  $Id: list.c,v 7.59 2003/06/16 04:01:29 joshk Exp $
 */

#include "stdinc.h"
#include "tools.h"
#include "channel.h"
#include "class.h"
#include "client.h"
#include "common.h"
#include "irc_string.h"
#include "list.h"
#include "hostmask.h"
#include "numeric.h"
#include "irc_res.h"
#include "restart.h"
#include "s_log.h"
#include "send.h"
#include "memory.h"
#include "balloc.h"

int32_t links_count  = 0;
int32_t slinks_count = 0;
int32_t user_count   = 0;

/* initUser()
 *
 * inputs	- none
 * outputs	- none
 *
 * side effects - Creates a block heap for struct Users
 *
 */
static BlockHeap *user_heap;
void
initUser(void)
{
  user_heap = BlockHeapCreate(sizeof(struct User), USER_HEAP_SIZE);
}

/* make_user()
 *
 * inputs	- pointer to client struct
 * output	- pointer to struct User
 * side effects - add's an User information block to a client
 *                if it was not previously allocated.
 */
struct User *
make_user(struct Client *client_p)
{
  if (client_p->user == NULL)
  {
    client_p->user = (struct User *)BlockHeapAlloc(user_heap);
    ++user_count;

    memset(client_p->user, 0, sizeof(struct User));
    client_p->user->refcnt = 1;
  }

  return(client_p->user);
}

/* make_server()
 *
 * inputs	- pointer to client struct
 * output	- pointer to struct Server
 * side effects - add's an Server information block to a client
 *                if it was not previously allocated.
 */
struct Server *
make_server(struct Client *client_p)
{
  struct Server *serv = client_p->serv;

  if (!serv)
  {
    serv = (struct Server *)MyMalloc(sizeof(struct Server));
    client_p->serv = serv;
  }

  return(client_p->serv);
}

/* free_user()
 * 
 * inputs	- pointer to user struct
 *		- pointer to client struct
 * output	- NONE
 * side effects - Decrease user reference count by one and release block,
 *                if count reaches 0
 */
void
free_user(struct User *user, struct Client *client_p)
{
  if (--user->refcnt <= 0)
  {
    if (user->away)
      MyFree(user->away);

    /* sanity check
     */
    if (user->joined || user->refcnt < 0 ||
        user->invited.head || user->channel.head)
    {
      sendto_realops_flags(UMODE_ALL, L_ALL,
			   "* %#lx user (%s!%s@%s) %#lx %#lx %#lx %d %d *",
			   (unsigned long)client_p, client_p ? client_p->name : "<noname>",
			   client_p->username, client_p->host, (unsigned long)user,
			   (unsigned long)user->invited.head,
			   (unsigned long)user->channel.head, user->joined,
			   user->refcnt);
      assert(!user->joined);
      assert(!user->refcnt);
      assert(!user->invited.head);
      assert(!user->channel.head);
    }

    BlockHeapFree(user_heap, user);
    --user_count;
    assert(user_count >= 0);
  }
}

/* init_dlink_nodes()
 *
 */
static BlockHeap *dnode_heap;
void init_dlink_nodes(void)
{
  dnode_heap = BlockHeapCreate(sizeof(dlink_node), DNODE_HEAP_SIZE);
}

/* make_dlink_node()
 *
 * inputs	- NONE
 * output	- pointer to new dlink_node
 * side effects	- NONE
 */
dlink_node*
make_dlink_node(void)
{
  dlink_node *lp;

  lp = (dlink_node *)BlockHeapAlloc(dnode_heap);
  ++links_count;

  lp->next = NULL;
  lp->prev = NULL;
  return(lp);
}

/* free_dlink_node()
 *
 * inputs	- pointer to dlink_node
 * output	- NONE
 * side effects	- free given dlink_node 
 */
void
free_dlink_node(dlink_node *ptr)
{
  BlockHeapFree(dnode_heap, ptr);
  --links_count;
  assert(links_count >= 0);
}

/* init_slink_nodes()
 *
 */
static BlockHeap *snode_heap;
void init_slink_nodes(void)
{
  snode_heap = BlockHeapCreate(sizeof(slink_node), SNODE_HEAP_SIZE);
}

/* make_slink_node()
 *
 * inputs   - NONE
 * output   - pointer to new slink_node
 * side effects - NONE
 */
slink_node *
make_slink_node(void)
{
  slink_node *lp;

  lp = (slink_node *)BlockHeapAlloc(snode_heap);
  ++slinks_count;

  lp->next = NULL;
  return(lp);
}

/* free_slink_node()
 *
 * inputs   - pointer to slink_node
 * output   - NONE
 * side effects - free given slink_node
 */
void
free_slink_node(slink_node *ptr)
{
  BlockHeapFree(snode_heap, ptr);
  --slinks_count;
  assert(slinks_count >= 0);
}

/* count_user_memory()
 *
 * inputs	- pointer to user memory actually used
 *		- pointer to user memory allocated total in block allocator
 * output	- NONE
 * side effects	- NONE
 */
void
count_user_memory(int *count, unsigned long *user_memory_used)
{
  *count = user_count;
  *user_memory_used = user_count * sizeof(struct User);
}

/* count_links_memory()
 *
 * inputs	- pointer to dlinks memory actually used
 *		- pointer to dlinks memory allocated total in block allocator
 * output	- NONE
 * side effects	- NONE
 */
void
count_links_memory(int *count, unsigned long *links_memory_used)
{
  *count = links_count;
  *links_memory_used = links_count * sizeof(dlink_node);
}

