#if !defined(AFX_UNINSTALLPAGE_H__FDB5A43D_FCE8_4EC4_B857_FF887BDD99BE__INCLUDED_)
#define AFX_UNINSTALLPAGE_H__FDB5A43D_FCE8_4EC4_B857_FF887BDD99BE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UninstallPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUninstallPage dialog

class CUninstallPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CUninstallPage)

        // Construction
        public:
    CUninstallPage();
    ~CUninstallPage();

    // Dialog Data
    //{{AFX_DATA(CUninstallPage)
    enum { IDD = IDD_PROPPAGE_UNINSTALL };
    BOOL	m_bViewLog;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CUninstallPage)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnWizardFinish();
    virtual BOOL OnSetActive();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    // Generated message map functions
    //{{AFX_MSG(CUninstallPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    afx_msg LRESULT OnStartUninstall (WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

	void StopUninstall();
    static UINT UninstallThread(LPVOID pParam);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNINSTALLPAGE_H__FDB5A43D_FCE8_4EC4_B857_FF887BDD99BE__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

    
