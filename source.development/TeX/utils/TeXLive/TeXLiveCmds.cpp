#include <stdafx.h>
#include <wininet.h>
#include "Win32Util.h"
#include "KeyValFile.h"
#include "Registry.h"
#include "resource.h"
#include "ActionProgressDlg.h"
#include "TeXLive.h"
#include "TeXLiveCmds.h"

extern "C" {
#include "gs32lib.h"

#include "windll.h"
}

#define TEXLIVE_REGENTRY "SOFTWARE\\TeX Users Group\\TeXLive"
#define TEXLIVE_VERSION "8.0"

extern CTeXLiveApp theApp;

bool g_bIsWindowsNT = false;
bool g_bIsAdmin = false;
bool g_bIE5 = false;

CString g_sTempPath,
    g_sRegKey,
    g_sDriveRootPath,
    g_sFolderName("TeXLive"),
    g_sBinDir,
    g_sTexmfMain,
    g_sVarTexmf,
    g_sSetupw32,
    g_sSupport,
    g_sEditor("bin\\win32\\xemacs.exe -q -f TeXLive-from-cdrom"),
    g_sGhostscript,
    g_sLastError,
    g_sInternetMethod,
    g_sDefaultPath;             // PATH prior changing it
	
const char *g_confFiles[] = {
    "web2c\\texmf.cnf", 			"web2c\\texmf.cnf",
    "web2c\\mktex.cnf", 			"web2c\\mktex.cnf",
    "web2c\\updmap.cfg", 			"web2c\\updmap.cfg",
    "web2c\\fmtutil.cnf", 			"web2c\\fmtutil.cnf",
    "tex\\generic\\config\\language.dat", 	"tex\\generic\\config\\language.dat",
    "dvips\\config\\config.ps", 		"dvips\\config\\config.ps",
    "dvipdfm\\config\\config-win32", 		"dvipdfm\\config\\config",
    "pdftex\\config\\pdftex.cfg", 		"pdftex\\config\\pdftex.cfg",
    "tex4ht\\base\\tex4ht-netpbm-win32.env",	"tex4ht\\base\\tex4ht.env"
};

const char *g_formatFiles[] = {
    "tex.fmt",
    "latex.fmt",
    "pdflatex.fmt",
    "cont-en.efmt"
};

void FatalError (UINT nMsgID, ...)
{
    va_list argList;
    va_start (argList, nMsgID);

    CString fmt;
    fmt.LoadString (nMsgID);

    LPVOID lpMsgBuf;
    FormatMessage (FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_STRING,
                   (const char *) fmt,
                   0,
                   MAKELANGID (LANG_NEUTRAL, SUBLANG_DEFAULT),
                   (LPTSTR) &lpMsgBuf,
                   0,
                   &argList);  
    g_sLastError = (char *) lpMsgBuf;
    LocalFree (lpMsgBuf);
    va_end (argList);

    CWnd *hWnd = AfxGetMainWnd();
    hWnd->MessageBox(g_sLastError, NULL, MB_ICONERROR | MB_OK);

    throw (nMsgID);
}

bool CreateBackupFile(const CString &sFile)
{
    bool bRet = true;
    if (FileExists(sFile)) {
        CString sBak;
        for (int i = 1; i < 1024; i++) {
            sBak.Format("%s.bak_%d", sFile, i);
            if (! FileExists(sBak)) {
                ::MoveFile(sFile, sBak);
                //                theLog("Backup of %s in %s\n", sFile, sBak);
                break;
            }
        }
        if (i == 1024) {
            //            theLog("Too many backups of %s, skipping its installation.\n", sFile);
            bRet = false;
        }
    }
    return bRet;
}

