// RootPage.h : header file
//

#ifndef __ROOTPAGE_H__
#define __ROOTPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CRootPage dialog

class CRootPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CRootPage)

        // Construction
        public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    CRootPage();
    ~CRootPage();

    // Dialog Data
    //{{AFX_DATA(CRootPage)
    enum { IDD = IDD_PROPPAGE_ROOT };
    CString	m_sInstallRootDir;
    CString	m_sSpaceAvailable;
    CString	m_sSpaceNeeded;
    CString	m_sTexmfExtra;
    CString	m_sTexmfHome;
    CString	m_sTexmfLocal;
    CString	m_sVarTexmf;
    CString	m_sVarTexFonts;
    BOOL	m_bCustomDirs;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CRootPage)
public:
    virtual LRESULT OnWizardBack();
    virtual LRESULT OnWizardNext();
    virtual BOOL OnQueryCancel();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CString *m_psLastEdit;
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    void ValidateControls();
    void UpdatePaths();
    // Generated message map functions
    //{{AFX_MSG(CRootPage)
    afx_msg void OnBrowse();
    afx_msg void OnKillfocusRootdir();
    afx_msg void OnSetfocusExtraTexmf();
    afx_msg void OnSetfocusHomeTexmf();
    afx_msg void OnSetfocusLocalTexmf();
    afx_msg void OnSetfocusVarFonts();
    afx_msg void OnSetfocusVarTexmf();
    afx_msg void OnSetfocusRootdir();
    afx_msg void OnCheckCustomDirs();
    afx_msg void OnChangeRootdir();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

        };

#endif // __ROOTPAGE_H__

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

    
