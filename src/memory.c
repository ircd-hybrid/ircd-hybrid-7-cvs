/************************************************************************
 *   IRC - Internet Relay Chat, src/memory.c
 *   Copyright (C) 1990 Jarkko Oikarinen and
 *                      University of Oulu, Computing Center
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
 * $Id: memory.c,v 7.20 2001/06/05 01:45:34 ejb Exp $
 */
#include <stdlib.h>
#include <string.h>

#define WE_ARE_MEMORY_C

#include "ircd_defs.h"
#include "ircd.h"
#include "irc_string.h"
#include "memory.h"
#include "list.h"
#include "client.h"
#include "send.h"
#include "tools.h"
#include "s_log.h"
#include "restart.h"
#ifdef MEMDEBUG
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif


#ifdef MEMDEBUG
/* Hopefully this debugger will work better than the existing one...
 * -A1kmm. */

#define DATA(me) (void*)(((char*)me)+sizeof(MemoryEntry))

typedef struct _MemEntry
{
  size_t size;
  time_t ts;
  char file[50];
  int line;
  struct _MemEntry *next, *last;
  /* Data follows... */
} MemoryEntry;
MemoryEntry *first_mem_entry = NULL;

void*
_MyMalloc(size_t length, char *file, int line)
{
  MemoryEntry *mem_entry;
  mem_entry = malloc(sizeof(MemoryEntry)+length);
  if (!mem_entry)
    outofmemory();
  else
    memset(mem_entry, 0, length+sizeof(MemoryEntry));
  mem_entry->size = length;
  mem_entry->ts = CurrentTime;
  if (line > 0)
    strncpy_irc(mem_entry->file, file, 50)[49] = 0;
  else
    *mem_entry->file = 0;
  mem_entry->line = line;
  mem_entry->next = first_mem_entry;
  if (first_mem_entry)
    first_mem_entry->last = mem_entry;
  first_mem_entry = mem_entry;
  return DATA(mem_entry);
}

void
_MyFree(void *what, char *file, int line)
{
  MemoryEntry *mme;
  if (!what)
    return;
  mme = (MemoryEntry*)((char *)what - sizeof(MemoryEntry));
  if (mme->last)
    mme->last->next = mme->next;
  else
    first_mem_entry = mme->next;
  if (mme->next)
    mme->next->last = mme->last;

#ifndef NDEBUG
  mem_frob(mme, mme->size+sizeof(MemoryEntry));
#endif

 free(mme);
}

void*
_MyRealloc(void *what, size_t size, char *file, int line)
{
  MemoryEntry *mme;
  if (!what)
    return _MyMalloc(size, file, line);
  if (!size)
    {
      _MyFree(what, file, line);
      return NULL;
    }
  mme = (MemoryEntry*)((char *)what - sizeof(MemoryEntry));
  mme = realloc(mme, size+sizeof(MemoryEntry));
  mme->size = size;
  mme->next->last = mme;
  mme->last->next = mme; 
  return DATA(mme);
}

void
_DupString(char **x, const char *y, char *file, int line)
{
  *x = _MyMalloc(strlen(y)+1, file, line);
  strcpy(*x, y);
}

void ReportAllocated(struct Client*);

void
ReportAllocated(struct Client *client_p)
{
 int i = 2000;
 MemoryEntry *mme;
 sendto_one(client_p, ":%s NOTICE %s :*** -- Memory Allocation Report",
            me.name, client_p->name);
 for (i=0, mme = first_mem_entry; i < 1000 && mme; mme=mme->next,i++)
  sendto_one(client_p,
          ":%s NOTICE %s :*** -- %u bytes allocated for %lus at %s:%d",
          me.name, client_p->name, mme->size, CurrentTime-mme->ts,
          mme->file, mme->line);
 sendto_one(client_p,
            ":%s NOTICE %s :*** -- End Memory Allocation Report",
            me.name, client_p->name);
}

void
log_memory(void)
{
 MemoryEntry *mme;
 int fd;
 char buffer[200];
 fd = open("memory.log", O_CREAT|O_WRONLY);
 for (mme = first_mem_entry; mme; mme=mme->next)
 {
  sprintf(buffer, "%u bytes allocated for %lus at %s:%d\n",
          mme->size, CurrentTime-mme->ts, mme->file, mme->line);
  write(fd, buffer, strlen(buffer));
 }
 close(fd);
}

#else /* MEMDEBUG */
/*
 * MyMalloc - allocate memory, call outofmemory on failure
 */
void*
_MyMalloc(size_t size)
{
  void* ret = malloc(size);
  if (ret == NULL)
    outofmemory();
  else
    memset(ret, 0, size);
  return ret;
}

/*
 * MyRealloc - reallocate memory, call outofmemory on failure
 */
void*
_MyRealloc(void* x, size_t y)
{
  char *ret = realloc(x, y);

  if (!ret)
    outofmemory();
  return ret;
}

void
_MyFree(void *x)
{
  if ((x))
    free((x));
}

void
_DupString(char **x, const char *y) {
  (*x) = MyMalloc(strlen(y) + 1);
  strcpy((*x), y);
}


#endif /* !MEMDEBUG */

/*
 * outofmemory()
 *
 * input        - NONE
 * output       - NONE
 * side effects - simply try to report there is a problem. Abort if it was called more than once
 */
void outofmemory()
{
  static int was_here = 0;

  if (was_here)
    abort();

  was_here = 1;

  ilog(L_CRIT, "Out of memory: restarting server...");
#ifdef MEMDEBUG
  log_memory();
#endif
  restart("Out of Memory");
}

