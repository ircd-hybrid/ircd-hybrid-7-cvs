/* $Id: rehash.c,v 1.1 2005/09/19 10:36:44 adx Exp $ */

#include <windows.h>
#include "setup.h"

#define WM_REHASH  (WM_USER + 0x100)
#define WM_REMOTD  (WM_USER + 0x101)

int WINAPI
WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
        LPSTR lpCmdLine, int nCmdShow)
{
  HWND wndhandle = FindWindow(PACKAGE_NAME, NULL);

  if (!wndhandle)
  {
    MessageBox(NULL, "ircd-hybrid is not running", NULL, MB_OK | MB_ICONERROR);
    return 1;
  }

  PostMessage(wndhandle, WM_REHASH, 0, 0);
}
