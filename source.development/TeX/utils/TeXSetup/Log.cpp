#include <io.h>
#include <stdio.h>
#include <stdarg.h>
#include <fcntl.h>
#include "Log.h"

LogFile::LogFile(CString &s, CCriticalSection *cs) : lpCS(cs), sLogName(s)
{
    fLog = 0;
    if (!OpenLog("w")) {
        MessageBox(NULL, _strerror("Warning! Can't open log file"), NULL, MB_OK);
        fLog = 0;
    }
}

LogFile::LogFile()
{
    fLog = 0;
    lpCS = 0;
}

void LogFile::LogV (const char *s, va_list args)
{
    if (! lpCS) return;
    CSingleLock singleLock (lpCS, TRUE);
    if (fLog) {
        vfprintf (fLog, s, args);
    }
}

void LogFile::LogV (const int res, va_list args)
{
    if (! lpCS) return;
    CSingleLock singleLock (lpCS, TRUE);
    if (fLog) {
	CString s;
	if (! s.LoadString(res)) {
            fputs("(Resource not found)", fLog);
	}
	else {
            vfprintf (fLog, (LPCTSTR)s, args);
	}
    }
}

void LogFile::Log (const char *s, ...)
{
    va_list argList;
    va_start (argList, s);
    LogV (s, argList);
    va_end (argList);
}

void LogFile::operator() (const char *s, ...)
{
    va_list argList;
    va_start (argList, s);
    LogV (s, argList);
    va_end (argList);
}

void LogFile::operator() (const int res, ...)
{
    va_list argList;
    va_start (argList, res);
    LogV (res, argList);
    va_end (argList);
}

bool LogFile::OpenLog(CString &sName, CCriticalSection *cs)
{
    if (fLog)
        CloseLog();

    sLogName = sName;
    lpCS = cs;

    CSingleLock singleLock (lpCS, TRUE);
  
#if 0
    HANDLE hLog = CreateFile((LPCTSTR)sLogName,
                             GENERIC_READ|GENERIC_WRITE,
                             0,     // share mode
                             NULL,  // security attributes
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             0);
    if (hLog == INVALID_HANDLE_VALUE) {
        AfxMessageBox("Can't open log file", MB_ICONERROR);
        return false;
    }
    int fdLog = _open_osfhandle((intptr_t)hLog, _O_APPEND | _O_TEXT);
    if (fdLog == -1) {
        AfxMessageBox("Can't open log file", MB_ICONERROR);
        CloseHandle(hLog);
        return false;
    }
    fLog = _fdopen(fdLog, "a");
    if (fLog == NULL) {
#else
    if ((fLog =fopen((LPCTSTR)sLogName, "a")) == NULL) {
#endif
        AfxMessageBox(_strerror("Warning! Can't open log file"), MB_ICONERROR);
	return (false);
    }

    return true;
  
}

bool LogFile::OpenLog (char *mode)
{
    CSingleLock singleLock (lpCS, TRUE);
    // return if the log is already open
    if (fLog) return (true);
  
#if 0
    HANDLE hLog = CreateFile((LPCTSTR)sLogName,
                             GENERIC_READ|GENERIC_WRITE,
                             0,     // share mode
                             NULL,  // security attributes
                             OPEN_ALWAYS,
                             FILE_ATTRIBUTE_NORMAL,
                             0);

    if (hLog == INVALID_HANDLE_VALUE) {
        AfxMessageBox("Can't open log file", MB_ICONERROR);
        return false;
    }

    int fdLog = _open_osfhandle((intptr_t)hLog, _O_APPEND | _O_TEXT);
    if (fdLog == -1) {
        AfxMessageBox("Can't open log file", MB_ICONERROR);
        CloseHandle(hLog);
        return false;
    }
    fLog = _fdopen(fdLog, "a");
    if (fLog == NULL) {
#else
    if ((fLog =fopen((LPCTSTR)sLogName, "a")) == NULL) {
#endif
        AfxMessageBox(_strerror("Warning! Can't open log file"), MB_ICONERROR);
	return (false);
    }

    return true;
}

LogFile::~LogFile() { }

void LogFile::EndLog()
{
    Log ("This is the end !\n");
    CloseLog();
}

void LogFile::CloseLog()
{
    if (! lpCS) return;
    CSingleLock (lpCS, TRUE);
    if (fLog) {
        fclose (fLog);
        fLog = 0;
    }
}

HANDLE LogFile::getHandle()
{
    HANDLE hLog = 0;
    if (fLog) hLog = (HANDLE)_get_osfhandle(_fileno(fLog));
    return hLog;
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

