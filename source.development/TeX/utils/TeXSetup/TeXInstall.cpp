// TeXSetup.cpp: implementation of the TeXSetup class.
//
//////////////////////////////////////////////////////////////////////

#include <malloc.h>
#include "expat.h"
#include "stdafx.h"
#include "afxinet.h"
#include "regstr.h"
#include "Registry.h"
#include "Win32Util.h"
#include "FileDirectory.h"
#include "KeyValFile.h"
#include "Environment.h"
#include "TeXSetup.h"
#include "TeXSetupWizard.h"
#include "TeXInstall.h"
#include "gs32lib.h"
#include "resource.h"

extern "C" {
#include "windll.h"
}

extern CTeXSetupApp theApp;

#include <io.h>
#include <shlobj.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/* Where all information will be logged */
LogFile theLog;

SourceType g_eSourceType = SOURCE_CDROM;
InstallType g_eInstallType = INSTALL_FULL;

CString g_sDefaultRootDir;      /* The default place to install TeXLive */
CString g_sLocalSourceDir;	/* Source for TeX files, local in principle */
CString g_sRemoteSourceDir;	/* Source for extra files, remote in principle */

CString g_sSourceTPMDir;
CString g_sLocalTPMDir;
bool g_bRemoveSourceTPMDir = false;

CString g_sHomeDir;

CString g_sInstallRootDir;
CString g_sTexmfMain;
CString g_sTexmfLocal;
CString g_sTexmfExtra;
CString g_sTexmfHome;
CString g_sVarTexmf;
CString g_sVarTexFonts;
CString g_sWin32Dir;
CString g_sFpTeXDoc;
CString g_sModuleName;

CString g_sPreviousRoot;
CString g_sGlobalRegKey;
CString g_sUserRegKey;

unsigned long g_dwClusterSize;

unsigned long g_dwSpaceNeeded;
unsigned long g_dwSpaceAvailable;

unsigned long g_dwTPMNumber; // Number of tpm files in the texmf/tpm directory

CString g_sLastError;

bool g_bDryRun(false);
bool g_bAutomaticReboot(false);

bool g_bInstallDocumentation(true);
bool g_bInstallSource(false);
bool g_bInstallXEmTeX(false);
bool g_bAllUsers(false);

bool g_bQuickInstall(false);

bool g_bBuildLanguageDat(false);

bool g_bIsAdmin(false);
bool g_bIsWindowsNT(false);

bool g_bAddPackage(false);
CString g_sPackage;

CString g_sFolderName("TeXLive");;

CString g_sSchemeName;

CString g_sProxyAddress;
UINT g_uiProxyPort = 80;
NetIOType g_uiNetMethod = NetIOUnavail;

StatusData theStatus;
CCriticalSection g_csCritSec;

CString g_sTempPath;

Environment theEnvironment;

TPM *g_tpmRoot = 0;             // The root of the tree
TPMSet g_vSourceTPM;            // The map of available tpms
TPMSet g_vInstalledTPM;         // The map of installed tpms
TPMSet g_vSchemeTPM;            // The map of installation schemes

TPMSet g_vFormatsToBuild;   // The map of formats to build
TPMSet g_vLanguageDat; // The map of language.xx.dat segments

TPMSet g_vUpdMap;        // Map lines for updmap.cfg file
TPMSet g_vUpdMixedMap;   // MixedMap lines for updmap.cfg file
TPMSet g_vDvipsMap;      // Map lines for dvips config.ps file
TPMSet g_vPdfTeXMap;     // Map lines for pdftex pdftex.cfg file
TPMSet g_vDvipdfmMap;    // Map lines for dvipdfm config file

BOOL CountTPM(void *action_parameter, const CString &filename)
{
    DWORD *dw = reinterpret_cast<DWORD *>(action_parameter);
    if (filename.GetLength() > 4 
        &&_stricmp((LPCTSTR)filename + filename.GetLength() - 4, ".tpm") == 0)
        (*dw)++;
    return TRUE;
}

BOOL LoadTPM(void *action_parameter, const CString &filename)
{
    BOOL ret = TRUE;
    int iLength;
    TPMSet *map = reinterpret_cast<TPMSet *>(action_parameter);
    if ((iLength = filename.GetLength()) > 4 
        && _stricmp((LPCTSTR)filename + filename.GetLength() - 4, ".tpm") == 0) {

        CString sKey = filename.Left(iLength - 4);
//        AfxMessageBox(CString("Loading ") + filename, MB_OK);
        CString sPrefix = sKey;
        GetParentDirectory(sPrefix);
        GetParentDirectory(sPrefix);
        sKey.Replace(sPrefix, NULL);

        // Ignore -static packages !
        if (sKey.Right(7) == "-static"
            && sKey.Left(6) == "binary") {
            return TRUE;
        }

        TPM *tpm = 0;
        if (! map->Lookup(sKey, tpm)) {
            tpm = new TPM();
            tpm->SetKey(sKey);
        }

        CSingleLock singleLock(&g_csCritSec, TRUE);
        theStatus.tpm = tpm;
        theStatus.sName = (LPCTSTR)tpm->GetKey();
        singleLock.Unlock();

        if (tpm->Init(filename)) {
            (*map)[sKey] = tpm;
        }
        else {
            delete tpm;
            // ret = FALSE;
        }

        singleLock.Lock();
        theStatus.dwOverallDone++;
        singleLock.Unlock();
    }
#if 0
    else {
        AfxMessageBox("Skip " + filename);
    }
#endif
    ret = ret && ! GetCancelFlag();
    return ret;
}

BOOL FindHomeDir(CString &sHome)
{
    char tmp[_MAX_PATH], homeEnv[_MAX_PATH];

    sHome.Empty();
    tmp[0] = '\0';

    if ((GetEnvironmentVariable("HOME", tmp, sizeof(tmp)) == 0) && g_bIsWindowsNT) {
        strcpy(tmp, "%HOMEDRIVE%%HOMEPATH%");
    }

    if (tmp[0] != '\0' && ExpandEnvironmentStrings(tmp, homeEnv, sizeof(homeEnv)) != 0) {
            sHome = homeEnv;
            return TRUE;
    }
		
    // This will probably give the wrong value
    if (GetSpecialFolderPath(CSIDL_PERSONAL, tmp)) {
        sHome = tmp;
    }

    return (sHome.IsEmpty() ? FALSE : TRUE);
}

bool ReadTPM(const CString &sTPMDir, TPMSet &mapTPM)
{
    //    CString sOldTPMDir;
    
    // FIXME: should we bother about non-existent directories ???
    if (!DirectoryExists(sTPMDir)) {
        MessageBox(NULL, "Error: can't find the " 
                   + sTPMDir +  " TPM directory.", NULL, MB_OK | MB_ICONSTOP);
        return false;
        // return true;
      }
    
    // Reading packages descriptions	
    CFileDirectory cTPMdir;
    // Count them first
    FILE_ACTION_FUNCTION fa_fn = &CountTPM;
    g_dwTPMNumber = 0;
    if (cTPMdir.Open(sTPMDir)) {
        cTPMdir.Read(fa_fn, &g_dwTPMNumber);
    }	
    // Load them
    fa_fn = &LoadTPM;
    if (cTPMdir.Open(sTPMDir)) {
        cTPMdir.Read(fa_fn, &mapTPM);
    }	
    cTPMdir.Close();
    
    if (GetCancelFlag())
        return false;
    
    return true;
}

char *lpcTPMsubdirs[] = {
    "binary", "collection", "package", "support"
};

void SelectDefault(TPMSet &tpmMap)
{
    // Call this "DefaultSelection" and make a function
    // Just ensure everything is at default
    
    g_dwSpaceNeeded = 0;
    
    POSITION pos;
    CString sKey;
    TPM *tpm;
    pos = tpmMap.GetStartPosition();
    while (pos) {
        tpmMap.GetNextAssoc(pos, sKey, tpm);
        if (tpm->IsDefault())
            tpm->Select();
    }
}

void InitializeTPM(const CString &sTPMDir, TPMSet &tpmMap, TPMSet *tpmSchemeMap)
{
    CSingleLock singleLock(&g_csCritSec, TRUE);
    theStatus.dwOverallDone = 0;
    theStatus.nOpId = IDS_NOW_PARSING;
    theStatus.bReady = false;
    theStatus.bError = false;
    singleLock.Unlock();

    CString sKey;
    TPM *tpm;
  
    bool bSuccess = true;	
    int i, dwTpm = 0;
    char **s;

    // First remove anything from a previous try
    TPMMapCleanup(tpmMap);

    for (s = lpcTPMsubdirs, i = 0;
         bSuccess && i < sizeof(lpcTPMsubdirs)/sizeof(lpcTPMsubdirs[0]);
         s++, i++) {
        bSuccess = bSuccess 
            && ReadTPM(ConcatPath(sTPMDir, *s), tpmMap);
        dwTpm += g_dwTPMNumber;
    }

    bSuccess = true; // bSuccess && (dwTpm > 0);

    if (bSuccess) {    
        // Build 'RequiredBy'
        POSITION pos;
        pos = tpmMap.GetStartPosition();
        while(pos) {
            tpmMap.GetNextAssoc(pos, sKey, tpm);
            tpm->AddRequiredBy();
        }
    
        // Check for referenced TPMs that
        // have not been initialized.
    
    }
    else {
        singleLock.Lock();
        theStatus.bError=true;
        singleLock.Unlock();
    }

    if (tpmSchemeMap != NULL) {
        TPMMapCleanup(*tpmSchemeMap);
        // Reading schemes
        bSuccess = bSuccess 
            && ReadTPM(ConcatPath(sTPMDir, "scheme"), *tpmSchemeMap);
        bSuccess = true;
        if (!bSuccess) {
            singleLock.Lock();
            theStatus.bError=true;
            singleLock.Unlock();
        }
    }

    singleLock.Lock();
    theStatus.bReady = true;
    singleLock.Unlock();
}

/*
  This function will check for other tex versions and environment
  variables. It will report any problem found.
*/
void SanityCheck()
{
    char path[_MAX_PATH], *fp, **s;
    bool bProgramFound = false, bEnvVarFound = false;
    char *tex_programs[] = { "tex.exe", "mf.exe", "tex.bat", "mf.bat", "latex.bat" };
    char *tex_envvars[] = { "BIBINPUTS", "BSTINPUTS", "MFINPUTS", "TEXINPUTS", "MAKETEXPK", 
                            "MAKETEX_BASE_DPI", "PKFONTS", "TEXFONTS", "TEXINPUTS", "TEXPKS", 
                            "TEXPXLS", "TFMFONTS", "TEXMFMAIN", "TEXMFLOCAL", "HOMETEXMF", 
                            "VARTEXMF", "TEXMF", "VARTEXFONTS", "TEXMFDBS", "MPINPUTS", 
                            "MFTINPUTS", "TEXFORMATS", "MFBASES", "MPMEMS", "TEXPOOL", 
                            "MFPOOL", "MPPOOL", "VFFONTS", "GFFONTS", "TEXFONTSMAPS", 
                            "TEXPSHEADERS", "T1FONTS", "AFMFONTS", "TTFONTS", "TTF2TFMINPUTS",
                            "T42FONTS", "MISCFONTS", "TEXCONFIG", "INDEXSTYLE", "TRFONTS", 
                            "MPSUPPORT", "MIMELIBDIR", "MAILCAPLIBDIR", "TEXDOCS", "TEXSOURCES",
                            "DVIPSHEADERS", "GLYPHFONTS", "TEXPICTS", "DVIPSFONTS", 
                            "DVILJFONTS" };
    int i;
    // Fix against short pathnames vs long pathnames
    char lpShortPathName[_MAX_PATH];
    if (GetShortPathName((LPCTSTR)g_sInstallRootDir, lpShortPathName, sizeof(lpShortPathName)) == 0) {
        lpShortPathName[0] = '\0';
    }

    for (i = 0, s = tex_programs; i < sizeof(tex_programs)/sizeof(char *); i++, s++) {
        if (SearchPath(getenv("PATH"), *s, NULL, sizeof(path), path, &fp) != 0
            && _strnicmp((LPCTSTR)g_sInstallRootDir, path, strlen((LPCTSTR)g_sInstallRootDir)) != 0
            && (*lpShortPathName == '\0' || _strnicmp(lpShortPathName, path, strlen(lpShortPathName)) != 0)) {
            bProgramFound = true;
            theLog("Warning: Found this conflicting program : %s\n", path);
        }
    }

    for (i = 0, s = tex_envvars; i < sizeof(tex_envvars)/sizeof(char *); i++, s++) {
        if (getenv(*s) != NULL) {
            bEnvVarFound = true;
            theLog("Warning: Found this conflicting environment variable: %s\n", *s);
        }
    }

#if 0
    /* Do we need it at all ? It will be overwritten anyway, won't it ? */
    /* Check TEXMFCNF separately */
    char *s = getenv("TEXMFCNF");
    if (s != NULL) {
        CString sRoot(g_sInstallRootDir);
        sRoot.Replace('\\', '/');
        if (_strnicmp(s, (LPCTSTR)sRoot, sRoot.GetLength()) != 0) {
            theLog("Warning: Found this conflicting variable: TEXMFCNF\n", s);
            bEnvVarFound = true;
        }
    }
#endif

    if (bProgramFound) {
        if (!g_bQuickInstall) {
            MessageBox(NULL,
                       "I have found that you have at least one\r\nconflicting program in your PATH.\r\nPlease, check for any older TeX version on your system\r\nand remove it.\r\nMore information in the log file.", 
                       "Conflict", MB_OK | MB_ICONEXCLAMATION);
        }
        theLog("Warning: you have conflicting programs in your PATH. Please, consider editing\n\
your PATH to avoid running the wrong program.\n");
    }

    if (bEnvVarFound) {
        if (!g_bQuickInstall) {
            MessageBox(NULL,
                       "I have found that you have at least one\r\nconflicting variable in your environment.\r\nPlease, check for any older TeX version on your system\r\nand remove it.\r\nMore information in the log file.",
                       "Conflict", MB_OK | MB_ICONEXCLAMATION);
        }
        theLog("Warning: you have environment variables that can conflict with reliable use\n\
of the TeXLive software. If you set them intentionally, please consider editing your texmf.cnf\n\
file instead.\n");
    }
        
}

