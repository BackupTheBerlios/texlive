// Environment.cpp: implementation of the Environment class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TeXSetup.h"
#include "Environment.h"
#include "Registry.h"
#include "TeXInstall.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

Environment::Environment()
{
    m_bIsModified = false;
}

Environment::~Environment()
{
    POSITION pos = m_mEnv.GetStartPosition();
    while( pos != NULL ) {
        CString *value;
        CString variable;
        // Gets key (Cstring) and value (col)
        m_mEnv.GetNextAssoc( pos, variable, value );
        delete value;
    }
    m_mEnv.RemoveAll();
}

BOOL Environment::GetValue(const CString &sVariable, CString &sValue)
{
    CString *sFound;
    if (m_mEnv.Lookup(sVariable, sFound)) {
        sValue = *sFound;
        return TRUE;
    }
    else {
        sValue.Empty();
        return FALSE;
    }
}

BOOL Environment::SetValue(const CString &sVariable, const CString &sValue) 
{
    m_mEnv[sVariable] = new CString(sValue);
    m_bIsModified = true;
    return TRUE;
}

static char *autoexecBegin = "REM --- Added by fpTeX (%s) ---\n";
static int autoexecMatchLength = sizeof("REM --- Added by fpTeX");
static char *autoexecEnd = "REM --- End of fpTeX additions ---\n";

//
// This is a bit dangerous : it removes multiple occurences
// of directories in a path environment variable.
//
void Environment::CleanupPath(CString &sPath)
{
    if (sPath.IsEmpty())
        return;

    CStringArray saDirs;
    ::SplitString(sPath, ";", saDirs);

    CString si, sj;
    for (int i = 0; ; i++) {
        si = saDirs.GetAt(i);
        for (int j = saDirs.GetSize() - 1; j > i ; j--) {
            if (si == saDirs.GetAt(j))
                saDirs.RemoveAt(j);
        }
        if (i == saDirs.GetSize() - 1)
            break;
    }
  
    sPath.Empty();
    for (i = 0; i < saDirs.GetSize() - 1; i++)
        sPath += saDirs.GetAt(i) + ";";
    sPath += saDirs.GetAt(i);

    saDirs.RemoveAll();
}

//
// Removes an element from a path
//
void Environment::DeleteFromPath(CString &sPath, CString &sElt)
{
    if (sPath.IsEmpty())
        return;

    CStringArray saDirs;

    ::SplitString(sPath, ";", saDirs);

    for (int i = 0; ; i++) {
        if (saDirs.GetAt(i) == sElt) {
            saDirs.RemoveAt(i);
        }
        if (i == saDirs.GetSize() - 1)
            break;
    }
  
    sPath.Empty();
    for (i = 0; i < saDirs.GetSize() - 1; i++)
        sPath += saDirs.GetAt(i) + ";";
    sPath += saDirs.GetAt(i);
  
    saDirs.RemoveAll();
}

// Holds previous value of PATH to be able
// to accumulate changes
static CString sPreviousPath;

