#include "stdafx.h"
#include "wininet.h"
#include "Win32Util.h"
#include "shlwapi.h"
#include "resource.h"
#include "TeXInstall.h"
#include <ctype.h>

#include <accctrl.h>
#include <aclapi.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

// The System Type, global, initialized once
int g_iSystemType;

// OS version, various dll versions
CString g_sOsVersion;
CString g_sDllVersion;

//////////////////////////////////////////////////////////////////////
// Various System Primitives
//////////////////////////////////////////////////////////////////////

//
// Last error message in a MessageBox.
//
void Win32Error(const char *caller)
{
    LPTSTR lpMsgBuf;
    DWORD errCode;
    if (errCode = GetLastError()) {
	if (FormatMessage(
			  FORMAT_MESSAGE_ALLOCATE_BUFFER 
			  | FORMAT_MESSAGE_FROM_SYSTEM
			  | FORMAT_MESSAGE_IGNORE_INSERTS,
			  NULL,
			  errCode,
			  MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), /* Default language */
			  (LPTSTR) &lpMsgBuf,
			  0,
			  NULL 
			  )) {
	    /* Display the string. */
	    MessageBox( NULL,lpMsgBuf, caller, MB_OK|MB_ICONINFORMATION );
	    /* Free the buffer. */
	    LocalFree( lpMsgBuf );
	}
	else {
	    char szBuf[24];
	    wsprintf(szBuf, "%d", errCode);
	    MessageBox( NULL, szBuf, caller, MB_OK|MB_ICONINFORMATION );
	}
    }
}

//
// Concat 2 paths and polish a bit
//
CString ConcatPath(const CString &p1, const CString &p2, char sep)
{
    CString p, q;

    q = p2;
    q.TrimLeft("/\\");

    if (p1.IsEmpty()) {
        p = q;
    }
    else if (p1.GetAt(p1.GetLength() - 1) == '\\'
             || p1.GetAt(p1.GetLength() - 1) == '/') {
	p = p1 + q;
    }
    else {
	p = (p1 + "\\") + q;
    }
    if (sep == '/') {
        p.Replace('\\', '/');
    }
    else {
        p.Replace('/', '\\');
    }
    return p;
}

//
// Close handle and check that everything was ok.
//
inline bool CheckedCloseHandle(HANDLE h)
{
    bool ret = false;

    ret = CloseHandle(h);

    if ( ret == false ) {
	Win32Error("CloseHandle");
    }

    return(ret);
}


inline void
CloseHandleAndClear (HANDLE &h)
{
    if (h != INVALID_HANDLE_VALUE) {
        CheckedCloseHandle (h);
        h = INVALID_HANDLE_VALUE;
    }
}

//
// Test for existing path.
// Should would restrict to non-directories ?
//
inline bool FileExists(const CString &path)
{
    return (GetFileAttributes(path) != -1);
}

