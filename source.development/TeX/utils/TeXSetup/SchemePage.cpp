// SchemePage message handlers
// SchemePage.cpp : implementation file
//

#include "stdafx.h"
#include "TeXSetup.h"
#include "TeXSetupWizard.h"
#include "SchemePage.h"
#include "Win32Util.h"


// CSchemePage dialog

IMPLEMENT_DYNAMIC(CSchemePage, CPropertyPage)

CSchemePage::CSchemePage()
	: CPropertyPage(CSchemePage::IDD)
	, m_sSchemeDescription(_T(""))
	, m_bCustomizeScheme(FALSE)
	, m_sSpaceAvailable(_T(""))
	, m_sTotalSize(_T(""))
{
    //{{AFX_DATA_INIT(CPackagesPage)
    m_bCustomizeScheme = FALSE;
    m_sSchemeDescription = _T("");
    //}}AFX_DATA_INIT

    m_pageNext = 0;
}

CSchemePage::~CSchemePage()
{
}

void CSchemePage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_SCHEME, m_ctlListScheme);
	DDX_Text(pDX, IDC_STATIC_SCHEME, m_sSchemeDescription);
	DDX_Check(pDX, IDC_CHECK_CUSTOMIZE_SCHEME, m_bCustomizeScheme);
	DDX_Text(pDX, IDC_STATIC_TOTAL_SIZE, m_sTotalSize);
	DDX_Text(pDX, IDC_STATIC_SPACE_AVAILABLE, m_sSpaceAvailable);
}

#define WM_NEXTPAGE (WM_USER + 107)

BEGIN_MESSAGE_MAP(CSchemePage, CPropertyPage)

    //{{AFX_MSG_MAP(CPackagesPage)
    ON_BN_CLICKED(IDC_CHECK_CUSTOMIZE_SCHEME, OnBnClickedCheckCustomizeScheme)
    ON_LBN_SELCHANGE(IDC_LIST_SCHEME, OnLbnSelchangeListScheme)
    ON_WM_CREATE()
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSchemePage::PreTranslateMessage(MSG* pMsg)
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

// CSchemePage message handlers

BOOL CSchemePage::OnInitDialog()
{
    CWnd * wnd = GetDlgItem(IDC_STATIC_HEADER);
    ASSERT (wnd != 0);
    extern CFont fntHeader;
    wnd->SetFont (&fntHeader);
    SetWindowText(g_sCaption);

    CPropertyPage::OnInitDialog();	// CG: This was added by the ToolTips component.

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

    return TRUE;	// CG: This was added by the ToolTips component.
}

BOOL CSchemePage::OnSetActive()
{
    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (sheet != 0) {
        if (m_pageNext != 0) {
            sheet->RemovePage(m_pageNext);
            m_pageNext = 0;
        }
        sheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT);
    }

    // Populate the listbox
    m_ctlListScheme.ResetContent();

    CDC *pDC = m_ctlListScheme.GetDC();
    CSize szTextExtent = 0;
    int ctlListSchemeWidth = 0;
    POSITION pos = g_vSchemeTPM.GetStartPosition();
    CString sKey, sDefaultSel;
    TPM *tpm;
    int index = 0;
    while (pos != 0) {
        g_vSchemeTPM.GetNextAssoc(pos, sKey, tpm);
        index = m_ctlListScheme.AddString(tpm->GetTitle());
        m_ctlListScheme.SetItemData(index, (DWORD)tpm);
        szTextExtent = pDC->GetTextExtent(tpm->GetTitle());
        if (szTextExtent.cx > ctlListSchemeWidth)
            ctlListSchemeWidth = szTextExtent.cx;
        // either the scheme name has been given on the command line
        if (! g_sSchemeName.IsEmpty()) {
            if (tpm->GetName() == g_sSchemeName) {
                sDefaultSel = tpm->GetTitle();
            }
        }
        // or there might be a default one
        else if (tpm->IsDefault()) {
            sDefaultSel = tpm->GetTitle();
        }
    }

    int nDefault;

    nDefault = m_ctlListScheme.FindStringExact(0, sDefaultSel);
    if (nDefault == LB_ERR)
        nDefault = 0;

    // Select the default or first one
    m_ctlListScheme.SetCurSel(nDefault);

    // Adjust width
    RECT rectListScheme;
    m_ctlListScheme.GetClientRect(&rectListScheme);
    int naturalWidth = rectListScheme.right - rectListScheme.left;
    ctlListSchemeWidth = ctlListSchemeWidth + 1;
    
    if (ctlListSchemeWidth > naturalWidth) {
        m_ctlListScheme.SetHorizontalExtent(ctlListSchemeWidth);
        m_ctlListScheme.SetColumnWidth(ctlListSchemeWidth);
    }

    UpdateData(FALSE);

    UpdateInfo();

    int nRet = CPropertyPage::OnSetActive();
    
    if (g_bQuickInstall) {
        // sheet->PressButton(PSBTN_NEXT);
        GdiFlush();
        PostMessage(WM_NEXTPAGE);
    }

    return nRet;
}