/*
 */
BOOL EarlyInitialize(const CTeXSetupCommandLineInfo &cmdinfo)
{
    // Setup a temp path
    char buffer[_MAX_PATH], *fp;

    if (GetTempPath(sizeof(buffer), buffer) == 0
        || ! DirectoryExists(buffer)) {
        if (GetWindowsDirectory(buffer, sizeof(buffer)) == 0) {
            return false;
        }
        g_sTempPath = ConcatPath(CString(buffer), "TEMP");
        if (! DirectoryExists(g_sTempPath))
            CreateDirectoryPath(g_sTempPath);
        if (! DirectoryExists(g_sTempPath))
            return false;
    }
    else {
        g_sTempPath = buffer;
    }

    // Other variables
    g_bIsAdmin = ::IsAdmin();
    g_bIsWindowsNT = ::IsWindowsNT();

    // By default for all users
    g_bAllUsers = (g_bIsWindowsNT ? g_bIsAdmin : true);

    // Home Directory
    FindHomeDir(g_sHomeDir);

    // Default registry key
    g_sGlobalRegKey = ConcatPath( TEXLIVE_GLOBAL_REGENTRY, TEXLIVE_VERSION, '\\');
    g_sUserRegKey = ConcatPath( TEXLIVE_USER_REGENTRY, TEXLIVE_VERSION, '\\');

    // Menu name
    g_sFolderName = (cmdinfo.m_sFolderName.IsEmpty() ? TEXLIVE_FOLDERNAME : cmdinfo.m_sFolderName);

    // Default method for net access
    g_uiNetMethod = cmdinfo.m_uiNetMethod;
    g_uiProxyPort = 80;
    g_sProxyAddress = "";

    g_eInstallType = INSTALL_FULL;
    
    // Default parameters for --add-package option
    if (cmdinfo.m_bAddPackage) {
        g_eInstallType = INSTALL_MAINTENANCE;
        g_sPackage = cmdinfo.m_sPackage;
    }

    if (! cmdinfo.m_sScheme.IsEmpty()) {
        if (g_eInstallType != INSTALL_FULL) {
            AfxMessageBox("Conflicting command line options: you are requesting\r\ntwo different modes of installation.", MB_ICONSTOP);
            return false;
        }
        g_sSchemeName = cmdinfo.m_sScheme;
    }

    // Maintenance option
    if (cmdinfo.m_bMaintenance) {
        if (g_eInstallType != INSTALL_FULL) {
            AfxMessageBox("Conflicting command line options: you are requesting\r\ntwo different modes of installation.", MB_ICONSTOP);
            return false;
        }
        g_eInstallType = INSTALL_MAINTENANCE;
    }

    // Uninstall option
    if (cmdinfo.m_bUninstall) {
        if (g_eInstallType != INSTALL_FULL) {
            AfxMessageBox("Conflicting command line options: you are requesting\r\ntwo different modes of installation.", MB_ICONSTOP);
            return false;
        }
        g_eInstallType = INSTALL_UNINSTALL;
    }

    // Run without installing anything
    g_bDryRun = cmdinfo.m_bDryRun;
    // Run without prompt
    g_bQuickInstall = cmdinfo.m_bQuickInstall;

    // Install packages documentation
    g_bInstallDocumentation = cmdinfo.m_bWithDoc;
    // Install packages source
    g_bInstallSource = cmdinfo.m_bWithSource;
    // Install xemtex support files
    g_bInstallXEmTeX = cmdinfo.m_bXEmTeX;

    g_sFolderName = TEXLIVE_FOLDERNAME;
    g_sLocalSourceDir.Empty();
    g_sRemoteSourceDir = "ftp://ftp.dante.de/pub/fptex/current";

    // Default directory for installation
    if (! g_bAllUsers) {
        g_sDefaultRootDir = (g_sHomeDir.IsEmpty() ? "c:\\" : g_sHomeDir);
    }
    else {
        if ((GetDllVersion("shell32.dll") >= PACKVERSION(5,0)
             || GetDllVersion("shlwapi.dll") >= PACKVERSION(5,0))
            && GetSpecialFolderPath(CSIDL_PROGRAM_FILES, buffer))
            g_sDefaultRootDir = buffer;
        else {
            char path[_MAX_PATH];
            if (GetSystemDirectory(path, sizeof(path))) {
                // cut after x:
                path[2] = '\0';
                g_sDefaultRootDir = CString(path) + "\\Program Files";
            }
            if (! DirectoryExists(g_sDefaultRootDir)) {    
                g_sDefaultRootDir = "c:\\Program Files";
            }
        }
    }
    g_sDefaultRootDir = ConcatPath(g_sDefaultRootDir, g_sFolderName);

    // Look for the previous install path into the registry
    CRegistry theRegistry;
    HKEY hkRegRoot = (g_bAllUsers ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER);
    CString sRegKey = (g_bAllUsers ? g_sGlobalRegKey : g_sUserRegKey);
    if (theRegistry.Connect(hkRegRoot) == TRUE) {
        CString sTmp;
        if (theRegistry.Open(sRegKey) == TRUE) {
            theRegistry.GetValue("InstallRootDir", sTmp);
            if (! sTmp.IsEmpty()) {
                g_sInstallRootDir = sTmp;
            }
            theRegistry.GetValue("FolderName", sTmp);
            if (! sTmp.IsEmpty()) {
                g_sFolderName = sTmp;
            }
            theRegistry.GetValue("LocalSourceDir", sTmp);
            if (! sTmp.IsEmpty()) {
                g_sLocalSourceDir = sTmp;
            }
            theRegistry.GetValue("RemoteSourceDir", sTmp);
            if (! sTmp.IsEmpty()) {
                g_sRemoteSourceDir = sTmp;
            }
        }
        theRegistry.Close();
    }

    // Look for an installed version without the registry key, but using the PATH
    // SearchPath will find the kpathsea.dll in the current directory!
    if (g_sInstallRootDir.IsEmpty()
        && SearchPath(getenv("PATH"), "tl80kpse.dll", 
                      NULL, sizeof(buffer), buffer, &fp) != 0) {        
        int loc;
        CString sRoot(buffer);
        if ((loc = sRoot.Find("bin\\win32")) >= 0
            || (loc = sRoot.Find("bin\\i586-win32")) >= 0
            || (loc = sRoot.Find("bin")) >= 0) {
            sRoot = sRoot.Left(loc-1);
        }
        else {
            // c:\\TeX\\bin\\win32\\kpathsea.dll
            ::GetParentDirectory(sRoot);
            ::GetParentDirectory(sRoot);
            ::GetParentDirectory(sRoot);
        }
        g_sInstallRootDir = sRoot;
    }

    if (! cmdinfo.m_sFolderName.IsEmpty()) {
        g_sFolderName = cmdinfo.m_sFolderName;
    }
    if (! cmdinfo.m_sInstallRootDir.IsEmpty()) {
        g_sInstallRootDir = cmdinfo.m_sInstallRootDir;
    }
    if (! cmdinfo.m_sRemoteSourceDir.IsEmpty()) {
        g_sRemoteSourceDir = cmdinfo.m_sRemoteSourceDir;
    }

    if (g_eInstallType == INSTALL_FULL || g_sLocalSourceDir.IsEmpty()) {
        // Guess Local Source Directory only if FULL installation
        // in maintenance mode, value of full installation should have been 
        // kept in the registry.
        // Caveat: we can run TeXSetup to --add-package without
        // any previous installation.
        CString dummyZipDir;
        //    if (g_sLocalSourceDir.IsEmpty()) {
        // Get the path from which we run
        DWORD dummyModuleSize;
        ::GetExecutableDirectory(g_sWin32Dir, g_sModuleName, dummyModuleSize);
        // Finding root of distribution
        dummyZipDir = ConcatPath(g_sWin32Dir, "package");
        // if running from bin\\win32 directory, probably
        // off the cd or maintenance mode.
        if (_stricmp(g_sWin32Dir.Right(10), "bin\\win32\\") == 0) {
            g_sLocalSourceDir = g_sWin32Dir;
            ::GetParentDirectory(g_sLocalSourceDir);
            ::GetParentDirectory(g_sLocalSourceDir);
            g_sWin32Dir = ConcatPath(g_sLocalSourceDir, "setupw32");
            g_eSourceType = SOURCE_CDROM;
        }
        // else if fptex downloaded distribution
        else if (DirectoryExists(dummyZipDir)) {
            g_sLocalSourceDir = g_sWin32Dir;
            g_sWin32Dir = ConcatPath(g_sLocalSourceDir, "setupw32");
            g_eSourceType = SOURCE_ZIP; // Meaningful ???
        }
        // else fptex internet distribution
        else {
            // None of the previous, lets assume it is a network installation
            g_sLocalSourceDir = g_sTempPath; // FIXME: check that this is OK !
            g_sWin32Dir = ConcatPath(g_sRemoteSourceDir, "setupw32");
            g_eSourceType = SOURCE_INTERNET;
        }
        // Remove terminating '\\' if needed
        if (g_sLocalSourceDir.Right(1) == "\\"
            && g_sLocalSourceDir.Right(2) != ":\\")
            g_sLocalSourceDir = g_sLocalSourceDir.Left(g_sLocalSourceDir.GetLength() - 1);
    }
    else {
        // Get the path from which we run
        DWORD dummyModuleSize;
        CString dummyZipDir;
        // We need the module name in case of uninstall
        ::GetExecutableDirectory(g_sWin32Dir, g_sModuleName, dummyModuleSize);
        dummyZipDir = ConcatPath(g_sLocalSourceDir, "package");
        g_sWin32Dir = ConcatPath(g_sLocalSourceDir, "setupw32");
        if (DirectoryExists(dummyZipDir))
            g_eSourceType = SOURCE_ZIP;
        else
            g_eSourceType = SOURCE_CDROM;
    }
    
    if (! cmdinfo.m_sLocalSourceDir.IsEmpty()) {
        g_sLocalSourceDir = cmdinfo.m_sLocalSourceDir;
    }

    if (g_sInstallRootDir.IsEmpty()) {
        if (g_eInstallType == INSTALL_UNINSTALL) {
            ::MessageBox(NULL, "You can't `uninstall' without installing first !",
                         "TeXSetup Error", MB_ICONERROR);
            return false;
        }
        else if (g_eInstallType == INSTALL_MAINTENANCE && g_sPackage.IsEmpty()) {
            // If g_sPackage has been given, hope that the user knows what he is doing
            ::MessageBox(NULL, "You can't run `maintenance' mode without installing first !",
                         "TeXSetup Error", MB_ICONERROR);
            return false;
        }
        else {
            // This one won't be empty !
            g_sInstallRootDir = g_sDefaultRootDir;
        }
    }
    
    SetRootDir(g_sInstallRootDir);

    KeyValFile kvTexmfCnf;
    CString sTexmfCnf = ConcatPath(getenv("TEXMFCNF"), "texmf.cnf");
    if (! FileExists(sTexmfCnf)) {
        sTexmfCnf = ConcatPath(g_sInstallRootDir, "texmf-var\\web2c\\texmf.cnf", '\\');
        if (! FileExists(sTexmfCnf)) {
            sTexmfCnf.Empty();
        }
    }
    
    if (!sTexmfCnf.IsEmpty() && kvTexmfCnf.Open(sTexmfCnf)) {
        if (kvTexmfCnf.Lookup("TEXMFMAIN", g_sTexmfMain)) {
            g_sTexmfMain.Replace("$SELFAUTOPARENT", g_sInstallRootDir);
            g_sTexmfMain.Replace('/', '\\');
        }
        else {
            g_sTexmfMain.Empty();
        }
        
        if (kvTexmfCnf.Lookup("TEXMFLOCAL", g_sTexmfLocal)) {
            g_sTexmfLocal.Replace("$SELFAUTOPARENT", g_sInstallRootDir);
            g_sTexmfLocal.Replace('/', '\\');
        }
        else {
            g_sTexmfLocal.Empty();
        }
        
        if (kvTexmfCnf.Lookup("TEXMFEXTRA", g_sTexmfExtra)) {
            g_sTexmfExtra.Replace("$SELFAUTOPARENT", g_sInstallRootDir);
            g_sTexmfExtra.Replace('/', '\\');
        }
        else {
            g_sTexmfExtra.Empty();
        }
        
        if (kvTexmfCnf.Lookup("HOMETEXMF", g_sTexmfHome)) {
            if (! g_sHomeDir.IsEmpty())
                g_sTexmfHome.Replace("$HOME", g_sHomeDir);
            g_sTexmfHome.Replace("$SELFAUTOPARENT", g_sInstallRootDir);
            g_sTexmfHome.Replace('/', '\\');
        }
        else {
            g_sTexmfHome.Empty();
        }
        
        if (kvTexmfCnf.Lookup("VARTEXMF", g_sVarTexmf)) {
            g_sVarTexmf.Replace("$SELFAUTOPARENT", g_sInstallRootDir);
            g_sVarTexmf.Replace('/', '\\');
        }
        else {
            g_sVarTexmf.Empty();
        }
        
        if (kvTexmfCnf.Lookup("VARTEXFONTS", g_sVarTexFonts)) {
            if (! g_sVarTexmf.IsEmpty())
                g_sVarTexFonts.Replace("$VARTEXMF", g_sVarTexmf);
            g_sVarTexFonts.Replace("$SELFAUTOPARENT", g_sInstallRootDir);
            g_sVarTexFonts.Replace('/', '\\');
        }
        else {
            g_sVarTexFonts.Empty();
        }
    }

    if (! cmdinfo.m_sTexmfMainDir.IsEmpty()) {
        g_sTexmfMain = cmdinfo.m_sTexmfMainDir;
    }
    if (! cmdinfo.m_sTexmfLocalDir.IsEmpty()) {
        g_sTexmfLocal = cmdinfo.m_sTexmfLocalDir;
    }
    if (! cmdinfo.m_sTexmfHomeDir.IsEmpty()) {
        g_sTexmfHome = cmdinfo.m_sTexmfHomeDir;
    }
    if (! cmdinfo.m_sTexmfExtraDir.IsEmpty()) {
        g_sTexmfExtra = cmdinfo.m_sTexmfExtraDir;
    }
    if (! cmdinfo.m_sVarTexmfDir.IsEmpty()) {
        g_sVarTexmf = cmdinfo.m_sVarTexmfDir;
    }
    if (! cmdinfo.m_sVarTexFontsDir.IsEmpty()) {
        g_sVarTexFonts = cmdinfo.m_sVarTexFontsDir;
    }

    UpdateSize();
    
    return true;
}

