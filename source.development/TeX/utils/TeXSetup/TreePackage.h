#if !defined(AFX_TREEPACKAGE_H__389CA1DA_5FF4_4920_B60B_9D7B07BC5E78__INCLUDED_)
#define AFX_TREEPACKAGE_H__389CA1DA_5FF4_4920_B60B_9D7B07BC5E78__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TreePackage.h : header file
//

#include <afx.h>
#include "TPM.h"

/////////////////////////////////////////////////////////////////////////////
// CTreePackage window

class CTreePackage : public CTreeCtrl
{
    // Construction
public:
    CTreePackage();
    DECLARE_DYNCREATE(CTreePackage)

        // Attributes
        public:

    // Operations
public:

    // Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CTreePackage)
    //}}AFX_VIRTUAL

    // Implementation
private:
    void SwitchState(const HTREEITEM);
    // void IncrementState(const HTREEITEM);
    void UpdateParent(HTREEITEM hItem);
    void Insert(const HTREEITEM hFather, TPM *t);

public:
    virtual ~CTreePackage();
    void Initialize();
    void UpdateStateImage(const HTREEITEM);
    CImageList m_cImageList;

    // Generated message map functions
protected:
    //{{AFX_MSG(CTreePackage)
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnKeydown(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnPaint();
	//}}AFX_MSG

    DECLARE_MESSAGE_MAP()
        };

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TREEPACKAGE_H__389CA1DA_5FF4_4920_B60B_9D7B07BC5E78__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

