// TeXLive.h : main header file for the TEXLIVE application
//

#if !defined(AFX_TEXLIVE_H__1B1D9D48_6ED3_4BA0_BD59_70C35ADEF8A5__INCLUDED_)
#define AFX_TEXLIVE_H__1B1D9D48_6ED3_4BA0_BD59_70C35ADEF8A5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CTeXLiveApp:
// See TeXLive.cpp for the implementation of this class
//

class CTeXLiveApp : public CWinApp
{
public:
	CTeXLiveApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTeXLiveApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CTeXLiveApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXLIVE_H__1B1D9D48_6ED3_4BA0_BD59_70C35ADEF8A5__INCLUDED_)
