// GetTPMPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "Win32Util.h"
#include "TeXSetup.h"
#include "TeXSetupWizard.h"
#include "TeXInstall.h"
#include "GetTPMPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT nIDTimer = 314;

IMPLEMENT_DYNCREATE(CGetTPMPage, CPropertyPage)

/////////////////////////////////////////////////////////////////////////////
// CGetTPMPage dialog


CGetTPMPage::CGetTPMPage() : CPropertyPage(CGetTPMPage::IDD)
{
	//{{AFX_DATA_INIT(CGetTPMPage)
	m_sPrompt = _T("");
	m_sDownloading = _T("");
	m_sReading = _T("");
	m_sTPMRead = _T("");
	m_sTPMZip = _T("");
	m_sPercentDownload = _T("");
	m_sPercentUnit = _T("");
	//}}AFX_DATA_INIT
        m_pageNext = 0;
}

CGetTPMPage::~CGetTPMPage()
{
}

void CGetTPMPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetTPMPage)
	DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS_READ, m_prgRead);
	DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS_DOWNLOAD, m_prgDownload);
	DDX_Text(pDX, IDC_PROMPT, m_sPrompt);
	DDX_Text(pDX, IDC_STATIC_DOWNLOADING, m_sDownloading);
	DDX_Text(pDX, IDC_STATIC_READING, m_sReading);
	DDX_Text(pDX, IDC_STATIC_TPMREAD, m_sTPMRead);
	DDX_Text(pDX, IDC_STATIC_TPMZIP, m_sTPMZip);
	DDX_Text(pDX, IDC_STATIC_PERCENT_DOWNLOAD, m_sPercentDownload);
	DDX_Text(pDX, IDC_STATIC_PERCENT_UNIT, m_sPercentUnit);
	//}}AFX_DATA_MAP
}


#define WM_NEXTPAGE (WM_USER + 107)

BEGIN_MESSAGE_MAP(CGetTPMPage, CPropertyPage)
    //{{AFX_MSG_MAP(CGetTPMPage)
    // NOTE: the ClassWizard will add message map macros here
    ON_WM_TIMER()
    ON_STN_CLICKED(IDC_PROMPT, OnStnClickedPrompt)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetTPMPage message handlers

BOOL CGetTPMPage::PreTranslateMessage(MSG* pMsg)
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

BOOL CGetTPMPage::OnInitDialog()
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
        //m_tooltip.AddTool(GetDlgItem(IDC_<name>), <string-table-id>);
    }

    return TRUE;	// CG: This was added by the ToolTips component.
}

LRESULT CGetTPMPage::OnWizardNext() 
{
    // TODO: Add your specialized code here and/or call the base class

    LRESULT nRet = -1;

    // CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    
    nRet = CPropertyPage::OnWizardNext();
    return nRet;
}

void CGetTPMPage::RemoveNextFromWizard()
{
    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (sheet != 0 && m_pageNext != 0) {
        sheet->RemovePage(m_pageNext);
        m_pageNext = 0;
    }
}

BOOL CGetTPMPage::OnSetActive()
{
    CTeXSetupWizard *sheet = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (sheet != 0) {

        // Not wise to allow the user to get back while
        // the thread reading tpm files is active
        // sheet->SetWizardButtons (PSWIZB_BACK);
        sheet->SetWizardButtons (0);

	if (m_pageNext != 0) {
	    sheet->RemovePage(m_pageNext);
	    m_pageNext = 0;
	}
    }

    int nRet = CPropertyPage::OnSetActive();

    CSingleLock singleLock (&g_csCritSec, TRUE);
    theStatus.bTerminate = false;
    theStatus.bReady = false;
    theStatus.bError = false;
    theStatus.sName = "";
    theStatus.dwOverallDone=0;
    singleLock.Unlock();
    
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, FALSE};
    // TODO: Add extra initialization here
    AfxBeginThread ((AFX_THREADPROC)TPMThread, 0,
                    /* nPriority */ THREAD_PRIORITY_NORMAL,
                    /* nStackSize */ 0,
                    /* dwCreateFlags */ 0,
                    /* lpSecurityAttrs */ &sa);
    
    // start the timer
    SetTimer (nIDTimer, 50, 0);

    return nRet;
}

