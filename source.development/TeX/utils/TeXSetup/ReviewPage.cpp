// ReviewPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "texsetup.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"
#include "ReviewPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CReviewPage, CPropertyPage)


    /////////////////////////////////////////////////////////////////////////////
// CReviewPage property page

CReviewPage::CReviewPage() : CPropertyPage(CReviewPage::IDD)
{
    //{{AFX_DATA_INIT(CReviewPage)
    m_sSummary = _T("");
    //}}AFX_DATA_INIT

    m_pageNext = 0;
}

CReviewPage::~CReviewPage()
{
}

void CReviewPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CReviewPage)
    DDX_Text(pDX, IDC_EDIT_SUMMARY, m_sSummary);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReviewPage, CPropertyPage)
    //{{AFX_MSG_MAP(CReviewPage)
    // NOTE: the ClassWizard will add message map macros here
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL CReviewPage::OnInitDialog()
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

void CReviewPage::Initialize()
{
    // All we have to do is to remind the user the various options
    // he has choosen.
    // Title
    m_sSummary.Empty();
    CString m_sTmp;
    m_sTmp.Format("You are about to install the TeXLive %s Software.\r\n\r\n", TEXLIVE_VERSION);
    m_sSummary += m_sTmp;

    // Space needed and available
    m_sTmp.Format("You have %d kb available on your disk\r\nand you need %d kb to install the files requested.\r\n\r\n", 
                  g_dwSpaceAvailable, g_dwSpaceNeeded);
    // Folder Name
    m_sSummary += m_sTmp;
    m_sTmp.Format("Menu entries will be stored in a folder named %s\r\n\r\n", g_sFolderName);
    // Directories
    m_sSummary += m_sTmp;
    m_sTmp.Format("    Root        Dir: %s\r\n", (const char *) g_sInstallRootDir);
    m_sSummary += m_sTmp;
    m_sTmp.Format("    Main  Texmf Dir: %s\r\n", (const char *) g_sTexmfMain);
    m_sSummary += m_sTmp;
    m_sTmp.Format("    Local Texmf Dir: %s\r\n", (const char *) g_sTexmfLocal);
    m_sSummary += m_sTmp;
    m_sTmp.Format("    Extra Texmf Dir: %s\r\n", (const char *) g_sTexmfExtra);
    m_sSummary += m_sTmp;
    m_sTmp.Format("    Home  Texmf Dir: %s\r\n", (const char *) g_sTexmfHome);
    m_sSummary += m_sTmp;
    m_sTmp.Format("    Var   Texmf Dir: %s\r\n", (const char *) g_sVarTexmf);
    m_sSummary += m_sTmp;
    m_sTmp.Format("    Var   Fonts Dir: %s\r\n", (const char *) g_sVarTexFonts);
    m_sSummary += m_sTmp;

	
    // Doc and source
    m_sTmp.Format("TeX packages will be installed %s their documentation.\r\n",
                  g_bInstallDocumentation ? "with" : "without");
    m_sSummary += m_sTmp;
    m_sTmp.Format("TeX packages will be installed %s their source files.\r\n",
                  g_bInstallSource ? "with" : "without");
    m_sSummary += m_sTmp;
    m_sTmp.Format("XEmTeX support files will %s be installed.\r\n", g_bInstallXEmTeX ? "" : "not");
    m_sSummary += m_sTmp;
    m_sTmp.Format("\r\n");
    m_sSummary += m_sTmp;

    m_sTmp.Format("\r\nSelected Packages:\r\n");
    m_sSummary += m_sTmp;

    POSITION pos;
    TPM *tpm;
    CString sKey;
    pos = g_vSourceTPM.GetStartPosition();
    while (pos) {
        g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
        m_sTmp.Format("%s(%ld),\r\n", sKey, tpm->GetSize(false));
        if (tpm->IsSelected()) {
            m_sSummary += m_sTmp;
        }
    }

    UpdateData(FALSE);
	
    CEdit *editSummary = reinterpret_cast<CEdit *>(GetDlgItem(IDC_EDIT_SUMMARY));
    editSummary->SetSel(-1, -1);
}

BOOL CReviewPage::OnSetActive() 
{
    // TODO: Add your specialized code here and/or call the base class

    Initialize();

    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (sheet != 0) {
        sheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT);
        if (m_pageNext != 0) {
            sheet->RemovePage(m_pageNext);
            m_pageNext = 0;
        }
    }
	
    return CPropertyPage::OnSetActive();
}

LRESULT CReviewPage::OnWizardNext() 
{
    // TODO: Add your specialized code here and/or call the base class
    CTeXSetupWizard *sheet = (CTeXSetupWizard*) GetParent();
    m_pageNext = sheet->m_pageFileCopy;
    sheet->AddPage(m_pageNext);
	
    return CPropertyPage::OnWizardNext();
}

LRESULT CReviewPage::OnWizardBack() 
{
    // TODO: Add your specialized code here and/or call the base class
	
    return CPropertyPage::OnWizardBack();

}

BOOL CReviewPage::OnQueryCancel() 
{
    // TODO: Add your specialized code here and/or call the base class
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    //    BOOL ret = (AfxMessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    BOOL ret = ::MessageBox(NULL, (LPCTSTR)sMsg, (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION);
    return ret;
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

