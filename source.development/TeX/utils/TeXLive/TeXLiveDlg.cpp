// TeXLiveDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeXLive.h"
#include "TeXLiveDlg.h"
#include "ListChoiceDlg.h"
#include "TeXLiveCmds.h"
#include "Win32Util.h"
#include <process.h>
#include <Htmlhelp.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
    CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
    enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    //{{AFX_MSG(CAboutDlg)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
        };

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTeXLiveDlg dialog

CTeXLiveDlg::CTeXLiveDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CTeXLiveDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CTeXLiveDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDI_TEXLIVE);
}

void CTeXLiveDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CTeXLiveDlg)
    //	DDX_Control(pDX, IDC_STATIC_INSTALLATION, m_stcInstallation);
    //	DDX_Control(pDX, IDC_STATIC_DOCUMENTATION, m_stcDocumentation);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CTeXLiveDlg, CDialog)
    //{{AFX_MSG_MAP(CTeXLiveDlg)
    ON_WM_SYSCOMMAND()
    ON_WM_DESTROY()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND(ID_EXPLORECD_SELECTEDITOR, OnSelectEditor)
    ON_COMMAND(ID_EXPLORECD_RUNTEXOFFCD, OnRunTexCdrom)
    ON_COMMAND(ID_EXPLORECD_CLEANUPCD, OnCdromCleanup)
    ON_COMMAND(ID_EXPLORECD_BROWSECD, OnBrowseCdrom)
    ON_COMMAND(ID_INSTALL_INTERNETACCESS, OnInternetAccess)
    ON_COMMAND(ID_INSTALL_TEXLIVE, OnRunTexsetup)
    ON_COMMAND(ID_INSTALL_UNINSTALL, OnTexliveUninstall)
    ON_COMMAND(ID_EDITORS_NTEMACS, OnEditorsNTEmacs)
    ON_COMMAND(ID_EDITORS_TEXNICCENTER, OnEditorsTeXnicCenter)
    ON_COMMAND(ID_EDITORS_WINEDT, OnEditorsWinEdt)
    ON_COMMAND(ID_EDITORS_WINSHELL, OnEditorsWinShell)
    ON_COMMAND(ID_EDITORS_XEMACS, OnEditorsXEmacs)
    ON_COMMAND(ID_SUPPORT_FREEGHOSTSCRIPT, OnSupportFreeGhostscript)
    ON_COMMAND(ID_SUPPORT_GNUTOOLS, OnSupportGnuTools)
    ON_COMMAND(ID_SUPPORT_IMAGEMAGICK, OnSupportImageMagick)
    ON_COMMAND(ID_SUPPORT_ISPELL, OnSupportIspell)
    ON_COMMAND(ID_SUPPORT_NETPBM, OnSupportNetPbm)
    ON_COMMAND(ID_SUPPORT_PERL, OnSupportPerl)
    ON_COMMAND(ID_DOCUMENTATION_TEXLIVE, OnRunTexliveDoc)
    ON_COMMAND(ID_DOCUMENTATION_RUNTEXDOCTK, OnRunTeXDocTK)
    ON_COMMAND(ID_DOCUMENTATION_FPTEXWEBSITE, OnGotoFptex)
    ON_COMMAND(ID_DOCUMENTATION_TUGWEBSITE, OnGotoTug)
    ON_COMMAND(ID_MAINTENANCE_TEXLIVEMAINTENANCE, OnTexliveMaintenance)
    ON_COMMAND(ID_MAINTENANCE_SYSTEMDLLSUPGRADE, OnUpgradeDlls)
    ON_BN_CLICKED(IDCLOSE, OnClose)
    ON_WM_DRAWITEM()
    ON_COMMAND( ID_HELP, OnHelp ) 
	//}}AFX_MSG_MAP
	ON_STN_CLICKED(IDC_STATIC_WELCOME_TEXLIVE, OnStnClickedStaticWelcomeTexlive)
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
// CTeXLiveDlg message handlers

