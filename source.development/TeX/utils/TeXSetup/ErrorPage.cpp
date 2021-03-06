// ErrorPage.cpp : implementation file
//

#include "stdafx.h"
#include "texsetup.h"
#include "ErrorPage.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CErrorPage dialog


CErrorPage::CErrorPage()
    : CPropertyPage(CErrorPage::IDD)
{
    //{{AFX_DATA_INIT(CErrorPage)
    m_bViewLogFile = FALSE;
    //}}AFX_DATA_INIT

    m_pageNext = 0;
}

CErrorPage::~CErrorPage()
{
}


void CErrorPage::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CErrorPage)
    DDX_Check(pDX, IDC_CHECK_VIEW_LOGFILE, m_bViewLogFile);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CErrorPage, CDialog)
    //{{AFX_MSG_MAP(CErrorPage)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
// CErrorPage message handlers

BOOL CErrorPage::OnInitDialog() 
{
    CWnd * wnd = GetDlgItem(IDC_STATIC_HEADER);
    ASSERT (wnd != 0);
    extern CFont fntHeader;
    wnd->SetFont (&fntHeader);
    SetWindowText(g_sCaption);

    CPropertyPage::OnInitDialog();

    // CG: This was added by the ToolTips component.
    // CG: The following block was added by the ToolTips component.
    {
        // Create the ToolTip control.
        if (m_tooltip.m_hWnd == NULL) {
            m_tooltip.Create(this);
        }
        m_tooltip.Activate(TRUE);

        // TODO: Use one of the following forms to add controls:
        // m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
        // m_tooltip.AddTool(GetDlgItem(IDC_<name>), "<text>");
    }
	
    // TODO: Add extra initialization here

    if (theLog.GetName().GetLength() == 0) {
        wnd = GetDlgItem (IDC_PROMPT);
        ASSERT (wnd != 0);
        wnd->EnableWindow (FALSE);
        wnd = GetDlgItem (IDC_CHECK_VIEW_LOGFILE);
        ASSERT (wnd != 0);
        wnd->EnableWindow (FALSE);
        m_bViewLogFile = FALSE;
    }
    else {
        wnd = GetDlgItem (IDC_STATIC_LOGNAME);
        ASSERT (wnd != 0);
        wnd->SetWindowText (theLog.GetName());
        m_bViewLogFile = TRUE;
    }
    wnd = GetDlgItem (IDC_EDIT_ERROR);
    ASSERT (wnd != 0);
    wnd->SetWindowText (g_sLastError);
	
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CErrorPage::PreTranslateMessage(MSG* pMsg)
{
    // CG: The following block was added by the ToolTips component.
    {
        // Let the ToolTip process this message.
        m_tooltip.RelayEvent(pMsg);
    }
    return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

BOOL CErrorPage::OnSetActive() 
{
    MessageBeep (MB_ICONHAND);
    CTeXSetupWizard *sheet = reinterpret_cast<CTeXSetupWizard *>(GetParent());
    CancelToClose();
    sheet->SetWizardButtons(PSWIZB_FINISH);
    if (sheet != 0)
        sheet->SetFinishText ("Exit");
    return CPropertyPage::OnSetActive();
}

BOOL CErrorPage::OnWizardFinish() 
{
    UpdateData(TRUE);
    BOOL ret = CPropertyPage::OnWizardFinish();

    // Flush the log file.
    theLog.CloseLog();

    if (ret && m_bViewLogFile)
        ShellExecute (0, "open", "notepad.exe", (LPCTSTR)theLog.GetName(), 0, SW_SHOWNORMAL);

    return (ret);
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
