/*
 * restart.c
 *
 * $Id: restart.c,v 7.5 2000/11/26 00:42:11 db Exp $
 */
#include "restart.h"
#include "common.h"
#include "fdlist.h"
#include "ircd.h"
#include "send.h"
#include "s_debug.h"
#include "s_log.h"

#include <unistd.h>

/* external var */
extern char** myargv;

void restart(char *mesg)
{
  static int was_here = NO; /* redundant due to restarting flag below */

  if (was_here)
    abort();
  was_here = YES;

  log(L_NOTICE, "Restarting Server because: %s, memory data limit: %ld",
         mesg, get_maxrss());

  server_reboot();
}

void server_reboot(void)
{
  int i;
  
  sendto_realops("Aieeeee!!!  Restarting server... memory: %d", get_maxrss());

  log(L_NOTICE, "Restarting server...");
  /*
   * XXX we used to call flush_connections() here. But since this routine
   * doesn't exist anymore, we won't be flushing. This is ok, since 
   * when close handlers come into existance, comm_close() will be called
   * below, and the data flushing will be implicit.
   *    -- adrian
   */
  for (i = 0; i < MAXCONNECTIONS; ++i)
    fd_close(i);
  execv(SPATH, myargv);

  exit(-1);
}


