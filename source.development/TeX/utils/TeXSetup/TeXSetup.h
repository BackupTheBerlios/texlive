// TeXSetup.h : main header file for the TEXSETUP application
//

#if !defined(AFX_TEXSETUP_H__B1C471BC_DBE8_11D2_8AA6_000000000000__INCLUDED_)
#define AFX_TEXSETUP_H__B1C471BC_DBE8_11D2_8AA6_000000000000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

extern CString g_sCaption;

/////////////////////////////////////////////////////////////////////////////
// Command Line
class CTeXSetupCommandLineInfo;

/////////////////////////////////////////////////////////////////////////////
// CTeXSetupApp:
// See TeXSetup.cpp for the implementation of this class
//

class CTeXSetupApp : public CWinApp
{
public:
    void OnWizard();
    CTeXSetupApp();

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTeXSetupApp)
public:
    virtual BOOL InitInstance();
    virtual void WinHelp(DWORD dwData, UINT nCmd = HELP_CONTEXT);
    //}}AFX_VIRTUAL

    // Implementation

    //{{AFX_MSG(CTeXSetupApp)
    // NOTE - the ClassWizard will add and remove member functions here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
        };


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXSETUP_H__B1C471BC_DBE8_11D2_8AA6_000000000000__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

