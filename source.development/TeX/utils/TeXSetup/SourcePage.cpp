// SourcePage.cpp : implementation file
//

#include "stdafx.h"
#include "texsetup.h"
#include "texinstall.h"
#include "TeXSetupWizard.h"
#include "ListChoiceDlg.h"
#include "SourcePage.h"
#include "Win32Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

HANDLE CSourcePage::m_hEventDone = 0;

/////////////////////////////////////////////////////////////////////////////
// CSourcePage property page

IMPLEMENT_DYNCREATE(CSourcePage, CPropertyPage)

    CSourcePage::CSourcePage() : CPropertyPage(CSourcePage::IDD)
{
    //{{AFX_DATA_INIT(CSourcePage)
    m_bNetAccess = FALSE;
    m_sProxyAddress = _T("");
    m_uiProxyPort = 80;
    m_sRemoteSourceDir = _T("");
    m_sLocalSourceDir = _T("");
    //}}AFX_DATA_INIT
    m_bInitialized = false;
    m_uiSourceType = SOURCE_CDROM;
    m_uiNetMethod = NetIOUnavail;
    m_pageNext = 0;
}

CSourcePage::~CSourcePage()
{
}

void CSourcePage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CSourcePage)
    DDX_Check(pDX, IDC_CHECK_NETDOWNLOAD, m_bNetAccess);
    DDX_Text(pDX, IDC_EDIT_PROXY_ADDRESS, m_sProxyAddress);
    DDX_Text(pDX, IDC_EDIT_PROXY_PORT, m_uiProxyPort);
    DDV_MinMaxUInt(pDX, m_uiProxyPort, 1, 65534);
    DDX_Text(pDX, IDC_EDIT_REMOTE_DIR, m_sRemoteSourceDir);
    DDX_Text(pDX, IDC_EDIT_SOURCE_DIR, m_sLocalSourceDir);
    //}}AFX_DATA_MAP
}

#define WM_NEXTPAGE (WM_USER + 107)

BEGIN_MESSAGE_MAP(CSourcePage, CPropertyPage)
    //{{AFX_MSG_MAP(CSourcePage)
    //	ON_WM_TIMER()
    ON_BN_CLICKED(IDC_RADIO_SOURCE_CDROM, OnRadioSourceCdrom)
    ON_BN_CLICKED(IDC_RADIO_SOURCE_ZIP, OnRadioSourceZip)
    ON_BN_CLICKED(IDC_RADIO_SOURCE_INTERNET, OnRadioSourceInternet)
    ON_BN_CLICKED(IDC_INTERNET_CONNECTION_IE5, OnInternetConnectionIe5)
    ON_BN_CLICKED(IDC_INTERNET_CONNECTION_DIRECT, OnInternetConnectionDirect)
    ON_BN_CLICKED(IDC_INTERNET_CONNECTION_PROXY, OnInternetConnectionProxy)
    ON_BN_CLICKED(IDC_BROWSE_LOCAL, OnBrowseLocal)
    ON_BN_CLICKED(IDC_BROWSE_REMOTE, OnBrowseRemote)
    ON_BN_CLICKED(IDC_CHECK_NETDOWNLOAD, OnCheckNetdownload)
    //}}AFX_MSG_MAP
    //  ON_MESSAGE(WM_STARTTPMINIT, OnStartTPMInit)
    END_MESSAGE_MAP()

BOOL CSourcePage::PreTranslateMessage(MSG* pMsg)
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

BOOL CSourcePage::OnInitDialog()
{
    // Export data towards the dialog controls
  
    UpdateData(FALSE);
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
    UpdateData(FALSE);

    return TRUE;	// CG: This was added by the ToolTips component.
}

/////////////////////////////////////////////////////////////////////////////
// CSourcePage message handlers

void CSourcePage::OnRadioSourceCdrom() 
{
    // TODO: Add your control notification handler code here
    m_uiSourceType = SOURCE_CDROM;
    ValidateControls();
}

void CSourcePage::OnRadioSourceZip() 
{
    // TODO: Add your control notification handler code here
    m_uiSourceType = SOURCE_ZIP;
    ValidateControls();
}

void CSourcePage::OnRadioSourceInternet() 
{
    // TODO: Add your control notification handler code here
    m_uiSourceType = SOURCE_INTERNET;
    ValidateControls();
}

void CSourcePage::OnInternetConnectionIe5() 
{
    // TODO: Add your control notification handler code here
    m_uiNetMethod = NetIOIE5;
    ValidateControls();
}

void CSourcePage::OnInternetConnectionDirect() 
{
    // TODO: Add your control notification handler code here
    m_uiNetMethod = NetIODirect;
    ValidateControls();
}

void CSourcePage::OnInternetConnectionProxy() 
{
    // TODO: Add your control notification handler code here
    m_uiNetMethod = NetIOProxy;
    ValidateControls();
}

void CSourcePage::OnCheckNetdownload() 
{
    // TODO: Add your control notification handler code here
    m_bNetAccess = ! m_bNetAccess;
    if (m_bNetAccess && m_uiNetMethod == NetIOUnavail)
        m_uiNetMethod = NetIOIE5;
    ValidateControls();
}