inline bool DirectoryExists(const CString &path)
{
    DWORD fa = GetFileAttributes(path);
    return (fa != -1 && ((fa & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY));
}


bool GetParentDirectory(CString &path)
{
    int limit = 0, loc = 0;
    bool bRet = true;

    path.Replace('/', '\\');

    ASSERT(! path.IsEmpty());

    if (path[0] == TEXT('\\') && path[1] == TEXT('\\')) {
	// UNC name
	limit = path.Find(TEXT('\\'), 2);
	if (limit == -1) {
	    // Malformed path, change nothing, return failure.
	    return false;
	}
	limit = path.Find(TEXT('\\'), limit+1);
	if (limit == -1) {
	    // Root of mount point, wrong
	    return false;
	}
        if (limit < path.GetLength())
            // We want to keep the trailing '\\'
	  {
	    //            limit++;
	  }
        else
            // mount point, returns it unchanged
            return true;
    }
    else if (path[1] == TEXT(':') && isalnum(path[0])) {
        limit = (path[2] == '\\' ? 2 : 1);
    }
    else {
        limit = 0;
    }

    // If already ends with a \\, then remove it before doing anything
    if (limit < path.GetLength() - 1
        && path[path.GetLength() - 1] == '\\')
        path = path.Left(path.GetLength() - 1);

    loc = path.ReverseFind(TEXT('\\'));
    fprintf(stderr, "path = %s, loc = %d, limit = %d\n", (LPCTSTR)path, loc, limit);
    if (loc == -1) {
	bRet = false;
    }
    else if (loc >= limit) {
	// returned path will not end with a '\\'
	path = path.Left(loc);
	bRet = true;
    }
    else {
      // loc < limit, returns itself
      bRet = true;
    }

    if (path[path.GetLength() - 1] != '\\')
      path = path + "\\";
    
    return bRet;
}

//
// Test if the current user has administrator rights
//
bool IsAdmin ()
{
    // Taken from Microsoft KB Q118626
    HANDLE access_token_handle = NULL;
    BYTE buffer[ 1024 ];
    PTOKEN_GROUPS token_groups_p = (PTOKEN_GROUPS) buffer;
    DWORD buffer_size = 0;
    bool success = false;

    if (OpenThreadToken(GetCurrentThread(),
			TOKEN_QUERY,
			TRUE,
			&access_token_handle) == FALSE) {
	if (GetLastError() != ERROR_NO_TOKEN) {
	    return(false);
	}
	// retry against process token if no thread token exists
	if (OpenProcessToken(GetCurrentProcess(),
			     TOKEN_QUERY,
			     &access_token_handle) == FALSE) {
	    return(false);
	}
    }

    success = GetTokenInformation(access_token_handle,
				  TokenGroups,
				  buffer,
				  1024,
				  &buffer_size);

    CheckedCloseHandle(access_token_handle);
    access_token_handle = NULL;

    if (success == FALSE) {
	return(false);
    }

    DWORD dwSubAuthority1 = DOMAIN_ALIAS_RID_POWER_USERS;
    SID_IDENTIFIER_AUTHORITY nt_authority_sid = SECURITY_NT_AUTHORITY;
    PSID administrators_sid_p = NULL;
    success = false;
    while (dwSubAuthority1 && !success) {
        if (AllocateAndInitializeSid(&nt_authority_sid,
                                     2,
                                     SECURITY_BUILTIN_DOMAIN_RID,
                                     dwSubAuthority1,
                                     0,
                                     0,
                                     0,
                                     0,
                                     0,
                                     0,
                                     &administrators_sid_p) == TRUE) {
            // assume that we don't find the SID.
            UINT x = 0;
            for( x = 0; x < token_groups_p->GroupCount; x++ ) {
                if (EqualSid(administrators_sid_p, token_groups_p->Groups[x].Sid) != FALSE) {
                    success = true;
                    break;
                }
            }
        }
        FreeSid(administrators_sid_p);
        if (! success) {
            switch (dwSubAuthority1) {
            case DOMAIN_ALIAS_RID_POWER_USERS:
                dwSubAuthority1 = DOMAIN_ALIAS_RID_ADMINS;
                break;
            case DOMAIN_ALIAS_RID_ADMINS:
                dwSubAuthority1 = 0;
                break;
            default:
                dwSubAuthority1 = 0;
                break;
            }
        }
    }
    return(success);
}

//
// Retrieves the directory from where the current module
// was run.
//
void GetExecutableDirectory(CString& module_directory_name, 
			    CString& module_filename,
			    DWORD &module_filesize)
{
    TCHAR path[4096];
    ZeroMemory(path, sizeof(path));
    
    module_directory_name.Empty(); // Always ends with a '\\'
    module_filename.Empty();
    module_filesize = 0;
    if (GetModuleFileName(NULL, path, DIMENSION_OF(path)) != 0) {
        HANDLE hFile = ::CreateFile((LPCTSTR)path, GENERIC_READ, FILE_SHARE_READ, NULL,
                                    OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
        if (hFile != INVALID_HANDLE_VALUE) {
            module_filesize = ::GetFileSize(hFile, NULL);
            CloseHandle(hFile);
        }
        module_filename = _tcsrchr(path, TEXT('\\')) + 1;
        *( _tcsrchr(path, TEXT('\\')) + 1) = 0x00;
        module_directory_name = path;
    }
    else {
	module_directory_name = "\\";
    }
}

//
// Opens a file mapping on the `name' file and
// applies the `processor' function to the mapping
// using the user-supplied `arg' argument.
//
BOOL FileMapAndProcess(const TCHAR *name,
		       PROCESSOR_FUNCTION processor,
		       void *arg)
{
    HANDLE hFile;
    HANDLE hMapping;
    DWORD dwSizeLo;
    DWORD dwSizeHi;
    BOOL ret = TRUE;
    void *p;

    hFile = CreateFile(name, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
		       FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
	Win32Error(name);
	ret = FALSE;
	goto exit2;
    }

    dwSizeLo = GetFileSize(hFile, &dwSizeHi);
    if (dwSizeLo == -1) {
	Win32Error(name);
	ret = FALSE;
	goto exit1;
    }

    if (dwSizeHi) {
	::MessageBox(NULL, "Can't process file bigger than 2Gb.", "FileMap", MB_OK);
	ret = FALSE;
	goto exit1;
    }

    // CreateFileMapping barfs on zero length files
    if (dwSizeLo == 0) {
	static const char c = '\0';
	processor(&c, 0, name, arg);
	ret = TRUE;
	goto exit1;
    }
    else {
	hMapping = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, 0, NULL);
	if (hMapping == NULL) {
	    Win32Error(name);
	    ret = FALSE;
	    goto exit1;
	}
	p = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	if (p == NULL) {
	    Win32Error(name);
	    ret = FALSE;
	    goto exit0;
	}
    
	processor(p, dwSizeLo, name, arg); 

	UnmapViewOfFile(p);

    exit0:
	CloseHandle(hMapping);
    }

 exit1:
    CloseHandle(hFile);

 exit2:
    return ret;
}

//
// Split a CString into a CStringArray using provided separators
//
void SplitString(const CString &s, LPCTSTR sep, CStringArray &sa)
{
    int size = sa.GetUpperBound();

    if (s.IsEmpty()) {
	sa.SetAtGrow(++size, (LPCTSTR)s);
	return;
    }
    else {
	CString temp(s);
	char *p = temp.GetBuffer(temp.GetLength()+1);
	
	p = strtok(p, sep);
	while (p) {
	    sa.SetAtGrow(++size, p);
	    p = strtok(NULL, sep);
	}
	// No need to ReleaseBuffer()
    }
}

//
// Retrieves the DLL version number
//
DWORD GetDllVersion(LPCTSTR lpszDllName, bool bTrace)
{
    HINSTANCE hinstDll;
    DWORD dwVersion = 0;

    hinstDll = LoadLibrary(lpszDllName);
	
    if(hinstDll) {
	DLLGETVERSIONPROC pDllGetVersion;
	pDllGetVersion = (DLLGETVERSIONPROC) GetProcAddress(hinstDll, "DllGetVersion");

	// Because some DLLs may not implement this function, you
	// must test for it explicitly. Depending on the particular 
	// DLL, the lack of a DllGetVersion function may
	//	be a useful indicator of the version.

	if(pDllGetVersion) {
	    DLLVERSIONINFO dvi;
	    HRESULT hr;

	    ZeroMemory(&dvi, sizeof(dvi));
	    dvi.cbSize = sizeof(dvi);

	    hr = (*pDllGetVersion)(&dvi);

	    if(SUCCEEDED(hr)) {
		dwVersion = PACKVERSION(dvi.dwMajorVersion, dvi.dwMinorVersion);

                if (bTrace) {
                    CString sDllVersion;
                    sDllVersion.Format("%s: version %d.%d.%d\n",
                                   lpszDllName, dvi.dwMajorVersion, 
                                       dvi.dwMinorVersion, dvi.dwBuildNumber);
                    g_sDllVersion = g_sDllVersion + sDllVersion;
                }
            }
	}
	
	FreeLibrary(hinstDll);
    }
    return dwVersion;
}

//
// Browse for a folder.
//

char szStartDir[_MAX_PATH];

int CALLBACK BrowseCallbackProc(HWND hwnd,UINT uMsg,LPARAM lp, LPARAM pData) 
{
    TCHAR szDir[MAX_PATH];
  
    switch(uMsg) {
    case BFFM_INITIALIZED: {
	// WParam is TRUE since you are passing a path.
	// It would be FALSE if you were passing a pidl.
	SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)szStartDir);
	break;
    }
    case BFFM_SELCHANGED: {
	// Set the status window to the currently selected path.
	if (SHGetPathFromIDList((LPITEMIDLIST) lp ,szDir)) {
	    SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)szDir);
	}
	break;
    }
    default:
	break;
    }
    return 0;
}