BOOL Environment::CommitW9x(BOOL bAdd)
{
    // Remove the ad hoc lines from autoexec.bat
    CString sAutoBat = "c:\\autoexec.bat";
    CString sAutoBak, sTempDir("c:\\");
    CreateTempFile(sTempDir, sAutoBak);
    bool bWriteProtected;
    DWORD fa;
    FILE *fbak, *fbat;
    char line[1024];

    // First case : no autoexec.bat
    if (! FileExists((LPCTSTR)sAutoBat)) {
        // If not requesting to add the lines,
        // then fail silently
        if (! bAdd)
            return TRUE;

        fbat = fopen((LPCTSTR)sAutoBat, "w");
        if (fbat == NULL) {
            theLog("Error: No `autoexec.bat' and failed to create one. Please fix it by hand.\n");
            return FALSE;
        }
        CTime t = CTime::GetCurrentTime();
        fprintf(fbat, autoexecBegin, (LPCTSTR)t.Format("%d/%m/%Y %H:%M"));
        POSITION pos = m_mEnv.GetStartPosition();
        while( pos != NULL ) {
            CString *value;
            CString variable;
            // Gets key (Cstring) and value (col)
            m_mEnv.GetNextAssoc( pos, variable, value );
            fprintf(fbat, "set %s=%s\n", (LPCTSTR)variable, (LPCTSTR)(*value));
            theLog("Environment: %s = %s\n", (LPCTSTR)variable, (LPCTSTR)*value);
        }
        fprintf(fbat, autoexecEnd);
        fclose(fbat);
        theLog("Your `c:\\autoexec.bat' file has been created.\n");
        return TRUE;
    }

    // Second case: autoexec.bat already exists
    // Copy autoexec.bat to autoexec.bak
    if (((fa = GetFileAttributes((LPCTSTR)sAutoBat)) == 0xffffffff)
        || (::CopyFile((LPCTSTR)sAutoBat, (LPCTSTR)sAutoBak, FALSE) == 0)) {
        theLog("Error: Fail to copy %s to %s (Error %lu)\n", sAutoBat, sAutoBak, GetLastError());
        theLog("Error: Remove the lines about fpTeX from your `c:\\autoexec.bat' file by hand.\n");
        return FALSE;
    }
    // Be safe ! It might be needed
    bWriteProtected = (fa & FILE_ATTRIBUTE_READONLY);
    if (bWriteProtected)
        SetFileAttributes((LPCTSTR)sAutoBat, FILE_ATTRIBUTE_NORMAL);
    //
    fbak = fopen((LPCTSTR)sAutoBak, "r");
    if (fbak == NULL)
        return FALSE;
    fbat = fopen((LPCTSTR)sAutoBat, "w");
    if (fbat == NULL) {
        fclose(fbak);
        unlink((LPCTSTR)sAutoBak);
        return FALSE;
    }
    while (fgets(line, sizeof(line), fbak)
           && _strnicmp(line, autoexecBegin, autoexecMatchLength) != 0) {
        fputs(line, fbat);
    }
    // Beware, autoexec.bat might not end with a '\n'
    *(line + sizeof(line) - 1) = '\0'; // Be safe
    if (*(line + strlen(line) - 1) != '\n')
        fprintf(fbat, "\n");
    while (fgets(line, sizeof(line), fbak)
           && _stricmp(line, autoexecEnd) != 0) {
        // Copy variables that are not overwritten by the current theEnvironment
        char *equal;
        CString sVariable, sValue, *lpsDummy;
        int nPos;
        if (equal = strchr(line, '=')) {
            sValue = equal+1;
            sValue.TrimLeft();
            sValue.TrimRight();
            *equal = '\0';
            if (_strnicmp(line,"set", strlen("set")) == 0) {
                sVariable = line + strlen("set");
            }
            else {
                sVariable = line;
            }
            sVariable.TrimLeft();
            sVariable.TrimRight();
            // reinject variables set previously and not overwritten
            if (!sVariable.IsEmpty() && !m_mEnv.Lookup(sVariable, lpsDummy)) {
                m_mEnv[sVariable] = new CString (sValue);
            }
            if (_stricmp((LPCTSTR)sVariable, "PATH") == 0) {
                sPreviousPath = sValue;
            }
        }
    }
    theLog("Environment: found previous PATH = %s\n", (LPCTSTR)sPreviousPath);
    if (bAdd) {
        CTime t = CTime::GetCurrentTime();
        fprintf(fbat, autoexecBegin, (LPCTSTR)t.Format("%d/%m/%Y %H:%M"));
        POSITION pos = m_mEnv.GetStartPosition();
        while( pos != NULL ) {
            CString *value;
            CString variable;
            // Gets key (Cstring) and value (col)
            m_mEnv.GetNextAssoc( pos, variable, value );
            if (_strnicmp((LPCTSTR)variable, "PATH", 4) == 0) {
                if (!sPreviousPath.IsEmpty()) {
                    value->Replace("%PATH%", (LPCTSTR)sPreviousPath);
                    CleanupPath(*value);
                }
            }
            fprintf(fbat, "set %s=%s\n", (LPCTSTR)variable, (LPCTSTR)(*value));
            theLog("Environment: %s = %s\n", (LPCTSTR)variable, (LPCTSTR)*value);
        }
        fprintf(fbat, autoexecEnd);
    }
    while (fgets(line, sizeof(line), fbak)) {
        fputs(line, fbat);
    }
    fclose(fbat);
    fclose(fbak);
    
    // Remove the temporary file
    unlink((LPCTSTR)sAutoBak);

    if (bWriteProtected)
        SetFileAttributes((LPCTSTR)sAutoBat, FILE_ATTRIBUTE_READONLY);

    if (bAdd) {
        theLog("Your `c:\\autoexec.bat' file has been modified.\n");
    }
    else {
        theLog("Your `c:\\autoexec.bat' file has been restored.\n");
        theLog("The previous one has been copied to `c:\\autoexec.bak'.\n");
    }
    return TRUE;
}

