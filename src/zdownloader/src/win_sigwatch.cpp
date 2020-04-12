#include "win_sigwatch.h"
#include <meta_object_ext.h>

win_sigwatch * win_sigwatch::myobj = nullptr;

win_sigwatch::win_sigwatch(QObject *parent)
  :QObject(parent)
{
  win_sigwatch::myobj = this;
}

void win_sigwatch::watch_for_signals()
{
  SetConsoleCtrlHandler(&win_sigwatch::CtrlHandler, TRUE);
}

BOOL WINAPI win_sigwatch::CtrlHandler(DWORD fdwCtrlType)
{
  switch (fdwCtrlType)
  {
    // Handle the CTRL-C signal.
    case CTRL_C_EVENT:
    case CTRL_CLOSE_EVENT:
    case CTRL_LOGOFF_EVENT:
    case CTRL_SHUTDOWN_EVENT:
        meta_object_ext::invoke_async(myobj, &win_sigwatch::signal_received);
        Sleep(10000);
        return TRUE;

    default:
        return FALSE;
  }
}
