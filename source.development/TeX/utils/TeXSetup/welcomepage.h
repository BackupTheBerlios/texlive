// WelcomePage.h : header file
//

#ifndef __WELCOMEPAGE_H__
#define __WELCOMEPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CWelcomePage dialog

class CWelcomePage : public CPropertyPage
{
    DECLARE_DYNCREATE(CWelcomePage)

        // Construction
        public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    CWelcomePage();
    ~CWelcomePage();

    // Dialog Data
    //{{AFX_DATA(CWelcomePage)
	enum { IDD = IDD_PROPPAGE_WELCOME };
	CButton	m_btnAdmin;
	CButton	m_btnQuickInstall;
	CButton	m_btnXEmTeX;
    BOOL	m_default_install;
    BOOL	m_bQuickInstall;
    BOOL	m_bXEmTeX;
    CString	m_sLicense;
    BOOL	m_bAllUsers;
	CString	m_sExplanation;
	//}}AFX_DATA

    BOOL    m_bInitialized;

    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CWelcomePage)
public:
    virtual LRESULT OnWizardNext();
    virtual BOOL OnSetActive();
    virtual BOOL OnQueryCancel();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    virtual BOOL OnInitDialog();
    // Generated message map functions
    //{{AFX_MSG(CWelcomePage)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

        };

#endif // __WELCOMEPAGE_H__

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

    