CString BrowseForFolder(HWND hWnd, LPCSTR lpszTitle, UINT nFlags, const CString &sStartDir)
{
    // We're going to use the shell to display a 
    // "Choose Directory" dialog box for the user.
  
    CString strResult = "";
  
    LPMALLOC lpMalloc;	// pointer to IMalloc

    if (::SHGetMalloc(&lpMalloc) != NOERROR)
	return strResult; // failed to get allocator

    char szDisplayName[_MAX_PATH];
    char szBuffer[_MAX_PATH];

    BROWSEINFO browseInfo;
    browseInfo.hwndOwner = hWnd;
    // set root at Desktop
    browseInfo.pidlRoot = NULL; 
    browseInfo.pszDisplayName = szDisplayName;
    browseInfo.lpszTitle = lpszTitle;	// passed in
    browseInfo.ulFlags = nFlags;   // also passed in
    browseInfo.lpfn = BrowseCallbackProc;      // not used
    browseInfo.lParam = 0;	// not used   

    LPITEMIDLIST lpItemIDList;
	
    // Initialize start directory
    strcpy(szStartDir, (LPCTSTR)sStartDir);

    if ((lpItemIDList = ::SHBrowseForFolder(&browseInfo)) != NULL) {

	// Get the path of the selected folder from the
	// item ID list.
	if (::SHGetPathFromIDList(lpItemIDList, szBuffer)) {
	    // At this point, szBuffer contains the path 
	    // the user chose.
	    if (szBuffer[0] == '\0') {
		// SHGetPathFromIDList failed, or
		// SHBrowseForFolder failed.
                CString sMsg;
                sMsg.LoadString(IDS_FAILED_GET_DIRECTORY);
		MessageBox(NULL, (LPCTSTR)sMsg, NULL, MB_ICONSTOP|MB_OK);
		return strResult;
	    }
     
	    // We have a path in szBuffer!
	    // Return it.
	    strResult = szBuffer;
	    return strResult;
	}
	else {
	    // The thing referred to by lpItemIDList 
	    // might not have been a file system object.
	    // For whatever reason, SHGetPathFromIDList
	    // didn't work!
            CString sMsg;
            sMsg.LoadString(IDS_FAILED_GET_DIRECTORY);
            MessageBox(NULL, (LPCTSTR)sMsg, NULL, MB_ICONSTOP|MB_OK);
            //	    AfxMessageBox(IDS_FAILED_GET_DIRECTORY, MB_ICONSTOP|MB_OK);
	    return strResult; // strResult is empty 
	}

	lpMalloc->Free(lpItemIDList);
	lpMalloc->Release();	  
    }

    // If we made it this far, SHBrowseForFolder failed.
    return strResult;
}

/*
  CreateDirectoryPath
  Create a directory path (borrowed from GNU File Utilities).
*/

bool CreateDirectoryPath (const char *path)
{
#define IS_DIR_SEP(c) (c == '\\')
#define S_ISDIR(m) (((m) & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
    char *dirpath = _strdup(path);
    if (! dirpath)
	return false;

    bool ret = true;

    // No more slashes
    for (char *p = dirpath; p && *p; p++)
	*p = (*p == '/' ? '\\' : *p);

    DWORD fa;
    if ((fa = GetFileAttributes(dirpath)) == -1) {
	struct ptr_list {
	    char *dirname_end;
	    struct ptr_list *next;
	} * leading_dirs = 0;
	char * slash = dirpath;
	if (slash[0] && slash[1] == ':')
	    slash += 2;
	else if (IS_DIR_SEP(slash[0]) && slash[1] == slash[0]) {
	    // UNC file name
	    slash += 2;
	    while (*slash && !IS_DIR_SEP(*slash))
		slash++;
	    if (IS_DIR_SEP(*slash)) {
		slash++;
		while (*slash && ! IS_DIR_SEP(*slash))
		    slash++;
	    }
	}
	while (IS_DIR_SEP(*slash))
	    slash++;
	char * cp;
	while ((cp = strchr(slash, '/')) || (cp = strchr(slash, '\\'))) {
	    slash = cp;
	    *slash = '\0';
	    if ((fa = GetFileAttributes(dirpath)) == -1) {
		if (CreateDirectory(dirpath, NULL) == 0) {
		    ret = false;
		    goto done;
		}
	    }
	    else if (! S_ISDIR(fa)){
		ret = true;
		goto done;
	    }
	    *slash++ = '\\';
	    while (IS_DIR_SEP(*slash))
		slash++;
	}
	if (CreateDirectory(dirpath, NULL) == 0) {
	    ret = false;
	    goto done;
	}
    }
    else {
	if (! S_ISDIR(fa)) {
	    ret = false;
	    goto done;
	}
    }
 done:
    free(dirpath);
    return ret;
#undef S_ISDIR
}


