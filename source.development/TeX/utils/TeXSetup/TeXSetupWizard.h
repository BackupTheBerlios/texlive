// TeXSetupWizard.h : header file
//
// This class defines custom modal property sheet 
// CTeXSetupWizard.
// CTeXSetupWizard has been customized to include
// a preview window.
 
#ifndef __TEXSETUPWIZARD_H__
#define __TEXSETUPWIZARD_H__

#include <afxtempl.h>
#include "WelcomePage.h"
#include "RootPage.h"
#include "SourcePage.h"
#include "PackagesPage.h"
#include "ReviewPage.h"
#include "FileCopyPage.h"
#include "ConfigPage.h"
#include "RebootPage.h"
#include "ErrorPage.h"
#include "UninstallPage.h"
#include "SourcePage.h"
#include "GetTPMPage.h"
#include "SchemePage.h"

extern CFont fntWelcome;
extern CFont fntHeader;

/////////////////////////////////////////////////////////////////////////////
// CTeXSetupWizard

class CTeXSetupWizard : public CPropertySheet
{
    DECLARE_DYNAMIC(CTeXSetupWizard)

        // Construction
        public:
    CTeXSetupWizard(CWnd* pWndParent = NULL);

    // Attributes
public:
    CWelcomePage *m_pageWelcome;
    CRootPage *m_pageRoot;
    CPackagesPage *m_pagePackages;
    CReviewPage *m_pageReview;
    CFileCopyPage *m_pageFileCopy;
    CConfigPage *m_pageConfiguration;
    CRebootPage *m_pageReboot;
    CErrorPage *m_pageError;
    CUninstallPage *m_pageUninstall;
    CSourcePage *m_pageSource;
    CGetTPMPage *m_pageGetTPM;
    CSchemePage *m_pageScheme;

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTeXSetupWizard)
    //}}AFX_VIRTUAL

    // Implementation
public:
    int m_nPages;
    virtual ~CTeXSetupWizard();
    virtual BOOL OnInitDialog();

    // Generated message map functions
protected:
	HICON m_hIcon;

    //{{AFX_MSG(CTeXSetupWizard)
    // NOTE - the ClassWizard will add and remove member functions here.
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnHelp();
	afx_msg HCURSOR OnQueryDragIcon();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

#endif	// __TEXSETUPWIZARD_H__

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

    
