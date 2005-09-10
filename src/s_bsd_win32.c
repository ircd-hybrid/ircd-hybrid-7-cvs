/*
 *  ircd-hybrid: an advanced Internet Relay Chat Daemon(ircd).
 *  s_bsd_win32.c: Winsock WSAAsyncSelect() compatible network routines.
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
 *  $Id: s_bsd_win32.c,v 7.14 2005/09/10 17:44:12 michael Exp $
 */

#include "stdinc.h"
#include "fdlist.h"
#include "ircd.h"
#include "s_bsd.h"
#include "common.h"
#include "client.h"
#include "restart.h"

HWND wndhandle;
static dlink_list dns_queries = {NULL, NULL, 0};

extern int main(int, char *[]);

/*
 * Initial entry point for Win32 GUI applications, called by the C runtime.
 *
 * It should be only a wrapper for main(), since when compiled as a console
 * application, main() is called instead.
 */
int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow)
{
  /* Do we really need these pidfile, logfile etc arguments?
   * And we are not on a console, so -help or -foreground is meaningless. */

  char *argv[2] = {"ircd", NULL};

  return main(1, argv);
}

/*
 * Updates monitored network events on a given fde_t.
 */
static void
update_winsock_events(fde_t *F)
{
  int events = 0;

  if (F->read_handler != NULL)
    events |= FD_ACCEPT | FD_CLOSE | FD_READ;

  if (F->write_handler != NULL)
    events |= FD_CONNECT | FD_WRITE;

  WSAAsyncSelect(F->fd, wndhandle, WM_SOCKET, events);
}

/*
 * Handler for Win32 messages.
 */
static LRESULT CALLBACK
hybrid_wndproc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  switch (uMsg)
  {
    case WM_SOCKET:
    {
      fde_t *F = lookup_fd((int) wParam);
      PF *hdl;

      if (F != NULL)
        switch (WSAGETSELECTEVENT(lParam))
	{
	  case FD_ACCEPT:
	  case FD_CLOSE:
	  case FD_READ:
	    hdl = F->read_handler;
	    F->read_handler = NULL;

	    if (hdl != NULL)
              hdl(F, F->read_data);

            if (hdl == NULL)
	      update_winsock_events(F);
	    break;

          case FD_CONNECT:
          case FD_WRITE:
	    hdl = F->write_handler;
	    F->write_handler = NULL;

            if (hdl != NULL)
	      hdl(F, F->write_data);

            if (hdl == NULL)
	      update_winsock_events(F);
        }

      return 0;
    }

    case WM_DNS:
    {
      dlink_node *ptr;

      DLINK_FOREACH(ptr, dns_queries.head)
        if (((struct DNSQuery *) ptr->data)->handle == wParam)
	{
	  struct DNSQuery *query = ptr->data;
	  struct DNSReply *reply = NULL;

          dlinkDelete(&query->node, &dns_queries);

          if (WSAGETASYNCERROR(lParam) == 0)
	  {
	    struct hostent *h = (struct hostent *) &query->reply;
	    static struct DNSReply _reply;
	    reply = &_reply;

            reply->h_name = h->h_name;
	    reply->addr.ss.ss_family = h->h_addrtype;

	    switch (h->h_addrtype)
	    {
	      case AF_INET:
	        memcpy(&((struct sockaddr_in *) &reply->addr)->sin_addr,
		       h->h_addr_list[0], h->h_length);
		break;

#ifdef IPV6
              case AF_INET6:
	        memcpy(&((struct sockaddr_in6 *) &reply->addr)->sin6_addr,
		       h->h_addr_list[0], h->h_length);
		break;
#endif

              default:  /* shouldn't happen */
	        reply = NULL;
	    }
	  }

          query->callback(query->ptr, reply);
	  return 0;
	}

      return 0;
    }

    case WM_REHASH:
      dorehash = 1;
      return 0;

    case WM_REMOTD:
      doremotd = 1;
      return 0;

    case WM_DESTROY:
      PostQuitMessage(0);
      return 0;

    default:
      return DefWindowProc(hwnd, uMsg, wParam, lParam);
  }
}

/*
 * Initialize Winsock, create a window handle.
 */
