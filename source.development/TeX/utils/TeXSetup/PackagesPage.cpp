// PackagesPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "texsetup.h"
#include "Win32Util.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"
#include "PackagesPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const UINT nIDTimer = 314;

IMPLEMENT_DYNCREATE(CPackagesPage, CPropertyPage)

    /////////////////////////////////////////////////////////////////////////////
// CPackagesPage property page

CPackagesPage::CPackagesPage() : CPropertyPage(CPackagesPage::IDD)
{
    //{{AFX_DATA_INIT(CPackagesPage)
    m_sSpaceAvailable = _T("");
    m_sPackageSize = _T("");
    m_sTotalSize = _T("");
    m_sDescription = _T("");
    m_bInstallDocumentation = FALSE;
    m_bInstallSource = FALSE;
    //}}AFX_DATA_INIT

    m_pageNext = 0;
}

CPackagesPage::~CPackagesPage()
{
}

void CPackagesPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPackagesPage)
    DDX_Control(pDX, IDC_TREE_PACKAGES, m_cPackages);
    DDX_Text(pDX, IDC_STATIC_SPACE_AVAILABLE, m_sSpaceAvailable);
    DDX_Text(pDX, IDC_STATIC_PACKAGE_SIZE, m_sPackageSize);
    DDX_Text(pDX, IDC_STATIC_TOTAL_SIZE, m_sTotalSize);
    DDX_Text(pDX, IDC_EDIT_DESCRIPTION, m_sDescription);
    DDX_Check(pDX, IDC_CHECK_DOC, m_bInstallDocumentation);
    DDX_Check(pDX, IDC_CHECK_SOURCE, m_bInstallSource);
    //}}AFX_DATA_MAP
}

#define WM_NEXTPAGE (WM_USER + 107)

BEGIN_MESSAGE_MAP(CPackagesPage, CPropertyPage)
    //{{AFX_MSG_MAP(CPackagesPage)
    ON_BN_CLICKED(IDC_CHECK_DOC, OnCheckDoc)
    ON_WM_CREATE()
    ON_BN_CLICKED(IDC_CHECK_SOURCE, OnCheckSource)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


BOOL CPackagesPage::PreTranslateMessage(MSG* pMsg)
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

BOOL CPackagesPage::OnInitDialog()
{
    CWnd * wnd = GetDlgItem(IDC_STATIC_HEADER);
    ASSERT (wnd != 0);
    extern CFont fntHeader;
    wnd->SetFont (&fntHeader);
    SetWindowText(g_sCaption);

    if (m_cPackages.m_hWnd == 0) {
	//	DWORD ret = m_cPackages.SubclassDlgItem(IDC_TREE_PACKAGES, this);
    }

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

    m_cPackages.Initialize();

    if (g_bInstallXEmTeX) {
        TPM *tpm = NULL; 
        if (g_vSourceTPM.Lookup("collection/tex-xemtex", tpm))
            tpm->Select();
    }

    return TRUE;	// CG: This was added by the ToolTips component.
}

BOOL CPackagesPage::OnSetActive()
{
    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (sheet != 0) {
        if (m_pageNext != 0) {
            sheet->RemovePage(m_pageNext);
            m_pageNext = 0;
        }
        sheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT);
        switch (g_eInstallType) {
        case INSTALL_FULL:
            // FIXME: Selection based on current / default scheme.
            SelectDefault(g_vSourceTPM);
            break;
        case INSTALL_MAINTENANCE:
            // If some package was given select it and jump to the next dialog
            if (! g_sPackage.IsEmpty()) {
                TPM *tpm = PackageLookup(g_sPackage);
                if (tpm != 0) {
                    tpm->Select();
                }
                else {
                    theLog("Warning: `--add-package=%s' required, but package not found!\n",
                           g_sPackage);
                }
            }
            break;
        case INSTALL_UNINSTALL:
            break;
        }
    }

    m_cPackages.UpdateStateImage(m_cPackages.GetRootItem());
    
    m_bInstallDocumentation = g_bInstallDocumentation;
    m_bInstallSource = g_bInstallSource;
    UpdateData(FALSE);
    
    int nRet = CPropertyPage::OnSetActive();
    
    if (g_bQuickInstall) {
        // sheet->PressButton(PSBTN_NEXT);
        GdiFlush();
        PostMessage(WM_NEXTPAGE);
    }

    return nRet;
}

LRESULT CPackagesPage::OnWizardBack() 
{
    DWORD dwRet;

    if (g_eInstallType == INSTALL_FULL) {
        TPM *tpm = NULL; 
        if (g_vSourceTPM.Lookup("collection/tex-xemtex", tpm))
            g_bInstallXEmTeX = tpm->IsSelected();
    }

    // TODO: Add your specialized code here and/or call the base class
    if (g_eInstallType == INSTALL_MAINTENANCE) {
        dwRet = IDD_PROPPAGE_GETTPM;
    }
    else {
        dwRet = IDD_PROPPAGE_SCHEME;
    }

    return dwRet;
    //    return CPropertyPage::OnWizardBack();
}

LRESULT CPackagesPage::OnWizardNext() 
{
    // TODO: Add your specialized code here and/or call the base class
    UpdateData(TRUE);
    g_bInstallDocumentation = m_bInstallDocumentation;
    g_bInstallSource = m_bInstallSource;

    if (m_pageNext == 0) {
        CTeXSetupWizard *sheet = (CTeXSetupWizard*) GetParent();
        if (g_bQuickInstall)
            m_pageNext = sheet->m_pageFileCopy;
	else
            m_pageNext = sheet->m_pageReview;
        sheet->AddPage(m_pageNext);
    }
    return CPropertyPage::OnWizardNext();
}

void CPackagesPage::UpdateSizes(DWORD dwPackageSize)
{
    RecalcSize();
    m_sSpaceAvailable.Format("%ld", g_dwSpaceAvailable);
    m_sTotalSize.Format("%ld", g_dwSpaceNeeded);
    m_sPackageSize.Format("%ld", dwPackageSize);
    UpdateData(FALSE);
}

BOOL CPackagesPage::OnQueryCancel() 
{
    // TODO: Add your specialized code here and/or call the base class
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    //    BOOL ret = (AfxMessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    BOOL bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);
    return bRet;
}

void CPackagesPage::OnCheckDoc() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    g_bInstallDocumentation = m_bInstallDocumentation;
    HTREEITEM h = m_cPackages.GetSelectedItem();
    if (h) {
        TPM *tpm = (TPM *)m_cPackages.GetItemData(h);
        if (tpm) 
            m_sPackageSize.Format("%ld", tpm->GetSize(true, g_dwClusterSize, true, true) / 1024);
    }
    RecalcSize();
    m_sTotalSize.Format("%ld", g_dwSpaceNeeded);
    UpdateData(FALSE);
	
}

void CPackagesPage::OnCheckSource() 
{
    // TODO: Add your control notification handler code here
    UpdateData(TRUE);
    g_bInstallSource = m_bInstallSource;
    HTREEITEM h = m_cPackages.GetSelectedItem();
    if (h) {
        TPM *tpm = (TPM *)m_cPackages.GetItemData(h);
        if (tpm)
            m_sPackageSize.Format("%ld", tpm->GetSize(true, g_dwClusterSize, true, true) / 1024);
    }
    RecalcSize();
    m_sTotalSize.Format("%ld", g_dwSpaceNeeded);
    UpdateData(FALSE);
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

