// Log.h: Logging facility.
//
//////////////////////////////////////////////////////////////////////

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <afxwin.h>
#include <afxmt.h>

class LogFile {
private:
    CString sLogName;
    FILE *fLog;
    CCriticalSection *lpCS;
  
public:
    HANDLE getHandle();
    LogFile(CString &s, CCriticalSection *cs = 0);
    LogFile();
    ~LogFile();
    void LogV (const char *s, va_list args);
    void LogV (const int res, va_list args);
    void __cdecl Log (const char *s, ...);
    void __cdecl operator()(const char *s, ...);
    void __cdecl operator()(const int res, ...);
    bool OpenLog(CString &s, CCriticalSection *cs = 0);
    bool OpenLog (char *mode = "a");
    void CloseLog ();
    void EndLog ();
    const CString & GetName() const { return sLogName; }
    //	void FatalError (UINT nMsgID, ...);
};

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

