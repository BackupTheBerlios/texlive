// TeXSetup.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TeXSetup.h"
#include "TeXSetupWizard.h"
#include "TeXInstall.h"
#include "TeXSetupCommandLineInfo.h"
#include "Win32Util.h"
#include <getopt.h>

#include "LimitSingleInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CFont fntWelcome;
CFont fntHeader;

CTeXSetupCommandLineInfo cmdinfo;

// The one and only CLimitSingleInstance object
CLimitSingleInstance g_SingleInstanceObj(TEXT("TeXLive 8.0 - TeXSetup"));

/////////////////////////////////////////////////////////////////////////////
// Command Line Parsing and handling

#define OPT_AUTOMATIC_REBOOT 2
#define OPT_DRYRUN 1
#define OPT_QUICK 'q'
#define OPT_INSTALL_DIR 'i'
#define OPT_TEXMFMAIN_DIR 'M'
#define OPT_TEXMFLOCAL_DIR 'L'
#define OPT_TEXMFEXTRA_DIR 'E'
#define OPT_TEXMFHOME_DIR 'H'
#define OPT_VARTEXMF_DIR 'T'
#define OPT_VARTEXFONTS_DIR 'F'
#define OPT_PROGRAM_FOLDER 'f'
#define OPT_SOURCE_DIRECTORY 's'
#define OPT_UNINSTALL 'u'
#define OPT_HELP 'h'
#define OPT_WITH_SOURCE 'S'
#define OPT_WITH_DOC 'D'
#define OPT_NET_METHOD 'n'
#define OPT_ADD_PACKAGE 'a'
#define OPT_SCHEME 'c'
#define OPT_MAINTENANCE 'm'
#define OPT_REMOTE_DIR 'r'
#define OPT_WITH_XEMTEX 'x'

const struct option long_options[] =
    {
        "automatic-reboot",             no_argument,            0,      OPT_AUTOMATIC_REBOOT,
        "dry-run",			no_argument,		0,	OPT_DRYRUN,
        "quick",			no_argument,		0,	OPT_QUICK,
        "net-method",                   required_argument,      0,      OPT_NET_METHOD,
        "remote-source-directory",	required_argument,      0,      OPT_REMOTE_DIR,
        "local-source-directory",	required_argument,	0,	OPT_SOURCE_DIRECTORY,
        "installation-directory",       required_argument,	0,	OPT_INSTALL_DIR,
        "texmfmain-directory",		optional_argument,	0,	OPT_TEXMFMAIN_DIR,
        "texmflocal-directory",		optional_argument,	0,	OPT_TEXMFLOCAL_DIR,
        "texmfextra-directory",		optional_argument,	0,	OPT_TEXMFEXTRA_DIR,
        "texmfhome-directory",		optional_argument,	0,	OPT_TEXMFHOME_DIR,
        "vartexmf-directory",		optional_argument,	0,	OPT_VARTEXMF_DIR,
        "vartexfonts-directory",	optional_argument,	0,	OPT_VARTEXFONTS_DIR,
        "with-source-files",		optional_argument,	0,	OPT_WITH_SOURCE,
        "with-documentation-files",	optional_argument,	0,	OPT_WITH_DOC,
        "with-xemtex",                  optional_argument,      0,      OPT_WITH_XEMTEX,
        "program-folder",		required_argument,	0,	OPT_PROGRAM_FOLDER,
        "add-package",		        optional_argument,	0,	OPT_ADD_PACKAGE,
        "maintenance",		        optional_argument,	0,	OPT_MAINTENANCE,
        "scheme",			required_argument,	0,	OPT_SCHEME,
        "uninstall",			no_argument,		0,	OPT_UNINSTALL,
        "help",				no_argument,		0,	OPT_HELP,
        0,				no_argument,		0,	0,
    };

static void AddArgument(const CString &arg, int &argc,
			char ** &argv, int &max_args)
{
    if (argc == max_args) {
	max_args += 10;
	argv = reinterpret_cast<char**>(realloc(argv, max_args * sizeof(argv[0])));
    }
    argv[ argc++ ] = _strdup(arg);
}

static void
GetArguments(const char *command_line, const char *argv0,
	     int &argc, char ** & argv)
{
    int max_args = 0;
    bool copying = false;
    bool in_quoted_arg = false;

    const char * cp = command_line;

    argc = 0;
    argv = 0;

    AddArgument (argv0, argc, argv, max_args);

    CString arg;

    while (*cp) {
	if (*cp == ' ' && ! in_quoted_arg) {
	    if (copying) {
		AddArgument (arg, argc, argv, max_args);
		arg = "";
		copying = false;
	    }
	}
	else if (*cp == '"') {
	    in_quoted_arg = ! in_quoted_arg;
	    copying = true;
	}
	else {
	    arg += *cp;
	    copying = true;
	}
	cp++;
    }

    if (copying)
	AddArgument (arg, argc, argv, max_args);
}