bool SafeCopyFile(const CString &src, const CString &dst, bool bBackup)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwSize = 0;
    bool bRet = true;
  
    hFile = ::CreateFile((LPCTSTR)src, 
                         GENERIC_READ, 
                         FILE_SHARE_READ, 
                         NULL,
                         OPEN_EXISTING, 
                         FILE_ATTRIBUTE_NORMAL, 
                         NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
            //            theLog("!!!Fail to open %s while copying to %s (Error %d)\n", src, dst, GetLastError());
        bRet = false;
        return bRet;
    }

    
    dwSize = ::GetFileSize(hFile, NULL);
    CloseHandle(hFile);

    if (bBackup)
        CreateBackupFile(dst);
        
    CString dir = dst;
    ::GetParentDirectory(dir);
    ::CreateDirectoryPath(dir);
        
    if (::CopyFile(src, dst, FALSE) == 0) {
        // There seems to be a weird bug with remote drives.
        // Try to guess a bit more about the failure.
        DWORD dwErrCode = GetLastError();
        if (dwErrCode != ERROR_ACCESS_DENIED) {
            //                theLog("Error: Fail to copy %s to %s (Error %lu)\n", src, dst, dwErrCode);
            bRet = false;
        }
        else {
            hFile = ::CreateFile((LPCTSTR)dst, 
                                 GENERIC_READ|GENERIC_WRITE, 
                                 FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                 NULL,
                                 OPEN_ALWAYS, 
                                 FILE_ATTRIBUTE_NORMAL, 
                                 NULL);
            CloseHandle(hFile);
            if (::CopyFile(src, dst, FALSE) == 0) {
                dwErrCode = GetLastError();
                //                    theLog("!!!Fail to copy %s to %s (Error %lu)\n", src, dst, dwErrCode);
                bRet = false;
            }
            else {
                //                    theLog("Copy %s to %s\n", src, dst);
            }
        }
    }
    else {
        //            theLog("Copy %s to %s\n", src, dst);
    }
    if (bRet && ::SetFileAttributes(dst, FILE_ATTRIBUTE_NORMAL) == 0) {
        //            theLog("!!!Fail to reset file attributes for %s (Error %lu)\n", dst, GetLastError());
        bRet = false;
    }
    //    DoProgress(dwSize);
  
    return bRet;
}

// Default values :
// bWait = false, bUseSW = false, wShow = SW_SHOWDEFAULT
bool RunProcess(const char *command_line, bool bWait, bool bUseSW, WORD wShow)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    DWORD dwCreationFlags = 0;
    DWORD ret = 0;
    char cmd[_MAX_PATH];

    ZeroMemory( &si, sizeof(STARTUPINFO) );
    si.cb = sizeof(STARTUPINFO);
    si.dwFlags = STARTF_USESHOWWINDOW;
    if (bUseSW) {
        si.wShowWindow = wShow;
    }
    else {
        si.wShowWindow = SW_HIDE;
    }
//      si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
//      si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
//      si.hStdError = GetStdHandle(STD_ERROR_HANDLE);

#ifdef _TRACE
    fprintf(stderr, "Executing: %s\n", new_cmd);
#endif
    strcpy(cmd, command_line);

    if (CreateProcess(NULL,
		      cmd,
		      NULL,
		      NULL,
		      FALSE,
		      dwCreationFlags,
		      0,
                      NULL,
		      &si,
		      &pi) == 0) {
        FatalError(IDS_CANNOT_RUN_PROCESS, command_line);
        return false;
    }
    
    /* Only the process handle is needed */
    CloseHandle(pi.hThread);
    
    if (bWait) {
      if (WaitForSingleObject(pi.hProcess, INFINITE) == WAIT_OBJECT_0) {
	if (GetExitCodeProcess(pi.hProcess, &ret) == 0) {
            //	  fprintf(stderr, "Failed to retrieve exit code: %s (Error %d)\n", cmd, GetLastError());
	  ret = -1;
	}
      }
      else {
          //	fprintf(stderr, "Failed to wait for process termination: %s (Error %d)\n", cmd, GetLastError());
	ret = -1;
      }
    }

    CloseHandle(pi.hProcess);

    return (ret == 0);
}

