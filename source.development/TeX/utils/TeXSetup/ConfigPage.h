#if !defined(AFX_CONFIGPAGE_H__66F1397B_A2E0_460B_B1FD_EF834C4BB6C5__INCLUDED_)
#define AFX_CONFIGPAGE_H__66F1397B_A2E0_460B_B1FD_EF834C4BB6C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfigPage.h : header file
//

#include <afxmt.h>

/////////////////////////////////////////////////////////////////////////////
// CConfigPage dialog

class CConfigPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CConfigPage)

        // Construction
        public:
    CConfigPage();
    ~CConfigPage();

    // Dialog Data
    //{{AFX_DATA(CConfigPage)
    enum { IDD = IDD_PROPPAGE_CONFIG };
    // NOTE - ClassWizard will add data members here.
    //    DO NOT EDIT what you see in these blocks of generated code !
    //}}AFX_DATA


    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CConfigPage)
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
    // Generated message map functions
    //{{AFX_MSG(CConfigPage)
    virtual BOOL OnInitDialog();
    afx_msg void OnTimer(UINT nIDEvent);
    //}}AFX_MSG
    afx_msg LRESULT OnStartConfiguration (WPARAM wParam, LPARAM lParam);
    DECLARE_MESSAGE_MAP()

	void StopConfiguration();
    static UINT ConfigurationThread(LPVOID pParam);

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONFIGPAGE_H__66F1397B_A2E0_460B_B1FD_EF834C4BB6C5__INCLUDED_)
/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

    
