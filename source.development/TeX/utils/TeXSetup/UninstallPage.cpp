// UninstallPage.cpp : implementation file
//

#include "stdafx.h"
#include "texsetup.h"
#include "UninstallPage.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT nIDTimer = 314;

/////////////////////////////////////////////////////////////////////////////
// CUninstallPage property page

IMPLEMENT_DYNCREATE(CUninstallPage, CPropertyPage)

    CUninstallPage::CUninstallPage() : CPropertyPage(CUninstallPage::IDD)
{
    //{{AFX_DATA_INIT(CUninstallPage)
    m_bViewLog = FALSE;
    //}}AFX_DATA_INIT
    m_pageNext = 0;
}

CUninstallPage::~CUninstallPage()
{
}

void CUninstallPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CUninstallPage)
    DDX_Check(pDX, IDC_CHECK_LOG, m_bViewLog);
    //}}AFX_DATA_MAP
}

#define WM_STARTUNINSTALL (WM_USER + 102)

#define WM_FINISH (WM_USER + 109)

BEGIN_MESSAGE_MAP(CUninstallPage, CPropertyPage)
    //{{AFX_MSG_MAP(CUninstallPage)
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_STARTUNINSTALL, OnStartUninstall)
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
// CUninstallPage message handlers

BOOL CUninstallPage::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_FINISH) {
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

LRESULT CUninstallPage::OnStartUninstall (WPARAM wParam, LPARAM lParam)
{
    // FIXME: check that wininit.ini does not yet exist

    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, FALSE};
    // start the worker thread
    AfxBeginThread ((AFX_THREADPROC)UninstallThread, 0,
                    /* nPriority */ THREAD_PRIORITY_NORMAL,
                    /* nStackSize */ 0,
                    /* dwCreateFlags */ 0,
                    /* lpSecurityAttrs */ &sa);

    // start the timer
    SetTimer (nIDTimer, 100, 0);

    return (0);
}

BOOL CUninstallPage::OnWizardFinish() 
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateData(TRUE);	
    if (m_bViewLog) {
	ShellExecute (0, "open", "notepad.exe", (LPCTSTR)theLog.GetName(), 0, SW_SHOWNORMAL);
    }
    return CPropertyPage::OnWizardFinish();
}

BOOL CUninstallPage::OnSetActive() 
{
    // TODO: Add your specialized code here and/or call the base class
    CPropertySheet * sheet = DYNAMIC_DOWNCAST(CPropertySheet, GetParent());

    if (sheet != 0) {
	sheet->SetWizardButtons(0);
	if (m_pageNext != 0) {
	    sheet->RemovePage(m_pageNext);
	    m_pageNext = 0;
	}
    }
    // Ask for confirmation
    {
        CString sMsg;

        if (g_sInstallRootDir.IsEmpty()) {
            sMsg.Format("No previous TeXLive %s installation found.", TEXLIVE_VERSION);
            ::MessageBox(NULL, (LPCTSTR)sMsg, "Warning", MB_ICONWARNING | MB_OK);
            EndDialog(IDOK);
        }

        sMsg.FormatMessage(IDS_UNINSTALL_PROMPT, (LPCTSTR)g_sInstallRootDir, 
                           (LPCTSTR)g_sFolderName);
        int nRep = AfxMessageBox(sMsg, MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2);
        if (nRep == IDOK) {
            // Double check !!!
            sMsg.FormatMessage(IDS_UNINSTALL_WARNING, (LPCTSTR)g_sInstallRootDir);
            nRep = AfxMessageBox(sMsg, MB_ICONWARNING | MB_OKCANCEL | MB_DEFBUTTON2);
            if (nRep == IDOK)
                PostMessage (WM_STARTUNINSTALL);
            else
                EndDialog(IDCANCEL);
        }
        else
            EndDialog(IDCANCEL);
    }
    return TRUE;
}