int GetSystemType()
{
    OSVERSIONINFOEX osvi;
    BOOL bOsVersionInfoEx;
    int result = -1;

    /* Try calling GetVersionEx using the OSVERSIONINFOEX structure,
       which is supported on Windows NT versions 5.0 and later.
       If that fails, try using the OSVERSIONINFO structure,
       which is supported on earlier versions of Windows and Windows NT */

    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEX));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

    if (! (bOsVersionInfoEx = GetVersionEx ( (OSVERSIONINFO *) &osvi) ) ) {
        /* If OSVERSIONINFOEX doesn't work, try OSVERSIONINFO. */
        osvi.dwOSVersionInfoSize = sizeof (OSVERSIONINFO);
        if (! GetVersionEx ( (OSVERSIONINFO *) &osvi) ) {
            // fprintf(stderr, "GetVersion() failed \n");
            MessageBox(NULL, "GetVersion() failed.", NULL, MB_APPLMODAL | MB_ICONHAND | MB_OK);
            return -1;
        }
    }
  
    switch (osvi.dwPlatformId) {
    case VER_PLATFORM_WIN32_NT:
        if (osvi.dwMajorVersion == 3)
            result = WINNT3;
        else if (osvi.dwMajorVersion == 4)
            result = WINNT4;
        else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion == 0)
            result = WIN2K;
        else if (osvi.dwMajorVersion == 5 && osvi.dwMinorVersion > 0)
            result = WHISTLER;
        break;
    case VER_PLATFORM_WIN32_WINDOWS:
        if ((osvi.dwMajorVersion > 4) || 
            ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 10)))
            result = WINME;
        else if ((osvi.dwMajorVersion == 4) && (osvi.dwMinorVersion > 0)) {
            if (osvi.szCSDVersion[1] == 'A')
                result = WIN98SE;
            else
                result = WIN98;
        }
        else {
            if (osvi.szCSDVersion[1] == 'C')
                result = WIN95OSR2;
            else
                result = WIN95;
        }

        break;
    case VER_PLATFORM_WIN32s:
        result = WIN32s;
        break;
    }

    CString sTmp;
    sTmp.Format("Windows version %d.%d (Build %d)\r\n",
                        osvi.dwMajorVersion, 
                        osvi.dwMinorVersion, 
                        osvi.dwBuildNumber & 0xFFFF);
    g_sOsVersion = sTmp;
    if (bOsVersionInfoEx) {
        sTmp.Format("Service Pack %d.%d\r\n", 
                    osvi.wServicePackMajor, 
                    osvi.wServicePackMinor);
        g_sOsVersion = g_sOsVersion + sTmp;
    }
    else {
        sTmp.Format("%s\r\n", osvi.szCSDVersion);
        g_sOsVersion = g_sOsVersion + sTmp;
    }
    
    sTmp.Format("Identified by TeXSetup as %d.\r\n", result);
    g_sOsVersion = g_sOsVersion + sTmp;
        
    // FIXME : trace dll versions.
    // g_sDllVersion.Format();

    return result;
}

