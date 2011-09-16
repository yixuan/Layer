#include <gtk/gtk.h>
#include <R.h>
#ifdef G_OS_WIN32
#include <windows.h>
#else
#include "R_ext/eventloop.h"
#include <gdk/gdkx.h>
#endif


void
R_gtk_eventHandler(void *userData)
{
  while (gtk_events_pending())
    gtk_main_iteration();
}

#ifdef G_OS_WIN32

/* On Windows, run the GTK+ event loop in a separate thread, synchronizing
   through the Windows event loop on the main thread.
   This currently doesn't handle timed tasks.
   More to come later on an overhaul of the R event loop.
*/

/* should exist win2k/xp and later, but mingw does not have it */
#define HWND_MESSAGE                ((HWND)-3)

#define CD_TIMER_ID 0
#define CD_TIMER_DELAY 50

VOID CALLBACK R_gtk_timer_proc(HWND hwnd, UINT uMsg, UINT_PTR idEvent,
                               DWORD dwTime)
{
  R_gtk_eventHandler(NULL);
}
#endif

void
R_gtk_setEventHandler()
{
#ifndef WIN32
  static InputHandler *h = NULL;
  if(!h)
    {
      if (!GDK_DISPLAY())
        error("GDK display not found - please make sure X11 is running");
      h = addInputHandler(R_InputHandlers, ConnectionNumber(GDK_DISPLAY()),
                          R_gtk_eventHandler, -1);
    }
#else
  /* Create a dummy window for receiving messages */
  LPCTSTR class = "cairoDevice";
  HINSTANCE instance = GetModuleHandle(NULL);
  WNDCLASS wndclass = { 0, DefWindowProc, 0, 0, instance, NULL, 0, 0, NULL,
                        class };
  RegisterClass(&wndclass);
  HWND win = CreateWindow(class, NULL, 0, 1, 1, 1, 1, HWND_MESSAGE,
                          NULL, instance, NULL);

  SetTimer(win, CD_TIMER_ID, CD_TIMER_DELAY, (TIMERPROC)R_gtk_timer_proc);
#endif
}
