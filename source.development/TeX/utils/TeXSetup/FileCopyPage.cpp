// FileCopyPage.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "TeXInstall.h"
#include "TeXSetup.h"
#include "TeXSetupWizard.h"
#include "FileCopyPage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

const UINT nIDTimer = 314;

extern CTeXSetupApp theApp;

IMPLEMENT_DYNCREATE(CFileCopyPage, CPropertyPage)

    /////////////////////////////////////////////////////////////////////////////
// CFileCopyPage property page

CFileCopyPage::CFileCopyPage() : CPropertyPage(CFileCopyPage::IDD)
{
    //{{AFX_DATA_INIT(CFileCopyPage)
	m_sPercentPackage = _T("");
	m_sPercentTotal = _T("");
	m_sPercentUnitPackage = _T("");
	m_sPercentUnitTotal = _T("");
	m_sInstalling = _T("");
	m_sPackage = _T("");
	m_sPrompt = _T("TeXSetup will now install the selected packages:");
	m_sHeader = _T("File Copy Page");
	m_sInstallingPackage = _T("");
	m_sTotal = _T("");
	//}}AFX_DATA_INIT

    m_pageNext = 0;
}

CFileCopyPage::~CFileCopyPage()
{
}

void CFileCopyPage::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CFileCopyPage)
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS_TOTAL, m_prgTotal);
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS_PACKAGE, m_prgPackage);
	DDX_Text(pDX, IDC_STATIC_PERCENT_PACKAGE, m_sPercentPackage);
	DDX_Text(pDX, IDC_STATIC_PERCENT_TOTAL, m_sPercentTotal);
	DDX_Text(pDX, IDC_STATIC_PERCENT_UNIT, m_sPercentUnitPackage);
	DDX_Text(pDX, IDC_STATIC_PERCENT_UNIT2, m_sPercentUnitTotal);
	DDX_Text(pDX, IDC_STATIC_INSTALLING, m_sInstalling);
	DDX_Text(pDX, IDC_STATIC_PACKAGE, m_sPackage);
	DDX_Text(pDX, IDC_PROMPT, m_sPrompt);
	DDX_Text(pDX, IDC_STATIC_HEADER, m_sHeader);
	DDX_Text(pDX, IDC_STATIC_INSTALLING_PACKAGE, m_sInstallingPackage);
	DDX_Text(pDX, IDC_STATIC_TOTAL, m_sTotal);
	//}}AFX_DATA_MAP
}

#define WM_STARTFILECOPY (WM_USER + 100)
#define WM_NEXTPAGE (WM_USER + 107)

BEGIN_MESSAGE_MAP(CFileCopyPage, CPropertyPage)
    //{{AFX_MSG_MAP(CFileCopyPage)
    // NOTE: the ClassWizard will add message map macros here
    ON_WM_TIMER()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_STARTFILECOPY, OnStartFileCopy)
    END_MESSAGE_MAP()

    BOOL CFileCopyPage::OnInitDialog ()
{
    CWnd * wnd = GetDlgItem(IDC_STATIC_HEADER);
    ASSERT (wnd != 0);
    extern CFont fntHeader;
    wnd->SetFont (&fntHeader);
    SetWindowText(g_sCaption);
    return CPropertyPage::OnInitDialog();
}

BOOL CFileCopyPage::PreTranslateMessage(MSG* pMsg)
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

LRESULT CFileCopyPage::OnStartFileCopy (WPARAM wParam, LPARAM lParam)
{
    // FIXME: check that wininit.ini does not yet exist
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, FALSE};

    // initialize total progress bar
    m_prgTotal.SetPos (0);
    m_sInstallingPackage = "Package:";
    m_sTotal = "Global:";

    UpdateData(FALSE);

    // start the worker thread
    AfxBeginThread ((AFX_THREADPROC)InstallThread, 0,
                    /* nPriority */ THREAD_PRIORITY_NORMAL,
                    /* nStackSize */ 0,
                    /* dwCreateFlags */ 0,
                    /* lpSecurityAttrs */ &sa);

    // start the timer
    SetTimer (nIDTimer, 100, 0);

    return (0);
}

BOOL CFileCopyPage::OnSetActive ()
{
    CPropertySheet *sheet = DYNAMIC_DOWNCAST(CPropertySheet, GetParent());
    if (sheet != 0) {
        sheet->SetWizardButtons (0);
        if (m_pageNext != 0) {
            sheet->RemovePage(m_pageNext);
            m_pageNext = 0;
        }
    }
    CSingleLock singleLock(&g_csCritSec);
    theStatus.bError = theStatus.bReady = theStatus.bTerminate = false;
    PostMessage (WM_STARTFILECOPY);
    return (TRUE);
}

BOOL CFileCopyPage::OnQueryCancel ()
{
    CString sMsg, sCaption("Cancel");
    sMsg.LoadString(IDS_CANCEL_SETUP);

    theLog ("\n<<<Cancel? ");
    //  BOOL ret = MessageBox(IDS_CANCEL_SETUP, MB_YESNO | MB_ICONQUESTION) == IDYES);
    BOOL bRet = (::MessageBox(NULL, (LPCTSTR)sMsg, 
                              (LPCTSTR)sCaption, MB_YESNO | MB_ICONQUESTION) == IDYES);
    if (bRet) {
        theLog ("Yes!>>>\n");
        SetCancelFlag (true);
    }
    else {
        theLog ("No!>>>\n");
    }
    return bRet;
}

