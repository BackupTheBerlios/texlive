#if !defined(AFX_GETTPMPAGE_H__03D571C2_439D_4DE4_9DB7_F4284DE359DA__INCLUDED_)
#define AFX_GETTPMPAGE_H__03D571C2_439D_4DE4_9DB7_F4284DE359DA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetTPMPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetTPMPage dialog

class CGetTPMPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CGetTPMPage)

        // Construction
        public:
    CGetTPMPage();
    ~CGetTPMPage();

    // Dialog Data
    //{{AFX_DATA(CGetTPMPage)
	enum { IDD = IDD_PROPPAGE_GETTPM };
    CProgressCtrl	m_prgRead;
    CProgressCtrl	m_prgDownload;
    CString	m_sPrompt;
    CString	m_sDownloading;
    CString	m_sReading;
	CString	m_sTPMRead;
	CString	m_sTPMZip;
	CString	m_sPercentDownload;
	CString	m_sPercentUnit;
	//}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CGetTPMPage)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual LRESULT OnWizardNext();
    virtual BOOL OnSetActive();
    virtual BOOL OnQueryCancel();
    virtual void RemoveNextFromWizard();

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    // Generated message map functions
    //{{AFX_MSG(CGetTPMPage)
    // NOTE: the ClassWizard will add member functions here
    afx_msg void OnTimer(UINT nIDEvent);
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
        private:
    void InitCompleted();
    static UINT TPMThread (LPVOID pParam);

public:
	afx_msg void OnStnClickedPrompt();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETTPMPAGE_H__03D571C2_439D_4DE4_9DB7_F4284DE359DA__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
