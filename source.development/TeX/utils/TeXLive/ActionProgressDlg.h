#if !defined(AFX_ACTIONPROGRESSDLG_H__31E89F6B_2495_4C6B_865A_296AFBBC82F9__INCLUDED_)
#define AFX_ACTIONPROGRESSDLG_H__31E89F6B_2495_4C6B_865A_296AFBBC82F9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ActionProgressDlg.h : header file
//

typedef bool (__cdecl * pfnAction)(LPVOID pParam);

struct ActionsDesc {
    CString sDescription;
    pfnAction theAction;
    LPVOID theParam;
};

#define MAX_ACTIONS 64

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
    void AddAction(CString sDesc, pfnAction theAction, LPVOID pParam = 0);
    void StartActions();

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
    //	afx_msg LRESULT OnStartAction (WPARAM wParam, LPARAM lParam);
    afx_msg LRESULT OnNextAction (WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

	static UINT iCurrentAction;
    static UINT iMaxActions;
    static ActionsDesc theActions[MAX_ACTIONS];

    static UINT CActionProgressDlg::ActionThread (LPVOID pParam);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ACTIONPROGRESSDLG_H__31E89F6B_2495_4C6B_865A_296AFBBC82F9__INCLUDED_)
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
