#ifndef _TEXLIVECMDS_H_
#define _TEXLIVECMDS_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afx.h>

extern CString g_sTempPath,
    g_sDriveRootPath,
    g_sFolderName,
    g_sBinDir,
    g_sTexmfMain,
    g_sVarTexmf,
    g_sSetupw32,
    g_sEditor,
    g_sLastError,
    g_sInternetMethod;
extern bool g_bIsWindowsNT,
    g_bIsAdmin;

extern void Initialize();
extern bool RunProcess(const char *cmd, bool bWait = false, bool bUseSW = false, WORD wShow = SW_SHOWDEFAULT);
bool SelectEditor(CString &sEditor);
bool GetEditorLocation(CString &sEditor);
bool HasInternetConnection();
extern void RunOffCDRom();
extern void RunTeXDocTK();
extern bool __cdecl CleanupEnv(LPVOID pParam = 0);
extern bool __cdecl CleanupCDRom(LPVOID pParam = 0);
extern void FatalError(UINT, ...);
extern CString GetSafePathName(const CString &path, bool bQuoted = true);

#endif /* _TEXLIVECMDS_H_ */

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