#if 0
static BOOL IsInternetConnected (void)
{
    int nCheck = AfxSocketInit();
    CSocket m_Server;
    HKEY hKey;
    DWORD dwDial, dwDialType = REG_DWORD, dwDialSize = 4;
    DWORD dwNew = 0;
    BOOL bResult = true;
    
    if ( RegOpenKeyEx ( HKEY_CURRENT_USER,
                        "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
                        0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS)
        ; // We cannot find the key. Handle this situation or just continue
    
    if ( RegQueryValueEx( hKey, "EnableAutodial", NULL, &dwDialType,
                          (BYTE *) &dwDial, &dwDialSize ) != ERROR_SUCCESS )
        ; // We cannot find the value. Handle it.
    
    if ( dwDial ) { // We need to change the value, in order to make
        // a dialup window not to show up.
        
        if ( (nCheck = RegSetValueEx( hKey, "EnableAutodial", NULL,
                                      dwDialType, (BYTE *) &dwNew, dwDialSize )) != ERROR_SUCCESS)
            ; // Failed? We shouldn't get here. You decide how to handle it
    }
    
    
    if ( !m_Server.Create() ) {                                                       
        // m_sError = _T( "Unable to create the socket." );
        bResult = false;
    }   
    
    // You can use www.microsoft.com in order to check whether DNS is available
    // or numeric IP otherwise 
    else if ( !m_Server.Connect( "www.microsoft.com", 80 ) ) {     //  207.46.130.150
        // m_sError = _T( "Unable to connect to server" );        
        m_Server.Close();
        bResult = false;
    }
    
    
    if ( dwDial ) {
        if ( (nCheck = RegSetValueEx( hKey, "EnableAutodial", NULL,
                                      dwDialType, (BYTE *) &dwDial, dwDialSize )) != ERROR_SUCCESS)
            ; // Failed? We shouldn't get it. You decide how to handle this.
    }
    
    RegCloseKey( hKey );
    return ( bResult );
}

static DWORD WINAPI ConnectThreadProc(LPVOID lpResult)
{
    BOOL* lpbResult = (BOOL*) lpResult;
    
    *lpbResult = IsInternetConnected();
    return 0;		
}

BOOL mfIsInternetConnected()
{
    BOOL bResult = FALSE;
    DWORD dwThreadId = 0;
    
    BOOL bInternetConnected = FALSE;
    HANDLE hConThread = CreateThread(NULL, 0, ConnectThreadProc, &bInternetConnected, 0, &dwThreadId); 
    
    if (WaitForSingleObject(hConThread, 5000) == WAIT_TIMEOUT) {
        TerminateThread(hConThread, 0);
    }
    return (bInternetConnected);
}
#endif

#if 0
BOOL Win32InternetConnect()
{
    // Wininet by itself does not implement the API for getting version info !
    if (GetDllVersion("shdocvw.dll") < PACKVERSION(4,71)) {
        AfxMessageBox("Can't ensure that you have an active Internet connection.\nTake time to establish one if you can.\nYou  should upgrade your windows to IE4.0 or later.\n", MB_OK | MB_ICONEXCLAMATION);
	return FALSE;
    }
    else {
        HINSTANCE hWinInetDll;
	BOOL bRet = FALSE;

        if ((hWinInetDll = LoadLibrary("wininet.dll")) != 0) {

            typedef	BOOL (__stdcall * pInternetAutodialFNPTR)(DWORD, HWND);
            typedef BOOL (__stdcall * pInternetGetConnectedStateFNPTR)(LPDWORD, DWORD);
	
            pInternetGetConnectedStateFNPTR pInternetGetConnectedState = reinterpret_cast<pInternetGetConnectedStateFNPTR>(GetProcAddress(hWinInetDll, "InternetGetConnectedState"));
            pInternetAutodialFNPTR pInternetAutodial = reinterpret_cast<pInternetAutodialFNPTR>(GetProcAddress(hWinInetDll, "InternetAutodial"));
    
            if (pInternetGetConnectedState != NULL && pInternetAutodial != NULL) {
    
                DWORD dwConnectionTypes = INTERNET_CONNECTION_LAN |
                    INTERNET_CONNECTION_MODEM |
                    INTERNET_CONNECTION_PROXY;
                if (!pInternetGetConnectedState(&dwConnectionTypes, 0)) {
                    AfxMessageBox("I was given the '--net-download' option\nso I will try to establish an Internet connection",
                                  MB_ICONINFORMATION | MB_OK);
                    pInternetAutodial(INTERNET_AUTODIAL_FORCE_UNATTENDED,0);
                }
                bRet = TRUE;
            }
            FreeLibrary(hWinInetDll);
	}
	return bRet;
    }
    // not reached
    return FALSE;
}
#endif

DWORD AddAceToObjectsSecurityDescriptor (
                                         HANDLE hObject,             // handle to object
                                         SE_OBJECT_TYPE ObjectType,  // type of object
					 TRUSTEE *pTrustee, 	     // the trustee
                                         DWORD dwAccessRights,       // access mask for new ACE
                                         ACCESS_MODE AccessMode,     // type of ACE
                                         DWORD dwInheritance         // inheritance flags for new ACE
                                         ) 
{
    DWORD dwRes;
    PACL pOldDACL = NULL, pNewDACL = NULL;
    PSECURITY_DESCRIPTOR pSD = NULL;
    EXPLICIT_ACCESS ea;
    
    if (NULL == hObject) 
        return ERROR_INVALID_PARAMETER;
    
    // Get a pointer to the existing DACL.
    
    dwRes = GetSecurityInfo(hObject, ObjectType, 
                            DACL_SECURITY_INFORMATION,
                            NULL, NULL, &pOldDACL, NULL, &pSD);
    if (ERROR_SUCCESS != dwRes) {
        // printf( "GetSecurityInfo Error %u\n", dwRes );
        goto Cleanup; 
    }  
    
    // Initialize an EXPLICIT_ACCESS structure for the new ACE. 
    
    ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
    ea.grfAccessPermissions = dwAccessRights;
    ea.grfAccessMode = AccessMode;
    ea.grfInheritance= dwInheritance;
    CopyMemory(&(ea.Trustee), pTrustee, sizeof(TRUSTEE));
    
    // Create a new ACL that merges the new ACE
    // into the existing DACL.
    
    dwRes = SetEntriesInAcl(1, &ea, pOldDACL, &pNewDACL);
    if (ERROR_SUCCESS != dwRes)  {
        // printf( "SetEntriesInAcl Error %u\n", dwRes );
        goto Cleanup; 
    }  
    
    // Attach the new ACL as the object's DACL.
    
    dwRes = SetSecurityInfo(hObject, ObjectType, 
                            DACL_SECURITY_INFORMATION,
                            NULL, NULL, pNewDACL, NULL);
    if (ERROR_SUCCESS != dwRes)  {
        // printf( "SetSecurityInfo Error %u\n", dwRes );
        goto Cleanup; 
    }  
    
 Cleanup:

    if(pSD != NULL) 
        LocalFree((HLOCAL) pSD); 
    if(pNewDACL != NULL) 
        LocalFree((HLOCAL) pNewDACL); 
    
    return dwRes;
}

BOOL GrantPermissions(const char *sPath, const char *sUser, const char *sPerm, 
		      BOOL bRec, HANDLE  hLog)
{
    DWORD dwRes;

 again:    
    HANDLE hDir = CreateFile((LPCTSTR)sPath,
                             STANDARD_RIGHTS_ALL,
                             0,
                             NULL,
                             OPEN_EXISTING,
                             FILE_FLAG_BACKUP_SEMANTICS,
                             NULL);
    if (hDir == INVALID_HANDLE_VALUE) {
        // FIXME: should display a MessageBox()
        // if file sharing error.
        if (GetLastError() == ERROR_SHARING_VIOLATION) {
            int nRet = AfxMessageBox("Please, close any window that could prevent\nexclusive access to your\ndestination directory and try again.", MB_ICONHAND | MB_RETRYCANCEL);
            if (nRet == IDRETRY)
                goto again;
        }
        // In any other case
        return FALSE;
    }
    
    ACCESS_MASK dwAccessRead = FILE_GENERIC_READ | FILE_EXECUTE;
    ACCESS_MASK dwAccessFull = STANDARD_RIGHTS_ALL |
        FILE_READ_DATA |
        FILE_WRITE_DATA |
        FILE_APPEND_DATA |
        FILE_READ_EA |
        FILE_WRITE_EA |
        FILE_EXECUTE |
        FILE_DELETE_CHILD |
        FILE_READ_ATTRIBUTES |
        FILE_WRITE_ATTRIBUTES;
    ACCESS_MASK dwAccess;
    TRUSTEE theTrustee;
    PSID pEveryoneSID = NULL;
    PSID pAdminSID = NULL;
    SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
    SID_IDENTIFIER_AUTHORITY SIDAuthNT = SECURITY_NT_AUTHORITY;
    
    dwAccess =  (_stricmp(sPerm, "full") == 0 ? dwAccessFull : dwAccessRead);
    
    ZeroMemory(&theTrustee, sizeof(TRUSTEE));
    
    if (! _stricmp(sUser, "Everyone")) {
        // Create a well-known SID for the Everyone group.
        dwRes = AllocateAndInitializeSid( &SIDAuthWorld, 1,
                                          SECURITY_WORLD_RID,
                                          0, 0, 0, 0, 0, 0, 0,
                                          &pEveryoneSID);
        if (! dwRes) {
            // printf( "AllocateAndInitializeSid Error %u\n", GetLastError() );
            goto CleanUp;
        }
        theTrustee.TrusteeForm = TRUSTEE_IS_SID;
        theTrustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
        theTrustee.ptstrName = (LPTSTR)pEveryoneSID;
    }
    else {
        theTrustee.TrusteeForm = TRUSTEE_IS_NAME;
        theTrustee.TrusteeType = TRUSTEE_IS_USER;
        theTrustee.ptstrName = (LPTSTR)sUser;
    }
    
    dwRes = AddAceToObjectsSecurityDescriptor(hDir, SE_FILE_OBJECT,
					      &theTrustee,
					      dwAccess,
					      GRANT_ACCESS,
					      SUB_CONTAINERS_AND_OBJECTS_INHERIT);
    
    if (dwRes != ERROR_SUCCESS) {
        // printf("Failed to add ACE to object, error = %ld\n", GetLastError());
        goto CleanUp;
    }
    
    // Create a well-known SID for the BUILTIN\Administrators group.
    dwRes = AllocateAndInitializeSid( &SIDAuthNT, 2,
                                      SECURITY_BUILTIN_DOMAIN_RID,
				      DOMAIN_ALIAS_RID_ADMINS,
				      0, 0, 0, 0, 0, 0,
				      &pAdminSID);
    if (! dwRes) {
        // printf( "AllocateAndInitializeSid Error %u\n", GetLastError() );
        dwRes = AllocateAndInitializeSid( &SIDAuthNT, 2,
                                          SECURITY_BUILTIN_DOMAIN_RID,
                                          DOMAIN_ALIAS_RID_POWER_USERS,
                                          0, 0, 0, 0, 0, 0,
                                          &pAdminSID);
        if (! dwRes)
            goto CleanUp;
    }

    theTrustee.TrusteeForm = TRUSTEE_IS_SID;
    theTrustee.TrusteeType = TRUSTEE_IS_GROUP;
    theTrustee.ptstrName = (LPTSTR)pAdminSID;
    
    dwRes = AddAceToObjectsSecurityDescriptor(hDir, 
					      SE_FILE_OBJECT,
					      &theTrustee,
					      dwAccessFull,
					      GRANT_ACCESS,
					      SUB_CONTAINERS_AND_OBJECTS_INHERIT);

    if (dwRes != ERROR_SUCCESS) {
        // printf("Failed to add ACE to object, error = %ld\n", GetLastError());
        goto CleanUp;
    }

 CleanUp:
    CloseHandle(hDir);

    if (pEveryoneSID)
      FreeSid(pEveryoneSID);
    if (pAdminSID)
      FreeSid(pAdminSID);

    return dwRes == ERROR_SUCCESS;
}

bool CreateTempFile (CString &tempdir, CString &tempfile)
{
    char tempname[_MAX_PATH];
    if (GetTempFileName((LPCTSTR)tempdir, "tex", 0, tempname) == 0) {
        return false;
    }
    if (_unlink(tempname) != 0) {
        return false;
    }
    tempfile = tempname;
    return (true);
}

static BOOL __parse_ymdhms( LPCTSTR time_string,
                            int&    year,
                            int&    month,
                            int&    day,
                            int&    hours,
                            int&    minutes,
                            int&    seconds,
                            TCHAR&  offset_character,
                            int&    offset_hours,
                            int&    offset_minutes )
{

   // Here's a sample ISO8601 date string
   //
   // 1969-07-20T22:56:15-04:00

   // Do a little idiot checking
   if ( time_string == NULL ) {
       // WFCTRACE( TEXT( "time string is NULL!" ) );
       return( FALSE );
   }

   // Start with some believable defaults

   year             = 0;
   month            = 1;
   day              = 1;
   hours            = 0;
   minutes          = 0;
   seconds          = 0;
   offset_character = TEXT( 'Z' );
   offset_hours     = 0;
   offset_minutes   = 0;

   // We were passed a pointer, don't trust it

   try {
      if ( _tcslen( time_string ) < 4 ) {
          // There ain't enough characters to even attempt to parse
          // WFCTRACE( TEXT( "Not enough character to even attempt to parse" ) );
          return( FALSE );
      }
      
      // OK, let's start parsing
      if ( _istdigit( time_string[ 0 ] ) == 0 ) {
          // WFCTRACE( TEXT( "First character is not a digit" ) );
          return( FALSE );
      }

      if ( _istdigit( time_string[ 1 ] ) == 0 ) {
          // WFCTRACE( TEXT( "Second character is not a digit" ) );
          return( FALSE );
      }

      if ( _istdigit( time_string[ 2 ] ) == 0 ) {
          // WFCTRACE( TEXT( "Third character is not a digit" ) );
          return( FALSE );
      }

      if ( _istdigit( time_string[ 3 ] ) == 0 ) {
          // WFCTRACE( TEXT( "Fourth character is not a digit" ) );
          return( FALSE );
      }

      // If we get here, it means we've got 4 good digits
      TCHAR temp_string[ 5 ];

      temp_string[ 0 ] = time_string[ 0 ];
      temp_string[ 1 ] = time_string[ 1 ];
      temp_string[ 2 ] = time_string[ 2 ];
      temp_string[ 3 ] = time_string[ 3 ];
      temp_string[ 4 ] = 0x00;

      year = _ttoi( temp_string );

      DWORD index = 4;

      // index should be pointing here
      //     |
      //     v
      // 1969-07-20T22:56:15-04:00

      if ( time_string[ index ] == 0x00 ) {
          // We're at the end of the string
          return( TRUE );
      }
      
#define IS_DATE_SEPARATOR(c) ((c) == TEXT('-') || (c) == TEXT('/'))

      if ( !IS_DATE_SEPARATOR(time_string[ index ])) {
          // WFCTRACE( TEXT( "The separator between year and month is not -" ) );
          return( FALSE );
      }

      index++;

      // index should be pointing here
      //      |
      //      v
      // 1969-07-20T22:56:15-04:00

      if ( time_string[ index ] == 0x00 ) {
          // We're at the end of the string
          return( TRUE );
      }

      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "First digit of month field isn't a digit at all" ) );
          return( FALSE );
      }

      index++;
      
      // index should be pointing here
      //       |
      //       v
      // 1969-07-20T22:56:15-04:00
      
      // This test is safe because we could be sitting on a NULL
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "Second digit of month field isn't a digit at all" ) );
          return( FALSE );
      }
      
      temp_string[ 0 ] = time_string[ index - 1 ];
      temp_string[ 1 ] = time_string[ index ];
      temp_string[ 2 ] = 0x00;
      
      month = _ttoi( temp_string );
      
      // Do a little idiot proofing
      
      if ( month < 1 || month > 12 ) {
          // WFCTRACE( TEXT( "Funky month" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //        |
      //        v
      // 1969-07-20T22:56:15-04:00
      
      if ( time_string[ index ] == 0x00 ) {
          return( TRUE );
      }
      
      if (! IS_DATE_SEPARATOR(time_string[ index ])) {
          // WFCTRACE( TEXT( "The separator between month and day fields is not -" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //         |
      //         v
      // 1969-07-20T22:56:15-04:00
      
      if ( time_string[ index ] == 0x00 ) {
          // Odd, but not illegal
          return( TRUE );
      }
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "First digit of day field isn't a digit at all" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //          |
      //          v
      // 1969-07-20T22:56:15-04:00
      
      // This test is safe because we could be sitting on a NULL
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "Second digit of month field isn't a digit at all" ) );
          return( FALSE );
      }
      
      temp_string[ 0 ] = time_string[ index - 1 ];
      temp_string[ 1 ] = time_string[ index ];
      
      // We don't need to set temp_string[ 2 ] because we did that at line 158 above
      
      day = _ttoi( temp_string );
      
      // Do a very little bit of error checking
      
      if ( day < 1 || day > 31 ) {
          // WFCTRACE( TEXT( "Funky day" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //           |
      //           v
      // 1969-07-20T22:56:15-04:00
      
      if ( time_string[ index ] == 0x00 ) {
          // Odd, but not illegal
          return( TRUE );
      }
      
      if ( time_string[ index ] != TEXT( 'T' )
           && !isspace(time_string[ index ])) {
          // WFCTRACE( TEXT( "Separator between date and time ain't T" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //            |
      //            v
      // 1969-07-20T22:56:15-04:00
      
      if ( time_string[ index ] == 0x00 ) {
          // Odd, but not illegal
          return( TRUE );
      }
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "First digit of hours field isn't a digit at all" ) );
          return( FALSE );
      }

      index++;

      // index should be pointing here
      //             |
      //             v
      // 1969-07-20T22:56:15-04:00

      // This test is safe because we could be sitting on a NULL

      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "Second digit of month field isn't a digit at all" ) );
          return( FALSE );
      }
      
      temp_string[ 0 ] = time_string[ index - 1 ];
      temp_string[ 1 ] = time_string[ index ];
      
      // We don't need to set temp_string[ 2 ] because we did that at line 158 above
      
      hours = _ttoi( temp_string );
      
      index++;
      
      // index should be pointing here
      //              |
      //              v
      // 1969-07-20T22:56:15-04:00
      
      if ( time_string[ index ] == 0x00 ) {
          // Odd, but not illegal
          return( TRUE );
      }
      
