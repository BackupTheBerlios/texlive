// SchemePackage.h: header file
//

#pragma once
#include "afxwin.h"


/////////////////////////////////////////////////////////////////////////////
// CSchemePage dialog

class CSchemePage : public CPropertyPage
{
	DECLARE_DYNAMIC(CSchemePage)

public:
	CSchemePage();
    virtual ~CSchemePage();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // Dialog Data
    //{{AFX_DATA(CPackagesPage)
    enum { IDD = IDD_PROPPAGE_SCHEME };
    CListBox m_ctlListScheme;
    CString m_sSchemeDescription;
    BOOL m_bCustomizeScheme;
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
    void UpdateInfo();
    // Generated message map functions
    //{{AFX_MSG(CPackagesPage)
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()


public:
	afx_msg void OnBnClickedCheckCustomizeScheme();
	afx_msg void OnLbnSelchangeListScheme();
	CString m_sTotalSize;
	CString m_sSpaceAvailable;
};

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