BOOL CGetTPMPage::OnQueryCancel() 
{
    // TODO: Add your specialized code here and/or call the base class
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    //    BOOL ret = (AfxMessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    BOOL bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);

    if (bRet == IDYES) {
        CSingleLock singleLock (&g_csCritSec, TRUE);
        KillTimer (nIDTimer);
        theStatus.bTerminate = true;
        singleLock.Unlock();
    }

    return bRet;
}

void CGetTPMPage::OnStnClickedPrompt()
{
	// TODO: Add your control notification handler code here
}

void CGetTPMPage::OnTimer(UINT nIDEvent) 
{
    bool bReady, bError;
    unsigned int nOpId;
    DWORD dwTotal, dwPartial, dwDone;
    double ratio;
    CString sName;
    char tmp[64];
    // Be safe
    CSingleLock singleLock (&g_csCritSec, TRUE);
    bReady = theStatus.bReady;
    bError = theStatus.bError;
    nOpId = theStatus.nOpId;
    sName = theStatus.sName;
    dwDone = theStatus.dwPackageDone;
    dwPartial = theStatus.dwPackageSize;
    dwTotal = theStatus.dwOverallDone;
    singleLock.Unlock();
    
    // TODO: Add your message handler code here and/or call default
    if (bReady || bError) {
        CDialog::OnTimer(nIDEvent);
        InitCompleted();
        return;
    }
    else {
        CWnd *hWnd;
        switch (nOpId) {
        case IDS_NOW_DOWNLOADING:
            m_sTPMZip = sName;
            m_sDownloading.LoadString(nOpId);
            if (dwPartial == -1) {
                m_prgDownload.SetPos(0);
                wsprintf(tmp, "%ld", (int)(dwDone / 1024));
                m_sPercentDownload = tmp;
                m_sPercentUnit = "Kb";
            }
            else {
                ratio = ((double)dwDone/(double)dwPartial)*100.0;
                m_prgDownload.SetPos((int)ratio);
                wsprintf(tmp, "%d", (int)ratio);
                m_sPercentDownload = tmp;
                m_sPercentUnit = "%";
            }
            break;
        case IDS_NOW_UNZIPPING:
            m_sTPMZip = sName;
            m_sDownloading.LoadString(nOpId);
            ratio = ((double)dwDone/(double)dwPartial)*100.0;
            m_prgDownload.SetPos((int)ratio);
            wsprintf(tmp, "%d", (int)ratio);
            m_sPercentDownload = tmp;
            m_sPercentUnit = "%";
            break;
        case IDS_NOW_PARSING:
            hWnd = GetDlgItem(IDC_STATIC_DOWNLOADING);
            if (hWnd)
                hWnd->ShowWindow(SW_HIDE);
            hWnd = GetDlgItem(IDC_STATIC_TPMZIP);
            if (hWnd)
                hWnd->ShowWindow(SW_HIDE);
            hWnd = GetDlgItem(CG_IDC_PROGDLG_PROGRESS_DOWNLOAD);
            if (hWnd)
                hWnd->ShowWindow(SW_HIDE);
            hWnd = GetDlgItem(IDC_STATIC_PERCENT_DOWNLOAD);
            if (hWnd)
                hWnd->ShowWindow(SW_HIDE);
            hWnd = GetDlgItem(IDC_STATIC_PERCENT_UNIT);
            if (hWnd)
                hWnd->ShowWindow(SW_HIDE);
            m_sReading.LoadString(nOpId);
            m_sTPMRead = sName;
            ratio = ((double)dwTotal/(double)g_dwTPMNumber)*100.0;
            m_prgRead.SetPos((int)ratio);
            break;
        }
        UpdateData(FALSE);
    }
    CDialog::OnTimer(nIDEvent);
}

