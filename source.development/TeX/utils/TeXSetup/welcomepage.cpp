// WelcomePage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "texsetup.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"
#include "WelcomePage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const UINT nIDTimer = 314;

IMPLEMENT_DYNCREATE(CWelcomePage, CPropertyPage)


/////////////////////////////////////////////////////////////////////////////
// CWelcomePage property page

CWelcomePage::CWelcomePage() : CPropertyPage(CWelcomePage::IDD)
{
    //{{AFX_DATA_INIT(CWelcomePage)
    m_bQuickInstall = FALSE;
    m_bXEmTeX = FALSE;
    m_sLicense = _T("");
    m_bAllUsers = FALSE;
	m_sExplanation = _T("");
	//}}AFX_DATA_INIT
    m_bInitialized = false;
    m_pageNext = 0;
}

CWelcomePage::~CWelcomePage()
{
}

void CWelcomePage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CWelcomePage)
//	DDX_Text(pDX, IDC_STATIC_EXPLANATION, m_sExplanation);
    DDX_Check(pDX, IDC_CHECK_ADMINISTRATOR, m_bAllUsers);
    DDX_Check(pDX, IDC_CHECK_QUICKINSTALL, m_bQuickInstall);
    DDX_Check(pDX, IDC_CHECK_XEMTEX, m_bXEmTeX);
    DDX_Control(pDX, IDC_CHECK_ADMINISTRATOR, m_btnAdmin);
    DDX_Control(pDX, IDC_CHECK_QUICKINSTALL, m_btnQuickInstall);
    DDX_Control(pDX, IDC_CHECK_XEMTEX, m_btnXEmTeX);
    DDX_Text(pDX, IDC_EDIT_LICENSE, m_sLicense);
	//}}AFX_DATA_MAP
}

#define WM_NEXTPAGE (WM_USER + 107)

BEGIN_MESSAGE_MAP(CWelcomePage, CPropertyPage)
    //{{AFX_MSG_MAP(CWelcomePage)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CWelcomePage::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_NEXTPAGE) {
        CTeXSetupWizard *sheet = (CTeXSetupWizard*) GetParent();
        if (sheet) sheet->PressButton(PSBTN_NEXT);
        return TRUE; // handled
    }
    // CG: The following block was added by the ToolTips component.
    {
	// Let the ToolTip process this message.
	m_tooltip.RelayEvent(pMsg);
    }
    return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

BOOL CWelcomePage::OnInitDialog()
{
    if (g_eInstallType == INSTALL_MAINTENANCE) {
        m_sLicense.LoadString(IDS_WELCOME_MAINTENANCE);
    }
    else {
        m_sLicense.LoadString(IDS_WELCOME_LICENSE);
    }
    SetWindowText("TeXSetup Wizard for TeXLive " TEXLIVE_VERSION);
    CEdit *editLicense = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_LICENSE));
    editLicense->SetSel(-1, -1);
    UpdateData(FALSE);
    CWnd * wnd = GetDlgItem(IDC_STATIC_HEADER);
    ASSERT (wnd != 0);
    extern CFont fntHeader;
    wnd->SetFont (&fntHeader);
    SetWindowText(g_sCaption);
    CPropertyPage::OnInitDialog();	
    // CG: The following block was added by the ToolTips component.
    {
        // Create the ToolTip control.
        if (m_tooltip.m_hWnd == NULL) {
            m_tooltip.Create(this);
        }
        m_tooltip.Activate(TRUE);
    
        // TODO: Use one of the following forms to add controls:
        //m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
        m_tooltip.AddTool(GetDlgItem(IDC_CHECK_QUICKINSTALL), IDS_TOOLTIP_QUICKINSTALL);
        m_tooltip.AddTool(GetDlgItem(IDC_CHECK_ADMINISTRATOR), IDS_TOOLTIP_ADMINISTRATOR);
        m_tooltip.AddTool(GetDlgItem(IDC_CHECK_XEMTEX), IDS_TOOLTIP_XEMTEX);
    }
    // For this page only: the 'quick' option might have been given on the command line
    m_bQuickInstall = g_bQuickInstall;
    m_bAllUsers = g_bAllUsers;
    m_bXEmTeX = g_bInstallXEmTeX;
    m_sExplanation.LoadString(IDS_WELCOME_QUICK);

    return TRUE;	// CG: This was added by the ToolTips component.
}

LRESULT CWelcomePage::OnWizardNext() 
{
    // TODO: Add your specialized code here and/or call the base class

    UpdateData(TRUE);
    g_bQuickInstall = m_bQuickInstall;
    g_bAllUsers = m_bAllUsers;
    g_bInstallXEmTeX = m_bXEmTeX;
    CTeXSetupWizard *pParent = (CTeXSetupWizard*) GetParent();

    LRESULT nRet = -1;
    
    if (m_pageNext == 0) {
        if (g_bQuickInstall && g_eInstallType != INSTALL_MAINTENANCE) {
            m_pageNext = pParent->m_pageGetTPM;
        }
        else {
            m_pageNext = pParent->m_pageSource;
        }
        pParent->AddPage(m_pageNext);
    }
    
    nRet =  CPropertyPage::OnWizardNext();

    return nRet;
}


BOOL CWelcomePage::OnSetActive()
{
    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (sheet != 0) {
	if (m_pageNext != 0) {
	    sheet->RemovePage(m_pageNext);
	    m_pageNext = 0;
	}
    }

    int nRet = CPropertyPage::OnSetActive();

    if (g_eInstallType == INSTALL_FULL) {
        if (! g_bIsWindowsNT || (g_bIsWindowsNT && !g_bIsAdmin)) {
            m_btnAdmin.EnableWindow(FALSE);
            m_btnAdmin.ShowWindow(SW_HIDE);
        }
    }
    else {
        m_btnAdmin.EnableWindow(FALSE);
        m_btnAdmin.ShowWindow(SW_HIDE);
        m_btnXEmTeX.EnableWindow(FALSE);
        m_btnXEmTeX.ShowWindow(SW_HIDE);
        m_btnQuickInstall.EnableWindow(FALSE);
        m_btnQuickInstall.ShowWindow(SW_HIDE);
        m_sExplanation.Empty();
    }

    UpdateData(FALSE);

    if (g_bQuickInstall || g_eInstallType == INSTALL_MAINTENANCE) {
        PostMessage(WM_NEXTPAGE);
        //        sheet->PressButton(PSBTN_NEXT);
    }

    return nRet;
}

BOOL CWelcomePage::OnQueryCancel() 
{
    // TODO: Add your specialized code here and/or call the base class
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    BOOL bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);

    return bRet;
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

