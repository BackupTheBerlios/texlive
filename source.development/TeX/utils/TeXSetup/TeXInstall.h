// TeXSetup.h: interface for the TeXSetup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TEXSETUP_H__ADD1E498_C8E3_4BA7_B749_84303FFE65C6__INCLUDED_)
#define AFX_TEXSETUP_H__ADD1E498_C8E3_4BA7_B749_84303FFE65C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "afxmt.h"
#include "afxtempl.h"
#include "resource.h"
#include "geturl.h"
#include "expat.h"
#include "Log.h"
#include "Environment.h"
#include "ShellLinkData.h"
#include "TeXSetupCommandLineInfo.h"
#include "TPM.h"

#define TEXLIVE_GLOBAL_REGENTRY "SOFTWARE\\TeX Users Group\\TeXLive"
#define TEXLIVE_USER_REGENTRY "Software\\TeX Users Group\\TeXLive"
#define TEXLIVE_VERSION "8.0"
#define TEXLIVE_FOLDERNAME "TeXLive"

enum ConfigurationState {
    CFG_BEGIN = 1,
    CFG_SYSTEM_DLLS,
    CFG_SUPPLEMENTARY_TOOLS,
    CFG_LSR_DATABASES,
    CFG_FORMAT_FILES,
    CFG_ENVIRONMENT,
    CFG_SHELL_OBJECTS,
    CFG_END
};

enum UninstallState {
    RMBEGIN = 1,
    RMFILES,
    RMMENUS,
    RMREGISTRY,
    RMENVIRONMENT,
    RMEND
};

enum SourceType {
    SOURCE_CDROM = 1,
    SOURCE_ZIP,
    SOURCE_INTERNET
};

enum InstallType {
    INSTALL_FULL = 1,
    INSTALL_MAINTENANCE,
    INSTALL_UNINSTALL
};

/* Where all information will be logged */
extern LogFile theLog;

/* Structure used to communicate between threads */
struct StatusData {
    inline StatusData() : 
        dwOverallDone(0), dwPackageDone(0), dwPackageSize(0), sName(0),
        bReady(false), bError(false), bTerminate(false),
        bNewTPM(false), nOpId(IDS_NOW_INSTALLING), nConfig(::CFG_BEGIN),
        bReboot(false) { tpm = 0;}
    DWORD dwPackageDone;        /* How much of the current package is done */
    DWORD dwPackageSize;        /* Total size of the package */
    DWORD dwOverallDone;        /* How much of the overall size is installed */
    TPM *tpm;			/* The current tpm being processed */
    const char *sName;		/* Some name (maybe we could use tpm->m_sName */
    bool bNewTPM;               /* If the tpm has changed */
    bool bReady;                /* Thread terminated normally */
    bool bError;                /* Thread terminated in error */
    bool bTerminate;		/* Thread has been cancelled */
    bool bReboot;
    UINT nConfig;
    UINT nOpId;
};

extern StatusData theStatus;

extern CCriticalSection g_csCritSec;
extern CString g_sTempPath;

extern CString g_sOsVersion;
extern CString g_sDllVersion;

extern SourceType g_eSourceType;
extern InstallType g_eInstallType;

extern CString g_sDefaultRootDir;       /* The default place to install TeXLive */ 
extern CString g_sLocalSourceDir;	/* Source for TeX files, local in principle */
extern CString g_sRemoteSourceDir;	/* Source for extra files, remote in principle */

extern CString g_sSourceTPMDir;
extern CString g_sLocalTPMDir;
extern bool g_bRemoveSourceTPMDir;

extern CString g_sHomeDir;

extern CString g_sInstallRootDir;
extern CString g_sTexmfMain;
extern CString g_sTexmfLocal;
extern CString g_sTexmfExtra;
extern CString g_sTexmfHome;
extern CString g_sVarTexmf;
extern CString g_sVarTexFonts;
extern CString g_sWin32Dir;
extern CString g_sFpTeXDoc;
extern CString g_sBaseUrl;
extern CString g_sModuleName;

extern CString g_sPreviousRoot;
extern CString g_sGlobalRegKey;
extern CString g_sUserRegKey;

extern unsigned long g_dwClusterSize;

extern unsigned long g_dwSpaceNeeded;
extern unsigned long g_dwSpaceAvailable;

extern unsigned long g_dwTPMNumber; // Number of tpm files in the texmf/tpm directory

extern CString g_sLastError;

extern bool g_bDryRun;
extern bool g_bAutomaticReboot;

extern bool g_bInstallDocumentation;
extern bool g_bInstallSource;
extern bool g_bInstallXEmTeX;
extern bool g_bAllUsers;

extern bool g_bQuickInstall;