void CSchemePage::UpdateInfo()
{
    UpdateData(TRUE);

    // Reset any selection
    // g_tpmRoot->DeSelect();
    DeSelectAll();

    // Select the collections / packages required by this scheme
    int sel = m_ctlListScheme.GetCurSel();
    if (sel != LB_ERR) {
        TPM *tpmSelected = (TPM*)m_ctlListScheme.GetItemData(sel);
        // keep global name in case we go back there
        g_sSchemeName = tpmSelected->GetName();
        
        for (int i = 0; i < tpmSelected->GetRequired().GetSize(); i++) {
            TPM *tReq;
            CString sKey = tpmSelected->GetRequired().GetAt(i);
            if (g_vSourceTPM.Lookup(sKey, tReq)) {
                tReq->Select();
            }
        }
        
        RecalcSize();
        m_sSchemeDescription = tpmSelected->GetDescription();
    }
    m_sSpaceAvailable.Format("%ld", g_dwSpaceAvailable);
    m_sTotalSize.Format("%ld", g_dwSpaceNeeded);

    UpdateData(FALSE);
}

LRESULT CSchemePage::OnWizardBack() 
{
    // TODO: Add your specialized code here and/or call the base class

    // Remove unneeded tpm files
    if (g_bRemoveSourceTPMDir && DirectoryExists(g_sSourceTPMDir))
        RecursivelyRemove(g_sSourceTPMDir);

    return IDD_PROPPAGE_ROOT;
    //    return CPropertyPage::OnWizardBack();
}

LRESULT CSchemePage::OnWizardNext() 
{
    UpdateInfo();

    if (g_bInstallXEmTeX) {
        TPM *tpm = NULL; 
        if (g_vSourceTPM.Lookup("collection/tex-xemtex", tpm)) {
            tpm->Select();
        }
    }

    if (m_pageNext == 0) {
        CTeXSetupWizard *sheet = (CTeXSetupWizard*) GetParent();
        if (g_bQuickInstall)
            m_pageNext = sheet->m_pageFileCopy;
	else if (m_bCustomizeScheme)
            m_pageNext = sheet->m_pagePackages;
        else
            m_pageNext = sheet->m_pageReview;
        sheet->AddPage(m_pageNext);
    }
    return CPropertyPage::OnWizardNext();
}

BOOL CSchemePage::OnQueryCancel() 
{
    // TODO: Add your specialized code here and/or call the base class
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    //    BOOL ret = (AfxMessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    BOOL bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);
    return bRet;
}

void CSchemePage::OnBnClickedCheckCustomizeScheme()
{
    // TODO: Add your control notification handler code here

}

void CSchemePage::OnLbnSelchangeListScheme()
{
    // TODO: Add your control notification handler code here
    // TODO: Add your specialized code here and/or call the base class
    UpdateInfo();
}


/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

