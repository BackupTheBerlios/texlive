// RootPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "texsetup.h"
#include "TeXInstall.h"
#include "TeXSetupWizard.h"
#include "RootPage.h"
#include "Win32Util.h"
#include "shlwapi.h"
#include "shlobj.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

static CString g_sLongInstallRootDir = _T("");

IMPLEMENT_DYNCREATE(CRootPage, CPropertyPage)


    /////////////////////////////////////////////////////////////////////////////
// CRootPage property page

CRootPage::CRootPage() : CPropertyPage(CRootPage::IDD)
{
    //{{AFX_DATA_INIT(CRootPage)
    m_sInstallRootDir = _T("");
    m_sSpaceAvailable = _T("");
    m_sSpaceNeeded = _T("");
    m_sTexmfExtra = _T("");
    m_sTexmfHome = _T("");
    m_sTexmfLocal = _T("");
    m_sVarTexmf = _T("");
    m_sVarTexFonts = _T("");
    m_bCustomDirs = FALSE;
    //}}AFX_DATA_INIT
    m_psLastEdit = 0;
    m_pageNext = 0;
}

CRootPage::~CRootPage()
{
}

void CRootPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CRootPage)
    DDX_Text(pDX, IDC_ROOTDIR, m_sInstallRootDir);
    DDX_Text(pDX, IDC_STATIC_SPACE_AVAILABLE, m_sSpaceAvailable);
    DDX_Text(pDX, IDC_STATIC_DEFAULT_SIZE, m_sSpaceNeeded);
    DDX_Text(pDX, IDC_EXTRA_TEXMF, m_sTexmfExtra);
    DDX_Text(pDX, IDC_HOME_TEXMF, m_sTexmfHome);
    DDX_Text(pDX, IDC_LOCAL_TEXMF, m_sTexmfLocal);
    DDX_Text(pDX, IDC_VAR_TEXMF, m_sVarTexmf);
    DDX_Text(pDX, IDC_VAR_FONTS, m_sVarTexFonts);
    DDX_Check(pDX, IDC_CHECK_CUSTOM_DIRS, m_bCustomDirs);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRootPage, CPropertyPage)
    //{{AFX_MSG_MAP(CRootPage)
    ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
    ON_EN_KILLFOCUS(IDC_ROOTDIR, OnKillfocusRootdir)
    ON_EN_SETFOCUS(IDC_EXTRA_TEXMF, OnSetfocusExtraTexmf)
    ON_EN_SETFOCUS(IDC_HOME_TEXMF, OnSetfocusHomeTexmf)
    ON_EN_SETFOCUS(IDC_LOCAL_TEXMF, OnSetfocusLocalTexmf)
    ON_EN_SETFOCUS(IDC_VAR_FONTS, OnSetfocusVarFonts)
    ON_EN_SETFOCUS(IDC_VAR_TEXMF, OnSetfocusVarTexmf)
    ON_EN_SETFOCUS(IDC_ROOTDIR, OnSetfocusRootdir)
    ON_BN_CLICKED(IDC_CHECK_CUSTOM_DIRS, OnCheckCustomDirs)
    ON_EN_CHANGE(IDC_ROOTDIR, OnChangeRootdir)
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    BOOL CRootPage::PreTranslateMessage(MSG* pMsg)
{
    // CG: The following block was added by the ToolTips component.
    {
        // Let the ToolTip process this message.
        m_tooltip.RelayEvent(pMsg);
    }
    return CPropertyPage::PreTranslateMessage(pMsg);	// CG: This was added by the ToolTips component.
}

BOOL CRootPage::OnInitDialog()
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

// FIXME : disable if NT 3.51 !
void CRootPage::OnBrowse() 
{
    DWORD dwFlags = BIF_RETURNONLYFSDIRS;

    if (dwShlwapiVersion >= PACKVERSION(4,71)
        || dwShell32Version >= PACKVERSION(4,71)) {
        dwFlags |= BIF_EDITBOX | BIF_VALIDATE;
    }

    if (dwShlwapiVersion >= PACKVERSION(5,0)
        || dwShell32Version >= PACKVERSION(5,0)) {
        dwFlags |= BIF_USENEWUI;
    }
    {
        CString sTmp;
        sTmp = ::BrowseForFolder(this->m_hWnd, "Please select a root folder below.", dwFlags, *m_psLastEdit);	
        if (m_psLastEdit && ! sTmp.IsEmpty()) 
	    *m_psLastEdit = sTmp;
        if (m_psLastEdit == &m_sInstallRootDir) {
            OnChangeRootdir();
        }
    }
    UpdateData(FALSE);
}

void CRootPage::UpdatePaths()
{
    UpdateData(TRUE);

    if (m_bCustomDirs) {

        m_sInstallRootDir.TrimLeft();
        m_sInstallRootDir.TrimRight();
        g_sInstallRootDir = m_sInstallRootDir;

        m_sTexmfExtra.TrimLeft();
        m_sTexmfExtra.TrimRight();
        g_sTexmfExtra = m_sTexmfExtra;
        
        m_sTexmfLocal.TrimLeft();
        m_sTexmfLocal.TrimRight();
        g_sTexmfLocal = m_sTexmfLocal;
        
        m_sTexmfHome.TrimLeft();
        m_sTexmfHome.TrimRight();
        g_sTexmfHome = m_sTexmfHome;
        
        m_sVarTexmf.TrimLeft();
        m_sVarTexmf.TrimRight();
        g_sVarTexmf = m_sVarTexmf;
        
        m_sVarTexFonts.TrimLeft();
        m_sVarTexFonts.TrimRight();
        g_sVarTexFonts = m_sVarTexFonts;
    }
    else {
        m_sInstallRootDir.TrimLeft();
        m_sInstallRootDir.TrimRight();
        if (m_sInstallRootDir != g_sInstallRootDir) {
            SetRootDir(m_sInstallRootDir);
        }
    }
}

