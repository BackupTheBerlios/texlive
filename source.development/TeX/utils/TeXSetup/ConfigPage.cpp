
// ConfigPage.cpp : implementation file
//

#include "stdafx.h"
#include "TeXSetup.h"
#include "ConfigPage.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT nIDTimer = 314;

/////////////////////////////////////////////////////////////////////////////
// CConfigPage property page

IMPLEMENT_DYNCREATE(CConfigPage, CPropertyPage)

    CConfigPage::CConfigPage() : CPropertyPage(CConfigPage::IDD)
{
    //{{AFX_DATA_INIT(CConfigPage)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    
    m_pageNext = 0;
}

CConfigPage::~CConfigPage()
{
}

void CConfigPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CConfigPage)
    // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}

#define WM_STARTCONFIGURATION (WM_USER + 102)
#define WM_NEXTPAGE (WM_USER + 107)

BEGIN_MESSAGE_MAP(CConfigPage, CPropertyPage)
    //{{AFX_MSG_MAP(CConfigPage)
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_STARTCONFIGURATION, OnStartConfiguration)
    END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConfigPage message handlers

BOOL CConfigPage::PreTranslateMessage(MSG* pMsg)
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

BOOL CConfigPage::OnInitDialog() 
{
    CWnd * wnd = GetDlgItem(IDC_STATIC_HEADER);
    ASSERT (wnd != 0);
    extern CFont fntHeader;
    wnd->SetFont (&fntHeader);
    SetWindowText(g_sCaption);

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

    return CPropertyPage::OnInitDialog();
}

LRESULT CConfigPage::OnStartConfiguration (WPARAM wParam, LPARAM lParam)
{
    // FIXME: check that wininit.ini does not yet exist
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, FALSE};

    // start the worker thread
    AfxBeginThread ((AFX_THREADPROC)ConfigurationThread, 0,
                    /* nPriority */ THREAD_PRIORITY_NORMAL,
                    /* nStackSize */ 0,
                    /* dwCreateFlags */ 0,
                    /* lpSecurityAttrs */ &sa);

    // start the timer
    SetTimer (nIDTimer, 100, 0);

    return (0);
}

BOOL CConfigPage::OnSetActive() 
{
    // TODO: Add your specialized code here and/or call the base class
	
    CPropertySheet *sheet = DYNAMIC_DOWNCAST(CPropertySheet, GetParent());
    if (sheet != 0) {
        sheet->SetWizardButtons (0);
        if (m_pageNext != 0) {
            sheet->RemovePage(m_pageNext);
            m_pageNext = 0;
        }
    }
    theStatus.bError = theStatus.bReady = theStatus.bTerminate = false;
    PostMessage (WM_STARTCONFIGURATION);

    return CPropertyPage::OnSetActive();
}

BOOL CConfigPage::OnQueryCancel ()
{
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);
    theLog ("\n<<<Cancel? ");
    //  BOOL ret = (AfxMessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    bool bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);
    if (bRet) {
        theLog ("Yes!>>>\n");
    }
    else {
        theLog ("No!>>>\n");
    }
    SetCancelFlag (bRet);

    return bRet;
}

void CConfigPage::OnTimer(UINT nIDEvent) 
{
    static UINT nConfig = 0;
    CWnd *hWnd;

    CSingleLock theLock(&g_csCritSec, TRUE);
    
    // We should enable the bitmaps one after the other. TeXInstall will post
    // progression, it is up to us to decode it.
#if 0
    if (theStatus.nConfig != nConfig) {
#endif
        nConfig = theStatus.nConfig;
        switch (nConfig) {
        case ::CFG_END:
        case ::CFG_SHELL_OBJECTS:
            hWnd = GetDlgItem(IDC_OK_SHELL);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
        case ::CFG_ENVIRONMENT:
            hWnd = GetDlgItem(IDC_OK_ENVIRONMENT);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
            hWnd = GetDlgItem(IDC_STATIC_SHELL);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
        case ::CFG_FORMAT_FILES:
            hWnd = GetDlgItem(IDC_OK_FORMAT);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
            hWnd = GetDlgItem(IDC_STATIC_ENVIRONMENT);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
        case ::CFG_LSR_DATABASES:
            hWnd = GetDlgItem(IDC_OK_LSR);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
            hWnd = GetDlgItem(IDC_STATIC_FORMAT);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
        case ::CFG_SUPPLEMENTARY_TOOLS:
            hWnd = GetDlgItem(IDC_OK_SUPP);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
            hWnd = GetDlgItem(IDC_STATIC_LSR);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
        case ::CFG_BEGIN:
            hWnd = GetDlgItem(IDC_STATIC_SUPP);
            if (hWnd)
                hWnd->ShowWindow(SW_SHOWNORMAL);
        }
#if 0
    }
#endif

    if (nConfig == ::CFG_BEGIN && theStatus.bNewTPM == TRUE) {
        hWnd = GetDlgItem(IDC_STATIC_PACKAGE);
        hWnd->SetWindowText(theStatus.tpm->GetName());
        theStatus.bNewTPM = FALSE;
    }

    if (theStatus.bReady || theStatus.bError) {
        StopConfiguration();
    }
    
    CPropertyPage::OnTimer (nIDEvent);
}

UINT CConfigPage::ConfigurationThread (LPVOID pParam)
{
    CoInitialize (0);
    try {
        DoConfiguration();
    }
    catch (...) {
        CSingleLock singleLock (&g_csCritSec, TRUE);
        theStatus.bError = true;
        theLog("Error: Configuration error flagged!");
    }
    CoUninitialize ();
    return (0);
}

void CConfigPage::StopConfiguration ()
{
    CSingleLock singleLock (&g_csCritSec, TRUE);
    KillTimer (nIDTimer);
    theStatus.bTerminate = true;
  
    //  if (theStatus.bReady) {
    // Notify user
    CWnd * wnd = GetDlgItem (IDC_PROMPT);
    ASSERT (wnd != 0);
    CString prompt;
    if (theStatus.bError)
        prompt.FormatMessage (IDS_CONFIGURATION_ERROR, (const char *) theLog.GetName());
    else
        prompt.FormatMessage (IDS_CONFIGURATION_COMPLETED, (const char *) theLog.GetName());

    wnd->SetWindowText (prompt);
  
    CTeXSetupWizard *pParent = reinterpret_cast<CTeXSetupWizard *>(GetParent());
  
    if (theStatus.bError) {
        if (pParent != 0) {
            pParent->AddPage (pParent->m_pageError);
            pParent->SetActivePage (pParent->m_pageError);
            if (g_bQuickInstall) {
                // pParent->PressButton(PSBTN_NEXT);
                PostMessage(WM_NEXTPAGE);
            }
        }
    }
    else {
        // Enable Next button, assume bReady
        if (pParent != 0) {
            pParent->SetWizardButtons (PSWIZB_NEXT);
            if (m_pageNext == 0) {
                m_pageNext = pParent->m_pageReboot;
                pParent->AddPage(m_pageNext);
            }
      
            if (g_bQuickInstall) {
                // pParent->PressButton(PSBTN_NEXT);
                PostMessage(WM_NEXTPAGE);
            }
        }
    }
}

LRESULT CConfigPage::OnWizardNext() 
{
    // TODO: Add your specialized code here and/or call the base class
    if (m_pageNext == 0) {
        CTeXSetupWizard *sheet = reinterpret_cast<CTeXSetupWizard *>(GetParent());
        m_pageNext = sheet->m_pageReboot;
        sheet->AddPage(m_pageNext);
    }   
	
    return CPropertyPage::OnWizardNext();
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