void TPMMapCleanup(TPMSet &tpmMap)
{
    POSITION pos;
    TPM *tpm;
    CString sKey;
    pos = tpmMap.GetStartPosition();
    while(pos) {
        tpmMap.GetNextAssoc(pos, sKey, tpm);
        tpmMap.RemoveKey(sKey);
        delete tpm;
    }
}

void TeXInstallCleanUp()
{
    // FIXME : needed ???
    TPMMapCleanup(g_vSourceTPM);
    TPMMapCleanup(g_vInstalledTPM);
}

bool GetLogFile()
{

    CString sConfigDir;
    if (g_bDryRun
        || !g_bAllUsers
        || g_eInstallType == INSTALL_UNINSTALL) {
        sConfigDir = g_sTempPath;
    }
    else {
        sConfigDir = ConcatPath(g_sInstallRootDir, "texmf-var/web2c");
        if (! CreateDirectoryPath(sConfigDir))
            return (false);
    }

    // make the log path
    CTime t = CTime::GetCurrentTime();
    CString sLogName = ConcatPath(sConfigDir, t.Format("setup-%Y-%m-%d-%H-%M") + ".log");
  
  
    return theLog.OpenLog(sLogName, &g_csCritSec);

}

void DoPrepareInstallation()
{
    TPM *tpm;
    CString sKey, sTmp;

    // Get and Open Log File
    if (! GetLogFile()) {
        // Should we really abort ?
        FatalError (IDS_CANT_OPEN_LOGFILE);
    }

    // log general info
    CTime t = CTime::GetCurrentTime();
    theLog ("TeXLive %s Setup Wizard Report\n\n", TEXLIVE_VERSION);
    theLog ("Date: %s\n", t.Format("%A, %B %d, %Y"));
    theLog ("Time: %s\n", t.Format("%H:%M:%S"));

    theLog ("OS   Version: %s\n", g_sOsVersion);
    GetDllVersion("comctl32.dll", true);
    GetDllVersion("comdlg32.dll", true);
    GetDllVersion("shell32.dll", true);
    GetDllVersion("shlwapi.dll", true);
    GetDllVersion("shdocvw.dll" , true);
    theLog ("DLLs Versions:\n%s\n",g_sDllVersion);
    if (g_bIsAdmin) {
        theLog ("The user is a power user or an administrator.\n");
    }
    else {
        theLog ("The user is a normal user.\n");
    }

    if (g_bInstallXEmTeX) {
        theLog ("XEmTeX support files will be installed.\n");
    }
    
    theLog ("\n	Source	    Dir: %s\n", (const char *) g_sLocalSourceDir);
    theLog ("\n	Root	    Dir: %s\n", (const char *) g_sInstallRootDir);
    theLog ("\n	Main  Texmf Dir: %s\n", (const char *) g_sTexmfMain);
    theLog ("\n	Local Texmf Dir: %s\n", (const char *) g_sTexmfLocal);
    theLog ("\n	Extra Texmf Dir: %s\n", (const char *) g_sTexmfExtra);
    theLog ("\n	Home  Texmf Dir: %s\n", (const char *) g_sTexmfHome);
    theLog ("\n	Var   Texmf Dir: %s\n", (const char *) g_sVarTexmf);
    theLog ("\n	Var   Fonts Dir: %s\n", (const char *) g_sVarTexFonts);
    theLog ("\nSelected Packages:\n");

    if (g_eInstallType != INSTALL_UNINSTALL) {
        int col = 0;
        POSITION pos = g_vSourceTPM.GetStartPosition();
        while (pos) {
            g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
            sTmp.Format("%s(%ld),", sKey, tpm->GetSize(false));
            if (tpm->IsSelected()) {
                col += sTmp.GetLength();
                if (col >= 80) {
                    theLog("\n");
                    col = sTmp.GetLength();
                }
                theLog((LPCTSTR)sTmp);
            }
        }
    }
}

void DoPrepareCopying()
{
    // Set protection on files if under WinNT
    // Doing this here should avoid to do it 
    // recursively later : protections are inherited by default
    if (g_bIsWindowsNT && g_bIsAdmin) {
        HANDLE hLog = 0;

        theLog("Changing permissions...\n");
        hLog = theLog.getHandle();
	
        theLog("Creating %s directory ", (LPCTSTR)g_sInstallRootDir);
        CreateDirectoryPath(g_sInstallRootDir);
        theLog("and granting everyone read access to it.\n");
        if (! GrantPermissions((LPCTSTR)g_sInstallRootDir, "everyone", "read", FALSE, hLog)) {
            // Appropriate message
            theLog("Warning: Fail to grant everyone read access to %s (Error %ld).\n", 
                   (LPCTSTR)g_sInstallRootDir, GetLastError());
        }
        if (! g_sVarTexmf.IsEmpty()) {
            theLog("Creating %s directory ", (LPCTSTR)g_sVarTexmf);
            CreateDirectoryPath(g_sVarTexmf);
            theLog("and granting everyone full access to it.\n");
            if (! GrantPermissions((LPCTSTR)g_sVarTexmf, "everyone", "full", FALSE, hLog)) {
                // Appropriate message
                theLog("Warning: Fail to grant everyone full access to %s (Error %ld).\n", 
                       (LPCTSTR)g_sVarTexmf, GetLastError());
            }
        }
        // Beware of Win2k : files are copied/moved together with their permissions.
        // We need to setup a TEMP directory specific for TeX.
        CString sTexmfTemp = ConcatPath(g_sInstallRootDir, "temp");
        theLog("Creating %s directory ", (LPCTSTR)sTexmfTemp);
        CreateDirectoryPath(sTexmfTemp);
        theLog("and granting everyone full access to it.\n");
        if (! GrantPermissions((LPCTSTR)sTexmfTemp, "everyone", "full", FALSE, hLog)) {
            // Appropriate message
            theLog("Warning: Fail to grant everyone full access to %s (Error %ld).\n", 
                   (LPCTSTR)sTexmfTemp, GetLastError());
        }
        SetEnvironmentVariable("TEMP", (LPCTSTR)sTexmfTemp);
        theLog("Setting temporarily TEMP = %s\n", (LPCTSTR)sTexmfTemp);
    }

    // Make sure we have all the tpm directories
    CreateDirectoryPath(ConcatPath(g_sTexmfMain, "tpm/binary"));
    CreateDirectoryPath(ConcatPath(g_sTexmfMain, "tpm/collection"));
    CreateDirectoryPath(ConcatPath(g_sTexmfMain, "tpm/package"));
    CreateDirectoryPath(ConcatPath(g_sTexmfMain, "tpm/support"));

    // Removed ! The GS we are providing is meant
    // to work only with our xemacs/tex
    // Check the Ghostscript thing
    // CheckForGs();
}

void DoDownloadFiles()
{
    POSITION pos;
    TPM *tpm;
    CString sKey, sTmp;
    DWORD dwSize;
    DWORD dwOverall; // used to synchronise overall size.

    CSingleLock singleLock(&g_csCritSec, TRUE);
    theStatus.dwOverallDone = 0;
    theStatus.dwPackageDone = 0;
    theStatus.dwPackageSize = 0;
    theStatus.bError = false;
    theStatus.bNewTPM = false;
    theStatus.bReady = false;
    theStatus.bReboot = false;
    theStatus.bTerminate = false;
    singleLock.Unlock();

    // Loop over all packages
    pos = g_vSourceTPM.GetStartPosition();
    while (pos) {
        g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
	// Initialize the progress bar	
        singleLock.Lock();
        dwOverall = theStatus.dwOverallDone;
        theStatus.bNewTPM = true;
        theStatus.tpm = tpm;
        theStatus.dwPackageDone = 0;
        theStatus.dwPackageSize = tpm->GetSize(false);
        singleLock.Unlock();

        if (tpm->IsSelected()) {
            tpm->Download();
        }

	// Increment the progress bar
        singleLock.Lock();
        theStatus.dwPackageDone = theStatus.dwPackageSize;
        theStatus.dwOverallDone = dwOverall + tpm->GetSize(false);
        singleLock.Unlock();
    }	
}

void DoCopyFiles()
{
    POSITION pos;
    TPM *tpm;
    CString sKey, sTmp;
    DWORD dwSize;
    DWORD dwOverall; // used to synchronise overall size.

    CSingleLock singleLock(&g_csCritSec, TRUE);
    theStatus.dwOverallDone = 0;
    theStatus.dwPackageDone = 0;
    theStatus.dwPackageSize = 0;
    theStatus.bError = false;
    theStatus.bNewTPM = false;
    theStatus.bReady = false;
    theStatus.bReboot = false;
    theStatus.bTerminate = false;
    singleLock.Unlock();

    // Recompute this size, unrounded this time
    g_dwSpaceNeeded = 0;
    pos = g_vSourceTPM.GetStartPosition();
    while (pos) {
        g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
        g_dwSpaceNeeded += tpm->GetSize(false);
    }

    // First, install  the special packages
    // configuration will occur after this step.
    for (TPMSpecialDesc *special = g_fnTPMSpecial;
         special->lpctstrTpmName != NULL;
         special++) {
        if (g_vSourceTPM.Lookup(special->lpctstrTpmName, tpm)
            && tpm->IsSelected()
            ) {
            singleLock.Lock();
            dwOverall = theStatus.dwOverallDone;
            theStatus.bNewTPM = true;
            theStatus.tpm = tpm;
            theStatus.dwPackageDone = 0;
            theStatus.dwPackageSize = tpm->GetSize(false);
            singleLock.Unlock();
            
            pfTpmInstall thePF = (special->pfInstall ? special->pfInstall : &TPM::Install);
            theLog("Installing support package %s\n", special->lpctstrTpmName);
            (tpm->*thePF)();
            
            // Increment the progress bar
            singleLock.Lock();
            theStatus.dwPackageDone = theStatus.dwPackageSize;
            theStatus.dwOverallDone = dwOverall + tpm->GetSize(false);
            singleLock.Unlock();
        }

    }

    // Loop over all packages
    pos = g_vSourceTPM.GetStartPosition();
    while (pos) {
        g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
	// Initialize the progress bar	
        singleLock.Lock();
        dwOverall = theStatus.dwOverallDone;
        theStatus.bNewTPM = true;
        theStatus.tpm = tpm;
        theStatus.dwPackageDone = 0;
        theStatus.dwPackageSize = tpm->GetSize(false);
        singleLock.Unlock();

        if (!tpm->IsInstalled())
            tpm->Install();

	// Increment the progress bar
        singleLock.Lock();
        theStatus.dwPackageDone = theStatus.dwPackageSize;
        theStatus.dwOverallDone = dwOverall + tpm->GetSize(false);
        singleLock.Unlock();
    }	

    // Remove unneeded tpm files
    if (g_bRemoveSourceTPMDir)
        RecursivelyRemove(g_sSourceTPMDir);

    // finish
    {
        CSingleLock (&g_csCritSec, TRUE);
        theStatus.bReady = true;
    }

}

void Finalize()
{
    // Put some key in the registry
    // FIXME : only if win32 selected
    if (! g_bDryRun && g_eInstallType == INSTALL_FULL) {
        CRegistry theRegistry;
        HKEY hkRegRoot = (g_bAllUsers ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER);
        CString sRegKey = (g_bAllUsers ? g_sGlobalRegKey : g_sUserRegKey);
        CString  sTeXSetup = ConcatPath(g_sInstallRootDir, "bin\\win32\\TeXSetup.exe --uninstall");
        CString sRegUninstKey = ConcatPath(REGSTR_PATH_UNINSTALL, g_sFolderName);
        
        if (theRegistry.Connect(hkRegRoot) == FALSE) {
            theLog("Registry: can't connect to %s (Error %d)\n",
                   (hkRegRoot == HKEY_LOCAL_MACHINE ? "HKEY_LOCAL_MACHINE" : "HKEY_CURRENT_USER"),
                   GetLastError());
        }
        else {
            if (theRegistry.Create(sRegKey) == FALSE ) {
                theLog("Registry: can't create TeXLive subkey (Error %d)\n",
                       GetLastError());
                goto exit_2;
            }
            if (theRegistry.SetValue("InstallRootDir", (LPCTSTR)g_sInstallRootDir ) == FALSE) {
                theLog("Registry: can't set Root value (Error %d)\n",
                       GetLastError());
                goto exit_2;
            }
            if (theRegistry.SetValue("FolderName", (LPCTSTR)g_sFolderName ) == FALSE) {
                theLog("Registry: can't set Folder value (Error %d)\n",
                       GetLastError());
                goto exit_2;
            }
            if (theRegistry.SetValue("LocalSourceDir", (LPCTSTR)g_sLocalSourceDir ) == FALSE) {
                theLog("Registry: can't set Local Source directory value (Error %d)\n",
                       GetLastError());
                goto exit_2;
            }
            if (theRegistry.SetValue("RemoteSourceDir", (LPCTSTR)g_sRemoteSourceDir ) == FALSE) {
                theLog("Registry: can't set Local Source directory value (Error %d)\n",
                       GetLastError());
                goto exit_2;
            }
            theRegistry.Close();
        }
        if (g_bAllUsers && theRegistry.Connect(HKEY_LOCAL_MACHINE) == TRUE) {
            // Set the Uninstall Key
            if (theRegistry.Create(sRegUninstKey) == FALSE ) {
                theLog("Registry: can't create %s Uninstall subkey (Error %d)\n",
                       (LPCTSTR)sRegUninstKey, GetLastError());
                goto exit_2;
            }
            if (theRegistry.SetValue(REGSTR_VAL_UNINSTALLER_DISPLAYNAME, "TeXLive" ) == FALSE) {
                theLog("Registry: can't set Uninstall Display Name value (Error %d)\n",
                       GetLastError());
                goto exit_2;
            }
            if (theRegistry.SetValue(REGSTR_VAL_UNINSTALLER_COMMANDLINE, sTeXSetup ) == FALSE) {
                theLog("Registry: can't set Uninstall Command Line value (Error %d)\n",
                       GetLastError());
                goto exit_2;
                
            }
        exit_2:
            theRegistry.Close();
        }
    }
}