void CRootPage::OnKillfocusRootdir() 
{
    // TODO: Add your control notification handler code here
    UpdatePaths();

    m_sSpaceAvailable.Format("%ld", g_dwSpaceAvailable);
    m_sSpaceNeeded.Format("%ld", g_dwSpaceNeeded);

    UpdateData(FALSE);
}

LRESULT CRootPage::OnWizardBack() 
{
    // TODO: Add your specialized code here and/or call the base class
    UpdatePaths();

    return CPropertyPage::OnWizardBack();
}

LRESULT CRootPage::OnWizardNext() 
{
    // TODO: Add your specialized code here and/or call the base class
    UpdatePaths();

    if (m_pageNext == 0) {
        CTeXSetupWizard *sheet = (CTeXSetupWizard*) GetParent();
        m_pageNext = sheet->m_pageGetTPM;
        sheet->AddPage(m_pageNext);
    }

    return CPropertyPage::OnWizardNext();
}

BOOL CRootPage::OnSetActive() 
{
    // TODO: Add your specialized code here and/or call the base class

    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (sheet != 0) {
        sheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT);
        if (m_pageNext != 0) {
            sheet->m_pageGetTPM->RemoveNextFromWizard();
            sheet->RemovePage(m_pageNext);
            m_pageNext = 0;
        }
    }

#if 0
    g_sInstallRootDir = g_sLongInstallRootDir;
#endif
    m_sInstallRootDir = g_sInstallRootDir;
    m_sTexmfLocal = g_sTexmfLocal;
    m_sTexmfHome = g_sTexmfHome;
    m_sTexmfExtra = g_sTexmfExtra;
    m_sVarTexmf = g_sVarTexmf;
    m_sVarTexFonts = g_sVarTexFonts;

    m_bCustomDirs = FALSE;

    m_sSpaceAvailable.Format("%ld", g_dwSpaceAvailable);
    m_sSpaceNeeded.Format("%ld", g_dwSpaceNeeded);

    ValidateControls();

    UpdateData(FALSE);

    return CPropertyPage::OnSetActive();
}

void CRootPage::OnSetfocusExtraTexmf() 
{
    // TODO: Add your control notification handler code here
    m_psLastEdit = &m_sTexmfExtra;	
}

void CRootPage::OnSetfocusHomeTexmf() 
{
    // TODO: Add your control notification handler code here
    m_psLastEdit = &m_sTexmfHome;	
}

void CRootPage::OnSetfocusLocalTexmf() 
{
    // TODO: Add your control notification handler code here
    m_psLastEdit = &m_sTexmfLocal;	
}

void CRootPage::OnSetfocusVarFonts() 
{
    // TODO: Add your control notification handler code here
    m_psLastEdit = &m_sVarTexFonts;	
}

void CRootPage::OnSetfocusVarTexmf() 
{
    // TODO: Add your control notification handler code here
    m_psLastEdit = &m_sVarTexmf;
}

void CRootPage::OnSetfocusRootdir() 
{
    // TODO: Add your control notification handler code here
    m_psLastEdit = &m_sInstallRootDir;
}

void CRootPage::OnCheckCustomDirs() 
{
    // TODO: Add your control notification handler code here
    m_bCustomDirs = ! m_bCustomDirs;
    ValidateControls();
}

void CRootPage::ValidateControls()
{
    CWnd *hwnd;
    hwnd = GetDlgItem(IDC_EXTRA_TEXMF);
    if (hwnd) hwnd->EnableWindow(m_bCustomDirs);
    hwnd = GetDlgItem(IDC_LOCAL_TEXMF);
    if (hwnd) hwnd->EnableWindow(m_bCustomDirs);
    hwnd = GetDlgItem(IDC_HOME_TEXMF);
    if (hwnd) hwnd->EnableWindow(m_bCustomDirs);
    hwnd = GetDlgItem(IDC_VAR_TEXMF);
    if (hwnd) hwnd->EnableWindow(m_bCustomDirs);
    hwnd = GetDlgItem(IDC_VAR_FONTS);
    if (hwnd) hwnd->EnableWindow(m_bCustomDirs);
}

void CRootPage::OnChangeRootdir() 
{
    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CPropertyPage::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.
	
    // TODO: Add your control notification handler code here

    UpdateData(TRUE);

    if (!m_bCustomDirs) {
        // m_sTexmfMain = ConcatPath(m_sInstallRootDir, "texmf");
        m_sTexmfLocal = ConcatPath(m_sInstallRootDir, "texmf-local");
        m_sVarTexmf = ConcatPath(m_sInstallRootDir, "texmf-var");
        m_sVarTexFonts = ConcatPath(m_sVarTexmf, "fonts");
    }

    UpdateData(FALSE);
}

BOOL CRootPage::OnQueryCancel() 
{
    // TODO: Add your specialized code here and/or call the base class
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    //    BOOL ret = (AfxMessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    //    return CPropertyPage::OnQueryCancel();

    BOOL bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);
    return bRet;
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