extern bool g_bBuildLanguageDat;

extern bool g_bIsAdmin;
extern bool g_bIsWindowsNT;

extern bool g_bAddPackage;
extern CString g_sPackage;

extern CString g_sSchemeName;

extern CString g_sFolderName;

extern CString g_sProxyAddress;
extern UINT g_uiProxyPort;
extern NetIOType g_uiNetMethod;

extern TPM *g_tpmRoot;          // The root item of the tree
extern TPMSet g_vSourceTPM;     // The map of available tpms
extern TPMSet g_vInstalledTPM;  // The map of installed tpms
extern TPMSet g_vSchemeTPM;     // The map of schemes

// A map of <CString, TPM *> pairs
extern TPMSet g_vFormatsToBuild; // The map of formats names to build
extern TPMSet g_vLanguageDat; // The map of language.xx.dat

extern TPMSet g_vUpdMap;        // Map lines for updmap.cfg file
extern TPMSet g_vUpdMixedMap;   // MixedMap lines for updmap.cfg file
extern TPMSet g_vDvipsMap;      // Map lines for dvips config.ps file
extern TPMSet g_vPdfTeXMap;     // Map lines for pdftex pdftex.cfg file
extern TPMSet g_vDvipdfmMap;    // Map lines for dvipdfm config file

/*
  Support functions
*/
extern bool GetFileVersion (const char *filename, DWORD &dwMS, DWORD &dwLS);
extern int CompareFileVersions (const char *filename1, const char *filename2);
extern void AddRunOnce (const char *szValue, const char *szCommand);
extern bool MoveFile (const char *oldfile, const char *newfile, bool &bReboot);
extern bool GrabInternetFile(const CString &url, CString &filename);
extern CString GrabInternetFile(const CString &url);

extern int WINAPI DoProgress(LPCSTR filename, unsigned long size);
extern int WINAPI DoReplace (char *filename);
extern int WINAPI DoPrint (LPSTR buf, unsigned long size);

extern bool UnzipFile(const CString &zipName, const CString &destdir, 
                      const CStringArray *sa = NULL, bool bTotalSizeUnknown = false);

extern bool CreateShellLink(const char *szFolderPath,
                     const ShellLinkData &ld, const CString &sRootDir);
extern bool CreateShellObjects (const ShellLinkData s[], size_t n, const CString &sRootDir);
extern bool GetSpecialFolderPath (int nFolder, char *path);
extern bool CreateProgramFolder (char *szPath);

extern BOOL FindPreviousTeX(CString &);
extern BOOL FindHomeDir(CString &);

extern bool ReadTPM(const CString &sSubdir, TPMSet *mapTPM);

extern bool GetLogFile();

extern void FatalError (UINT nMsgID, ...);

extern void SanityCheck();

extern bool BuildLanguageDat();

extern void CheckForGs();

/*
  Functions related to the installation job
*/

extern void TPMMapCleanup(TPMSet &tpmMap);
extern void TeXInstallCleanUp();

extern TPM * PackageLookup(const CString& sName);
extern void DeSelectAll();

extern BOOL EarlyInitialize(const CTeXSetupCommandLineInfo &);
extern void InitializeTPM(const CString &, TPMSet &, TPMSet *scheme = NULL);
extern void SelectDefault(TPMSet &);

extern CString GetSafePathName(CString &path, bool bQuoted = true);

extern void DoPrepareInstallation();
extern void DoDownloadFiles();
extern void DoPrepareCopying();
extern void DoCopyFiles();
extern void DoConfiguration();
extern void Finalize();
extern void DoUninstall();
extern bool RunProcess(const char *command_line, bool bLog = true, bool bWait = true, bool bShow = true);

extern bool RecursivelyRemove(const CString &sDir);

extern bool SafeCopyFile(const CString &src, const CString &dst, bool bBackup = false, bool bLog = true);

extern BOOL RemovePath(const char *filename);

extern bool DoProgress(unsigned long size);
extern bool GetCancelFlag();
extern void SetCancelFlag (bool f);

extern void UpdateSelection();
extern BOOL UpdateSize();
extern BOOL RecalcSize();

extern void SelectDefaultSetup();

extern void SetRootDir(const CString &);

extern XML_Parser theParser;

extern BOOL parseTPM(void *action_parameter, const CString &filename);

extern void AddFormatToBuild(TPM *tpm, const char *fmtname);
extern void AddLanguageDat(TPM *tpm, const char *lang);
extern void AddMapFile(TPMSet &vMap, TPM *tpm, const char *map);

extern Environment theEnvironment;

#endif // !defined(AFX_TEXSETUP_H__ADD1E498_C8E3_4BA7_B749_84303FFE65C6__INCLUDED_)
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

