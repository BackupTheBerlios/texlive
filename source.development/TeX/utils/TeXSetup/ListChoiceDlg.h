#if !defined(AFX_LISTCHOICEDLG_H__27020AB4_E0FE_464A_9A78_2F399EE95BE8__INCLUDED_)
#define AFX_LISTCHOICEDLG_H__27020AB4_E0FE_464A_9A78_2F399EE95BE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListChoiceDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListChoiceDlg dialog

class CListChoiceDlg : public CDialog
{
// Construction
public:
	CListChoiceDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CListChoiceDlg)
	enum { IDD = IDD_DIALOG_LIST_CHOICE };
	CString	m_sListChoice;
	CString	m_sDescription;
	//}}AFX_DATA
	
	CString m_sTitle;
	CStringArray m_saListChoice;
	static UINT uiRadioButtons[];
	static bool bRadioButtonsValidated[];
	static CString sRadioButtonsLabels[];
	UINT nChoice;

	bool AddChoice(const CString &);
	bool SetCheck(UINT nCheck, CString sLabel, bool bValidate);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListChoiceDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CListChoiceDlg)
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnListchoiceRadio1();
	afx_msg void OnListchoiceRadio2();
	afx_msg void OnListchoiceRadio3();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTCHOICEDLG_H__27020AB4_E0FE_464A_9A78_2F399EE95BE8__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