void CTeXLiveDlg::EnableInternetMenuItem(bool bEnable)
{
    DWORD dwFlags = (bEnable ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED);

    m_Menu.EnableMenuItem(ID_EDITORS_XEMACS, dwFlags);
    m_Menu.EnableMenuItem(ID_EDITORS_WINEDT, dwFlags);
    m_Menu.EnableMenuItem(ID_EDITORS_TEXNICCENTER, dwFlags);
    m_Menu.EnableMenuItem(ID_SUPPORT_PERL, dwFlags);
    m_Menu.EnableMenuItem(ID_SUPPORT_IMAGEMAGICK, dwFlags);
    m_Menu.EnableMenuItem(ID_SUPPORT_GSVIEW, dwFlags);
    m_Menu.EnableMenuItem(ID_SUPPORT_GNUTOOLS, dwFlags);
}

void CTeXLiveDlg::EnableRunOffCDMenuItem(bool bEnable)
{
    DWORD dwFlags = (bEnable ? MF_BYCOMMAND | MF_ENABLED : MF_BYCOMMAND | MF_GRAYED);

    m_Menu.EnableMenuItem(ID_EXPLORECD_RUNTEXOFFCD, dwFlags);
    m_Menu.EnableMenuItem(ID_EXPLORECD_CLEANUPCD, dwFlags);
}

BOOL CTeXLiveDlg::OnInitDialog()
{
    m_Menu.LoadMenu(IDR_MENU);
    SetMenu(&m_Menu);

    m_InternetBitmap.LoadBitmap(IDB_INTERNET);

    m_Menu.SetMenuItemBitmaps(ID_EDITORS_XEMACS, MF_BYCOMMAND, &m_InternetBitmap, NULL);
    m_Menu.SetMenuItemBitmaps(ID_EDITORS_WINEDT, MF_BYCOMMAND, &m_InternetBitmap, NULL);
    m_Menu.SetMenuItemBitmaps(ID_EDITORS_TEXNICCENTER, MF_BYCOMMAND, &m_InternetBitmap, NULL);
    m_Menu.SetMenuItemBitmaps(ID_SUPPORT_PERL, MF_BYCOMMAND, &m_InternetBitmap, NULL);
    m_Menu.SetMenuItemBitmaps(ID_SUPPORT_IMAGEMAGICK, MF_BYCOMMAND, &m_InternetBitmap, NULL);
    m_Menu.SetMenuItemBitmaps(ID_SUPPORT_GSVIEW, MF_BYCOMMAND, &m_InternetBitmap, NULL);
    m_Menu.SetMenuItemBitmaps(ID_SUPPORT_GNUTOOLS, MF_BYCOMMAND, &m_InternetBitmap, NULL);
        
    EnableRunOffCDMenuItem(!g_sEditor.IsEmpty());
    EnableInternetMenuItem(false);
    
    CDialog::OnInitDialog();

    // TODO: Add extra initialization here
	
    m_fntStatic.CreateFont(-16, 0, 0, 0, FW_BOLD, TRUE, FALSE,0,0,0,0,0,0, "Times New Roman");
    m_fntWelcome.CreateFont(-48, 0, 0, 0, FW_BOLD, TRUE, FALSE,0,0,0,0,0,0, "Times New Roman");

    m_stcInstallation.SubclassDlgItem(IDC_STATIC_INSTALLATION, this);
    m_stcInstallation.SetBkColor(RGB(255,255,255));
    m_stcInstallation.SetTextColor(RGB(0,0,255));
    m_stcInstallation.SetFont(&m_fntStatic);

    m_stcDocumentation.SubclassDlgItem(IDC_STATIC_DOCUMENTATION, this);
    m_stcDocumentation.SetBkColor(RGB(255,255,255));
    m_stcDocumentation.SetTextColor(RGB(0,0,255));
    m_stcDocumentation.SetFont(&m_fntStatic);


    CString sWelcome;
    sWelcome.LoadString(IDS_WELCOME_TEXLIVE);
    
    m_stcWelcome.SubclassDlgItem(IDC_STATIC_WELCOME_TEXLIVE, this);
    m_stcWelcome.SetBkColor(RGB(255,255,255));
    m_stcWelcome.SetTextColor(RGB(0,0,255));
    m_stcWelcome.SetWindowText(sWelcome);
    m_stcWelcome.SetFont(&m_fntWelcome);

    // Add "About..." menu item to system menu.

    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);

    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL)
	{
            CString strAboutMenu;
            strAboutMenu.LoadString(IDS_ABOUTBOX);
            if (!strAboutMenu.IsEmpty())
		{
                    pSysMenu->AppendMenu(MF_SEPARATOR);
                    pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

    return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTeXLiveDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
            CAboutDlg dlgAbout;
            dlgAbout.DoModal();
	}
    else
	{
            CDialog::OnSysCommand(nID, lParam);
	}
}