void CGetTPMPage::InitCompleted ()
{
    CSingleLock singleLock (&g_csCritSec, TRUE);
    theStatus.bReady = true;
    singleLock.Unlock();

    KillTimer (nIDTimer);

    CTeXSetupWizard * pParent = DYNAMIC_DOWNCAST(CTeXSetupWizard, GetParent());
    if (pParent != 0)
        pParent->SetWizardButtons (PSWIZB_NEXT);

    if (m_pageNext == 0) {

        CSingleLock singleLock(&g_csCritSec, TRUE);
        if (theStatus.bError) {
            g_sLastError.LoadString(IDS_CANT_READ_TPM_FILES);
            m_pageNext = pParent->m_pageError;
        }
        else if (g_eInstallType == INSTALL_MAINTENANCE) {
            m_pageNext = pParent->m_pagePackages;
        }
        else {
            m_pageNext = pParent->m_pageScheme;
        }
        pParent->AddPage(m_pageNext);        
    }
    
    PostMessage(WM_NEXTPAGE);
    // pParent->PressButton(PSBTN_NEXT);
}

UINT CGetTPMPage::TPMThread (LPVOID pParam)
{
    bool bRemoveSourceTPMDir = false;
    try {
        //        CSingleLock theLock(&g_csCritSec, FALSE);
        CString sTPMZipFile = ConcatPath(g_sLocalSourceDir, "tpm.zip");
        CString sRemoteTPMFiles = ConcatPath(g_sRemoteSourceDir, "tpm.zip", '/');
        if (g_eSourceType == SOURCE_ZIP) {
            g_sSourceTPMDir = g_sLocalSourceDir;
        }
        else {
            g_sSourceTPMDir = ConcatPath(g_sLocalSourceDir, "texmf\\tpm");
        }
        g_sLocalTPMDir = ConcatPath(g_sInstallRootDir, "texmf\\tpm");
        g_bRemoveSourceTPMDir = false;
        switch (g_eSourceType) {
        case SOURCE_INTERNET:
            // Should we suppose that the ftp server is able to zip on the fly ?
            if (! GrabInternetFile(sRemoteTPMFiles, sTPMZipFile)) {
                FatalError(IDS_CANT_GRAB_TPM_INTERNET, (LPCTSTR)sRemoteTPMFiles);
                return 1;
            }
            if (! UnzipFile(sTPMZipFile, g_sLocalSourceDir, NULL, true)) {
                FatalError(IDS_CANT_UNZIP_TPM, (LPCTSTR)sTPMZipFile);
                //            theLog("Error: can't unzip tpm files grabbed from the Internet.\n", (LPCTSTR)sTmp);
            }
            else {
                g_bRemoveSourceTPMDir = true;
                DeleteFile(sTPMZipFile);
            }
            // Pass through and the files will be unzipped !
        case SOURCE_ZIP:
            // CHANGE for TeXLive 8 : the expected structure has changed.
#if 0
            // We don't know the size !
            if (DirectoryExists(g_sSourceTPMDir)) {
                RecursivelyRemove(g_sSourceTPMDir);
            }
            if (! UnzipFile(sTPMZipFile, g_sLocalSourceDir, NULL, true)) {
                // Let's try somewhere else !
                g_sSourceTPMDir = ConcatPath(g_sTempPath, "texmf\\tpm");
                if (DirectoryExists(g_sSourceTPMDir)) {
                    RecursivelyRemove(g_sSourceTPMDir);
                }
                if (! UnzipFile(sTPMZipFile, g_sTempPath, NULL, true)) {
                    FatalError(IDS_CANT_UNZIP_TPM, (LPCTSTR)sTPMZipFile);
                    //            theLog("Error: can't unzip tpm files grabbed from the Internet.\n", (LPCTSTR)sTmp);
                }
                if (g_eSourceType == SOURCE_INTERNET)
                    DeleteFile(sTPMZipFile);
                return 1;
            }
            else {
                g_bRemoveSourceTPMDir = true;
            }
#endif
            // Pass through and the files will be parsed !
        case SOURCE_CDROM:
            g_tpmRoot = 0;
            InitializeTPM(g_sSourceTPMDir, g_vSourceTPM, &g_vSchemeTPM);
            if (g_eInstallType == INSTALL_MAINTENANCE) {
                if (!g_sPackage.IsEmpty() && !DirectoryExists(g_sLocalTPMDir)) {
                    // The user is trying to extract some package without previous installation
                    TPMMapCleanup(g_vInstalledTPM);
                }
                else {
                    InitializeTPM(g_sLocalTPMDir, g_vInstalledTPM);
                }
            }
            break;
        }
    }
    catch (...) {
        CSingleLock singleLock (&g_csCritSec, TRUE);
        theStatus.bError = true;
    }

    return (0);
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
