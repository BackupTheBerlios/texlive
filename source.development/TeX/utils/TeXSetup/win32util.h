// Win32Util.h: interface to various Win32 functions.
//
//////////////////////////////////////////////////////////////////////

#if !defined(_WIN32_UTIL_H_)
#define _WIN32_UTIL_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if ! defined( DIMENSION_OF )
#define DIMENSION_OF( argument ) ( sizeof( argument ) / sizeof( *( argument ) ) )
#endif

#include <afx.h>

typedef void (* PROCESSOR_FUNCTION)(const void *, size_t, const TCHAR *, void *arg);

extern void Win32Error(const char *caller);
extern CString ConcatPath(const CString &p1, const CString &p2, char sep = '\\');
extern bool CheckedCloseHandle(HANDLE h);
extern void CloseHandleAndClear(HANDLE &h);
extern bool IsAdmin();
extern void GetExecutableDirectory(CString& module_directory_name,
				   CString& module_filename, DWORD &module_filesize);
extern bool GetParentDirectory(CString &);
extern bool FileExists(const CString &path);
extern bool DirectoryExists(const CString &path);
extern bool CreateTempFile (CString &tempdir, CString &tempfile);

extern BOOL FileMapAndProcess(const TCHAR *name, PROCESSOR_FUNCTION processor, void *arg);

extern void SplitString(const CString &, LPCTSTR, CStringArray &);

extern BOOL Win32InternetConnect();

extern BOOL wfc_parse_iso_8601_string(LPCTSTR time_string, CTime& the_time);

// DLL Versions

#define PACKVERSION(major,minor) MAKELONG(minor,major)

extern DWORD dwComctl32Version;
extern DWORD dwShell32Version;
extern DWORD dwShlwapiVersion;

extern DWORD GetDllVersion(LPCTSTR lpszDllName, bool bTrace = false);

extern CString BrowseForFolder(HWND hWnd, LPCSTR lpszTitle, UINT nFlags, const CString &sStartDir);

extern bool CreateDirectoryPath (const char *path);

BOOL GrantPermissions(const char *sPath, const char *sUser, const char *sPerm, 
		      BOOL bRec, HANDLE  hLog);

#define PIPE_SIZE 4096

extern bool StartProcess (const char *command_line,
                          PROCESS_INFORMATION &piProcessInformation,
                          HANDLE *phStdoutRd,
                          const char *directory);

#ifdef WIN32
#define WIN32s		0
#define WIN95		WIN32s	+	1
#define WIN95OSR2	WIN95	+	1
#define WIN98		WIN95OSR2 +	1
#define	WIN98SE		WIN98	+	1
#define	WINME		WIN98SE	+	1
#define WINNT3		WINME	+	1
#define WINNT4		WINNT3	+	1
#define WIN2K		WINNT4	+	1
#define WHISTLER	WIN2K	+	1

extern int g_iSystemType;

/* Makes it easier to determine appropriate code paths: */
#if defined (WIN32)
#define IS_WIN32 TRUE
#else
#define IS_WIN32 FALSE
#endif
#define IS_NT      (g_iSystemType >= WINNT3)
#define IS_WIN32S  (g_iSystemType == WIN32s)
#define IS_WIN95 (g_iSystemType == WIN95 || g_iSystemType == WIN95OSR2 )
#define IS_WIN98 (g_iSystemType == WIN98 || g_iSystemType == WIN98SE)
#endif /* WIN32 */

extern int g_g_iSystemType;
int GetSystemType();

inline bool IsWindowsNT() { return (g_iSystemType >= WINNT3); }
inline bool IsWindowsNT4() { return (g_iSystemType >= WINNT4); }


#endif /* _WIN32_UTIL_H_ */
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

