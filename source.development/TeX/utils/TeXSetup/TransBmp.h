#if !defined(AFX_TRANSBMP_H__A117D240_76B9_4271_B3F7_872F5182928D__INCLUDED_)
#define AFX_TRANSBMP_H__A117D240_76B9_4271_B3F7_872F5182928D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TransBmp.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CTransBmp window

class CTransBmp : public CBitmap
{
// Construction
public:
	CTransBmp();
    ~CTransBmp();
    void Draw(HDC hDC, int x, int y);
    void Draw(CDC* pDC, int x, int y);
    void DrawTrans(HDC hDC, int x, int y);
    void DrawTrans(CDC* pDC, int x, int y);
    int GetWidth();
    int GetHeight();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTransBmp)
	//}}AFX_VIRTUAL

	// Generated message map functions
protected:
	//{{AFX_MSG(CTransBmp)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	//DECLARE_MESSAGE_MAP()

private:
    int m_iWidth;
    int m_iHeight;
    HBITMAP m_hbmMask;    // Handle to mask bitmap

    void GetMetrics();
    void CreateMask(HDC hDC);

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRANSBMP_H__A117D240_76B9_4271_B3F7_872F5182928D__INCLUDED_)


/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