void
init_netio(void)
{
  WNDCLASS wndclass;
  WSADATA wsa;

  /* Initialize Winsock networking */

  if (WSAStartup(0x101, &wsa) != 0)
  {
    MessageBox(NULL, "Cannot initialize Winsock -- terminating ircd",
      NULL, MB_OK | MB_ICONERROR);
    exit(1);
  }

  /* First, we need a class for our window that has message handler
   * set to hybrid_wndproc() */

  memset(&wndclass, 0, sizeof(wndclass));

  wndclass.lpfnWndProc = hybrid_wndproc;
  wndclass.hInstance = GetModuleHandle(NULL);
  wndclass.lpszClassName = PACKAGE_NAME;

  RegisterClass(&wndclass);

  /* Now, initialize the window */

  wndhandle = CreateWindow(PACKAGE_NAME, NULL, 0,
    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
    NULL, NULL, wndclass.hInstance, NULL);

  if (!wndhandle)
  {
    MessageBox(NULL, "Cannot allocate window handle -- terminating ircd",
      NULL, MB_OK | MB_ICONERROR);
    exit(1);
  }

  /* Set up a timer which will periodically post a message to our queue.
   * This way, ircd won't wait infinitely for a network event */

  SetTimer(wndhandle, 0, SELECT_DELAY, NULL);
}

/*
 * comm_setselect
 *
 * This is a needed exported function which will be called to register
 * and deregister interest in a pending IO state for a given FD.
 */
void
comm_setselect(fde_t *F, unsigned int type, PF *handler,
               void *client_data, time_t timeout)
{
  int do_update = NO;

  switch (type)
  {
    case COMM_SELECT_READ:
      if (!F->read_handler != !handler)
        do_update = YES;

      F->read_handler = handler;
      F->read_data = client_data;
      break;

    case COMM_SELECT_WRITE:
      if (!F->write_handler != !handler)
        do_update = YES;

      F->write_handler = handler;
      F->write_data = client_data;
  }

  if (do_update)
    update_winsock_events(F);
}

/*
 * Waits until a message is posted in our window queue and deals with it.
 */
void
comm_select(void)
{
  MSG msg;

  if (!GetMessage(&msg, NULL, 0, 0))
    server_die("received message WM_QUIT");

  set_time();

  DispatchMessage(&msg);
}

/* This is our win32 super-light resolver ;) */

void
delete_resolver_queries(const struct DNSQuery *query)
{
  WSACancelAsyncRequest(query->handle);
  dlinkDelete(&query->node, &dns_queries);
}

void
gethost_byname_type(const char *name, struct DNSQuery *query, int type)
{
  gethost_byname(name, query);
}

void
gethost_byname(const char *name, struct DNSQuery *query)
{
  query->handle = WSAAsyncGetHostByName(wndhandle, WM_DNS, name, query->reply,
    sizeof(query->reply));

  if (!query->handle)
    query->callback(query->ptr, NULL);
  else
    dlinkAdd(query, &query->node, &dns_queries);
}

void
gethost_byaddr(const struct irc_ssaddr *addr, struct DNSQuery *query)
{
  query->handle = WSAAsyncGetHostByAddr(
    wndhandle, WM_DNS,
#ifdef IPV6
    addr.ss.ss_family == AF_INET6 ? &((struct sockaddr_in6*)addr)->sin6_addr) :
#endif
    &((struct sockaddr_in *) addr)->sin_addr,
#ifdef IPV6
    addr.ss.ss_family == AF_INET6 ? sizeof(struct in6_addr) :
#endif
    sizeof(struct in_addr),
    addr->ss.ss_family, query->reply, sizeof(query->reply)
  );

  if (!query->handle)
    query->callback(query->ptr, NULL);
  else
    dlinkAdd(query, &query->node, &dns_queries);
}

void
report_dns_servers(struct Client *source_p)
{
  /* todo */
}

/* Copyright (C) 2001 Free Software Foundation, Inc.
 *
 * Get name and information about current kernel.
 */