BOOL CUninstallPage::OnInitDialog() 
{
    // TODO: Add extra initialization here
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
    }

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CUninstallPage::OnTimer(UINT nIDEvent) 
{
    static UINT nConfig = 0;
    
    CSingleLock singleLock (&g_csCritSec, TRUE);
    
    if (theStatus.bError) {
        // 	CTeXSetupWizard * pParent = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
        // 	if (pParent != 0) {
        // 	    if (! g_bQuickInstall) {	
        // 		pParent->AddPage (pParent->m_pageError);
        // 		pParent->SetActivePage (pParent->m_pageError);
        // 	    }
        StopUninstall ();
        // 	}
    }
    else {
	CWnd *hWnd;
	if (nConfig == ::RMBEGIN) {
	    hWnd = GetDlgItem(IDC_STATIC_RMDIR);
	    ASSERT (hWnd != 0);
	    if (theStatus.sName)
                hWnd->SetWindowText (theStatus.sName);
	}
	if (theStatus.nConfig != nConfig) {
	    nConfig = theStatus.nConfig;
	    switch (nConfig) {
	    case ::RMEND:
	    case ::RMENVIRONMENT:
		hWnd = GetDlgItem(IDC_OK_ENVIRONMENT);
		if (hWnd)
		    hWnd->ShowWindow(SW_SHOWNORMAL);
	    case ::RMREGISTRY:
		hWnd = GetDlgItem(IDC_OK_REGISTRY);
		if (hWnd)
		    hWnd->ShowWindow(SW_SHOWNORMAL);
                hWnd = GetDlgItem(IDC_STATIC_ENVIRONMENT);
                if (hWnd)
                    hWnd->ShowWindow(SW_SHOWNORMAL);
	    case ::RMMENUS:
                hWnd = GetDlgItem(IDC_OK_MENU);
                if (hWnd)
                    hWnd->ShowWindow(SW_SHOWNORMAL);
                hWnd = GetDlgItem(IDC_STATIC_REGISTRY);
                if (hWnd)
                    hWnd->ShowWindow(SW_SHOWNORMAL);
	    case ::RMFILES:
                hWnd = GetDlgItem(IDC_OK_FILES);
                if (hWnd)
                    hWnd->ShowWindow(SW_SHOWNORMAL);
                hWnd = GetDlgItem(IDC_STATIC_MENU);
                if (hWnd)
                    hWnd->ShowWindow(SW_SHOWNORMAL);
	    case ::RMBEGIN:
		hWnd = GetDlgItem(IDC_STATIC_FILES);
		if (hWnd)
		    hWnd->ShowWindow(SW_SHOWNORMAL);
	    }
	}
	if (theStatus.bReady) {
            StopUninstall ();
	}    
    }
    CPropertyPage::OnTimer (nIDEvent);
}

UINT CUninstallPage::UninstallThread (LPVOID pParam)
{
    CoInitialize (0);
    try {
	DoUninstall();
    }
    catch (...) {
	CSingleLock singleLock (&g_csCritSec, TRUE);
	theStatus.bError = true;
    }
    CoUninitialize ();
    return (0);
}

void CUninstallPage::StopUninstall ()
{
    CSingleLock singleLock (&g_csCritSec, TRUE);
    KillTimer (nIDTimer);
    theStatus.bTerminate = true;

    // Flush the log file.
    theLog.CloseLog();

    CPropertySheet * pParent = DYNAMIC_DOWNCAST(CPropertySheet, GetParent());

    if (theStatus.bReady) {
	// Notify user
	CWnd * wnd = GetDlgItem (IDC_PROMPT);
	ASSERT (wnd != 0);
	CString prompt;
	prompt.FormatMessage (IDS_UNINSTALL_COMPLETED, (const char *) theLog.GetName());
	wnd->SetWindowText (prompt);
	
	// Enable Finish button
	if (pParent != 0)
            pParent->SetFinishText ("Finish");
        pParent->SetWizardButtons (PSWIZB_FINISH);
    }

    CancelToClose();

    // Enable check view log
    CWnd *wnd = GetDlgItem(IDC_CHECK_LOG);
    ASSERT (wnd != 0);
    wnd->EnableWindow (TRUE);

    if (g_bQuickInstall) {
        PostMessage(WM_FINISH);
	// pParent->PressButton(PSBTN_FINISH);
    }
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