void CSourcePage::OnBrowseLocal() 
{
    // TODO: Add your control notification handler code here
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
        sTmp = ::BrowseForFolder(this->m_hWnd, "Please select a source directory below.", dwFlags, m_sLocalSourceDir);	
        if (m_sLocalSourceDir && ! sTmp.IsEmpty()) 
	    m_sLocalSourceDir = sTmp;
    }
    UpdateData(FALSE);	
}

void CSourcePage::OnBrowseRemote() 
{
    // TODO: Add your control notification handler code here
    ImportData();
    // First : retrieve the list of mirrors
    CString sUrl = "http://www.fptex.org/mirrors.lst";
    CString sMirrors = GrabInternetFile(sUrl);
    if (sMirrors.IsEmpty()) {
        AfxMessageBox("Failed to get the list of mirrors !");
        return;
    }
    
    CListChoiceDlg dlgBrowseUrls;
    dlgBrowseUrls.m_sTitle = "TeXLive/Win32 Mirrors";
    dlgBrowseUrls.m_sDescription = "Choose the nearest mirror :";
    sMirrors += "\n";
    int nPos = sMirrors.FindOneOf(" \r\n");
    CString sDummy;
    while (nPos != -1) {
        sDummy = sMirrors.Left(nPos);
        if (! sDummy.IsEmpty())
            dlgBrowseUrls.AddChoice(sDummy);
        sMirrors = sMirrors.Right(sMirrors.GetLength() - nPos - 1);
        nPos = sMirrors.FindOneOf(" \r\n");
    }
    dlgBrowseUrls.SetCheck(1, "", FALSE);
    dlgBrowseUrls.SetCheck(2, "", FALSE);
    dlgBrowseUrls.SetCheck(3, "", FALSE);
    if (dlgBrowseUrls.DoModal() == IDOK) {
        m_sRemoteSourceDir = dlgBrowseUrls.m_sListChoice;
        UpdateData(FALSE);
    }

    
}

BOOL CSourcePage::OnQueryCancel ()
{
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    //    BOOL ret = (AfxMessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    BOOL bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);
    return bRet;
}

LRESULT CSourcePage::OnWizardNext() 
{
    // TODO: Add your specialized code here and/or call the base class

    ImportData();

    // FIXME: we might want to do some checking there.
    if (m_pageNext == 0) {
        CTeXSetupWizard *pParent = (CTeXSetupWizard*) GetParent();
        if (g_eInstallType == INSTALL_MAINTENANCE) {
            m_pageNext = pParent->m_pageGetTPM;
        }
        else {
            m_pageNext = pParent->m_pageRoot;
        }
        pParent->AddPage(m_pageNext);
    }

    int nRet = -1;

    nRet = CPropertyPage::OnWizardNext();

    return nRet;
}

BOOL CSourcePage::OnSetActive()
{
    m_bInitialized = false; // FIXME: Should be more precise,

    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());

    // reset only if tpm files location changed
    ExportData();
    if (sheet != 0) {
        sheet->SetWizardButtons (PSWIZB_BACK | PSWIZB_NEXT);
        if (m_pageNext != 0) {
            sheet->RemovePage(m_pageNext);
            m_pageNext = 0;
        }
    }
	
    CButton *hBtn;
    switch (m_uiSourceType) {
    case SOURCE_CDROM:
        hBtn = (CButton *)GetDlgItem(IDC_RADIO_SOURCE_CDROM);
        if (hBtn) hBtn->SetCheck(true);
        break;
    case SOURCE_ZIP:
        hBtn = (CButton *)GetDlgItem(IDC_RADIO_SOURCE_ZIP);
        if (hBtn) hBtn->SetCheck(true);
        //		SendDlgItemMessage(IDC_RADIO_SOURCE_ZIP, WM_COMMAND, BN_CLICKED, 0);
        break;
    case SOURCE_INTERNET:
        hBtn = (CButton *)GetDlgItem(IDC_RADIO_SOURCE_INTERNET);
        if (hBtn) hBtn->SetCheck(true);
        hBtn = (CButton *)GetDlgItem(IDC_INTERNET_CONNECTION_IE5);
        if (hBtn) hBtn->SetCheck(true);
        m_uiNetMethod = NetIOIE5;
        m_bNetAccess = true;
        //		SendDlgItemMessage(IDC_RADIO_SOURCE_INTERNET, WM_COMMAND, BN_CLICKED, 0);
        break;
    }
    UpdateData(FALSE);
    ValidateControls();

    int nRet = CPropertyPage::OnSetActive();

    if (g_bQuickInstall) {
        PostMessage(WM_NEXTPAGE);
        //        sheet->PressButton(PSBTN_NEXT);
    }

    return nRet;
}


