#if !defined(AFX_SOURCEPAGE1_H__0FE82C11_DFE8_4FF0_AE31_DBED68CAF66D__INCLUDED_)
#define AFX_SOURCEPAGE1_H__0FE82C11_DFE8_4FF0_AE31_DBED68CAF66D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SourcePage1.h : header file
//
#include "geturl.h"
#include "TeXInstall.h"

/////////////////////////////////////////////////////////////////////////////
// CSourcePage dialog

class CSourcePage : public CPropertyPage
{
    DECLARE_DYNCREATE(CSourcePage)

        // Construction
        public:
    CSourcePage();
    ~CSourcePage();

    // Dialog Data
    //{{AFX_DATA(CSourcePage)
    enum { IDD = IDD_PROPPAGE_SOURCE };
    BOOL	m_bNetAccess;
    CString	m_sProxyAddress;
    UINT	m_uiProxyPort;
    CString	m_sRemoteSourceDir;
    CString	m_sLocalSourceDir;
    //}}AFX_DATA

    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CSourcePage)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual LRESULT OnWizardNext();
    virtual BOOL OnSetActive();
    virtual BOOL OnQueryCancel();
    virtual LRESULT OnWizardBack();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    virtual BOOL OnInitDialog();
    SourceType m_uiSourceType;
    // Generated message map functions
    //{{AFX_MSG(CSourcePage)
    afx_msg void OnRadioSourceCdrom();
    afx_msg void OnRadioSourceInternet();
    afx_msg void OnRadioSourceZip();
    afx_msg void OnInternetConnectionIe5();
    afx_msg void OnInternetConnectionDirect();
    afx_msg void OnInternetConnectionProxy();
    afx_msg void OnBrowseLocal();
    afx_msg void OnBrowseRemote();
    afx_msg void OnCheckNetdownload();
    //}}AFX_MSG
    // afx_msg LRESULT OnStartTPMInit (WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

        private:
    BOOL m_bInitialized;
    NetIOType m_uiNetMethod;
	
    static HANDLE m_hEventDone;

    void InitCompleted();
    static UINT TPMThread(LPVOID pParam);

    void ExportData();
    void ImportData();
    void ValidateControls();

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SOURCEPAGE1_H__0FE82C11_DFE8_4FF0_AE31_DBED68CAF66D__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

    
