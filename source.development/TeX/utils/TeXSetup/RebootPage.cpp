// RebootPage.cpp : implementation file
//

#include "stdafx.h"
#include "texsetup.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"
#include "RebootPage.h"
#include "Win32Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRebootPage dialog


CRebootPage::CRebootPage()
    : CPropertyPage(CRebootPage::IDD)
{
    //{{AFX_DATA_INIT(CRebootPage)
    m_bViewLog = FALSE;
    m_bViewDoc = FALSE;
    m_bReboot = FALSE;
	m_sMsg = _T("");
	m_sReboot = _T("");
	//}}AFX_DATA_INIT

    m_pageNext = 0;
}


void CRebootPage::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRebootPage)
    DDX_Check(pDX, IDC_CHECK_LOG, m_bViewLog);
    DDX_Check(pDX, IDC_CHECK_DOC, m_bViewDoc);
    DDX_Check(pDX, IDC_CHECK_REBOOT, m_bReboot);
	DDX_Text(pDX, IDC_STATIC_MESSAGE, m_sMsg);
	DDX_Text(pDX, IDC_STATIC_REBOOT, m_sReboot);
	//}}AFX_DATA_MAP
}

#define WM_REBOOT (WM_USER + 108)

BEGIN_MESSAGE_MAP(CRebootPage, CDialog)
    //{{AFX_MSG_MAP(CRebootPage)
    ON_BN_CLICKED(IDC_CHECK_REBOOT, OnCheckReboot)
    
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
// CRebootPage message handlers

bool CRebootPage::Reboot()
{
    // Kill the TeXLive if existing !
    CWnd *hWnd = FindWindow(NULL, "TeXLive");
    if (hWnd) {
        // SendMessage(hWnd, WM_DESTROY, 0, 0);
        hWnd->SendMessage(WM_CLOSE, 0, 0);
    }

    if (!ExitWindowsEx(EWX_REBOOT /* | EWX_FORCE */, 0)) {
        Win32Error("ExitWindowsEx"); 
        return false;
    }
    return true;
}

BOOL CRebootPage::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_REBOOT) {
        CTeXSetupWizard *sheet = (CTeXSetupWizard*) GetParent();
        if (sheet) sheet->PressButton(PSBTN_FINISH);
        return TRUE; // handled
    }
    // CG: The following block was added by the ToolTips component.
    {
	// Let the ToolTip process this message.
	m_tooltip.RelayEvent(pMsg);
    }
    return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

BOOL CRebootPage::OnInitDialog()
{
    CWnd * wnd = GetDlgItem (IDC_STATIC_HEADER);
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
    }

    return TRUE;	// CG: This was added by the ToolTips component.
}

BOOL CRebootPage::OnSetActive() 
{
    CPropertySheet * sheet = DYNAMIC_DOWNCAST(CPropertySheet, GetParent());

    CSingleLock theLock(&g_csCritSec, TRUE);
    m_bReboot = theStatus.bReboot;
    theLock.Unlock();

    // Reinstate the registry key
    Finalize();
 
    if (sheet != 0) {
        sheet->SetWizardButtons(PSWIZB_FINISH);
        if (m_bReboot)
            sheet->SetFinishText ("Reboot");
        else
            sheet->SetFinishText ("Finish");
    }
    CancelToClose ();

    if (m_bReboot) {
		m_sReboot.LoadString(IDS_REBOOT_COMPUTER);
		m_sMsg.LoadString(IDS_CLICK_REBOOT);

        CWnd *pWnd = GetDlgItem(IDC_STATIC_REBOOT);
        ASSERT (pWnd != 0);
        pWnd->EnableWindow (TRUE);

        pWnd = GetDlgItem(IDC_CHECK_REBOOT);
        ASSERT (pWnd != 0);
        pWnd->EnableWindow (TRUE);

    }
	else {
		m_sReboot.Empty();
		m_sMsg.LoadString(IDS_CLICK_FINISH);
	}

    UpdateData(FALSE);

    // Flush the log file.
    theLog.CloseLog();

    BOOL bRet = CPropertyPage::OnSetActive();

    if (g_bAutomaticReboot || (g_bQuickInstall && !m_bReboot)) {
        PostMessage(WM_REBOOT);
        // sheet->PressButton(PSBTN_FINISH);
    }
    return bRet;
}

BOOL CRebootPage::OnWizardFinish() 
{
    UpdateData(TRUE);

    if (m_bViewDoc) {
        char szDocumentation[_MAX_PATH];
        _makepath (szDocumentation, 0, g_sInstallRootDir, "texmf\\doc\\tldoc\\english\\live", ".html");
        ShellExecute (0, "open", szDocumentation, 0, 0, SW_SHOWNORMAL);
    }

    if (m_bViewLog) {
        ShellExecute (0, "open", "notepad.exe", (LPCTSTR)theLog.GetName(), 0, SW_SHOWNORMAL);
    }
  
    if (m_bReboot) {
        Reboot();
    }
    return (CPropertyPage::OnWizardFinish());
}

void CRebootPage::OnCheckReboot() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);

    CPropertySheet * sheet = DYNAMIC_DOWNCAST(CPropertySheet, GetParent());

    if (sheet != 0) {
        if (m_bReboot) {
            sheet->SetFinishText ("Reboot");
			m_sMsg.LoadString(IDS_CLICK_REBOOT);
		}
        else {
            sheet->SetFinishText ("Finish");
			m_sMsg.LoadString(IDS_CLICK_FINISH);
		}
    }
	UpdateData(FALSE);
	
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