static void
FreeArguments (int argc, char ** &argv)
{
    for (int i = 0; i < argc; i++) {
	free (argv[i]);
	argv[i] = 0;
    }
    free (argv);
    argv = 0;
}

static void
ShowHelpAndExit (int code = 0)
{
    MessageBox (NULL, 
                "Usage: TeXSetup [OPTIONS]\r\n\r\n\
Options:\r\n\r\n\
  --automatic-reboot\r\n\
  --dry-run\r\n\
  --quick\r\n\
  --net-method (ie5|direct)\r\n\
  --local-source-directory <dir>\r\n\
  --remote-source-directory <url>\r\n\
  --installation-directory <dir>\r\n\
  --texmfmain-directory <dir>\r\n\
  --texmflocal-directory <dir>\r\n\
  --texmfextra-directory <dir>\r\n\
  --texmfhome-directory <dir>\r\n\
  --vartexmf-directory <dir>\r\n\
  --vartexfonts-directory <dir>\r\n\
  --with-source-files\r\n\
  --with-documentation-files\r\n\
  --with-xemtex\r\n\
  --program-folder <folder>\r\n\
  --add-package <pkg>\r\n\
  --scheme <scheme>\r\n\
  --maintenance\r\n\
  --uninstall\r\n\
  --help\r\n\r\n", 
		(code == 0 ? "TeXSetup valid options" : "Error: invalid TeXSetup option"),
		MB_OK
		);
    exit (code);
}

static void
ParseSetupCommandLine(const char *command_line, CTeXSetupCommandLineInfo &cmdinfo)
{
    int argc;
    char **argv;

    int option_index = 0;
    int c;

    GetArguments (command_line, AfxGetAppName(), argc, argv);

    cmdinfo.m_nShellCommand = CCommandLineInfo::FileNothing;

    optind = 0;
    while ((c = getopt_long_only(argc, argv, "", long_options, &option_index)) != EOF) {
	switch (c) {
	    
	case OPT_HELP:
	    FreeArguments (argc, argv);
	    ShowHelpAndExit ();
	    break;
	    
	case OPT_INSTALL_DIR:
	    cmdinfo.m_sInstallRootDir = optarg;
	    break;
	    
	case OPT_AUTOMATIC_REBOOT:
	    cmdinfo.m_bAutomaticReboot = true;
	    break;
		  
	case OPT_TEXMFMAIN_DIR:
	    cmdinfo.m_sTexmfMainDir = optarg;
	    break;
	    		  
	case OPT_TEXMFLOCAL_DIR:
	    cmdinfo.m_sTexmfLocalDir = optarg;
	    break;
	    		  
	case OPT_TEXMFEXTRA_DIR:
	    cmdinfo.m_sTexmfExtraDir = optarg;
	    break;
	    
	case OPT_TEXMFHOME_DIR:
	    cmdinfo.m_sTexmfHomeDir = optarg;
	    break;
	    
	case OPT_VARTEXMF_DIR:
	    cmdinfo.m_sVarTexmfDir = optarg;
	    break;
	    
	case OPT_VARTEXFONTS_DIR:
	    cmdinfo.m_sVarTexFontsDir = optarg;
	    break;
	    
	case OPT_PROGRAM_FOLDER:
	    cmdinfo.m_sFolderName = optarg;
	    break;

	case OPT_SOURCE_DIRECTORY:
	    cmdinfo.m_sLocalSourceDir = optarg;
	    break;
		  
	case OPT_QUICK:
	    cmdinfo.m_bQuickInstall = true;
	    break;
	    
	case OPT_NET_METHOD:
            if (! optarg) {
                MessageBox(NULL, "Argument required for `--net-method' option!",
                           "Invalid option", MB_OK);
            }
            else {
                if (_stricmp(optarg, "ie5") == 0) {
                    cmdinfo.m_uiNetMethod = NetIOIE5;
                }
                else if (_stricmp(optarg, "direct") == 0) {
                    cmdinfo.m_uiNetMethod = NetIODirect;
                }
                else {
                    MessageBox(NULL, "Invalid argument for `--net-method' option!",
                               "Invalid option", MB_OK);
                    cmdinfo.m_uiNetMethod = NetIOUnavail;
                }
            }
            break;
	    
	case OPT_REMOTE_DIR:
	    cmdinfo.m_sRemoteSourceDir = optarg;
	    break;
	    
	case OPT_WITH_SOURCE:
	    cmdinfo.m_bWithSource = true;
            if (optarg && (*optarg == 'n' || *optarg == 'N'))
                cmdinfo.m_bWithSource = false;
	    break;
	    
	case OPT_WITH_DOC:
	    cmdinfo.m_bWithDoc = true;
            if (optarg && (*optarg == 'n' || *optarg == 'N'))
                cmdinfo.m_bWithDoc = false;
	    break;

	case OPT_WITH_XEMTEX:
	    cmdinfo.m_bXEmTeX = true;
            if (optarg && (*optarg == 'n' || *optarg == 'N'))
                cmdinfo.m_bXEmTeX = false;
	    break;
	    
	case OPT_DRYRUN:
	    cmdinfo.m_bDryRun = true;
	    break;

	case OPT_ADD_PACKAGE:
	    cmdinfo.m_bAddPackage = true;
	    cmdinfo.m_sPackage = optarg;
	    break;

        case OPT_SCHEME:
            cmdinfo.m_sScheme = optarg;
            break;

	case OPT_MAINTENANCE:
	    cmdinfo.m_bMaintenance = true;
	    break;

	case OPT_UNINSTALL:
	    cmdinfo.m_bUninstall = true;
	    break;

	default:
	    FreeArguments (argc, argv);
	    ShowHelpAndExit (1);
	    break;
	}
    }

}