int
uname(struct utsname *uts)
{
  enum { WinNT, Win95, Win98, WinUnknown };
  OSVERSIONINFO osver;
  SYSTEM_INFO sysinfo;
  DWORD sLength;
  DWORD os = WinUnknown;

  memset(uts, 0, sizeof(*uts));

  osver.dwOSVersionInfoSize = sizeof (osver);
  GetVersionEx (&osver);
  GetSystemInfo (&sysinfo);

  switch (osver.dwPlatformId)
    {
    case VER_PLATFORM_WIN32_NT: /* NT, Windows 2000 or Windows XP */
      if (osver.dwMajorVersion == 4)
        strcpy (uts->sysname, "Windows NT4x"); /* NT4x */
      else if (osver.dwMajorVersion <= 3)
        strcpy (uts->sysname, "Windows NT3x"); /* NT3x */
      else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion  < 1)
        strcpy (uts->sysname, "Windows 2000"); /* 2k */
      else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 2)
        strcpy (uts->sysname, "Windows 2003"); /* 2003 */
      else if (osver.dwMajorVersion == 5 && osver.dwMinorVersion == 1)
        strcpy (uts->sysname, "Windows XP");   /* XP */
      os = WinNT;
      break;

    case VER_PLATFORM_WIN32_WINDOWS: /* Win95, Win98 or WinME */
      if ((osver.dwMajorVersion > 4) || 
          ((osver.dwMajorVersion == 4) && (osver.dwMinorVersion > 0)))
        {
	  if (osver.dwMinorVersion >= 90)
	    strcpy (uts->sysname, "Windows ME"); /* ME */
	  else
	    strcpy (uts->sysname, "Windows 98"); /* 98 */
          os = Win98;
        }
      else
        {
          strcpy (uts->sysname, "Windows 95"); /* 95 */
          os = Win95;
        }
      break;

    case VER_PLATFORM_WIN32s: /* Windows 3.x */
      strcpy (uts->sysname, "Windows");
      break;
    }

  sprintf (uts->version, "%ld.%02ld", 
           osver.dwMajorVersion, osver.dwMinorVersion);

  if (osver.szCSDVersion[0] != '\0' &&
      (strlen (osver.szCSDVersion) + strlen (uts->version) + 1) < 
      sizeof (uts->version))
    {
      strcat (uts->version, " ");
      strcat (uts->version, osver.szCSDVersion);
    }

  sprintf (uts->release, "build %ld", osver.dwBuildNumber & 0xFFFF);

  switch (sysinfo.wProcessorArchitecture)
    {
    case PROCESSOR_ARCHITECTURE_PPC:
      strcpy (uts->machine, "ppc");
      break;
    case PROCESSOR_ARCHITECTURE_ALPHA:
      strcpy (uts->machine, "alpha");
      break;
    case PROCESSOR_ARCHITECTURE_MIPS:
      strcpy (uts->machine, "mips");
      break;
    case PROCESSOR_ARCHITECTURE_INTEL:
      /* 
       * dwProcessorType is only valid in Win95 and Win98 and WinME
       * wProcessorLevel is only valid in WinNT 
       */
      switch (os)
        {
        case Win95:
        case Win98:
          switch (sysinfo.dwProcessorType)
            {
            case PROCESSOR_INTEL_386:
            case PROCESSOR_INTEL_486:
            case PROCESSOR_INTEL_PENTIUM:
              sprintf (uts->machine, "i%ld", sysinfo.dwProcessorType);
              break;
            default:
              strcpy (uts->machine, "i386");
              break;
          }
          break;
        case WinNT:
          switch(sysinfo.wProcessorArchitecture)
            {
            case PROCESSOR_ARCHITECTURE_INTEL:
              sprintf (uts->machine, "x86(%d)", sysinfo.wProcessorLevel);
              break;
            case PROCESSOR_ARCHITECTURE_IA64:
              sprintf (uts->machine, "ia64(%d)", sysinfo.wProcessorLevel);
              break;
#ifdef PROCESSOR_ARCHITECTURE_AMD64
            case PROCESSOR_ARCHITECTURE_AMD64:
              sprintf (uts->machine, "x86_64(%d)", sysinfo.wProcessorLevel);
              break;
#endif
            default:
              sprintf (uts->machine, "unknown(%d)", sysinfo.wProcessorLevel);
              break;
            }
          break;
        default:
          strcpy (uts->machine, "unknown");
          break;
      }
      break;
    default:
      strcpy (uts->machine, "unknown");
      break;
  }
  
  sLength = sizeof (uts->nodename) - 1;
  GetComputerName (uts->nodename, &sLength);
  return 0;
}

