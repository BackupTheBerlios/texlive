#if !defined(AFX_ERRORPAGE_H__EC6ECE44_2FDD_4EE6_946D_6479E7EC4266__INCLUDED_)
#define AFX_ERRORPAGE_H__EC6ECE44_2FDD_4EE6_946D_6479E7EC4266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ErrorPage.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CErrorPage dialog

class CErrorPage : public CPropertyPage
{
    // Construction
public:
    CErrorPage();   // standard constructor
    ~CErrorPage();

    // Dialog Data
    //{{AFX_DATA(CErrorPage)
    enum { IDD = IDD_PROPPAGE_ERROR };
    BOOL	m_bViewLogFile;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CErrorPage)
public:
    virtual BOOL OnSetActive();
    virtual BOOL OnWizardFinish();
    virtual BOOL PreTranslateMessage(MSG* pMsg);

protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

    // Implementation
protected:

    CPropertyPage *m_pageNext;
    CToolTipCtrl m_tooltip;

    // Generated message map functions
    //{{AFX_MSG(CErrorPage)
    virtual BOOL OnInitDialog();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
        };

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ERRORPAGE_H__EC6ECE44_2FDD_4EE6_946D_6479E7EC4266__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

