// ReviewPage.h : header file
//

#ifndef __REVIEWPAGE_H__
#define __REVIEWPAGE_H__

/////////////////////////////////////////////////////////////////////////////
// CReviewPage dialog

class CReviewPage : public CPropertyPage
{
    DECLARE_DYNCREATE(CReviewPage)

        private:
    void Initialize();

    // Construction
public:
    CReviewPage();
    ~CReviewPage();

    // Dialog Data
    //{{AFX_DATA(CReviewPage)
    enum { IDD = IDD_PROPPAGE_REVIEW };
    CString	m_sSummary;
    //}}AFX_DATA


    // Overrides
    // ClassWizard generate virtual function overrides
    //{{AFX_VIRTUAL(CReviewPage)
public:
    virtual BOOL OnSetActive();
    virtual LRESULT OnWizardNext();
    virtual LRESULT OnWizardBack();
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
    //{{AFX_MSG(CReviewPage)
    // NOTE: the ClassWizard will add member functions here
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

        };

#endif // __REVIEWPAGE_H__

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