#define IS_TIME_SEPARATOR(c) ((c) == TEXT(':'))

      if (! IS_TIME_SEPARATOR(time_string[ index ])) {
          // WFCTRACE( TEXT( "Separator between hours and minutes ain't :" ) );
          return( FALSE );
      }
      
      index++;

      // index should be pointing here
      //               |
      //               v
      // 1969-07-20T22:56:15-04:00

      if ( time_string[ index ] == 0x00 ) {
          // Odd, but not illegal
          return( TRUE );
      }
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "First digit of time ain't no digit" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //                |
      //                v
      // 1969-07-20T22:56:15-04:00
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "First digit of time ain't no digit" ) );
          return( FALSE );
      }
      
      temp_string[ 0 ] = time_string[ index - 1 ];
      temp_string[ 1 ] = time_string[ index ];
      
      minutes = _ttoi( temp_string );
      
      index++;
      
      // index should be pointing here
      //                 |
      //                 v
      // 1969-07-20T22:56:15-04:00
      
      if ( time_string[ index ] == 0x00 ) {
          return( TRUE );
      }
      
      if (! IS_TIME_SEPARATOR(time_string[ index ])) {
          // WFCTRACE( TEXT( "Separator between minutes and seconds ain't :" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //                  |
      //                  v
      // 1969-07-20T22:56:15-04:00

      if ( time_string[ index ] == 0x00 ) {
          // Odd, but not illegal
          return( TRUE );
      }
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "First digit of seconds ain't no digit" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //                   |
      //                   v
      // 1969-07-20T22:56:15-04:00
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "Second digit of seconds ain't no digit" ) );
          return( FALSE );
      }
      
      temp_string[ 0 ] = time_string[ index - 1 ];
      temp_string[ 1 ] = time_string[ index ];
      
      seconds = _ttoi( temp_string );
      
      index++;
      
      // index should be pointing here
      //                    |
      //                    v
      // 1969-07-20T22:56:15-04:00
      
      if ( time_string[ index ] == 0x00 ||
           time_string[ index ] == TEXT( 'Z' ) ) {
          return( TRUE );
      }
      
      // OK, now is the time when parsing gets interesting
      // The current index may be sitting on a partial second.
      // Since CTime can't deal with partial seconds, we
      // will just skip that part entirely.
      
      if ( time_string[ index ] == TEXT( '.' ) ) {
          index++;
          
          BOOL exit_loop = FALSE;
          
          while( exit_loop == FALSE ) {
              if ( time_string[ index ] == 0x00 ) {
                  return( TRUE );
              }
              
              if ( _istdigit( time_string[ index ] ) == 0 ) {
                  exit_loop = TRUE;
              }
              else {
                  index++;
              }
          }
      }
      
      // If we get here, we should be sitting on a Z, + or -
      
      if ( time_string[ index ] == TEXT( 'Z' ) ) {
          return( TRUE );
      }
      
      if ( time_string[ index ] != TEXT( '+' ) &&
           time_string[ index ] != TEXT( '-' ) ) {
          // WFCTRACE( TEXT( "Time zone designator ain't beginning with + or -" ) );
          return( FALSE );
      }

      offset_character = time_string[ index ];

      index++;

      // index should be pointing here
      //                     |
      //                     v
      // 1969-07-20T22:56:15-04:00

      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "first digit of hours offset ain't a digit" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //                      |
      //                      v
      // 1969-07-20T22:56:15-04:00
      
      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "second digit of hours offset ain't a digit" ) );
          return( FALSE );
      }
      
      temp_string[ 0 ] = time_string[ index - 1 ];
      temp_string[ 1 ] = time_string[ index ];
      
      offset_hours = _ttoi( temp_string );
      
      index++;
      
      // index should be pointing here
      //                       |
      //                       v
      // 1969-07-20T22:56:15-04:00

      if ( time_string[ index ] != TEXT( ':' ) ) {
          // WFCTRACE( TEXT( "Separator between offset hours and minutes ain't a :" ) );
          return( FALSE );
      }
      
      index++;
      
      // index should be pointing here
      //                        |
      //                        v
      // 1969-07-20T22:56:15-04:00

      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "First digit of minutes offset ain't a digit" ) );
         return( FALSE );
      }

      index++;

      // index should be pointing here
      //                        |
      //                        v
      // 1969-07-20T22:56:15-04:00

      if ( _istdigit( time_string[ index ] ) == 0 ) {
          // WFCTRACE( TEXT( "Second digit of minutes offset ain't a digit" ) );
          return( FALSE );
      }

      temp_string[ 0 ] = time_string[ index - 1 ];
      temp_string[ 1 ] = time_string[ index ];

      offset_minutes = _ttoi( temp_string );

      return( TRUE );
   }
   catch( ... )
   {
      return( FALSE );
   }
}