BOOL Environment::Commit()
{
    CString sSubKey = TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment");

    if (g_bIsWindowsNT) {
        // The variables are in the registry
        CRegistry theRegistry;
        if (g_bAllUsers) {
            if (theRegistry.Connect(HKEY_LOCAL_MACHINE) == FALSE) {
                theLog("Registry: can't connect to HKEY_LOCAL_MACHINE (Error %d)\n",
		       GetLastError());
                return FALSE;
            }
            if (theRegistry.Open(sSubKey,
                                 (CRegistry::CreatePermissions)CRegistry::permissionAllAccess) == FALSE ) {
                theLog("Registry: can't open Environment key (Error %d)\n",
		       GetLastError());
                return FALSE;
            }
        }
        else {
            if (theRegistry.Connect(HKEY_CURRENT_USER) == FALSE) {
                theLog("Registry: can't connect to HKEY_CURRENT_USER (Error %d)\n",
		       GetLastError());
                return FALSE;
            }
            if (theRegistry.Open(TEXT("Environment"),
                                 (CRegistry::CreatePermissions)CRegistry::permissionAllAccess) == FALSE ) {
                theLog("Registry: can't open Environment key (Error %d)\n",
		       GetLastError());
                return FALSE;
            }
        }      
        CString sPath;
        theRegistry.GetValue("PATH", sPath);

        POSITION pos = m_mEnv.GetStartPosition();
        while( pos != NULL ) {
            CString *value;
            CString variable;
            // Gets key (Cstring) and value (col)
            m_mEnv.GetNextAssoc( pos, variable, value );
            if (_strnicmp((LPCTSTR)variable, "PATH", 4) == 0) {
                value->Replace("%PATH%", (LPCTSTR)sPath);
                CleanupPath(*value);
            }
            if (theRegistry.SetValue(variable, CRegistry::typeUnexpandedString, 
                                     (const PBYTE)(LPCTSTR)(*value), value->GetLength()) == FALSE) {
                theLog("Registry: can't create environment %s=%s (Error %d)\n", 
		       (LPCTSTR)variable, (LPCTSTR)value, GetLastError());
                theRegistry.Close();
                return FALSE;
            }
            theLog("Environment: %s = %s\n", (LPCTSTR)variable, (LPCTSTR)*value);
        }
        theRegistry.Close();

        // Commit changes immediately
        DWORD dwResult;
        ::SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                             (LPARAM) "Environment", SMTO_ABORTIFHUNG,
                             5000, &dwResult);

        CSingleLock theLock(&g_csCritSec);
        theStatus.bReboot = false;

        return TRUE;
    }
    else {
        // Because of WinME, we need to put everything (2 lines !) inside the autoexec.bat
        // Store everything in bin directory, file setvars.bat and in autoexec.bat
        CString sBinDir = ConcatPath(g_sInstallRootDir, "bin\\win32");
    
        // Modify autoexec.bat
        BOOL bRet = CommitW9x(TRUE);

        // Create setvars.bat file in bin/win32
        if (::CreateDirectoryPath((LPCTSTR)sBinDir) == FALSE) {
            theLog("Unable to create directory %s\nError, commit environment variable failed.\n", (LPCTSTR)sBinDir);
            return FALSE;
        }
        CString sBatName = ConcatPath(g_sInstallRootDir, "bin\\win32\\setvars.bat");
        FILE *f = fopen((LPCTSTR)sBatName, "w");
        if (f == NULL) {
            theLog("Unable to open file %s\nError, commit environment variable failed.\n", (LPCTSTR)sBatName);
            return FALSE;
        }
        theLog("Environment: creating %s\n", (LPCTSTR)sBatName);
        POSITION pos = m_mEnv.GetStartPosition();
        while( pos != NULL ) {
            CString *value;
            CString variable;
            // Gets key (Cstring) and value (col)
            m_mEnv.GetNextAssoc( pos, variable, value );
            if (_strnicmp((LPCTSTR)variable, "PATH", 4) == 0) {
                if (!sPreviousPath.IsEmpty()) {
                    value->Replace("%PATH%", (LPCTSTR)sPreviousPath);
                    CleanupPath(*value);
                }
            }
            fprintf(f, "set %s=%s\n", (LPCTSTR)variable, (LPCTSTR)(*value));
            theLog("Environment: %s = %s\n", (LPCTSTR)variable, (LPCTSTR)*value);
        }
        fclose(f);

        // Reboot
        CSingleLock theLock(&g_csCritSec);
        theStatus.bReboot = true;

        return bRet;
    }
    return TRUE;
}