static BOOL BuildLanguageDatHelper(void *action_parameter, const CString &filename)
{
    CString *s = reinterpret_cast<CString *>(action_parameter);
    FILE *f;
    CString basename = filename;
    basename.Replace('\\', '/');
    basename = basename.Right(basename.GetLength() - basename.ReverseFind('/') - 1);
    // If filename == language.us then prepend
    // else if filename == language.??.dat then append
    if (_stricmp((LPCTSTR)basename, "language.us") == 0) {
        CString sFore;
        if ((f = fopen((LPCTSTR)filename, "r")) != NULL) {
            int c = fgetc(f);
            while (c != EOF) {
                sFore = sFore + (char)c;
                c = fgetc(f);
            }
            fclose(f);
            *s = sFore + *s;
            theLog("The file `%s' has been added to `language.dat'\n", basename);
        }
        else {
            theLog("Error: can't open %s for reading (%s).\n", (LPCTSTR)filename, strerror(errno));
        }
    }
    // Poor man's matching
    else if (basename.GetLength() == 15
             && _strnicmp((LPCTSTR)basename, "language.", 9) == 0
             && _strnicmp(((LPCTSTR)basename)+11, ".dat", 4) == 0) {
        if ((f = fopen((LPCTSTR)filename, "r")) != NULL) {
            int c = fgetc(f);
            while (c != EOF) {
                *s = *s + (char)c;
                c = fgetc(f);
            }
            fclose(f);
            theLog("The file `%s' has been added to `language.dat'\n", basename);
        }
        else {
            theLog("Error: can't open %s for reading (%s).\n", (LPCTSTR)filename, strerror(errno));
        }
    }
    // else ignore it.
    return TRUE;
}

bool BuildLanguageDat()
{
    bool bRet = false;
    
    if (g_eInstallType == INSTALL_FULL) {
        g_bBuildLanguageDat = true;
    }
    else if (g_eInstallType == INSTALL_MAINTENANCE) {
        // Do we need to rebuild it ?
        POSITION pos;
        CString sKey, sTPMKey;
        TPM *tpm;
        pos = g_vLanguageDat.GetStartPosition();
        while (pos) {
            g_vLanguageDat.GetNextAssoc(pos, sKey, tpm);
            sTPMKey = tpm->GetKey();
            if (g_vSourceTPM.Lookup(sTPMKey, tpm) && tpm->IsInstalled()) {
                g_bBuildLanguageDat = true;
                theLog("The `language.dat' file needs to be rebuild because of `language.%s.dat.\n", 
                       (LPCTSTR)sKey);
                break;
            }
        }
    }

    // Stop if no language.dat segment has been added
    if (! g_bBuildLanguageDat)
        return bRet;

    if (g_bDryRun) {
        return true;
    }

    CString sLangDat, sSrc, sDest;
    sSrc = ConcatPath(g_sTexmfMain, "tex\\generic\\config");
    sDest = ConcatPath(g_sVarTexmf, "tex\\generic\\config");
    if (! DirectoryExists(sDest))
        CreateDirectoryPath((LPCTSTR)sDest);
    sDest = ConcatPath(sDest, "language.dat");
    CFileDirectory cTPMdir;
    FILE_ACTION_FUNCTION fa_fn = &BuildLanguageDatHelper;
    if (cTPMdir.Open(sSrc)) {
        cTPMdir.Read(fa_fn, &sLangDat);
    }
    // Write the language.dat file.
    FILE *f = fopen((LPCTSTR)sDest, "w");
    if (f != NULL) {
        fputs((LPCTSTR)sLangDat, f);
        fclose(f);
        theLog("The file `%s' has been built and written.\n", (LPCTSTR)sDest);
        bRet = true;
    }
    else {
        theLog("Error: can't open %s for writing (%s).\n", (LPCTSTR)sDest, strerror(errno));
        bRet = false;
    }

    return bRet;
}

#if 0
bool UnblockFormats()
{
    bool bRet = true;

    if (g_bDryRun) return bRet;

    KeyValFile kFmtUtilCnf;
    kFmtUtilCnf.Open(ConcatPath(g_sVarTexmf, "web2c\\fmtutil.cnf"));
    // FIXME: now we could unblock formats using g_vFormatsToBuild
    kFmtUtilCnf.RawReplace("#!#", "");
    kFmtUtilCnf.RawReplace("#!", "");
    kFmtUtilCnf.Commit();
    kFmtUtilCnf.Close();

    return bRet;
}
#endif

// Return short path name under W9x, else quoted or unquoted pathname
// if contains white space.
CString GetSafePathName(CString &path, bool bQuoted)
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
            short_path = CString(lpShortPathName);
    }
    
    return short_path;
}

void DoConfiguration()
{
    CString sKey;
    TPM *tpm;
    CSingleLock theLock(&g_csCritSec, TRUE);
    theStatus.dwOverallDone = 0;
    theStatus.dwPackageDone = 0;
    theStatus.bError = false;
    theStatus.bNewTPM = false;
    theStatus.bReady = false;
    theStatus.bTerminate = false;
    theStatus.tpm = 0;
    theStatus.nConfig = ::CFG_BEGIN;
    theLock.Unlock();

    // It will be a lengthy operation
    // AfxGetApp()->DoWaitCursor(1);

    // Remove the registry key except in maintenance mode
    // Beware: this is not called in uninstall mode !
    // FIXME : should we do this much earlier ???
    if (g_eInstallType == INSTALL_FULL) {
        CRegistry theRegistry;
        HKEY hkRegRoot = (g_bAllUsers ? HKEY_LOCAL_MACHINE : HKEY_CURRENT_USER);
        CString sRegKey = (g_bAllUsers ? g_sGlobalRegKey : g_sUserRegKey);
        if (theRegistry.Connect(hkRegRoot) == TRUE) {
            // Need to remove it in case of reinstallation
            if (theRegistry.DeleteKey(sRegKey) == TRUE) {
                theLog("Deleted registry key for TeXLive %s.\n", TEXLIVE_VERSION);
            }
            theRegistry.Close();
        }
    }

    // Open the log file
    if (! theLog.OpenLog()) {
        FatalError (IDS_CANT_OPEN_LOGFILE);
        // Should we abort ?
    }
    
    // Modify the environment
    if (g_eInstallType == INSTALL_FULL) {
        // Not needed if we are running maintenance mode ...
        CString sTeXBinDir = ConcatPath(g_sInstallRootDir, "bin\\win32");
        CString sTexmfcnfDir = ConcatPath(g_sVarTexmf, "web2c");
        CString sPath;
        theEnvironment.GetValue("PATH", sPath);
        if (sPath.IsEmpty()) {
            theEnvironment.SetValue("PATH", GetSafePathName(sTeXBinDir, false) + ";%PATH%");
        }
        else {
            theEnvironment.SetValue("PATH", GetSafePathName(sTeXBinDir, false) + ";" + sPath);
        }
        theEnvironment.SetValue("TEXMFCNF", sTexmfcnfDir);
        if (g_bIsWindowsNT && g_bAllUsers) {
            // Not needed if single user installation
            CString sTexmfTemp = ConcatPath(g_sInstallRootDir, "temp");
            theEnvironment.SetValue("TEXMFTEMP", sTexmfTemp);
        }
    }
    
    theLock.Lock();
    theStatus.nConfig = ::CFG_SYSTEM_DLLS;
    theLock.Unlock();
    
    for (TPMSpecialDesc *special = g_fnTPMSpecial;
         special->lpctstrTpmName != NULL;
         special++) {
        if (g_vSourceTPM.Lookup(special->lpctstrTpmName, tpm)
#if 0
            && tpm->IsSelected() // deported into the configuration function itself
            && tpm->IsInstalled()
#endif
            && special->pfConfigure != NULL) {
            theLock.Lock();
            theStatus.bNewTPM = true;
            theStatus.tpm = tpm;
            theStatus.dwPackageDone = 0;
            theStatus.dwPackageSize = tpm->GetSize(false);
            theLock.Unlock();
            
            theLog("Configuring package %s\n", special->lpctstrTpmName);
            (tpm->*(special->pfConfigure))();
        }

    }

    theLock.Lock();
    theStatus.nConfig = ::CFG_SUPPLEMENTARY_TOOLS;
    theLock.Unlock();
    
    // tex-basic should have been configured, but need
    // to rebuild language.dat
    BuildLanguageDat();
        
    // Rebuild the ls-R databases
    CString sWeb2cDir = ConcatPath(g_sVarTexmf, "web2c");
    CString sMktexlsrCmd = ConcatPath(g_sInstallRootDir, "bin\\win32\\mktexlsr.exe");
    CString sFmtutilCmd = ConcatPath(g_sInstallRootDir, "bin\\win32\\fmtutil.exe");
    CString sUpdMapCmd = ConcatPath(g_sInstallRootDir, "bin\\win32\\updmap.exe");
    CString sCmd, sTexmfTemp;
    // Sanity Checks against other tex installations
    SanityCheck();

    if (FileExists(sMktexlsrCmd)) {
        sMktexlsrCmd = GetSafePathName(sMktexlsrCmd);
        // Need to set PATH : fmtutil calls external programs !
        char pathBuf[8192];
        ::GetEnvironmentVariable("PATH", pathBuf, sizeof(pathBuf));
        CString sPath(pathBuf);
        sPath = GetSafePathName(ConcatPath(g_sInstallRootDir, "bin\\win32"), false) + CString(";") + sPath;
        ::SetEnvironmentVariable("PATH", sPath);
        theLog("Setting temporarily PATH = %s\n", (LPCTSTR)sPath);
        
        SetEnvironmentVariable("TEXMFCNF", sWeb2cDir);
        SetEnvironmentVariable("TEXMFMAIN", g_sTexmfMain);
        theLog("Setting temporarily TEXMFCNF = %s\n", (LPCTSTR)sWeb2cDir);
        theLog("Setting temporarily TEXMFMAIN = %s\n", (LPCTSTR)g_sTexmfMain);
        // Be safe, remove anything dangerous
        SetEnvironmentVariable("TEXMF", NULL);
        SetEnvironmentVariable("TEXMFLOCAL", NULL);
        SetEnvironmentVariable("TEXMFEXTRA", NULL);
        if (g_bIsWindowsNT && g_bAllUsers) {
            sTexmfTemp = ConcatPath(g_sInstallRootDir, "temp");
            SetEnvironmentVariable("TEXMFTEMP", (LPCTSTR)sTexmfTemp);
        }
        if (::DirectoryExists(g_sTexmfMain)) {
            sCmd = sMktexlsrCmd + CString(" ") + GetSafePathName(g_sTexmfMain);
            RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
        }
        else {
            if (! g_sTexmfMain.IsEmpty()) 
                theLog("Texmf tree %s does not exist !\n", (LPCTSTR)g_sTexmfMain);
        }
        
        if (::DirectoryExists(g_sTexmfLocal)) {
            sCmd = sMktexlsrCmd + CString(" ") + GetSafePathName(g_sTexmfLocal);
            RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
        }
        else {
            if (! g_sTexmfLocal.IsEmpty()) 
                theLog("Texmf tree %s does not exist !\n", (LPCTSTR)g_sTexmfLocal);
        }
        
        if (::DirectoryExists(g_sVarTexmf)) {
            sCmd = sMktexlsrCmd + CString(" ") + GetSafePathName(g_sVarTexmf);
            RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
        }
        else {
            if (! g_sVarTexmf.IsEmpty()) 
                theLog("Texmf tree %s does not exist !\n", (LPCTSTR)g_sVarTexmf);
        }
        
        if (::DirectoryExists(g_sTexmfHome)) {
            sCmd = sMktexlsrCmd + CString(" ") + GetSafePathName(g_sTexmfHome);
            RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
        }
        else {
            if (! g_sTexmfHome.IsEmpty()) 
                theLog("Texmf tree %s does not exist !\n", (LPCTSTR)g_sTexmfHome);
        }
        
        if (::DirectoryExists(g_sTexmfExtra)) {
            sCmd = sMktexlsrCmd + CString(" ") + GetSafePathName(g_sTexmfExtra);
            RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
        }
        else {
            if (! g_sTexmfExtra.IsEmpty()) 
                theLog("Texmf tree %s does not exist !\n", (LPCTSTR)g_sTexmfExtra);
        }
    }

    // AfxMessageBox("Mktexlsr done.", MB_OK);
    theLock.Lock();
    theStatus.nConfig = ::CFG_LSR_DATABASES;
    theLock.Unlock();
    
    // Update map files
    if (FileExists(sUpdMapCmd)) {
        sUpdMapCmd = GetSafePathName(sUpdMapCmd);
        sCmd = sUpdMapCmd + " --cnffile=" + GetSafePathName(ConcatPath(g_sVarTexmf, "web2c/updmap.cfg", '/'))
            + " --outputdir=" + GetSafePathName(ConcatPath(g_sVarTexmf, "dvips/config", '/'));
        RunProcess((LPCTSTR)sCmd, true, true, false); // log, wait, hide
        if (FileExists(sMktexlsrCmd)) {
            sCmd = sMktexlsrCmd + CString(" ") + GetSafePathName(g_sVarTexmf);
            RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
        }
    }

    
    // Rebuild all formats if language.dat has been changed
    if (FileExists(sFmtutilCmd)) {
        sFmtutilCmd = GetSafePathName(sFmtutilCmd);
        if (g_bBuildLanguageDat) {
            sCmd = sFmtutilCmd + " --all --dolinks --force";
            RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
        }
        else {
            // Rebuild only the needed formats, one by one.
            // FIXME : problem with dependencies !
            POSITION pos;
            CString sKey, sTPMKey;
            TPM *tpm;
            pos = g_vFormatsToBuild.GetStartPosition();
            while (pos) {
                g_vFormatsToBuild.GetNextAssoc(pos, sKey, tpm);
                // Weird ! we need to be sure we are talking to the ones 
                // in the g_sSourceTPM array
                sTPMKey = tpm->GetKey();
                if (g_vSourceTPM.Lookup(sTPMKey, tpm) && tpm->IsInstalled()) {
                    sCmd = sFmtutilCmd + " --byfmt=" + sKey + " --dolinks --force";
                    RunProcess((LPCTSTR)sCmd, true, true, false); // logging, wait, hide
                }
            }
        }
    }
    // AfxMessageBox("Format files done.", MB_OK);
    theLock.Lock();
    theStatus.nConfig = ::CFG_FORMAT_FILES;
    theLock.Unlock();
    
    // Setup the environment variables
    if (! g_bDryRun && theEnvironment.IsModified()) {
        theLock.Lock();
        theLog("Updating environment.\n");
        theEnvironment.Commit();
        theLock.Unlock();
    }

    // AfxMessageBox("Environment done.", MB_OK);
    theLock.Lock();
    theStatus.nConfig = ::CFG_ENVIRONMENT;
    theLock.Unlock();

    // Create menu entries
    if (!g_bDryRun && g_eInstallType == INSTALL_FULL) {

        CreateShellObjects(g_ShellLinks, g_nShellLinks, g_sInstallRootDir);

        // Associate DVI files with windvi
        //	  HKEY_CLASSES_ROOT\.dvi = DVI_File_assoc 
        //	  HKEY_CLASSES_ROOT\DVI_File_assoc = DVI File
        //	  HKEY_CLASSES_ROOT\DVI_File_assoc\shell\open\command = windvi.EXE 
        //	  HKEY_CLASSES_ROOT\DVI_File_assoc\shell\print\command = windvi.EXE
        CRegistry theRegistry;
        HKEY hkRegRoot = (g_bAllUsers ? HKEY_CLASSES_ROOT : HKEY_CURRENT_USER);
        CString sRegSubPath = (g_bAllUsers ? "" : "Software\\Classes");
        char *lpKeyValues[] = { ".dvi", "DVI.document",
                                "DVI.Document", "DVI Document",
                                "DVI.Document\\shell\\open\\command", "windvi.EXE %1",
                                "DVI.Document\\shell\\print\\command", "windvi.EXE %1",
                                NULL
        };

        if (theRegistry.Connect(hkRegRoot) == FALSE) {
            theLog("Registry: can't connect to %s (Error %d)\n", 
                   (hkRegRoot == HKEY_CLASSES_ROOT ? "HKEY_CLASSES_ROOT" : "HKEY_CURRENT_USER"),
                   GetLastError());
        }
        else {
            for (int i = 0; lpKeyValues[i] != NULL; i += 2) {
                CString sKey = ConcatPath(sRegSubPath, lpKeyValues[i], '\\');
                if (theRegistry.Create((LPCTSTR)sKey) == FALSE ) {
                    theLog("Registry: can't create TeXLive subkey %s (Error %d)\n",
                           sKey, GetLastError());
                    break;
                }
                if (theRegistry.SetValue("", lpKeyValues[i+1]) == FALSE) {
                    theLog("Registry: can't set default value %s (Error %d)\n",
                           lpKeyValues[i+1], GetLastError());
                    break;
                }
            }
            theRegistry.Close();
        }

        if (g_bInstallXEmTeX) {

            CreateShellObjects(g_XEmacsShellLinks, g_nXEmacsShellLinks, g_sInstallRootDir);

            // Associate TeX and Bib files with winclient
            //	  HKEY_CLASSES_ROOT\.tex = TEX_File_assoc 
            //	  HKEY_CLASSES_ROOT\TEX_File_assoc = TEX File
            //	  HKEY_CLASSES_ROOT\TEX_File_assoc\shell\open\command = wintex.EXE 
            //	  HKEY_CLASSES_ROOT\TEX_File_assoc\shell\print\command = wintex.EXE
            CRegistry theRegistry;
            HKEY hkRegRoot = (g_bAllUsers ? HKEY_CLASSES_ROOT : HKEY_CURRENT_USER);
            CString sRegSubPath = (g_bAllUsers ? "" : "Software\\Classes");
            char *lpKeyValues[][10] = { { ".tex", "TEX.document",
                                         "TEX.Document", "TeX Document",
                                         "TEX.Document\\shell\\open\\command", "winclient.EXE %1",
                                         "TEX.Document\\shell\\print\\command", "winclient.EXE %1",
                                         NULL },
                                       { ".bib", "BIB.document",
                                         "BIB.Document", "BibTeX Document",
                                         "BIB.Document\\shell\\open\\command", "winclient.EXE %1",
                                         "BIB.Document\\shell\\print\\command", "winclient.EXE %1",
                                         NULL }
            };
            
            if (theRegistry.Connect(hkRegRoot) == FALSE) {
                theLog("Registry: can't connect to %s (Error %d)\n", 
                       (hkRegRoot == HKEY_CLASSES_ROOT ? "HKEY_CLASSES_ROOT" : "HKEY_CURRENT_USER"),
                       GetLastError());
            }
            else {
                for (int j = 0; j < 2; j++) {
                    for (int i = 0; lpKeyValues[j][i] != NULL; i += 2) {
                        CString sKey = ConcatPath(sRegSubPath, lpKeyValues[j][i], '\\');
                        if (theRegistry.Create((LPCTSTR)sKey) == FALSE ) {
                            theLog("Registry: can't create TeXLive subkey %s (Error %d)\n",
                                   sKey, GetLastError());
                            break;
                        }
                        if (theRegistry.SetValue("", lpKeyValues[j][i+1]) == FALSE) {
                            theLog("Registry: can't set default value %s (Error %d)\n",
                                   lpKeyValues[i+1], GetLastError());
                            break;
                        }
                    }
                }
                theRegistry.Close();
            }
        }
    }
  
    theLock.Lock();
    theStatus.nConfig = ::CFG_SHELL_OBJECTS;
    theStatus.bReady = true;
    theLock.Unlock();

    // AfxGetApp()->DoWaitCursor(-1);
}