#if 0
BOOL wfc_parse_iso_8601_string( LPCTSTR time_string, CFileTime& the_time )
{
    // WFCLTRACEINIT( TEXT( "wfc_parse_iso_8601_string( CFileTime )" ) );

   CSystemTime system_time;
   the_time.Empty();
   if ( wfc_parse_iso_8601_string( time_string, system_time ) == FALSE ) {
       return( FALSE );
   }
   the_time = system_time;
   return( TRUE );
}
#endif

BOOL wfc_parse_iso_8601_string( LPCTSTR time_string, COleDateTime& the_time )
{
    //   WFCLTRACEINIT( TEXT( "wfc_parse_iso_8601_string( COleDateTime )" ) );
   the_time = COleDateTime( static_cast< time_t >( 0 ) );

   // Do a little idiot checking
   if ( time_string == NULL ) {
       // WFCTRACE( TEXT( "time_string is NULL!" ) );
       return( FALSE );
   }

   int year    = 0;
   int month   = 0;
   int day     = 0;
   int hours   = 0;
   int minutes = 0;
   int seconds = 0;

   TCHAR offset_character = 0;

   int offset_hours   = 0;
   int offset_minutes = 0;

   if ( __parse_ymdhms( time_string, year, month, day, hours, minutes, seconds, 
                        offset_character, offset_hours, offset_minutes ) == FALSE ) {
       // WFCTRACE( TEXT( "Can't parse string" ) );
       return( FALSE );
   }

   the_time = COleDateTime( year, month, day, hours, minutes, seconds );

   if ( offset_character != TEXT( 'Z' ) ) {
       COleDateTimeSpan time_zone_offset( 0, offset_hours, offset_minutes, 0 );
      if ( offset_character == TEXT( '-' ) ) {
          the_time += time_zone_offset;
      }
      else if ( offset_character == TEXT( '+' ) ) {
          the_time -= time_zone_offset;
      }
   }
   return( TRUE );
}

