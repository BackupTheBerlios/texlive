// PackagesPage.h : header file
//

#ifndef __PACKAGESPAGE_H__
#define __PACKAGESPAGE_H__

#include "TreePackage.h"

/////////////////////////////////////////////////////////////////////////////
// CPackagesPage dialog

class CPackagesPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CPackagesPage)

        // Construction
        public:
    CPackagesPage();
    ~CPackagesPage();
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    void UpdateSizes(DWORD size);

    // Dialog Data
    //{{AFX_DATA(CPackagesPage)
    enum { IDD = IDD_PROPPAGE_PACKAGES };
    CTreePackage	m_cPackages;
    CString	m_sSpaceAvailable;
    CString	m_sPackageSize;
    CString	m_sTotalSize;
    CString	m_sDescription;
    BOOL	m_bInstallDocumentation;
    BOOL	m_bInstallSource;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CPackagesPage)
public:
    virtual LRESULT OnWizardBack();
    virtual LRESULT OnWizardNext();
    virtual BOOL OnQueryCancel();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    virtual BOOL OnInitDialog();
    virtual BOOL OnSetActive();
    // Generated message map functions
    //{{AFX_MSG(CPackagesPage)
    afx_msg void OnCheckDoc();
    afx_msg void OnCheckSource();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

        };

#endif // __PACKAGESPAGE_H__

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

    