void CTeXLiveDlg::OnDestroy()
{
    CDialog::OnDestroy();
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTeXLiveDlg::OnPaint() 
{
    if (IsIconic())
	{
            CPaintDC dc(this); // device context for painting

            SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

            // Center icon in client rectangle
            int cxIcon = GetSystemMetrics(SM_CXICON);
            int cyIcon = GetSystemMetrics(SM_CYICON);
            CRect rect;
            GetClientRect(&rect);
            int x = (rect.Width() - cxIcon + 1) / 2;
            int y = (rect.Height() - cyIcon + 1) / 2;

            // Draw the icon
            dc.DrawIcon(x, y, m_hIcon);
	}
    else
	{
            CPaintDC dc(this);
            CBrush theBrush(RGB(255,255,255));
            CRect rc;
            GetClientRect(&rc);
            ScreenToClient(&rc);
            dc.SelectObject(theBrush);
            dc.Rectangle(0, 0, rc.Width(), rc.Height());
            dc.SetBkColor(RGB(255,255,255));
    
            CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTeXLiveDlg::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}

void CTeXLiveDlg::OnBrowseCdrom() 
{
    // TODO: Add your control notification handler code here
    ShellExecute(0, "open", g_sDriveRootPath, 0, 0, SW_SHOWNORMAL);
}

void CTeXLiveDlg::OnGotoTug() 
{
    // TODO: Add your control notification handler code here
    CString sUrl = "http://www.tug.org/";
    ShellExecute(0, "open", sUrl, 0, 0, SW_SHOWNORMAL);	
}

void CTeXLiveDlg::OnRunTexCdrom() 
{
    // TODO: Add your control notification handler code here
    try {
        RunOffCDRom();
    }
    catch (...) {
    }
}

void CTeXLiveDlg::OnRunTexliveDoc() 
{
    // TODO: Add your control notification handler code here
    CListChoiceDlg dlgDocLang;
    dlgDocLang.m_sTitle = "TeXLive Documentation";
    dlgDocLang.m_sDescription = "Choose your language :";
    dlgDocLang.AddChoice("English");
    dlgDocLang.AddChoice("Deutsch");
    dlgDocLang.AddChoice("French");
    dlgDocLang.AddChoice("Polish");
    dlgDocLang.AddChoice("Slovak");
    dlgDocLang.SetCheck(1, "", FALSE);
    dlgDocLang.SetCheck(2, "PDF", TRUE);
    dlgDocLang.SetCheck(3, "HTML", TRUE);
    if (dlgDocLang.DoModal() != IDOK) 
        return;
    if ((dlgDocLang.nChoice < 2) || (dlgDocLang.nChoice > 3))
        return;
	if (dlgDocLang.m_sListChoice.IsEmpty())
		return;
    CString sDocPath = g_sTexmfMain;
    // GetParentDirectory(sDocPath);
    sDocPath = ConcatPath(sDocPath, "doc\\tldoc");
    sDocPath = ConcatPath(sDocPath, dlgDocLang.m_sListChoice);
    sDocPath = ConcatPath(sDocPath, "live");
    sDocPath += (dlgDocLang.nChoice == 2 ? ".pdf" : ".html");
    if (FileExists(sDocPath)) {
        ShellExecute(0, "open", sDocPath, 0, 0, SW_SHOWNORMAL);	    
    }
    else {
        CString sError;
        sError.Format("Can't find file :\r\n%s", sDocPath);
        AfxMessageBox(sError, MB_ICONERROR);
    }
}

void CTeXLiveDlg::OnRunTexsetup() 
{
    // TODO: Add your control notification handler code here
    try {
        CString sCmd = ConcatPath(g_sBinDir, "TeXSetup.exe");
        RunProcess(sCmd);
    }
    catch (...) {

    }
}

void CTeXLiveDlg::OnUpgradeDlls() 
{
    // TODO: Add your control notification handler code here
   try {
       CString sCmd = ConcatPath(g_sSetupw32, "vc6redistsetup.exe");
       RunProcess(sCmd);
   }
   catch (...) {

   }
}

void CTeXLiveDlg::OnClose() 
{
    // TODO: Add your control notification handler code here
    CDialog::OnOK();
}

void CTeXLiveDlg::OnGotoFptex() 
{
    // TODO: Add your control notification handler code here
    CString sUrl = "http://www.fptex.org/";
    ShellExecute(0, "open", sUrl, 0, 0, SW_SHOWNORMAL);	
}

void CTeXLiveDlg::OnTexliveMaintenance() 
{
    // TODO: Add your control notification handler code here
    try {
        CString sCmd = ConcatPath(g_sBinDir, "TeXSetup.exe --maintenance");
        RunProcess(sCmd);
    }
    catch (...) {

    }
}

void CTeXLiveDlg::OnTexliveUninstall() 
{
    // TODO: Add your control notification handler code here
    try {
        CString sCmd = ConcatPath(g_sBinDir, "TeXSetup.exe --uninstall");
        RunProcess(sCmd);
    }
    catch (...) {

    }
}

void CTeXLiveDlg::OnCdromCleanup() 
{
    // TODO: Add your control notification handler code here
    try {
        CleanupCDRom();
        EnableRunOffCDMenuItem(false);
    }
    catch(...) {
    }
}

CString CTeXLiveDlg::BrowseForFolder(const CString &sInitialFolder) 
{
    static CString sPreviousFolder;
    CString sFolder;

    // Ask the user where he wants to install it
    static DWORD dwFlags = 0;

    if (sPreviousFolder.IsEmpty())
        sPreviousFolder = sInitialFolder;

    if (dwFlags == 0) {
        DWORD dwShlwapiVersion = GetDllVersion("shell32.dll");
        DWORD dwShell32Version = GetDllVersion("shlwapi.dll");
        dwFlags = BIF_RETURNONLYFSDIRS;

        if (dwShlwapiVersion >= PACKVERSION(4,71)
            || dwShell32Version >= PACKVERSION(4,71)) {
            dwFlags |= BIF_EDITBOX | BIF_VALIDATE;
        }
        
        if (dwShlwapiVersion >= PACKVERSION(5,0)
            || dwShell32Version >= PACKVERSION(5,0)) {
                dwFlags |= BIF_USENEWUI;
            }
    }

    sFolder = ::BrowseForFolder(AfxGetMainWnd()->m_hWnd, 
                                "Please select a root destination folder below.", 
                                dwFlags, sPreviousFolder);	

    sPreviousFolder = sFolder;

    return sFolder;
}

void CTeXLiveDlg::TeXSetupAddPackage(const CString &sPackage, const CString &sInstallationDir)
{
    CString sCmd = ConcatPath(g_sBinDir, 
                              "TeXSetup") + " --quick"
        + " --net-method " + g_sInternetMethod 
        + " --add-package=" + sPackage;
    
    if (! sInstallationDir.IsEmpty()) {
        sCmd = sCmd + " --installation-directory=" + GetSafePathName(sInstallationDir);
    }
     
    bool bRet = true;
    bRet = bRet && RunProcess(sCmd, true);
}

void CTeXLiveDlg::OnEditorsNTEmacs()
{
    CString &sInstallationDir = BrowseForFolder(g_sTempPath);
    sInstallationDir = ConcatPath(sInstallationDir, g_sFolderName);
    TeXSetupAddPackage("ntemacs", sInstallationDir);
}

void CTeXLiveDlg::OnEditorsTeXnicCenter()
{
    TeXSetupAddPackage("texniccenter");
}
void CTeXLiveDlg::OnEditorsWinEdt()
{
    TeXSetupAddPackage("winedt");
}

void CTeXLiveDlg::OnEditorsWinShell()
{
    TeXSetupAddPackage("winshell");
}

void CTeXLiveDlg::OnEditorsXEmacs()
{
    CString &sInstallationDir = BrowseForFolder(g_sTempPath);
    sInstallationDir = ConcatPath(sInstallationDir, g_sFolderName);
    TeXSetupAddPackage("xemacs", sInstallationDir);
}

void CTeXLiveDlg::OnSupportFreeGhostscript()
{
    TeXSetupAddPackage("ghostscript-free");
}

void CTeXLiveDlg::OnSupportGnuTools()
{
    CString &sInstallationDir = BrowseForFolder(g_sTempPath);
    sInstallationDir = ConcatPath(sInstallationDir, g_sFolderName);
    TeXSetupAddPackage("gnu-utils", sInstallationDir);
}

void CTeXLiveDlg::OnSupportImageMagick()
{
    CString &sInstallationDir = BrowseForFolder(g_sTempPath);
    sInstallationDir = ConcatPath(sInstallationDir, g_sFolderName);
    TeXSetupAddPackage("imagick", sInstallationDir);
}

void CTeXLiveDlg::OnSupportIspell()
{
    CString &sInstallationDir = BrowseForFolder(g_sTempPath);
    sInstallationDir = ConcatPath(sInstallationDir, g_sFolderName);
    TeXSetupAddPackage("ispell", sInstallationDir);
}

void CTeXLiveDlg::OnSupportNetPbm()
{
    CString &sInstallationDir = BrowseForFolder(g_sTempPath);
    sInstallationDir = ConcatPath(sInstallationDir, g_sFolderName);
    TeXSetupAddPackage("netpbm", sInstallationDir);
}

void CTeXLiveDlg::OnSupportPerl()
{
    CString &sInstallationDir = BrowseForFolder(g_sTempPath);
    sInstallationDir = ConcatPath(sInstallationDir, g_sFolderName);
    TeXSetupAddPackage("perl", sInstallationDir);
}

void CTeXLiveDlg::OnRunTeXDocTK()
{
    RunTeXDocTK();
}

void CTeXLiveDlg::OnStnClickedStaticWelcomeTexlive()
{
	// TODO: Add your control notification handler code here
}

void CTeXLiveDlg::OnSelectEditor()
{
    if (SelectEditor(g_sEditor)) {
        // Enable Run TeX off cdrom
        EnableRunOffCDMenuItem(true);
    }
}

void CTeXLiveDlg::OnInternetAccess()
{
    UINT state = m_Menu.GetMenuState(ID_INSTALL_INTERNETACCESS, MF_BYCOMMAND);
    ASSERT(state != 0xFFFFFFFF);

    if (state & MF_CHECKED) {
        m_Menu.CheckMenuItem(ID_INSTALL_INTERNETACCESS, MF_UNCHECKED | MF_BYCOMMAND);
        EnableInternetMenuItem(false);
    }
    else {
        if (HasInternetConnection()) {
            m_Menu.CheckMenuItem(ID_INSTALL_INTERNETACCESS, MF_CHECKED | MF_BYCOMMAND);
            EnableInternetMenuItem(true);
        }
        else {
            AfxMessageBox("No Internet connection detected.", MB_ICONEXCLAMATION);
        }
    }
}

void CTeXLiveDlg::OnHelp() 
{
#if 0
    CString sHelpFile = ConcatPath(g_sBinDir, "TeXLive.chm", '\\');
    if (FileExists(sHelpFile)) {
        ::HtmlHelp(NULL,"d:\\source\\texlive\\master\\bin\\win32\\TeXLive.chm",HH_DISPLAY_TOPIC, 0);
    }
    else {
        AfxMessageBox("Help file not found!\n", MB_ICONEXCLAMATION);
    }
#else
    AfxMessageBox("Please, refer to the TeX Live guide", MB_ICONINFORMATION);
#endif
} 

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