BOOL RemovePath(const char *filename)
{
    DWORD fa = ::GetFileAttributes((LPCTSTR)filename);
    if (fa == -1) {
        theLog("Error: Failed to get file attributes for %s (Error %d)!!!\n", 
               (LPCTSTR)filename, GetLastError());
    }
    else if ((fa & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
        CSingleLock theLock(&(g_csCritSec), TRUE);
        theStatus.sName = (LPCTSTR)filename + g_sInstallRootDir.GetLength() + 1;
        theLock.Unlock();
        if (RemoveDirectory((LPCTSTR)filename) == 0) {
            theLog("Error: Failed to remove directory %s (Error %d)!!!\n", 
                   (LPCTSTR)filename, GetLastError());
        }
        else {
            theLog ("Removed directory %s\n", (LPCTSTR)filename);
        }
    }
    else {
        if (DeleteFile((LPCTSTR)filename) == 0) {
            theLog("Error: Failed to delete file %s (Error %d)!!!\n", 
                   (LPCTSTR)filename, GetLastError());
        }
        else {
            theLog ("Deleted file %s\n", (LPCTSTR)filename);
        }
    }
    return TRUE;
}

/*
  Remove all files in the given directory.
*/

void RemoveAllFiles(CString &sPath, const char *lpExt = "*")
{
    WIN32_FIND_DATA find_file_data;
    HANDLE hnd;
    char path[_MAX_PATH];
    int path_len = 0;
    CString sExt(lpExt);

    CString sTemplate = ConcatPath(sPath, sExt, '\\');
    strcpy(path, (LPCTSTR)sTemplate);
    path_len = sPath.GetLength();

        
    hnd = FindFirstFile(path, &find_file_data);
    bool bContinue = true;
    while (hnd != INVALID_HANDLE_VALUE && bContinue) {
        if(strcmp(find_file_data.cFileName, ".")
           && strcmp(find_file_data.cFileName, "..")) {
            path[path_len+1] = '\0';
            strcat(path, find_file_data.cFileName);
            if ((find_file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) {
                if (DeleteFile((LPCTSTR)path) == 0) {
                    theLog("Error: Failed to delete file %s (Error %d)!!!\n", 
                           path, GetLastError());
                }
                else {
                    theLog ("Deleted file %s\n", path);
                }
            }
        }
        bContinue = (FindNextFile(hnd, &find_file_data) != FALSE);
    }
    path[path_len] = '\0';
    FindClose(hnd);
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
        theLog("Error: Failed to get file attributes for %s (Error %d)!!!\n", 
               (LPCTSTR)path, GetLastError());
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
        theLog("Error: Fail to get file attributes for %s (Error %d)!!!\n",
               filename, GetLastError());
        return false;
    }
    if ((fa & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY) {
        char path[1024];
        strcpy(path, (LPCTSTR)filename);
        ::__rec_rmdir(path);
    }
    else {
        if (DeleteFile((LPCTSTR)filename) == 0) {
            theLog("Error: Failed to delete file %s (Error %d)!!!\n", 
                   (LPCTSTR)filename, GetLastError());
        }
        else {
            theLog ("Deleted file %s\n", (LPCTSTR)filename);
        }
    }
    return true;
}

/*
  We need to be able to remove the TeXSetup.exe program by itself !
*/
bool DeleteExecutableBF() 
{
    bool bRet = false;
    HANDLE hfile;
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    CString sDelUs = ConcatPath(g_sTempPath, "DelUs.bat", '\\');
    
    // Create a batch file that continuously attempts to delete our executable
    // file.  When the executable no longer exists, remove its containing
    // subdirectory, and then delete the batch file too.
    hfile = CreateFile(sDelUs, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
                       FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (hfile == INVALID_HANDLE_VALUE)
        return bRet;

    TCHAR szBatFile[5*_MAX_PATH];
    DWORD dwNumberOfBytesWritten;
    CString sBinDir = ConcatPath(g_sInstallRootDir, "bin", '\\');
    CString sBinWin32Dir = ConcatPath(sBinDir, "win32", '\\');
    CString sModule = ConcatPath(sBinWin32Dir, g_sModuleName, '\\');
    
    // Construct the lines for the batch file.
    wsprintf(szBatFile,
             __TEXT(":Repeat\r\n")
             __TEXT("del \"%s\"\r\n")
             __TEXT("if exist \"%s\" goto Repeat\r\n")
             __TEXT("rmdir \"%s\"\r\n")
             __TEXT("rmdir \"%s\"\r\n")
             __TEXT("del \"%s\"\r\n"), 
             (LPCTSTR)sModule, (LPCTSTR)sModule, 
             (LPCTSTR)sBinWin32Dir, (LPCTSTR)sBinDir,
             sDelUs);
    
    // Write the batch file and close it.
    WriteFile(hfile, szBatFile, lstrlen(szBatFile) * sizeof(TCHAR),
              &dwNumberOfBytesWritten, NULL);
    CloseHandle(hfile);
    
    // Get ready to spawn the batch file we just created.
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    
    // We want its console window to be invisible to the user.
    si.dwFlags = STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_HIDE;
    
    char *lpCmd = _strdup((LPCTSTR)sDelUs);

    // Spawn the batch file with low-priority and suspended.
    if (CreateProcess(NULL, lpCmd, NULL, NULL, FALSE,
                      CREATE_SUSPENDED | IDLE_PRIORITY_CLASS, 
                      NULL, __TEXT("\\"), &si, &pi)) {

        free(lpCmd);

        // Lower the batch file's priority even more.
        SetThreadPriority(pi.hThread, THREAD_PRIORITY_IDLE);
        
        // Raise our priority so that we terminate as quickly as possible.
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
        SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
        
        // Allow the batch file to run and clean-up our handles.
        ResumeThread(pi.hThread);
        // We want to terminate right away now so that we can be deleted
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
        bRet = true;
    }
    return bRet;
}

void DoUninstall()
{
    CSingleLock theLock(&g_csCritSec, TRUE);
    theStatus.dwOverallDone = 0;
    theStatus.dwPackageDone = 0;
    theStatus.bError = false;
    theStatus.bNewTPM = false;
    theStatus.bReady = false;
    theStatus.bTerminate = false;
    theStatus.tpm = 0;
    theStatus.sName = 0;
    theStatus.nConfig = ::RMBEGIN;
    theLock.Unlock();

    if (! GetLogFile()) {
        // Should we really abort ?
        FatalError (IDS_CANT_OPEN_LOGFILE);
    }
    // Change directory
    if (SetCurrentDirectory(g_sTempPath) == 0) {
        theLog("Error: Failed to change current directory to %s (Error %d)!!!\n", 
               (LPCTSTR)g_sTempPath, GetLastError());
    }

#if 1
    CString sKey;
    TPM *tpm;
  
    bool bSuccess = true;	
    int i, dwTpm = 0;
    char **s;

    // First remove anything from a previous try
    TPMMapCleanup(g_vInstalledTPM);
    // Read the installed tpm files
    g_sLocalTPMDir = ConcatPath(g_sInstallRootDir, "texmf\\tpm");
    for (s = lpcTPMsubdirs, i = 0;
         bSuccess && i < sizeof(lpcTPMsubdirs)/sizeof(lpcTPMsubdirs[0]);
         s++, i++) {
        bSuccess = bSuccess 
            && ReadTPM(ConcatPath(g_sLocalTPMDir, *s), g_vInstalledTPM);
        dwTpm += g_dwTPMNumber;
    }
    // Remove the files installed
    POSITION pos = g_vInstalledTPM.GetStartPosition();
    while (pos) {
        g_vInstalledTPM.GetNextAssoc(pos, sKey, tpm);
        // Look if xemtex was installed
        if (sKey == "collection/tex-xemtex") {
            g_bInstallXEmTeX = true;
        }
        tpm->UnInstall();
    }

    {
        CString sPath;
        // Remove all generated fonts
        sPath = ConcatPath(g_sVarTexmf, "fonts");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        // Remove generated files from texmf-var/web2c
        sPath = ConcatPath(g_sVarTexmf, "web2c");
        RemoveAllFiles(sPath, "*.log");
        RemoveAllFiles(sPath, "*.fmt");
        RemoveAllFiles(sPath, "*.efmt");
        RemoveAllFiles(sPath, "*.tcx");
        RemoveAllFiles(sPath, "*.pool");
        RemoveAllFiles(sPath, "*.base");
        RemoveAllFiles(sPath, "*.mem");
        RemoveAllFiles(sPath, "*.opt");
        RemoveAllFiles(sPath, "*.aux");
        RemoveAllFiles(sPath, "*.bak_*");
    }
#else
    // Recursively remove the Root directory
    {
        CString sPath;
        if (DirectoryExists(g_sTexmfMain))
            RecursivelyRemove(g_sTexmfMain);
        sPath = ConcatPath(g_sInstallRootDir, "bin");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        sPath = ConcatPath(g_sInstallRootDir, "man");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        sPath = ConcatPath(g_sInstallRootDir, "info");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        sPath = ConcatPath(g_sInstallRootDir, "Books");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        sPath = ConcatPath(g_sInstallRootDir, "usergrps");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        sPath = ConcatPath(g_sInstallRootDir, "temp");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        // Remove all files at the root directory
        RemoveAllFiles(g_sInstallRootDir);
        // Remove all generated fonts
        sPath = ConcatPath(g_sVarTexmf, "fonts");
        if (DirectoryExists(sPath))
            RecursivelyRemove(sPath);
        // Remove generated files from texmf-var/web2c
        sPath = ConcatPath(g_sVarTexmf, "web2c");
        RemoveAllFiles(sPath, "*.log");
        RemoveAllFiles(sPath, "*.fmt");
        RemoveAllFiles(sPath, "*.efmt");
        RemoveAllFiles(sPath, "*.tcx");
        RemoveAllFiles(sPath, "*.pool");
        RemoveAllFiles(sPath, "*.base");
        RemoveAllFiles(sPath, "*.mem");
        RemoveAllFiles(sPath, "*.opt");
        RemoveAllFiles(sPath, "*.aux");
        RemoveAllFiles(sPath, "*.bak_*");
    }
#endif
      
    theLock.Lock();
    theStatus.sName = "";
    theStatus.nConfig = ::RMFILES;
    theLock.Unlock();

    // Recursively remove the Folder menu entry
    char path[_MAX_PATH], szPath[_MAX_PATH];
    // Do both !
    if (GetSpecialFolderPath(CSIDL_PROGRAMS, path)) {
        _makepath (szPath, 0, path, g_sFolderName, 0);
        RecursivelyRemove(szPath);
    }
    if (GetSpecialFolderPath(CSIDL_COMMON_PROGRAMS, path)) {
        _makepath (szPath, 0, path, g_sFolderName, 0);
        RecursivelyRemove(szPath);
    }

    theLock.Lock();
    theStatus.nConfig = ::RMMENUS;
    theLock.Unlock();

    // Remove the registry key
    CRegistry theRegistry;
    // Global one ...
    if (g_bAllUsers && theRegistry.Connect(HKEY_LOCAL_MACHINE) == TRUE) {
        if (theRegistry.DeleteKey(g_sGlobalRegKey) == TRUE) {
            theLog("Deleted registry key for TeXLive %s.\n", TEXLIVE_VERSION);
        }
        CString sUninstRegKey = ConcatPath(REGSTR_PATH_UNINSTALL, g_sFolderName);
        if (theRegistry.DeleteKey(sUninstRegKey) == TRUE) {
            theLog("Deleted registry key for uninstalling TeXLive %s.\n", TEXLIVE_VERSION);
        }
        theRegistry.Close();
    }
    // ... and User one
    if (theRegistry.Connect(HKEY_CURRENT_USER) == TRUE) {
        if (theRegistry.DeleteKey(g_sUserRegKey) == TRUE) {
            theLog("Deleted registry key for TeXLive %s.\n", TEXLIVE_VERSION);
        }
        if (theRegistry.DeleteKey("Software\\Classes\\.dvi") == TRUE) {
            theLog("Deleted registry key for `.dvi' association.\n");
        }
        if (theRegistry.DeleteKey("Software\\Classes\\DVI.Document") == TRUE) {
            theLog("Deleted registry key for `DVI.Document' association.\n");
        }

        if (g_bInstallXEmTeX) {
            if (theRegistry.DeleteKey("Software\\Classes\\.tex") == TRUE) {
                theLog("Deleted registry key for `.tex' association.\n");
            }
            if (theRegistry.DeleteKey("Software\\Classes\\TEX.Document") == TRUE) {
                theLog("Deleted registry key for `TEX.Document' association.\n");
            }
            
            if (theRegistry.DeleteKey("Software\\Classes\\.bib") == TRUE) {
                theLog("Deleted registry key for `.bib' association.\n");
            }
            if (theRegistry.DeleteKey("Software\\Classes\\BIB.Document") == TRUE) {
                theLog("Deleted registry key for `BIB.Document' association.\n");
            }
        }
        theRegistry.Close();
    }
    if (g_bAllUsers && theRegistry.Connect(HKEY_CLASSES_ROOT) == TRUE) {

        if (theRegistry.DeleteKey(".dvi") == TRUE) {
            theLog("Deleted registry key for `.dvi' association.\n");
        }
        if (theRegistry.DeleteKey("DVI.Document") == TRUE) {
            theLog("Deleted registry key for `DVI.Document' association.\n");
        }

        if (g_bInstallXEmTeX) {
            if (theRegistry.DeleteKey(".tex") == TRUE) {
                theLog("Deleted registry key for `.tex' association.\n");
            }
            if (theRegistry.DeleteKey("TEX.Document") == TRUE) {
                theLog("Deleted registry key for `TEX.Document' association.\n");
            }
            
            if (theRegistry.DeleteKey(".bib") == TRUE) {
                theLog("Deleted registry key for `.bib' association.\n");
            }
            if (theRegistry.DeleteKey("BIB.Document") == TRUE) {
                theLog("Deleted registry key for `BIB.Document' association.\n");
            }
        }
        theRegistry.Close();
    }
    theLock.Lock();
    theStatus.nConfig = ::RMREGISTRY;
    theLock.Unlock();

    // FIXME : Need to restore environment !
    // And especially PATH/autoexec.bat
    {
        char *vars[] = { "TEXMFCNF", "TEXMFTEMP", NULL };
        theEnvironment.Restore(vars);
    }
    theLock.Lock();
    theStatus.bReady = true;
    theStatus.nConfig = ::RMENVIRONMENT;
    theLock.Unlock();

    // Warn the user about files that will never be removed
    // automatically
    CString sUninstMsg, sFmt;
    sFmt.LoadString(IDS_UNINSTALL_FILES_NOT_REMOVED);
    sUninstMsg.Format((LPCTSTR)sFmt,g_sInstallRootDir);
    AfxMessageBox(sUninstMsg);

    // Last step : remove the TeXSetup.exe program by itself.
    DeleteExecutableBF();
}


bool CreateProgramFolder (char * szPath)
{
    char path[_MAX_PATH];
    int nFolder = ((g_bIsWindowsNT && g_bAllUsers) ? CSIDL_COMMON_PROGRAMS : CSIDL_PROGRAMS);

    if (GetSpecialFolderPath(nFolder, path)) {
        _makepath (szPath, 0, path, g_sFolderName, 0);
        if (! CreateDirectoryPath(szPath))
            return (false);
    }
    return (true);
}

//
// Get some special folder path (CSIDL_PERSONAL, ...)
//
bool GetSpecialFolderPath (int nFolder, char *path)
{
    LPITEMIDLIST pidl;
    HRESULT hr = SHGetSpecialFolderLocation(0, nFolder, &pidl);
    if (FAILED(hr))
        return (false);
    BOOL ret = SHGetPathFromIDList(pidl, path);
    LPMALLOC pMalloc;
    hr = SHGetMalloc(&pMalloc);
    if (SUCCEEDED(hr)) {
        pMalloc->Free (pidl);
        pMalloc->Release ();
    }
    return (ret ? true : false);
}

bool CreateShellObjects (const ShellLinkData ShellLinks[], size_t nShellLinks, 
				     const CString &sRootDir)
{
    char szFolderPath[_MAX_PATH];
    if (! CreateProgramFolder(szFolderPath))
        return (false);
    for (size_t i = 0; i < nShellLinks; i++) {
        if (! CreateShellLink(szFolderPath, ShellLinks[i], sRootDir))
            return (false);
    }
    return (true);
}

bool CreateShellLink(const char *szFolderPath,
				 const ShellLinkData &ld,
				 const CString &sRootDir)
{
    char szSubFolderPath[_MAX_PATH];
    if (ld.nSubFolderID > 0) {
        CString strSubFolder;
        strSubFolder.LoadString (ld.nSubFolderID);
        _makepath (szSubFolderPath, 0, szFolderPath, strSubFolder, 0);
        if (! CreateDirectoryPath(szSubFolderPath))
            return (false);
    }
    else {
        strcpy (szSubFolderPath, szFolderPath);
    }
  
    char szLinkPath[_MAX_PATH];
  
    CString strItemName;
    strItemName.LoadString (ld.nNameID);
  
    _makepath (szLinkPath, 0, szSubFolderPath, strItemName, ".lnk");
    if (_access(szLinkPath, 0) == 0) {
        theLog ("removing %s...\n", szLinkPath);
        if (! g_bDryRun)
            _unlink (szLinkPath);
    }
  
    if (ld.pszPathname == 0)
        return (true);
	
    char szPathName[_MAX_PATH];
    CString sPath = ld.pszPathname;
    CString  sTeXSetup = ConcatPath(g_sInstallRootDir, "bin\\win32\\TeXSetup.exe");

    if (sPath.Find("@@texsetup@@") != -1) {
        sPath.Replace("@@texsetup@@", sTeXSetup);
    }
    if (_strnicmp((LPCTSTR)sPath, "http://", 6) == 0
        || ( __isascii(sPath[0]) 
             && (sPath[2] == '/' || sPath[2] == '\\')
             && (sPath[1] == ':'))) {
        strcpy(szPathName, (LPCTSTR)sPath);
    }
    else {
        _makepath (szPathName, 0, sRootDir, (LPCTSTR)sPath, 0);
    }
    theLog ("creating shell link %s...\n", (const char *) szLinkPath);

    if (g_bDryRun)
        return (true);

    IShellLink * psl;
    HRESULT hr = CoCreateInstance (CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER,
                                   IID_IShellLink, (PVOID *) &psl);
    if (FAILED(hr))	{
        theLog ("	 ...cannot create IShellLink interface (%08x)\n", hr);
        return (false);
    }
	
    psl->SetPath (szPathName);
    theLog("\tPath = %s\n", szPathName);
	
    // Kludgy isn't it ???
    if (ld.fdwFlags & LD_USEARGS) {
        CString sArgs = ld.pszArgs;
        if (sArgs.Find("@@sourcedir@@") != -1) {
            sArgs.Replace("@@sourcedir@@", (LPCTSTR)g_sLocalSourceDir);
        }
        theLog("\tArguments = %s\n", (LPCTSTR)sArgs);
        psl->SetArguments ((LPCTSTR)sArgs);
    }

    if (ld.fdwFlags & LD_USEDESC) {
        psl->SetDescription (ld.pszDesc);
        theLog("\tDescription = %s\n", ld.pszDesc);
    }

    if (ld.fdwFlags & LD_USEICON) {
        CString sIcon = ld.pszIconPath;
        if (sIcon.Find("@@texsetup@@") != -1) {
            sIcon.Replace("@@texsetup@@", (LPCTSTR)sTeXSetup);
        }
        theLog("\tIcon = %s, %d\n", (LPCTSTR)sIcon, ld.nIconIndex);
        psl->SetIconLocation ((LPCTSTR)sIcon, ld.nIconIndex);
    }

    if (ld.fdwFlags & LD_USEWORKDIR) {
        theLog("\tWorking directory = %s\n", ld.pszWorkingDir);
        psl->SetWorkingDirectory(ld.pszWorkingDir);
    }

    if (ld.fdwFlags & LD_USESHOWCMD) {
        theLog("\tShow command = %d\n", ld.nShowCmd);
        psl->SetShowCmd(ld.nShowCmd);
    }

    if (ld.fdwFlags & LD_USEHOTKEY) {
        theLog("\tHot Key = %ld\n", ld.wHotkey);
        psl->SetHotkey (ld.wHotkey);
    }

    IPersistFile * ppf;
    hr = psl->QueryInterface(IID_IPersistFile, (PVOID *) &ppf);
    if (SUCCEEDED(hr)) {
#ifndef UNICODE
        WCHAR szPath[_MAX_PATH] = { 0 };
        MultiByteToWideChar (CP_ACP, 0, szLinkPath, strlen(szLinkPath), szPath, _MAX_PATH);
        hr = ppf->Save(szPath, TRUE);
#else
        hr = ppf->Save(szLinkPath, TRUE);
#endif
        if (FAILED(hr))	{
            theLog ("	  ...cannot Save() shell link object (%08x)\n", hr);
        }
        ppf->Release ();
    }
    else {
        theLog ("	 ...cannot QI() IPersistFile interface (%08x)\n", hr);
    }
  
    psl->Release ();
  
    return (SUCCEEDED(hr));
}

bool DoProgress(unsigned long size)
{
    CSingleLock singleLock (&g_csCritSec, TRUE);
    theStatus.dwPackageDone += size;
    theStatus.dwOverallDone += size;
    return (GetCancelFlag() ? true : false);
}

bool GetCancelFlag ()
{
    CSingleLock (&g_csCritSec, TRUE);
    return (theStatus.bTerminate);
}

void SetCancelFlag (bool f)
{
    CSingleLock (&g_csCritSec, TRUE);
    theStatus.bTerminate = f;
    if (f)
        theLog ("\n<<<Canceling the file copy operation...>>>\n");
}


typedef BOOL (WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER, 
                               PULARGE_INTEGER, PULARGE_INTEGER);

BOOL UpdateSize()
{
    unsigned long dwSectorsPerCluster, dwBytesPerSector, 
        dwNumberOfFreeClusters, dwTotalNumberOfClusters;
    CString sRoot = g_sInstallRootDir;

    ASSERT(! sRoot.IsEmpty());

    g_dwSpaceAvailable = 0;

    if (isascii(sRoot[0]) && sRoot[1] == TCHAR(':')) {
        sRoot = sRoot.Left(2) + "\\";
    }
    else if (sRoot[0] == TCHAR('\\') && sRoot[1] == TCHAR('\\')) {
        int loc = sRoot.Find(TCHAR('\\'), 2);
        if (loc == -1) {
            return false;
        }
        loc = sRoot.Find(TCHAR('\\'), loc+1);
        if (loc == -1) {
            return false;
        }
        sRoot = sRoot.Left(loc+1);

			
    }

    unsigned __int64 i64FreeBytesToCaller,
        i64TotalBytes,
        i64FreeBytes;
    BOOL fResult;

    fResult =  GetDiskFreeSpace (sRoot, 
                                 &dwSectorsPerCluster, 
                                 &dwBytesPerSector,
                                 &dwNumberOfFreeClusters, 
                                 &dwTotalNumberOfClusters);
    if (fResult) {
        // Beware : this value maybe wrong
        g_dwSpaceAvailable = (dwNumberOfFreeClusters * g_dwClusterSize) / 1024 + 1;
        g_dwClusterSize = dwSectorsPerCluster * dwBytesPerSector;
    }

    P_GDFSE pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress( GetModuleHandle("kernel32.dll"),
                                                           "GetDiskFreeSpaceExA");

    if (pGetDiskFreeSpaceEx) {
        fResult = pGetDiskFreeSpaceEx (sRoot,
                                       (PULARGE_INTEGER)&i64FreeBytesToCaller,
                                       (PULARGE_INTEGER)&i64TotalBytes,
                                       (PULARGE_INTEGER)&i64FreeBytes);
        // This value may fix the previous one
        if (fResult)
            g_dwSpaceAvailable = i64FreeBytes / 1024 + 1;
    }
    
    
    /* It is not necessary to call LoadLibrary on Kernel32.dll because
       it is already loaded into every process address space. */
    
    return (fResult != 0);
}

BOOL RecalcSize()
{
    unsigned long dwSpaceNeeded = 0;
    TPM *tpm;
    CString sKey;
    POSITION pos = g_vSourceTPM.GetStartPosition();
    while(pos) {
        g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
        if (tpm->IsSelected()) {
            dwSpaceNeeded += tpm->GetSize(true, g_dwClusterSize);
        }
    }
    g_dwSpaceNeeded = dwSpaceNeeded / 1024;
    return TRUE;
}

void SetRootDir(const CString &sDir)
{
    g_sInstallRootDir = sDir;
    g_sTexmfMain = ConcatPath(g_sInstallRootDir, "texmf");
    g_sTexmfLocal = ConcatPath(g_sInstallRootDir, "texmf-local");
    g_sVarTexmf = ConcatPath(g_sInstallRootDir, "texmf-var");
    g_sVarTexFonts = ConcatPath(g_sVarTexmf, "fonts");
    UpdateSize();
}

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
    theLog ("\nFatal error:\n%s\n", (const char *) g_sLastError);
    throw (nMsgID);
}

#if 1

#define MAX_BACKUPS 16

bool CreateBackupFile(const CString &sFile)
{
    bool bRet = true;
    if (FileExists(sFile)) {
        CString sBack, sBack1;
        for (int i = MAX_BACKUPS - 2; i >= 0; i--) {
            sBack.Format("%s.bak_%d", sFile, i);
            sBack1.Format("%s.bak_%d", sFile, i + 1);
            if (FileExists(sBack)) {
                ::MoveFile(sBack, sBack1);
                theLog("Moving backup file %s into %s\n", sBack, sBack1);
            }
        }
        sBack.Format("%s.bak_0", sFile, i);
        ::MoveFile(sFile, sBack);
        theLog("Creating backup file of %s into %s\n", sFile, sBack);
    }
    return bRet;
}
#endif

bool SafeCopyFile(const CString &src, const CString &dst, bool bBackup, bool bLog)
{
    HANDLE hFile = INVALID_HANDLE_VALUE;
    DWORD dwSize = 0;
    bool bRet = true;
  
    // Maybe it is a remote file ???
    if (_stricmp(src.Left(7),"http://") == 0 
        || _stricmp(src.Left(6),"ftp://") == 0) {
        if (g_uiNetMethod == NetIOUnavail) {
            theLog("Error: No net access allowed, cannot grab %s from the Internet!!!\n", (LPCTSTR)src);
            bRet = false;
        }
        else {
            if (!g_bDryRun) {
                // Keep the user original file
                CString newdst(dst);
                if (bBackup && FileExists(dst)) {
//                      sTmp = sTmp + CString(".orig");
//                      theLog("Not overwriting your file, copying %s to %s.\n", src, sTmp);
                    CreateBackupFile(dst);
                }
                if (GrabInternetFile(src, newdst)) {
                    theLog("Special file %s has been grabbed from the Internet.\n", (LPCTSTR)src);
                    bRet = true;
                }
                else {
                    theLog("Warning: failed to grab special file %s from the Internet.\n", (LPCTSTR)src);
                    bRet = false;
                }
            }
        }
        return bRet;
    }

    hFile = ::CreateFile((LPCTSTR)src, 
                         GENERIC_READ, 
                         FILE_SHARE_READ, 
                         NULL,
                         OPEN_EXISTING, 
                         FILE_ATTRIBUTE_NORMAL, 
                         NULL);
    if (hFile == INVALID_HANDLE_VALUE) {
        theLog("!!!Fail to open %s while copying to %s (Error %d)\n", src, dst, GetLastError());
        bRet = false;
        return bRet;
    }
    
    dwSize = ::GetFileSize(hFile, NULL);
    CloseHandle(hFile);
    
    if(!g_bDryRun) {
        CString dir(dst), newdst(dst);
        // Keep the user original file
        if (bBackup && FileExists(dst)) {
//              newdst = newdst + CString(".orig");
//              theLog("Not overwriting your file, copying %s to %s.\n", src, newdst);
            CreateBackupFile(dst);
        }
        ::GetParentDirectory(dir);
        ::CreateDirectoryPath(dir);
        
        if (::CopyFile(src, newdst, FALSE) == 0) {
            // There seems to be a weird bug with remote drives.
            // Try to guess a bit more about the failure.
            DWORD dwErrCode = GetLastError();
            if (dwErrCode != ERROR_ACCESS_DENIED) {
                theLog("Error: Fail to copy %s to %s (Error %lu)\n", src, newdst, dwErrCode);
                bRet = false;
            }
            else {
                hFile = ::CreateFile((LPCTSTR)newdst, 
                                     GENERIC_READ|GENERIC_WRITE, 
                                     FILE_SHARE_READ | FILE_SHARE_WRITE, 
                                     NULL,
                                     OPEN_ALWAYS, 
                                     FILE_ATTRIBUTE_NORMAL, 
                                     NULL);
                CloseHandle(hFile);
                if (::CopyFile(src, newdst, FALSE) == 0) {
                    dwErrCode = GetLastError();
                    theLog("!!!Fail to copy %s to %s (Error %lu)\n", src, newdst, dwErrCode);
                    bRet = false;
                }
                else {
#if 1
                    if (bLog)
                        theLog("Copy %s to %s\n", src, newdst);
#endif
                }
            }
        }
        else {
#if 1
            if (bLog)
                theLog("Copy %s to %s\n", src, newdst);
#endif
        }
        if (bRet && ::SetFileAttributes(newdst, FILE_ATTRIBUTE_NORMAL) == 0) {
            theLog("!!!Fail to reset file attributes for %s (Error %lu)\n", newdst, GetLastError());
            bRet = false;
        }
    }

    DoProgress(dwSize);
    return bRet;
}

bool MoveFile (const char *oldfile, const char *newfile,
			   bool &bReboot)
{
    bool bRemove = false;
    if (newfile && *newfile == 0) {
        bRemove = true;
        newfile = 0;
    }

    if (g_bDryRun) {
        theLog("Moving %s -> %s\n", oldfile, newfile);
        return true;
    }

    if (newfile) {

        if (::FileExists(newfile)) {
            int cmp = CompareFileVersions(oldfile, newfile);
            if (cmp < 0) {
                theLog ("%s is newer - no update required\n", newfile);
                bRemove = true;
                newfile = 0;
            }
            else if (cmp == 0) {
                theLog ("%s has same version number - no update required\n", newfile);
                bRemove = true;
                newfile = 0;
            }
            else {
                theLog ("%s is older - update required\n", newfile);
                theLog ("trying to remove %s...\n", newfile);
                if (_unlink(newfile) != 0) {
                    theLog ("   ...failed\n");
                }
                else {	
                    theLog ("trying to move file %s -> %s...\n", oldfile, newfile);
                    if (::MoveFile(oldfile, newfile)) {
                        theLog ("	 ...succeeded\n");
                        return (true);
                    }
                    else {
                        theLog ("	 ...failed\n");
                    }
                }
            }
        }
        else {
            // newfile does not exist
            theLog ("trying to move file %s -> %s...\n", oldfile, newfile);
            if (::MoveFile(oldfile, newfile)) {
                theLog ("	 ...succeeded\n");
                return (true);
            }
            else {
                theLog ("	 ...failed\n");
            }
        }
    }
    if (bRemove) {
        theLog ("trying to remove %s...\n", oldfile);
        if (_unlink(oldfile) == 0) {
            theLog ("	  ...succeeded\n");
            return (true);
        }
        else {
            theLog ("	  ...failed (%d)\n", _errno);
        }
    }
    if (g_bIsWindowsNT) {
        // We have the MoveFileEx() call
        theLog ("scheduling move-after-boot (MoveFileEx:) %s -> %s...\n",
                oldfile, newfile ? newfile : "NULL");
        if (! MoveFileEx(oldfile, newfile,
                         MOVEFILE_DELAY_UNTIL_REBOOT | MOVEFILE_REPLACE_EXISTING)) {
            theLog ("	  ...failed\n");
            if (newfile)
                FatalError (IDS_CANNOT_INSTALL_SHARED_FILE, newfile);
            return (false);
        }
        theLog ("	 ...succeded\n");
#if 0				// fixme: this should work under nt, but it doesn't
        if (newfile)
            bReboot = true;
#else
        bReboot = true;
#endif
        return (true);
    }
    else {
        theLog ("scheduling move-after-boot (wininit.ini)...\n");
        char windir[_MAX_PATH];
        GetWindowsDirectory (windir, sizeof(windir));
        char wininitini[_MAX_PATH];
        _makepath (wininitini, 0, windir, "wininit", ".ini");
        bool bAppending = false;
        if (_access(wininitini, 0) == 0)
            bAppending = true;
        FILE * file = fopen(wininitini, bAppending ? "a" : "w");
        if (file == 0) {
            theLog ("	  ...cannot open %s\n", wininitini);
            return (false);
        }
        if (! bAppending)
            fprintf (file, "[Rename]\n");
        fprintf (file, "%s=%s\n", newfile ? newfile : "NUL", oldfile);
        fclose (file);
        bReboot = true;
        return (true);
    }
}

int CompareFileVersions (const char *filename1, const char *filename2)
{
    DWORD dwVerMS1, dwVerLS1;
    if (! GetFileVersion(filename1, dwVerMS1, dwVerLS1))
        return (0);
    DWORD dwVerMS2, dwVerLS2;
    if (! GetFileVersion(filename2, dwVerMS2, dwVerLS2))
        return (0);
    if (dwVerMS1 == dwVerMS2) {
        if (dwVerLS1 == dwVerLS2)
            return (0);
        else if (dwVerLS1 < dwVerLS2)
            return (-1);
        else
            return (1);
    }
    else if (dwVerMS1 < dwVerMS2)
        return (-1);
    else
        return (1);
}

bool GetFileVersion (const char *filename, DWORD &dwMS, DWORD &dwLS)
{
    theLog ("getting version info of %s...\n", filename);
    DWORD dwHandle = 0;
    DWORD dwLen = GetFileVersionInfoSize((LPTSTR) filename, &dwHandle);
    if (dwLen == 0) {
        theLog ("	...has no version info\n");
        return (false);
    }
    void * pData = _alloca (dwLen);
    if (! ::GetFileVersionInfo((LPTSTR) filename, dwHandle, dwLen, pData)) {
        theLog ("	...cannot get version info\n");
        return (false);
    }
    VS_FIXEDFILEINFO * pfi;
    UINT uLen;
    if (! VerQueryValue(pData, "\\", reinterpret_cast<void**>(&pfi), &uLen)) {
        theLog ("	...cannot read fixed file info\n");
        return (false);
    }
    dwMS = pfi->dwFileVersionMS;
    dwLS = pfi->dwFileVersionLS;
    theLog ("   ...%08lx%08lx\n", dwMS, dwLS);
    return (true);
}

bool RunProcess(const char *command_line, bool bLog, bool bWait, bool bShow)
{
    theLog("Running %s\n", command_line);
    
    if (g_bDryRun) {
        Sleep (500);
        return (true);
    }

    char cmd[4096];
    strcpy(cmd, command_line);

    bool ret = false;
    DWORD dwCode = 0;
    PROCESS_INFORMATION piProcessInformation;
    STARTUPINFO siStartInfo;
    HANDLE hStdoutRd = INVALID_HANDLE_VALUE;
    HANDLE hStdoutWr = INVALID_HANDLE_VALUE;
    HANDLE hChildStdin = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hChildStdout = INVALID_HANDLE_VALUE;
    HANDLE hChildStderr = INVALID_HANDLE_VALUE;

    // start process
    memset (&siStartInfo, 0, sizeof(siStartInfo));
    siStartInfo.cb = sizeof(siStartInfo);

    siStartInfo.dwFlags = STARTF_USESHOWWINDOW;
    siStartInfo.wShowWindow = (bShow ? SW_SHOWDEFAULT : SW_HIDE);

    if (bLog) {
        HANDLE hProcess = GetCurrentProcess();
        // create an anonymous pipe for stdout
        // handles are created non-inheritable
        if (! CreatePipe(&hStdoutRd, &hStdoutWr, NULL, PIPE_SIZE)) {
            theLog("Failed to create pipe (%d)\n", GetLastError());
            CloseHandle(hProcess);
            return false;
        }
        
        // make inheritable child handles
        if (! DuplicateHandle(hProcess, hStdoutWr, 
                              hProcess, &hChildStdout, 
                              0, TRUE, DUPLICATE_SAME_ACCESS)) {
            theLog("Failed to duplicate handle (%d)\n", GetLastError());
            CloseHandle(hProcess);
            CloseHandle(hStdoutWr);
            CloseHandle(hStdoutRd);
            return false;
        }
        
        if (! DuplicateHandle(hProcess, hStdoutWr, 
                              hProcess, &hChildStderr, 
                              0, TRUE, DUPLICATE_SAME_ACCESS)) {
            theLog("Failed to duplicate handle (%d)\n", GetLastError());
            CloseHandle(hProcess);
            CloseHandle(hStdoutWr);
            CloseHandle(hStdoutRd);
            CloseHandle(hChildStdout);
            return false;
        }
        
#if 0
        if (! DuplicateHandle(hProcess, GetStdHandle(STD_INPUT_HANDLE), 
                              hProcess, &hChildStdin, 
                              0, TRUE, DUPLICATE_SAME_ACCESS)) {
            theLog("Failed to duplicate handle (%d)\n", GetLastError());
            CloseHandle(hProcess);
            CloseHandle(hStdoutWr);
            CloseHandle(hStdoutRd);
            CloseHandle(hChildStdout);
            CloseHandle(hChildStderr);
            return false;
        }
#endif
        
        // close handle to process
        CloseHandle (hProcess);
        // close our own write end
        CloseHandle (hStdoutWr);

        siStartInfo.dwFlags = siStartInfo.dwFlags | STARTF_USESTDHANDLES;
        siStartInfo.hStdInput = hChildStdin;
        siStartInfo.hStdOutput = hChildStdout;
        siStartInfo.hStdError = hChildStderr;
    }

    dwCode = CreateProcess(0, 
                           cmd,
                           NULL, 
                           NULL, 
                           bLog, // Inherit handles only if logging required
                           0,
                           0,
                           0,
                           &siStartInfo, 
                           &piProcessInformation);

    // We do not need the thread handle
    CloseHandle (piProcessInformation.hThread);


    bool bCanceled = false;
    bool bReadOK = true;
    if (bLog) {
        // close our copies
        CloseHandle (hChildStdout);
        CloseHandle (hChildStderr);

        if (dwCode == 0) {
            CloseHandle(hStdoutRd);
            FatalError(IDS_CANNOT_RUN_PROCESS, command_line);
        }

        // read the pipe
        char buf[ PIPE_SIZE ];
        DWORD n;
        while (! bCanceled && bReadOK) {
            bReadOK = (ReadFile(hStdoutRd, buf, PIPE_SIZE, &n, 0) > 0);
            if (bReadOK && n > 0) {
                theLog ("%.*s", (int) n, buf);
            }
            bCanceled = GetCancelFlag();
        }
        
        // close the read end of the pipe
        CloseHandle (hStdoutRd);
    }

    if (! bCanceled) {
        // if process ended with broken pipe, then ok
        if (! bReadOK && GetLastError () == ERROR_BROKEN_PIPE)
            bReadOK = true;
        // wait for the process to terminate
        if (bWait) {
            WaitForSingleObject (piProcessInformation.hProcess, INFINITE);
            // get the exit code
            if (bReadOK) {
                DWORD dwExitCode;
                GetExitCodeProcess (piProcessInformation.hProcess, &dwExitCode);
                ret = (dwExitCode == 0);
            }
        }
        else {
            // We don't want to wait, so assume it is ok
            ret = true;
        }
    }
    // close process object
    CloseHandle (piProcessInformation.hProcess);
    // Bring main window to front
    theApp.m_pMainWnd->SetForegroundWindow();

    return (ret);
}

#if 0
void AddRunOnce (const char *szValue, const char *szCommand)
{
    theLog("adding RunOnce value: %s=%s\n", szValue, szCommand);

    CRegistry theRegistry;
    if (theRegistry.Connect(HKEY_LOCAL_MACHINE) == FALSE) {
        theLog("Registry: can't connect to HKEY_LOCAL_MACHINE (Error %d)\n",
               GetLastError());
        FatalError(IDS_CANNOT_CREATE_RUNONCE_VALUE);
    }

    if (theRegistry.Open(TEXT("SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\RunOnce"),
                         (CRegistry::CreatePermissions)CRegistry::permissionAllAccess) == FALSE ) {
        theLog("Registry: can't open runonce key (Error %d)\n",
               GetLastError());
        FatalError(IDS_CANNOT_CREATE_RUNONCE_VALUE);
    }

    if (g_bDryRun) 
        return;

    else if (theRegistry.SetValue(szValue, szCommand) == FALSE) {
        theLog("Registry: can't create runonce %s=%s (Error %d)\n",
               GetLastError());
        FatalError(IDS_CANNOT_CREATE_RUNONCE_VALUE);
    }
}
#endif

void DeSelectAll()
{
    POSITION pos;
    CString sKey;
    TPM *tpm;
    pos = g_vSourceTPM.GetStartPosition();
    while (pos) {
        g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
        tpm->DeSelect();
    }
}

TPM * PackageLookup(const CString &sName)
{
    static char *subdirs[] = { "collection", "package", "support", "binary", NULL };
    TPM *tpm = 0;
    CString sKey = sName;
    if (! g_vSourceTPM.Lookup(sKey, tpm)) {
        for (char **p = subdirs; *p != NULL; p++) {
            CString sKey = ConcatPath(*p, sName, '/');
            if (g_vSourceTPM.Lookup(sKey, tpm))
                break;
        }
    }
    return tpm;
}

int WINAPI DoPrint (LPSTR buf, unsigned long size)
{
    // theLog ("%.*s", (int) size, (const char *) buf);
    return ((unsigned int) size);
}

int WINAPI DoReplace (char *filename)
{
    return (IDM_REPLACE_ALL);
}

int WINAPI DoProgress (LPCSTR filename, unsigned long size)
{
    CSingleLock theLock (&g_csCritSec, TRUE);
    theStatus.dwPackageDone += size;
    theStatus.dwOverallDone += size;
    return (GetCancelFlag() ? 1 : 0);
}

void WINAPI UnzipReceivedDllMessage(unsigned long ucsize, unsigned long csiz,
                                    unsigned cfactor,
                                    unsigned mo, unsigned dy, unsigned yr, unsigned hh, unsigned mm,
                                    char c, LPSTR filename, LPSTR methbuf, unsigned long crc, char fCrypt)
{

}

bool UnzipFile(const CString &zipName, const CString &destdir, 
               const CStringArray *sa, bool bTotalSizeUnknown)
{
    CSingleLock theLock(&g_csCritSec, TRUE);
    theStatus.nOpId = IDS_NOW_UNZIPPING;
    theStatus.sName = (LPCTSTR)zipName;
    theStatus.dwPackageDone = 0;
    theLock.Unlock();

    int nb_files_to_extract = 0;
    char **files_to_extract = NULL;
  
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


    
    theLog ("\nunzip %s -d %s\n", (LPCTSTR) zipName, (LPCTSTR) destdir);
    
    if (bTotalSizeUnknown) {
        // run unzip to get the size of the archive
        options.nvflag = 1;
        functions.SendApplicationMessage = UnzipReceivedDllMessage;
        int rc = ::Wiz_SingleEntryUnzip(nb_files_to_extract, files_to_extract, 0, 0, &options, &functions);
        if (rc != 0 && GetCancelFlag()) {
            theLog (IDS_ERROR_UNZIPPING, (const char *) zipName);
            return false;
        }
        theLock.Lock();
        theStatus.dwPackageSize = functions.TotalSize;
        theLock.Unlock();
	theLog("unzip has found that the archive has an uncompressed total size of %ld bytes.\n", functions.TotalSize);
    }

    // run unzip
    options.nvflag = 0;
    int rc = ::Wiz_SingleEntryUnzip(nb_files_to_extract, files_to_extract, 0, 0, &options, &functions);
    if (rc != 0 && GetCancelFlag()) {
        theLog (IDS_ERROR_UNZIPPING, (const char *) zipName);
        return false;
    }
    
    if (sa) {
        delete [] files_to_extract;
    }

    return true;
}


void __cdecl DoDownloadProgress(unsigned long partial, unsigned long total)
{
    CSingleLock singleLock(&g_csCritSec, TRUE);
    theStatus.dwPackageSize = (total == 0 ? -1 : total);
    theStatus.dwPackageDone = partial;
}

void __cdecl DoDownloadLog(char *s, ...)
{
    va_list argList;
    va_start (argList, s);
    theLog.LogV (s, argList);
    va_end (argList);
}

bool GrabInternetFile(const CString &url, CString & filename)
{
    // If being given an empty destination file, then build
    // a temporary one
    if (filename.IsEmpty()) {
        int pos = url.ReverseFind('/');
        if (pos == -1) {
            pos = url.ReverseFind('\\');
            if (pos == -1) {
                CreateTempFile(g_sTempPath, filename);
            }
            else {
                filename = ConcatPath(g_sTempPath, url.Right(url.GetLength() - pos - 1));
            }
        }
        else {
            filename = ConcatPath(g_sTempPath, url.Right(url.GetLength() - pos - 1));
        }
    }
    else {
        // Make sure the dest directory exists
        CString sDestDir = filename;
        GetParentDirectory(sDestDir);
        if (! DirectoryExists(sDestDir)) {
            CreateDirectoryPath(sDestDir);
        }
    }
    CSingleLock theLock(&g_csCritSec, FALSE);
    theStatus.nOpId = IDS_NOW_DOWNLOADING;
    theStatus.sName = url;
    theLock.Unlock();
    char *_url = new char[url.GetLength()+1];
    char *_file = new char[filename.GetLength()+1];
    char *_proxy_address = new char[g_sProxyAddress.GetLength()+1];
    strcpy(_url, (LPCTSTR)url);
    strcpy(_file, (LPCTSTR)filename);
    strcpy(_proxy_address, (LPCTSTR)g_sProxyAddress);
    bool bRet = (get_url_to_file(_url, _file, 0, 
                                 DoDownloadLog, DoDownloadProgress,
                                 AfxGetInstanceHandle(),
                                 g_uiNetMethod, 
                                 _proxy_address, 
                                 g_uiProxyPort) == 0);
    delete [] _url;
    delete [] _file;
    delete [] _proxy_address;
    return bRet;
}

CString GrabInternetFile(const CString &url)
{
    CSingleLock theLock(&g_csCritSec, FALSE);
    theStatus.nOpId = IDS_NOW_DOWNLOADING;
    theLock.Unlock();
    
    char *_url = new char[url.GetLength()+1];
    char *_proxy_address = new char[g_sProxyAddress.GetLength()+1];
    strcpy(_url, (LPCTSTR)url);
    strcpy(_proxy_address, (LPCTSTR)g_sProxyAddress);
    char *data = get_url_to_string(_url,
                                   DoDownloadLog, DoDownloadProgress,
                                   AfxGetInstanceHandle(),
                                   g_uiNetMethod, 
                                   _proxy_address, 
                                   g_uiProxyPort);
    CString sData(data);
    // Memory leak there because we can't free memory allocated in another
    // module !
    // if (data) free(data);
    delete [] _url;
    delete [] _proxy_address;
    return sData;

}

// Install a new Ghostscript only if no other version can be found
// else warn the user.
void CheckForGs()
{
    bool bInstallGs = (gs_locate() == NULL); // install if not found
    TPM *tpmGsFree = NULL;
    TPM *tpmGsNonFree = NULL;
    TPM *tpmGs = NULL;

    g_vSourceTPM.Lookup("support/ghostscript-free", tpmGsFree);
    g_vSourceTPM.Lookup("support/ghostscript-nonfree", tpmGsNonFree);

    // one must be selected
    if (tpmGsNonFree && tpmGsNonFree->IsSelected()) {
        tpmGs = tpmGsNonFree;
        // Keep only the non free one when both asked
        if (tpmGsFree && tpmGsFree->IsSelected()) {
            tpmGsFree->DeSelect();
        }
    }
    else if (tpmGsFree && tpmGsFree->IsSelected()) {
        tpmGs = tpmGsFree;
    }
    else {
        // Nothing to do
        return;
    }

    if (bInstallGs == true) {
        theLog("Ghostscript: installing %s version %s\n", tpmGs->GetName(), tpmGs->GetVersion());
    }
    else {
        // check for a newer version
        if (gs_version_cmp(tpmGs->GetVersion(), gs_revision) > 0) {
            CString sMsg;
            sMsg.Format("Ghostscript: your version is %s older than the one offered on the cdrom\r\nIt is recommended that you upgrade it.\r\nDo you accept ?", gs_revision);
            DWORD nRep = AfxMessageBox(sMsg, MB_YESNO);
            if (nRep == IDYES) {
                bInstallGs = true;
                theLog("Ghostscript: installing %s version %s\n", tpmGs->GetName(), tpmGs->GetVersion());
            }
            else {
                theLog("Ghostscript: user rejected installation of version %s when version %s is installed.\n",
                       tpmGs->GetVersion(), gs_revision);
            }
        }
        else {
            theLog("Ghostcript: version %s installed is newer than %s, do nothing.\n", gs_revision, tpmGs->GetVersion());
        }
    }
    // Deselect if not required.
    if (! bInstallGs) {
        tpmGs->DeSelect();
    }

}

void AddFormatToBuild(TPM *tpm, const char *fmtname)
{
    // Add it if not already there
    CString sKey(fmtname);
    TPM *tpmDummy;
    if (!g_vFormatsToBuild.Lookup(sKey, tpmDummy)) {
        g_vFormatsToBuild[sKey] = tpm;
    }
}

void AddLanguageDat(TPM *tpm, const char *lang)
{
    // Add it if not already there
    CString sKey(lang);
    TPM *tpmDummy;
    if (!g_vLanguageDat.Lookup(sKey, tpmDummy)) {
        g_vLanguageDat[sKey] = tpm;
    }
}

void AddMapFile(TPMSet &vMap, TPM *tpm, const char *map)
{
    // Add it if not already there
    CString sKey(map);
    TPM *tpmDummy;
    if (!vMap.Lookup(sKey, tpmDummy)) {
        vMap[sKey] = tpm;
    }
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