bool RemovePath(const char *filename)
{
    DWORD fa = ::GetFileAttributes((LPCTSTR)filename);
    if (fa == -1) {
//          theLog("Error: Failed to get file attributes for %s (Error %d)!!!\n", 
//                 (LPCTSTR)filename, GetLastError());
    }
    else if ((fa & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
//          CSingleLock theLock(&(g_csCritSec), TRUE);
//          theStatus.sName = (LPCTSTR)filename;
//          theLock.Unlock();
        if (RemoveDirectory((LPCTSTR)filename) == 0) {
//              theLog("Error: Failed to remove directory %s (Error %d)!!!\n", 
//                     (LPCTSTR)filename, GetLastError());
        }
        else {
//              theLog ("Removed directory %s\n", (LPCTSTR)filename);
        }
    }
    else {
        if (DeleteFile((LPCTSTR)filename) == 0) {
//              theLog("Error: Failed to delete file %s (Error %d)!!!\n", 
//                     (LPCTSTR)filename, GetLastError());
        }
        else {
//              theLog ("Deleted file %s\n", (LPCTSTR)filename);
        }
    }
    return true;
}

/*
  Recursive walk through the directory tree. Depth-first order. 
*/
void __rec_rmdir(char *path)
{
    /* In depth traversal of the subdir tree */
    WIN32_FIND_DATA find_file_data;
    HANDLE hnd;
    DWORD fa = ::GetFileAttributes(path);

    int path_len = strlen(path);

    /* current node */
    if (fa == -1) {
//          theLog("Error: Failed to get file attributes for %s (Error %d)!!!\n", 
//                 (LPCTSTR)path, GetLastError());
        return;
    }

    /* if it is a directory, recurse through all sons */
    if ((fa & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
        strcat(path, "/*");
        hnd = FindFirstFile(path, &find_file_data);
        while (hnd != INVALID_HANDLE_VALUE && 
               FindNextFile(hnd, &find_file_data) != FALSE) { 
            if(!strcmp(find_file_data.cFileName, ".")
               || !strcmp(find_file_data.cFileName, "..")) 
                continue;
            path[path_len+1] = '\0';
            strcat(path, find_file_data.cFileName);
            ::__rec_rmdir(path);
        }
        path[path_len] = '\0';
        FindClose(hnd);
    }
    /* execute after for the current node */
    RemovePath(path);
}

bool RecursivelyRemove(const CString &filename)
{
    DWORD fa = GetFileAttributes(filename);
    if (fa == -1) {
//          theLog("Error: Fail to get file attributes for %s (Error %d)!!!\n",
//                 filename, GetLastError());
        return false;
    }

    AfxGetApp()->DoWaitCursor(-1);

    if ((fa & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
        char path[1024];
        strcpy(path, (LPCTSTR)filename);
        ::__rec_rmdir(path);
    }
    else {
        if (DeleteFile((LPCTSTR)filename) == 0) {
//              theLog("Error: Failed to delete file %s (Error %d)!!!\n", 
//                     (LPCTSTR)filename, GetLastError());
        }
        else {
//              theLog ("Deleted file %s\n", (LPCTSTR)filename);
        }
    }

    AfxGetApp()->DoWaitCursor(1);

    return true;
}

void Initialize()
{
    char buffer[_MAX_PATH];
    if (GetTempPath(sizeof(buffer), buffer) == 0) {
        g_sTempPath = "c:\\";
    }
    else {
        g_sTempPath = buffer;
    }

    g_bIsAdmin = ::IsAdmin();
    g_bIsWindowsNT = ::IsWindowsNT();
    
    g_bIE5 = (GetDllVersion("shdocvw.dll") >= PACKVERSION(5,0));
    // Guess which mode !
    g_sInternetMethod = (g_bIE5 ? "ie5" : "direct");

    // The TeXLive registry key
    g_sRegKey = ConcatPath( TEXLIVE_REGENTRY, TEXLIVE_VERSION);

    // VarTexmf will be stored in temp dir
    g_sVarTexmf = ConcatPath(g_sTempPath, "TeX\\texmf");

    // Get the cdrom texmf tree
    CString sDummyName;
    DWORD dwDummySize;
#ifdef TEST
	g_sTexmfMain = "e:\\TeXLive\\setupw32";
#else
    GetExecutableDirectory(g_sTexmfMain, sDummyName, dwDummySize);
#endif
    // Assume we are in \bin\win32 subdirectory, so:
    GetParentDirectory(g_sTexmfMain);
    GetParentDirectory(g_sTexmfMain);
    g_sDriveRootPath = g_sTexmfMain;
    g_sBinDir = ConcatPath(g_sTexmfMain, "bin\\win32");
    g_sSetupw32 = ConcatPath(g_sTexmfMain, "setupw32");
    g_sSupport = ConcatPath(g_sTexmfMain, "support");
    g_sTexmfMain = ConcatPath(g_sTexmfMain, "texmf");
    if (! DirectoryExists(g_sTexmfMain) ) {
        AfxMessageBox(IDS_NO_TEXLIVE_TEXMF, MB_OK | MB_ICONSTOP);
        exit(1);
    }
    if (! DirectoryExists(g_sBinDir) ) {
        AfxMessageBox(IDS_NO_TEXLIVE_BINARIES, MB_OK | MB_ICONSTOP);
        exit(1);
    }
    GetEditorLocation(g_sEditor);
}

// Return short path name under W9x, else quoted or unquoted pathname
// if contains white space.
CString GetSafePathName(const CString &path, bool bQuoted)
{
    // Under Win9x, we need to rely on the short path name
    // else, autoexec.bat won't grok it. But what about WINME ?
    CString short_path(path);
    if (bQuoted) {
        if (strchr(path, ' ') && path[0] != '"') {
            short_path = CString("\"") + short_path + CString("\"");
        }
    }
    else if (!g_bIsWindowsNT) {
        char lpShortPathName[_MAX_PATH];
        if (GetShortPathName((LPCTSTR)path, lpShortPathName, sizeof(lpShortPathName)) != 0)
            short_path = lpShortPathName;
    }
    
    return short_path;
}

int WINAPI DoPrint (LPSTR buf, unsigned long size)
{
    //    theLog ("%.*s", (int) size, (const char *) buf);
    return ((unsigned int) size);
}

int WINAPI DoReplace (char *filename)
{
    return (IDM_REPLACE_ALL);
}

int WINAPI DoProgress (LPCSTR filename, unsigned long size)
{
    return 0;
}

bool UnzipFile(const CString &zipName, const CString &destdir, const CStringArray *sa)
{
    int nb_files_to_extract = 0;
    char **files_to_extract = NULL;
  
    AfxGetApp()->DoWaitCursor(1);

    if (sa) {
        nb_files_to_extract = sa->GetSize();
        files_to_extract = new char*[sa->GetSize()];
        for (int i = 0; i < sa->GetSize(); i++) 
            files_to_extract[i] = (char *)(LPCTSTR)sa->GetAt(i);
    }

    // initialize unzip structures
    USERFUNCTIONS functions;
    memset (&functions, 0, sizeof(functions));
    functions.print = DoPrint;
    functions.ServCallBk = DoProgress;
    functions.replace = DoReplace;

    DCL options;
    memset (&options, 0, sizeof(options));

    options.lpszZipFN = (char *)(LPCTSTR)zipName;
    options.PromptToOverwrite = 0;
    options.ndflag = 1;
    options.noflag = 1;
    options.lpszExtractDir = (char *)(LPCTSTR)destdir;
    
    // theLog ("\nunzip %s -d %s\n", (LPCTSTR) zipName, (LPCTSTR) destdir);
    
    // run unzip
    int rc = ::Wiz_SingleEntryUnzip(nb_files_to_extract, files_to_extract, 0, 0, &options, &functions);

    AfxGetApp()->DoWaitCursor(-1);

    if (rc != 0) {
        // theLog (IDS_ERROR_UNZIPPING, (const char *) zipName);
        return false;
    }
    
    if (sa) {
        delete [] files_to_extract;
    }

    return true;
}

static bool __IsInternetConnected (void)
{
    int nCheck = AfxSocketInit();
    CSocket m_Server;
    bool bResult = true;
#if 0
    HKEY hKey;
    DWORD dwDial = 0, dwDialType = REG_DWORD, dwDialSize = 4;
    DWORD dwNew = 0;
    
    if (RegOpenKeyEx(HKEY_CURRENT_USER,
                     "Software\\Microsoft\\Windows\\CurrentVersion\\Internet Settings",
                     0, KEY_SET_VALUE, &hKey) != ERROR_SUCCESS) {
        bResult = false; // We cannot find the key. Handle this situation or just continue
        goto exit_0;
    }

    if (RegQueryValueEx(hKey, "EnableAutodial", NULL, &dwDialType,
                        (BYTE *) &dwDial, &dwDialSize ) != ERROR_SUCCESS) {
        dwDial = 0;             // Be safe !
        goto skip_1; // We cannot find the value. Handle it.
    }
    
    if (dwDial) { // We need to change the value, in order to make
        // a dialup window not to show up.
        
        if ((nCheck = RegSetValueEx(hKey, "EnableAutodial", NULL,
                                    dwDialType, (BYTE *) &dwNew, dwDialSize)) != ERROR_SUCCESS) {
            ; // Failed? We shouldn't get here. You decide how to handle it
    }
    
 skip_1:
#endif
    if (!m_Server.Create()) {                                                       
        // m_sError = _T( "Unable to create the socket." );
        bResult = false;
    }   
    
    // You can use www.microsoft.com in order to check whether DNS is available
    // or numeric IP otherwise 
    else if (!m_Server.Connect("www.microsoft.com", 80)) { //  207.46.130.150
        // m_sError = _T( "Unable to connect to server" );        
        m_Server.Close();
        bResult = false;
    }
#if 0    
    
    if (dwDial) {
        if ((nCheck = RegSetValueEx(hKey, "EnableAutodial", NULL,
                                    dwDialType, (BYTE *) &dwDial, dwDialSize)) != ERROR_SUCCESS)
            ; // Failed? We shouldn't get it. You decide how to handle this.
    }

 exit_1:    
    RegCloseKey(hKey);
 exit_0:
#endif
    return (bResult);
}


DWORD WINAPI ConnectThreadProc(LPVOID lpResult)
{
    BOOL* lpbResult = (BOOL*) lpResult;
    
    *lpbResult = __IsInternetConnected();
    return 0;		
}

bool IsInternetConnected()
{
    bool bResult = false;
    DWORD dwThreadId = 0;

    bool bInternetConnected = false;
    HANDLE hConThread = CreateThread(NULL, 0, ConnectThreadProc, &bInternetConnected, 0, &dwThreadId); 
    
    if (WaitForSingleObject(hConThread, 5000) == WAIT_TIMEOUT) {
        TerminateThread(hConThread, 0);
    }
    return (bInternetConnected);
}


bool HasInternetConnection()
{
    bool bInternetConnected;

    if (g_bIE5) {
        DWORD dwConnectionTypes = INTERNET_CONNECTION_LAN |
            INTERNET_CONNECTION_MODEM |
            INTERNET_CONNECTION_PROXY;
        if (!InternetGetConnectedState(&dwConnectionTypes, 0)) {
            InternetAutodial(INTERNET_AUTODIAL_FORCE_UNATTENDED, 0);
        }
        bInternetConnected = InternetGetConnectedState(&dwConnectionTypes, 0);
    }
    else {
        AfxGetApp()->DoWaitCursor(1);
        bInternetConnected = __IsInternetConnected();
        AfxGetApp()->DoWaitCursor(-1);
    }

    if (bInternetConnected) {
        // Guess which mode !
        g_sInternetMethod = (g_bIE5 ? "ie5" : "direct");
    }
    return bInternetConnected;
}

#if 0
bool GetWinShellLocation(CString &sWinShell)
{
    CRegistry theRegistry;
    sWinShell.Empty();
    g_sEditor.Empty();

    if (theRegistry.Connect(HKEY_LOCAL_MACHINE) == FALSE) {
//          theLog("Registry: can't connect to HKEY_LOCAL_MACHINE (Error %d)\n",
//                 GetLastError());
        return false;
    }
    if (theRegistry.Open(TEXT("SOFTWARE\\Classes\\tex_auto_file\\DefaultIcon"),
                         (CRegistry::CreatePermissions)CRegistry::permissionRead) == FALSE ) {
//          theLog("Registry: can't open WinShell.Document\\DefaultIcon key (Error %d)\n",
//                 GetLastError());
        theRegistry.Close();
        return false;
    }
    theRegistry.GetValue("", sWinShell);
    theRegistry.Close();

    // It might be quoted !
    sWinShell.Remove(TEXT('\"'));
    sWinShell.Remove(TEXT('\''));
    ::GetParentDirectory(sWinShell);
    g_sEditor = ConcatPath(sWinShell, "WinShell.exe");

    if (! FileExists(g_sEditor)) {
        g_sEditor.Empty();
        sWinShell.Empty();
        return false;
    }

    return true;
}
#endif

bool GetGhostscriptLocation(CString &sGhostscript)
{
    sGhostscript = gs_locate();
    return !sGhostscript.IsEmpty();
}

bool GetEditorLocation(CString &sEditor)
{
    CRegistry theRegistry;
    bool bRet = false;

    if (sEditor.IsEmpty()) {
        if (theRegistry.Connect(HKEY_CURRENT_USER) == FALSE) {
            return false;
        }
        if (theRegistry.Open(g_sRegKey) == TRUE) {
            theRegistry.GetValue("TeXEditor", sEditor);
        }

        theRegistry.Close();
    }
    return (!sEditor.IsEmpty());
}

bool SaveEditorLocation(CString & sEditor)
{
    CRegistry theRegistry;
    bool bRet = false;

    if (theRegistry.Connect(HKEY_CURRENT_USER) == FALSE) {
        return false;
    }
    if (theRegistry.Create(g_sRegKey) == FALSE) {
        goto exit_2;
    }
    if (theRegistry.SetValue("TeXEditor", (LPCTSTR)sEditor) == FALSE) {
        goto exit_2;
    }
    bRet = true;

 exit_2:
    theRegistry.Close();
    return bRet;
}

bool InstallGhostscript()
{
    CString sCmd = ConcatPath(g_sBinDir, "TeXSetup --add-package=ghostscript-free --quick");
    bool bRet = true;
    bRet = bRet && RunProcess(sCmd, true);
    return bRet;
}

bool InstallWinShell()
{
    CString sCmd = ConcatPath(g_sBinDir, "TeXSetup --add-package=winshell --quick");
    bool bRet = true;
    bRet = bRet && RunProcess(sCmd);
    return bRet;
}

bool __cdecl SetupDirs(LPVOID pParam)
{
    CString sDir, sSrc, sDst;

    if (g_bIsWindowsNT) {
        HANDLE hLog = 0;
	// theLog("Creating %s directory ", (LPCTSTR)g_sInstallRootDir);
        // CreateDirectoryPath(g_sInstallRootDir);
        // theLog("and granting everyone read access to it.\n");
        // if (! GrantPermissions(g_sInstallRootDir, "everyone", "read", FALSE, hLog)) {
            // Appropriate message
            // theLog("Warning: Fail to grant everyone read access to %s (Error %ld).\n", 
            //        (LPCTSTR)g_sInstallRootDir, GetLastError());
        // }
        if (! g_sVarTexmf.IsEmpty()) {
            // theLog("Creating %s directory ", (LPCTSTR)g_sVarTexmf);
            CreateDirectoryPath(g_sVarTexmf);
            // theLog("and granting everyone full access to it.\n");
            if (! GrantPermissions(g_sVarTexmf, "everyone", "full", FALSE, hLog)) {
                // Appropriate message
                // theLog("Warning: Fail to grand everyone full access to %s (Error %ld).\n", 
                //        (LPCTSTR)g_sVarTexmf, GetLastError());
            }
        }
    }


    // First run : copy all configuration files.
    for (int i = 0; i < sizeof(g_confFiles)/sizeof(g_confFiles[0]); i += 2) {
        sSrc = ConcatPath(g_sTexmfMain, g_confFiles[i]);
        sDst = ConcatPath(g_sVarTexmf, g_confFiles[i+1]);
        if (FileExists(sSrc) && ! FileExists(sDst)){
            SafeCopyFile(sSrc, sDst, TRUE);
        }
    }
    
    return true;
}

bool __cdecl SetupEnv(LPVOID pParam)
{
    char buf[4096];
    CString sPath = g_sBinDir;
    sPath = GetSafePathName(sPath);
    sPath = sPath + ";";
    // Path
    GetEnvironmentVariable("PATH", buf, sizeof(buf));
    g_sDefaultPath = buf;
    sPath = sPath + buf;
    if (! g_sGhostscript.IsEmpty()) {
        char path[_MAX_PATH], *fp;
        // If not on the PATH, then add it
        if (SearchPath(buf, "gswin32c.exe", NULL, sizeof(path), path, &fp) == 0) {
            CString sGhostscriptBinDir = g_sGhostscript;
            sGhostscriptBinDir.Replace('/','\\');
            GetParentDirectory(sGhostscriptBinDir);
            // Remove terminating '\\' if needed
            if (sGhostscriptBinDir.Right(1) == "\\"
                && sGhostscriptBinDir.Right(2) != ":\\")
                sGhostscriptBinDir = sGhostscriptBinDir.Left(sGhostscriptBinDir.GetLength() - 1);
            // Retrieve safe path name for win9x users
            sGhostscriptBinDir = GetSafePathName(sGhostscriptBinDir, false);
            sPath = sPath + ";" + sGhostscriptBinDir;
        }
    }
    SetEnvironmentVariable("PATH", (LPCTSTR)sPath);

    CString sDir = ConcatPath(g_sVarTexmf, "web2c");
    SetEnvironmentVariable("TEXMFCNF", (LPCTSTR)sDir);

    SetEnvironmentVariable("TEXMFMAIN", (LPCTSTR)g_sTexmfMain);

    SetEnvironmentVariable("VARTEXMF", (LPCTSTR)g_sVarTexmf);

    return true;
}

bool __cdecl CleanupEnv(LPVOID pParam)
{
    SetEnvironmentVariable("PATH", (LPCTSTR)g_sDefaultPath);
    SetEnvironmentVariable("TEXMFCNF", (LPCTSTR)NULL);
    SetEnvironmentVariable("TEXMFMAIN", (LPCTSTR)NULL);
    SetEnvironmentVariable("VARTEXMF", (LPCTSTR)NULL);
    return true;
}

bool __cdecl UnblockFormats(LPVOID pParam)
{
    // Unblock formats
    KeyValFile kFmtutilCnf;
    kFmtutilCnf.Open(ConcatPath(g_sVarTexmf, "web2c\\fmtutil.cnf"));
    kFmtutilCnf.RawReplace("#!#", "");
    kFmtutilCnf.Commit();
    kFmtutilCnf.Close();
    return true;
}

static CString sCurrentFormat;

bool __cdecl MakeFormats(LPVOID pParam)
{
    char *cmd = "fmtutil --byfmt=%s --dolinks";
    char buf[_MAX_PATH];
    UINT i = (UINT)pParam;
    CString sFmt = ConcatPath(g_sVarTexmf, "web2c");
    sFmt = ConcatPath(sFmt, g_formatFiles[i]);
    if (! FileExists(sFmt)) {
        CString sFormat = g_formatFiles[i];
        int nPos;
        if ((nPos = sFormat.Find('.')) > -1) {
            sFormat = sFormat.Left(nPos);
        }
        wsprintf(buf, cmd, (LPCTSTR)sFormat);
        RunProcess(buf, true, true, SW_HIDE);
    }
    return true;
}

bool __cdecl MakelsR(LPVOID pParam)
{
    char *cmd = "mktexlsr %s";
    char buf[_MAX_PATH];

    wsprintf(buf, cmd, g_sVarTexmf);
    RunProcess(buf, true, true, SW_HIDE);
    return true;
}

bool __cdecl CleanupCDRom(LPVOID pParam)
{
    CString sTmp = g_sVarTexmf;
    GetParentDirectory(sTmp);
    RecursivelyRemove(sTmp);

#if 0
    CString sWinShellDir;
    if (! GetWinShellLocation(sWinShellDir) || ! DirectoryExists(sWinShellDir))
        return true;

    if (MessageBox(NULL, "Do you want to remove WinShell too ?", 
                   NULL, MB_YESNO | MB_ICONINFORMATION) == IDYES) {
        char buf[_MAX_PATH];
        CString sUninstCmd;
        for (int i = 0; i < 1000; i++) {
            wsprintf(buf, "unins%.3d.exe", i);
            sUninstCmd = ConcatPath(sWinShellDir, buf);
            if (! FileExists(sUninstCmd)) {
                i--;
                break;
            }
        }
        if (i > -1) {
            wsprintf(buf, "unins%.3d.exe", i);
            sUninstCmd = ConcatPath(sWinShellDir, buf);
            RunProcess(sUninstCmd, true);
            RecursivelyRemove(sWinShellDir);
        }
    }
#endif

    // Remove the registry key
    CRegistry theRegistry;
    if (theRegistry.Connect(HKEY_CURRENT_USER) == TRUE) {
        theRegistry.DeleteKey(g_sRegKey);
    }

    return true;
}

bool SelectEditor(CString &sEditor)
{
    OPENFILENAME ofn;       // common dialog box structure
    char szFile[260];       // buffer for file name
    
    //
    ZeroMemory(szFile, sizeof(szFile));
    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = AfxGetMainWnd()->m_hWnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Executables\0*.EXE;*.COM\0";
    ofn.nFilterIndex = 0;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = _TEXT("c:\\Program Files");
    ofn.lpstrTitle = _TEXT("Select a TeX oriented text editor");
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    // Display the Open dialog box. 
    if (GetOpenFileName(&ofn)==TRUE) {
        sEditor = szFile;
    }
    if (! sEditor.IsEmpty()) {
        SaveEditorLocation(sEditor);
    }
    return (! sEditor.IsEmpty());
}

void RunOffCDRom()
{
#if 0
    // Be safe !
    if (g_sEditor.IsEmpty()) {
        AfxMessageBox("You need to select a TeX-oriented text editor!", MB_ICONWARNING);
        return;
    }
    // Ghostscript might have been installed in the mean time ...
    if (g_sGhostscript.IsEmpty()) {
        GetGhostscriptLocation(g_sGhostscript);
    }
    
    if (g_sGhostscript.IsEmpty()) {
        AfxMessageBox("Ghostscript is missing,\r\nrendering might be poor.\r\nPlease, consider installing it.", MB_ICONWARNING);
    }
    
#if 0
    if (! GetEditorLocation(g_sEditor)) {
        bAskingEditor = true;

        if (!SelectEditor(g_sEditor)) {
            AfxMessageBox("You need to select a TeX-oriented text editor!",
                          MB_ICONWARNING);
            return;
        }
    }
#endif

    //pDialog initialized to NULL in the constructor of CMyWnd class
    CActionProgressDlg *pDialog = new CActionProgressDlg();
    //Check if new succeeded and we got a valid pointer to a dialog object
    if(pDialog != NULL) {
        BOOL ret = pDialog->Create(IDD_ACTION_PROGRESS_DIALOG,AfxGetMainWnd());
        if(!ret)   //Create failed.
            AfxMessageBox("Error creating Dialog");
        pDialog->CenterWindow(AfxGetMainWnd());
        pDialog->ShowWindow(SW_SHOW);
    }
    else
        AfxMessageBox("Error Creating Dialog Object");

    pDialog->AddAction("Setting up directories ...", SetupDirs);
    pDialog->AddAction("Setting up environment ...", SetupEnv);
    // No need to build formats anymore !
    // pDialog->AddAction("Building TeX format", MakeFormats, 0);
    // pDialog->AddAction("Building LaTeX format", MakeFormats, (LPVOID)1);
    // pDialog->AddAction("Building PDFLaTeX format", MakeFormats, (LPVOID)2);
    // pDialog->AddAction("Building ConTeXt format", MakeFormats, (LPVOID)3);
    pDialog->AddAction("Building ls-R database ...", MakelsR);
    pDialog->StartActions();
#endif
    
    RunProcess(ConcatPath(g_sDriveRootPath, g_sEditor), false, true, SW_SHOWDEFAULT);

#if 0
    if (bAskingEditor) {
        if (AfxMessageBox("Do you want me to remember this editor for another session ?", MB_YESNO)
            == IDYES) {
            SaveEditorLocation(g_sEditor);
        }
    }
#endif

    CleanupEnv();

}

void RunTeXDocTK()
{
    //pDialog initialized to NULL in the constructor of CMyWnd class
    CActionProgressDlg *pDialog = new CActionProgressDlg();
    //Check if new succeeded and we got a valid pointer to a dialog object
    if(pDialog != NULL) {
        BOOL ret = pDialog->Create(IDD_ACTION_PROGRESS_DIALOG,AfxGetMainWnd());
        if(!ret)   //Create failed.
            AfxMessageBox("Error creating Dialog");
        pDialog->CenterWindow(AfxGetMainWnd());
        pDialog->ShowWindow(SW_SHOW);
    }
    else
        AfxMessageBox("Error Creating Dialog Object");

    pDialog->AddAction("Setting up directories ...", SetupDirs);
    pDialog->AddAction("Setting up environment ...", SetupEnv);
    pDialog->StartActions();
        
    CString sCmd = ConcatPath(g_sBinDir, "TeXDocTK.exe");
    RunProcess(sCmd, false, true, SW_SHOWDEFAULT);

    CleanupEnv();
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
