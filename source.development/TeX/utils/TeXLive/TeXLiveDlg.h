// TeXLiveDlg.h : header file
//

#if !defined(AFX_TEXLIVEDLG_H__8CF846F3_72D0_43D7_A831_F6752D850644__INCLUDED_)
#define AFX_TEXLIVEDLG_H__8CF846F3_72D0_43D7_A831_F6752D850644__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorStaticST.h"

/////////////////////////////////////////////////////////////////////////////
// CTeXLiveDlg dialog

class CTeXLiveDlg : public CDialog
{
// Construction
public:
	CTeXLiveDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CTeXLiveDlg)
	enum { IDD = IDD_TEXLIVE_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTeXLiveDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
        CBitmap m_InternetBitmap;

        CString BrowseForFolder(const CString &sInitialDir);
        void TeXSetupAddPackage(const CString &sPackage, const CString &sInstallationDir = "");

	// Generated message map functions
	//{{AFX_MSG(CTeXLiveDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
        afx_msg void OnHelp();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRunTexsetup();
	afx_msg void OnTexliveUninstall();
	afx_msg void OnEditorsNTEmacs();
	afx_msg void OnEditorsTeXnicCenter();
	afx_msg void OnEditorsWinEdt();
	afx_msg void OnEditorsWinShell();
	afx_msg void OnEditorsXEmacs();
	afx_msg void OnSupportFreeGhostscript();
	afx_msg void OnSupportGnuTools();
	afx_msg void OnSupportImageMagick();
	afx_msg void OnSupportIspell();
	afx_msg void OnSupportNetPbm();
	afx_msg void OnSupportPerl();
	afx_msg void OnRunTexliveDoc();
	afx_msg void OnRunTeXDocTK();
	afx_msg void OnGotoFptex();
	afx_msg void OnGotoTug();
	afx_msg void OnRunTexCdrom();
	afx_msg void OnBrowseCdrom();
	afx_msg void OnCdromCleanup();
	afx_msg void OnTexliveMaintenance();
	afx_msg void OnSelectEditor();
	afx_msg void OnInternetAccess();
	afx_msg void OnUpgradeDlls();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	CColorStaticST	m_stcInstallation;
	CColorStaticST	m_stcDocumentation;
	CColorStaticST	m_stcWelcome;
        CFont m_fntStatic;
        CFont m_fntWelcome;
    CMenu m_Menu;;

    void EnableInternetMenuItem(bool bEnable);
    void EnableRunOffCDMenuItem(bool bEnable);
public:
	afx_msg void OnStnClickedStaticWelcomeTexlive();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TEXLIVEDLG_H__8CF846F3_72D0_43D7_A831_F6752D850644__INCLUDED_)

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
