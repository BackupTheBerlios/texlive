#if !defined(AFX_REBOOTPAGE_H__EE6171BB_D7D3_4EC5_81ED_11065448015C__INCLUDED_)
#define AFX_REBOOTPAGE_H__EE6171BB_D7D3_4EC5_81ED_11065448015C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// RebootPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CRebootPage dialog

class CRebootPage : public CPropertyPage
{
    // Construction
public:
    CRebootPage();   // standard constructor

    // Dialog Data
    //{{AFX_DATA(CRebootPage)
	enum { IDD = IDD_PROPPAGE_REBOOT };
    BOOL	m_bViewLog;
    BOOL	m_bViewDoc;
    BOOL	m_bReboot;
	CString	m_sMsg;
	CString	m_sReboot;
	//}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CRebootPage)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnSetActive();
    virtual BOOL OnWizardFinish();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    virtual BOOL OnInitDialog();
    // Generated message map functions
    //{{AFX_MSG(CRebootPage)
    afx_msg void OnCheckReboot();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

        private:
    bool Reboot();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REBOOTPAGE_H__EE6171BB_D7D3_4EC5_81ED_11065448015C__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