// Removes things from the environment
BOOL Environment::Restore(char *vars[])
{
    if (g_bIsWindowsNT) {
        // Remove the environment variables from the CStringArray
        // Need to look locally ...
        CString sBinDir = ConcatPath(g_sInstallRootDir, "bin\\win32");
        CRegistry theRegistry;
        CString sPath;
        char **v;
        if (theRegistry.Connect(HKEY_CURRENT_USER) == FALSE) {
            theLog("Registry: can't connect to HKEY_CURRENT_USER (Error %d)\n",
                   GetLastError());
            return FALSE;
        }
        if (theRegistry.Open(TEXT("Environment"),
                             (CRegistry::CreatePermissions)CRegistry::permissionAllAccess) == FALSE ) {
            theLog("Registry: can't open Environment key (Error %d)\n",
                   GetLastError());
            return FALSE;
        }
        for (v = vars; v && *v; v++) {
            theRegistry.DeleteValue(*v);
        }
        theRegistry.GetValue("PATH", sPath);
        if (! sPath.IsEmpty()) {
            DeleteFromPath(sPath, sBinDir);
            if (theRegistry.SetValue("PATH", CRegistry::typeUnexpandedString, 
                                     (const PBYTE)(LPCTSTR)(sPath), sPath.GetLength()) == FALSE) {
                theLog("Registry: can't create environment PATH=%s (Error %d)\n", (LPCTSTR)sPath,
		       GetLastError());
            }
        }
        theRegistry.Close();
        // ... and globally
        if (g_bIsAdmin) {
            if (theRegistry.Connect(HKEY_LOCAL_MACHINE) == FALSE) {
                theLog("Registry: can't connect to HKEY_LOCAL_MACHINE (Error %d)\n",
                       GetLastError());
                return FALSE;
            }
            CString sSubKey(TEXT("SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"));
            if (theRegistry.Open(sSubKey,
                                 (CRegistry::CreatePermissions)CRegistry::permissionAllAccess) == FALSE ) {
                theLog("Registry: can't open Environment key (Error %d)\n",
                       GetLastError());
                return FALSE;
            }
            for (v = vars; v && *v; v++) {
                theRegistry.DeleteValue(*v);
            }
            theRegistry.GetValue("PATH", sPath);
            if (! sPath.IsEmpty()) {
                DeleteFromPath(sPath, sBinDir);
                if (theRegistry.SetValue("PATH", CRegistry::typeUnexpandedString, 
                                         (const PBYTE)(LPCTSTR)(sPath), sPath.GetLength()) == FALSE) {
                    theLog("Registry: can't create environment PATH=%s (Error %d)\n", (LPCTSTR)sPath,
                           GetLastError());
                }
            }
            theRegistry.Close();
        }
    
        {
            DWORD dwResult;
            ::SendMessageTimeout(HWND_BROADCAST, WM_SETTINGCHANGE, 0,
                                 (LPARAM) "Environment", SMTO_ABORTIFHUNG,
                                 5000, &dwResult);
        }
    }
  
    else {			// Win9x
        return CommitW9x(FALSE);
    }
    return TRUE;
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