#if 0

BOOL wfc_parse_iso_8601_string( LPCTSTR time_string, CSystemTime& the_time )
{
   WFCLTRACEINIT( TEXT( "wfc_parse_iso_8601_string( CSystemTime )" ) );

   the_time.Empty();

   // Do a little idiot checking

   if ( time_string == NULL )
   {
      WFCTRACE( TEXT( "time_string is NULL!" ) );
      return( FALSE );
   }

   int year    = 0;
   int month   = 0;
   int day     = 0;
   int hours   = 0;
   int minutes = 0;
   int seconds = 0;

   TCHAR offset_character = 0;

   int offset_hours   = 0;
   int offset_minutes = 0;

   if ( __parse_ymdhms( time_string, year, month, day, hours, minutes, seconds, offset_character, offset_hours, offset_minutes ) == FALSE )
   {
      WFCTRACE( TEXT( "Can't parse string" ) );
      return( FALSE );
   }

   CTime ole_time = CTime( year, month, day, hours, minutes, seconds );

   if ( offset_character != TEXT( 'Z' ) )
   {
      CTimeSpan time_zone_offset( 0, offset_hours, offset_minutes, 0 );

      if ( offset_character == TEXT( '-' ) )
      {
         ole_time += time_zone_offset;
      }
      else if ( offset_character == TEXT( '+' ) )
      {
         ole_time -= time_zone_offset;
      }
   }

   the_time = ole_time;

   return( TRUE );
}

#endif

BOOL wfc_parse_iso_8601_string( LPCTSTR time_string, CTime& the_time )
{
    //WFCLTRACEINIT( TEXT( "wfc_parse_iso_8601_string( CTime )" ) );

   the_time = CTime( static_cast< time_t >( 0 ) );

   // Do a little idiot checking
   if ( time_string == NULL ) {
       // WFCTRACE( TEXT( "time_string is NULL!" ) );
       return( FALSE );
   }

   int year    = 0;
   int month   = 0;
   int day     = 0;
   int hours   = 0;
   int minutes = 0;
   int seconds = 0;

   TCHAR offset_character = 0;

   int offset_hours   = 0;
   int offset_minutes = 0;

   if ( __parse_ymdhms( time_string, year, month, day, hours, minutes, seconds, 
                        offset_character, offset_hours, offset_minutes ) == FALSE ) {
       // WFCTRACE( TEXT( "Can't parse string" ) );
       return( FALSE );
   }

   the_time = CTime( year, month, day, hours, minutes, seconds );

   if ( offset_character != TEXT( 'Z' ) ) {
       CTimeSpan time_zone_offset( 0, offset_hours, offset_minutes, 0 );
       
       if ( offset_character == TEXT( '-' ) ) {
           the_time += time_zone_offset;
       }
       else if ( offset_character == TEXT( '+' ) ) {
           the_time -= time_zone_offset;
       }
   }
   return( TRUE );
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