void CFileCopyPage::OnTimer(UINT nIDEvent) 
{
    static UINT nOpId = 0;

    CSingleLock singleLock (&g_csCritSec, TRUE);

    if (theStatus.bError || theStatus.bReady) {
        StopFileCopy ();
    }
    else {
        if (theStatus.bNewTPM) {
            m_sPackage = theStatus.tpm->GetName();
            m_sPercentUnitPackage.Empty();
            m_sPercentPackage.Empty();
            theStatus.bNewTPM = false;
        }

        // FIXME: a negative dwPackageSize should indicate that we are
        // dowloading from a ftp site, and that only the size should be
        // printed to the right of the progress bar, and the progress bar
        // should stay to 0.

        char tmp[64];

        if (theStatus.dwPackageSize == -1) {
            m_prgPackage.SetPos (0);
            wsprintf(tmp, "%ld", (int)(theStatus.dwPackageDone / 1024));
            m_sPercentPackage = tmp;
            m_sPercentUnitPackage = "Kb";
        }
        else {
            int iLocalRatio = (int)(((double)theStatus.dwPackageDone / (double)theStatus.dwPackageSize) * 100.0);
            int iGlobalRatio = (int)(((double)theStatus.dwOverallDone / (double)g_dwSpaceNeeded) * 100.0);
            iLocalRatio = (iLocalRatio < 100 ? iLocalRatio : 100);
            iGlobalRatio = (iGlobalRatio < 100 ? iGlobalRatio : 100);
            m_prgPackage.SetPos (iLocalRatio);
            m_prgTotal.SetPos (iGlobalRatio);

            wsprintf(tmp, "%ld", iLocalRatio);
            m_sPercentPackage = tmp;
            m_sPercentUnitPackage = "%";

        }

        if (nOpId != theStatus.nOpId) {
            nOpId = theStatus.nOpId;
            m_sInstalling.LoadString(nOpId);
        }
    }

    UpdateData(FALSE);

    CPropertyPage::OnTimer (nIDEvent);
}

UINT CFileCopyPage::InstallThread (LPVOID pParam)
{
    CoInitialize (0);
    try {
        DoPrepareInstallation();

        if (g_eSourceType == SOURCE_INTERNET) {
            DoDownloadFiles();
        }

        // FIXME : ask if we should go on.

        DoPrepareCopying();

        DoCopyFiles();
    }
    catch (...) {
        CSingleLock singleLock (&g_csCritSec, TRUE);
        theStatus.bError = true;
    }
    CoUninitialize ();
    return (0);
}

void CFileCopyPage::StopFileCopy ()
{
    CSingleLock singleLock (&g_csCritSec, TRUE);
    KillTimer (nIDTimer);
    theStatus.bTerminate = true;

    // Notify user
    if (theStatus.bError)
        m_sPrompt.FormatMessage (IDS_FILECOPY_ERROR, (const char *) theLog.GetName());
    else
        m_sPrompt.FormatMessage (IDS_FILECOPY_COMPLETED, (const char *) theLog.GetName());
  
    // Disable progress bars
    m_sInstalling.Empty();
    m_sInstallingPackage.Empty();
    m_sPackage.Empty();
    m_prgPackage.SetPos (0);
    m_prgPackage.EnableWindow (FALSE);
    m_sTotal.Empty();
    m_prgTotal.SetPos (0);
    m_prgTotal.EnableWindow (FALSE);
    m_sPercentPackage.Empty();
    m_sPercentUnitPackage.Empty();
    m_sPercentTotal.Empty();
    m_sPercentUnitTotal.Empty();

    UpdateData(FALSE);

    CTeXSetupWizard *pParent = reinterpret_cast<CTeXSetupWizard *>(GetParent());
  
    if (theStatus.bError) {
        if (pParent != 0) {
            pParent->AddPage (pParent->m_pageError);
            pParent->SetActivePage (pParent->m_pageError);
            if (g_bQuickInstall) {
                PostMessage(WM_NEXTPAGE);
                // pParent->PressButton(PSBTN_NEXT);
            }
        }
    }
    else {
        // Enable Next button, assume bReady
        if (pParent != 0) {
            pParent->SetWizardButtons (PSWIZB_NEXT);
            if (m_pageNext == 0) {
                m_pageNext = pParent->m_pageConfiguration;
                pParent->AddPage(m_pageNext);
            }
      
            if (g_bQuickInstall) {
                PostMessage(WM_NEXTPAGE);
                // pParent->PressButton(PSBTN_NEXT);
            }
        }
    }
}

LRESULT CFileCopyPage::OnWizardNext ()
{

    if (m_pageNext == 0) {
        CTeXSetupWizard *pParent = reinterpret_cast<CTeXSetupWizard *>(GetParent());
        m_pageNext = pParent->m_pageConfiguration;
        pParent->AddPage(m_pageNext);
    }
    return CPropertyPage::OnWizardNext();
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

