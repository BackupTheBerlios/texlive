// TeXLive.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "TeXLive.h"
#include "TeXLiveDlg.h"
#include "TeXLiveCmds.h"

#include "LimitSingleInstance.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// The one and only CLimitSingleInstance object
CLimitSingleInstance g_SingleInstanceObj(TEXT("TeXLive 7.0 - TeXLive"));

/////////////////////////////////////////////////////////////////////////////
// CTeXLiveApp

BEGIN_MESSAGE_MAP(CTeXLiveApp, CWinApp)
    //{{AFX_MSG_MAP(CTeXLiveApp)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    //    DO NOT EDIT what you see in these blocks of generated code!
    //}}AFX_MSG
    //    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
// CTeXLiveApp construction

CTeXLiveApp::CTeXLiveApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
    EnableHtmlHelp();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CTeXLiveApp object

CTeXLiveApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTeXLiveApp initialization

BOOL CTeXLiveApp::InitInstance()
{
    // Quit if we are already running
    if (g_SingleInstanceObj.IsAnotherInstanceRunning())
       return FALSE; 

    // InitCommonControls() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    InitCommonControls();
    
    CWinApp::InitInstance();
    
    if (!AfxSocketInit()) {
        AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
        return FALSE;
    }

    AfxEnableControlContainer();

    // Initialize a few paths
    Initialize();

    // Create an invisible window of the registered class.
    // Now the autorun will be able to detect than another
    // occurence of this program is running.
    CWnd wndBack;
    RECT rDesktop;
    GetWindowRect(::GetDesktopWindow(), &rDesktop);
	
    wndBack.Create("TeXLive", 
                   "TeXLive Background",
                   WS_DISABLED,
                   rDesktop, 
                   m_pMainWnd->GetDesktopWindow(), 
                   7253);

    CTeXLiveDlg dlg;
    m_pMainWnd = &dlg;
    int nResponse = dlg.DoModal();
    if (nResponse == IDOK) {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with OK
    }
    else if (nResponse == IDCANCEL) {
            // TODO: Place code here to handle when the dialog is
            //  dismissed with Cancel
    }
    
    // wndBack.DestroyWindow();

    // Since the dialog has been closed, return FALSE so that we exit the
    //  application, rather than start the application's message pump.
    return FALSE;
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