/////////////////////////////////////////////////////////////////////////////
// CTeXSetupApp

BEGIN_MESSAGE_MAP(CTeXSetupApp, CWinApp)
    //{{AFX_MSG_MAP(CTeXSetupApp)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
// CTeXSetupApp construction

CTeXSetupApp::CTeXSetupApp()
{
    EnableHtmlHelp();
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTeXSetupApp object

CTeXSetupApp theApp;

/////////////////////////////////////////////////////////////////////////////
// DLL versions
DWORD dwComctl32Version = GetDllVersion("comctl32.dll");
DWORD dwShell32Version = GetDllVersion("shell32.dll");
DWORD dwShlwapiVersion = GetDllVersion("shlwapi.dll");

CString g_sCaption;

/////////////////////////////////////////////////////////////////////////////
// CTeXSetupApp initialization

BOOL CTeXSetupApp::InitInstance()
{
    // Quit if we are already running
    if (g_SingleInstanceObj.IsAnotherInstanceRunning())
       return FALSE; 

    // InitCommonControls() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    InitCommonControls();
    
    CWinApp::InitInstance();
    
    if (!AfxSocketInit()) {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    // get command-line arguments
    // We might get command line arguments from some .ini file also
    ParseSetupCommandLine (m_lpCmdLine, cmdinfo);
    // Get system version
    g_iSystemType = GetSystemType();
    // Initialize critical information
    if (EarlyInitialize(cmdinfo) == FALSE) {
        return FALSE;
    }

    // Create the font for titles
    fntWelcome.CreatePointFont (120, "Arial Bold");
    LOGFONT logFont;
    memset (&logFont, 0, sizeof(LOGFONT));
    logFont.lfCharSet = DEFAULT_CHARSET;
    logFont.lfHeight = 80;
    logFont.lfWeight = FW_BOLD;
    strcpy (logFont.lfFaceName, "MS Sans Serif");
    fntHeader.CreatePointFontIndirect (&logFont);

    // The wizard
    CTeXSetupWizard dlg;
    m_pMainWnd = &dlg;

    // Create an invisible window of the registered class.
    // Now the autorun will be able to detect that another
    // occurence of the setup is running.
    CWnd wndBack;
    RECT rDesktop;
    GetWindowRect(::GetDesktopWindow(), &rDesktop);
    
    wndBack.Create("TeXSetupWizard", 
		   "TeXSetup Background",
		   WS_DISABLED,
		   rDesktop, 
		   m_pMainWnd->GetDesktopWindow(), 
		   7259);

    // Warn the user if there is a conflict between a previous setup
    // and command line
    if (g_eInstallType == INSTALL_FULL 
	&& ! g_sPreviousRoot.IsEmpty()
	&& (g_sPreviousRoot != g_sInstallRootDir)) {
        CString sMsg;
        sMsg.FormatMessage(IDS_PREVIOUS_SETUP_DETECTED, g_sPreviousRoot, 
                           g_sInstallRootDir);
        int nRep = AfxMessageBox(sMsg, MB_OKCANCEL | MB_ICONQUESTION);
        if (nRep == IDCANCEL) {
            return FALSE;
        }
    }

    g_sCaption.LoadString(IDS_CAPTION);
    dlg.SetTitle(g_sCaption);
    INT_PTR nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with OK
        // DoInstall();
    }
    else if (nResponse == IDCANCEL) {
        // TODO: Place code here to handle when the dialog is
        //  dismissed with Cancel
    }

    TeXInstallCleanUp();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

void CTeXSetupApp::OnWizard()
{
    // TODO: The property sheet attached to your project
    // via this function is not hooked up to any message
    // handler.  In order to actually use the property sheet,
    // you will need to associate this function with a control
    // in your project such as a menu item or tool bar button.

    //	CTeXSetupWizard propSheet;

    //	propSheet.DoModal();

    // This is where you would retrieve information from the property
    // sheet if propSheet.DoModal() returned IDOK.  We aren't doing
    // anything for simplicity.
}

void CTeXSetupApp::WinHelp(DWORD dwData, UINT nCmd) 
{
    // TODO: Add your specialized code here and/or call the base class
    ShellExecute(0, "open", g_sFpTeXDoc, 0, 0, SW_SHOWNORMAL);
    // CWinApp::WinHelp(dwData, nCmd);
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

