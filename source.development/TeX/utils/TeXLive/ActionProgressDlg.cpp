// ActionProgressDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeXLive.h"
#include "ActionProgressDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT nIDTimer = 314;

UINT CActionProgressDlg::iCurrentAction = 0;
UINT CActionProgressDlg::iMaxActions = 0;
ActionsDesc CActionProgressDlg::theActions[MAX_ACTIONS];

CCriticalSection g_csCritSec;

/////////////////////////////////////////////////////////////////////////////
// CActionProgressDlg dialog


CActionProgressDlg::CActionProgressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CActionProgressDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CActionProgressDlg)
	m_sDescription = _T("");
	//}}AFX_DATA_INIT
}


void CActionProgressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActionProgressDlg)
	DDX_Text(pDX, IDC_STATIC_ACTION, m_sDescription);
	//}}AFX_DATA_MAP
}

#define WM_STARTACTION (WM_USER + 103)
#define WM_NEXTACTION (WM_USER + 104)

BEGIN_MESSAGE_MAP(CActionProgressDlg, CDialog)
	//{{AFX_MSG_MAP(CActionProgressDlg)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
  //    ON_MESSAGE(WM_STARTACTION, OnStartAction)
    ON_MESSAGE(WM_NEXTACTION, OnNextAction)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActionProgressDlg message handlers


void CActionProgressDlg::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CDialog::OnTimer(nIDEvent);
}

BOOL CActionProgressDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CActionProgressDlg::AddAction(CString sDescription, pfnAction theAction, LPVOID pParam)
{
  theActions[iMaxActions].sDescription = sDescription;
  theActions[iMaxActions].theAction = theAction;
  theActions[iMaxActions].theParam = pParam;
  iMaxActions++;
}

void CActionProgressDlg::StartActions()
{
  AfxGetApp()->DoWaitCursor(1);
  //  PostMessage(WM_NEXTACTION);
  SendMessage(WM_NEXTACTION);
}

LRESULT CActionProgressDlg::OnNextAction (WPARAM wParam, LPARAM lParam)
{
  if (iCurrentAction < iMaxActions) {
      m_sDescription = theActions[iCurrentAction].sDescription;
      UpdateData(FALSE);
    // start the worker thread
      CWinThread *myThread = AfxBeginThread ((AFX_THREADPROC)ActionThread, 0);
    // start the timer
    //    SetTimer (nIDTimer, 100, 0);
      if (myThread)
          WaitForSingleObject(myThread->m_hThread, INFINITE);
      // Protect access to shared resource
      {
          CSingleLock theLock(&g_csCritSec);
          iCurrentAction++;
      }
      // PostMessage(WM_NEXTACTION);
      SendMessage(WM_NEXTACTION);
  }
  else {
    PostMessage(WM_COMMAND, IDOK, 0);
    AfxGetApp()->DoWaitCursor(-1);
  }
  return (0);
}

UINT CActionProgressDlg::ActionThread (LPVOID pParam)
{
    //    CoInitialize (0);
    UINT n;
    // Protect access to shared resource
    {
        CSingleLock theLock(&g_csCritSec);
        n = iCurrentAction;
    }

    try {
        Sleep(100);
        (theActions[n].theAction)(theActions[n].theParam);
    }
    catch (...) {
      CString sErrorMsg;
      sErrorMsg.Format("Error in executing command :\r\n%s", theActions[n].sDescription);
      AfxMessageBox(sErrorMsg);
    }
    //    CoUninitialize ();
    return (0);
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