void CSourcePage::ExportData()
{
    m_sLocalSourceDir = g_sLocalSourceDir;
    m_sRemoteSourceDir = g_sRemoteSourceDir;
    m_uiNetMethod = g_uiNetMethod;
    m_bNetAccess = (g_uiNetMethod != NetIOUnavail);
    m_sProxyAddress = g_sProxyAddress;
    m_uiProxyPort = g_uiProxyPort;
    m_uiSourceType = g_eSourceType;
    UpdateData(FALSE);
    ValidateControls();
}

void CSourcePage::ValidateControls()
{
    UpdateData(TRUE);
    CWnd *hwnd;
    CButton *hBtn;
    if (m_bNetAccess) {
        hwnd = GetDlgItem(IDC_EDIT_REMOTE_DIR);
        if (hwnd) hwnd->EnableWindow(TRUE);
        hwnd = GetDlgItem(IDC_INTERNET_CONNECTION_IE5);
        if (hwnd) hwnd->EnableWindow(TRUE);
        hwnd = GetDlgItem(IDC_INTERNET_CONNECTION_DIRECT);
        if (hwnd) hwnd->EnableWindow(TRUE);
        hwnd = GetDlgItem(IDC_INTERNET_CONNECTION_PROXY);
        if (hwnd) hwnd->EnableWindow(TRUE);
        hwnd = GetDlgItem(IDC_RADIO_SOURCE_INTERNET);
        if (hwnd) hwnd->EnableWindow(TRUE);
        switch (m_uiNetMethod) {
        case NetIOIE5:
            hBtn = (CButton *)GetDlgItem(IDC_INTERNET_CONNECTION_IE5);
            if (hBtn) hBtn->SetCheck(TRUE);
            hwnd = GetDlgItem(IDC_EDIT_PROXY_ADDRESS);
            if (hwnd) hwnd->EnableWindow(FALSE);
            hwnd = GetDlgItem(IDC_EDIT_PROXY_PORT);
            if (hwnd) hwnd->EnableWindow(FALSE);
            break;
        case NetIODirect:
            hBtn = (CButton *)GetDlgItem(IDC_INTERNET_CONNECTION_DIRECT);
            if (hBtn) hBtn->SetCheck(TRUE);
            // Invalidate
            hwnd = GetDlgItem(IDC_EDIT_PROXY_ADDRESS);
            if (hwnd) hwnd->EnableWindow(FALSE);
            hwnd = GetDlgItem(IDC_EDIT_PROXY_PORT);
            if (hwnd) hwnd->EnableWindow(FALSE);
            break;
        case NetIOProxy:
            hBtn = (CButton *)GetDlgItem(IDC_INTERNET_CONNECTION_PROXY);
            if (hBtn) hBtn->SetCheck(TRUE);
            // Validate Proxy port and host
            hwnd = GetDlgItem(IDC_EDIT_PROXY_ADDRESS);
            if (hwnd) hwnd->EnableWindow(TRUE);
            hwnd = GetDlgItem(IDC_EDIT_PROXY_PORT);
            if (hwnd) hwnd->EnableWindow(TRUE);
            break;
        }
    }
    else {
        // Invalidate all net related controls
        hwnd = GetDlgItem(IDC_EDIT_PROXY_ADDRESS);
        if (hwnd) hwnd->EnableWindow(FALSE);
        hwnd = GetDlgItem(IDC_EDIT_PROXY_PORT);
        if (hwnd) hwnd->EnableWindow(FALSE);
        hwnd = GetDlgItem(IDC_EDIT_REMOTE_DIR);
        if (hwnd) hwnd->EnableWindow(FALSE);
        hwnd = GetDlgItem(IDC_INTERNET_CONNECTION_IE5);
        if (hwnd) hwnd->EnableWindow(FALSE);
        hwnd = GetDlgItem(IDC_INTERNET_CONNECTION_DIRECT);
        if (hwnd) hwnd->EnableWindow(FALSE);
        hwnd = GetDlgItem(IDC_INTERNET_CONNECTION_PROXY);
        if (hwnd) hwnd->EnableWindow(FALSE);
        hwnd = GetDlgItem(IDC_RADIO_SOURCE_INTERNET);
        if (hwnd) hwnd->EnableWindow(FALSE);
    }
}

void CSourcePage::ImportData()
{
    UpdateData(TRUE);
    g_uiNetMethod = m_uiNetMethod;
    g_sProxyAddress = m_sProxyAddress;
    g_uiProxyPort = m_uiProxyPort;
    g_eSourceType = m_uiSourceType;
    g_sLocalSourceDir = m_sLocalSourceDir;
    g_sRemoteSourceDir = m_sRemoteSourceDir;
    if (g_eSourceType == SOURCE_INTERNET) {
        g_sWin32Dir = ConcatPath(g_sRemoteSourceDir, "setupw32", '/');
    }
    else {
        g_sWin32Dir = ConcatPath(g_sLocalSourceDir, "setupw32");
    }
}

LRESULT CSourcePage::OnWizardBack() 
{
    // TODO: Add your specialized code here and/or call the base class
    ImportData();	
    return CPropertyPage::OnWizardBack();
}
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

