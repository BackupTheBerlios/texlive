#if !defined(AFX_ACTIONPROGRESSDLG_H__31E89F6B_2495_4C6B_865A_296AFBBC82F9__INCLUDED_)
#define AFX_ACTIONPROGRESSDLG_H__31E89F6B_2495_4C6B_865A_296AFBBC82F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActionProgressDlg.h : header file
//

struct {
  CString sDescription;
  bool (__cdecl * pfnAction)();
} ActionsDesc;

/////////////////////////////////////////////////////////////////////////////
// CActionProgressDlg dialog

class CActionProgressDlg : public CDialog
{
// Construction
public:
	CActionProgressDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CActionProgressDlg)
	enum { IDD = IDD_ACTION_PROGRESS_DIALOG };
	CString	m_sDescription;
	//}}AFX_DATA
// Process
public:
	void SetDescription();
	void SetTitle();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CActionProgressDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CActionProgressDlg)
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIONPROGRESSDLG_H__31E89F6B_2495_4C6B_865A_296AFBBC82F9__INCLUDED_)
