// FileCopyPage.h : header file
//

#ifndef __MYFILECOPYPAGE_H__
#define __MYFILECOPYPAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxmt.h>

/////////////////////////////////////////////////////////////////////////////
// CFileCopyPage dialog

class CFileCopyPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CFileCopyPage)

        // Construction
        public:
    CFileCopyPage();
    ~CFileCopyPage();

    // Dialog Data
    //{{AFX_DATA(CFileCopyPage)
	enum { IDD = IDD_PROPPAGE_FILE_COPY };
    CProgressCtrl	m_prgTotal;
    CProgressCtrl	m_prgPackage;
	CString	m_sPercentPackage;
	CString	m_sPercentTotal;
	CString	m_sPercentUnitPackage;
	CString	m_sPercentUnitTotal;
	CString	m_sInstalling;
	CString	m_sPackage;
	CString	m_sPrompt;
	CString	m_sHeader;
	CString	m_sInstallingPackage;
	CString	m_sTotal;
	//}}AFX_DATA


    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CFileCopyPage)
public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    virtual BOOL OnSetActive();
    virtual BOOL OnQueryCancel();
    virtual LRESULT OnWizardNext();
protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:
    CToolTipCtrl m_tooltip;
    CPropertyPage *m_pageNext;
    BOOL OnInitDialog();
    // Generated message map functions
    //{{AFX_MSG(CFileCopyPage)
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    afx_msg LRESULT OnStartFileCopy (WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

        // void StartFileCopy();
	void StopFileCopy();
    static UINT InstallThread(LPVOID pParam);
};



#endif // __FILECOPYPAGE_H__

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
