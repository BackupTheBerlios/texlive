// TeXSetupWizard.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TeXSetupWizard.h"
#include "TeXInstall.h"
#include "TeXSetupCommandLineInfo.h"
#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
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
// CTeXSetupWizard

IMPLEMENT_DYNAMIC(CTeXSetupWizard, CPropertySheet)

    CTeXSetupWizard::CTeXSetupWizard(CWnd* pWndParent)
	: CPropertySheet(IDS_PROPSHT_CAPTION, pWndParent)
{
    // Add all of the property pages here.  Note that
    // the order that they appear in here will be
    // the order they appear in on screen.  By default,
    // the first page of the set is the active one.
    // One way to make a different property page the 
    // active one is to call SetActivePage().
    
    m_nPages = 1;
    
    m_pageWelcome = new CWelcomePage();
    m_pageRoot = new CRootPage();
    m_pageScheme = new CSchemePage();
    m_pageSource = new CSourcePage();
    m_pageGetTPM = new CGetTPMPage();
    m_pagePackages = new CPackagesPage();
    m_pageReview = new CReviewPage();
    m_pageFileCopy = new CFileCopyPage();
    m_pageConfiguration = new CConfigPage();
    m_pageReboot = new CRebootPage();
    m_pageError = new CErrorPage();
    m_pageUninstall = new CUninstallPage();

    if (cmdinfo.m_bUninstall) {
        AddPage(m_pageUninstall);
    }
    else {
        AddPage(m_pageWelcome);
    }

    m_hIcon = AfxGetApp()->LoadIcon(IDI_TEXSETUP);
    m_psh.dwFlags |= PSP_USEHICON;
    // m_psh.dwFlags &= ~PSH_HASHELP;  // Lose the Help button
    m_psh.hIcon = m_hIcon;

    SetWizardMode();
}

CTeXSetupWizard::~CTeXSetupWizard()
{
    delete m_pageWelcome;
    delete m_pageRoot;
    delete m_pageSource;
    delete m_pageScheme;
    delete m_pageGetTPM;
    delete m_pagePackages;
    delete m_pageReview;
    delete m_pageFileCopy;
    delete m_pageConfiguration;
    delete m_pageReboot;
    delete m_pageError;
    delete m_pageUninstall;
}


BEGIN_MESSAGE_MAP(CTeXSetupWizard, CPropertySheet)
    //{{AFX_MSG_MAP(CTeXSetupWizard)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    ON_WM_SYSCOMMAND()
    ON_WM_DESTROY()
    //    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_COMMAND( ID_HELP, OnHelp ) 
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    /////////////////////////////////////////////////////////////////////////////
// CTeXSetupWizard message handlers

BOOL CTeXSetupWizard::OnInitDialog()
{
    // IDM_ABOUTBOX must be in the system command range.
    ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
    ASSERT(IDM_ABOUTBOX < 0xF000);
    
    CMenu* pSysMenu = GetSystemMenu(FALSE);
    if (pSysMenu != NULL) {
        CString strAboutMenu;
        strAboutMenu.LoadString(IDS_ABOUTBOX);
        if (!strAboutMenu.IsEmpty()) {
            pSysMenu->AppendMenu(MF_SEPARATOR);
            pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
        }
    }
    
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    BOOL bResult = CPropertySheet::OnInitDialog();

#if 0
    // add a preview window to the property sheet.
    CRect rectWnd;
    GetWindowRect(rectWnd);
    SetWindowPos(NULL, 0, 0,
		 rectWnd.Width() + 100,
		 rectWnd.Height(),
		 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
    CRect rectPreview(rectWnd.Width() + 25, 25,
		      rectWnd.Width()+75, 75);

    m_wndPreview.Create(NULL, NULL, WS_CHILD|WS_VISIBLE,
			rectPreview, this, 0x1000);
#endif
    CenterWindow();

    return bResult;
}

void CTeXSetupWizard::OnSysCommand(UINT nID, LPARAM lParam)
{
    if ((nID & 0xFFF0) == IDM_ABOUTBOX)	{
        CAboutDlg dlgAbout;
        dlgAbout.DoModal();
    }
    else {
        CPropertySheet::OnSysCommand(nID, lParam);
    }
}

void CTeXSetupWizard::OnDestroy()
{
    WinHelp(0L, HELP_QUIT);
    CPropertySheet::OnDestroy();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTeXSetupWizard::OnQueryDragIcon()
{
    return (HCURSOR) m_hIcon;
}


void CTeXSetupWizard::OnHelp() 
{
#if 0
    CString sHelpFile = ConcatPath(g_sBinDir, "TeXSetup.chm", '\\');
    if (FileExists(sHelpFile)) {
        ::HtmlHelp(NULL,sHelpFile,HH_DISPLAY_TOPIC, 0);
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

