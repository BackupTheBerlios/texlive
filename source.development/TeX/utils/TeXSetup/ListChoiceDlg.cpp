// ListChoiceDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TeXSetup.h"
#include "ListChoiceDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

UINT CListChoiceDlg::uiRadioButtons[] = {
    0,
    IDC_LISTCHOICE_RADIO1,
    IDC_LISTCHOICE_RADIO2,
    IDC_LISTCHOICE_RADIO3,
    0
};

CString CListChoiceDlg::sRadioButtonsLabels[] = {
    "",
    "Radio1",
    "Radio2",
    "Radio3",
    ""
};

bool CListChoiceDlg::bRadioButtonsValidated[] = {
    false,
    true,
    true,
    true,
    false
};

/////////////////////////////////////////////////////////////////////////////
// CListChoiceDlg dialog


CListChoiceDlg::CListChoiceDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CListChoiceDlg::IDD, pParent)
{
    //{{AFX_DATA_INIT(CListChoiceDlg)
    m_sListChoice = _T("");
    m_sDescription = _T("");
    //}}AFX_DATA_INIT
}


void CListChoiceDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CListChoiceDlg)
    DDX_CBString(pDX, IDC_COMBO_LIST_CHOICE, m_sListChoice);
    DDX_Text(pDX, IDC_STATIC_DESCRIPTION, m_sDescription);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CListChoiceDlg, CDialog)
    //{{AFX_MSG_MAP(CListChoiceDlg)
    ON_BN_CLICKED(IDC_LISTCHOICE_RADIO1, OnListchoiceRadio1)
    ON_BN_CLICKED(IDC_LISTCHOICE_RADIO2, OnListchoiceRadio2)
    ON_BN_CLICKED(IDC_LISTCHOICE_RADIO3, OnListchoiceRadio3)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListChoiceDlg message handlers

void CListChoiceDlg::OnOK() 
{
    // TODO: Add extra validation here
    UpdateData(TRUE);
    CDialog::OnOK();
}

void CListChoiceDlg::OnCancel() 
{
    // TODO: Add extra cleanup here
	
    CDialog::OnCancel();
}

BOOL CListChoiceDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
	
    // TODO: Add extra initialization here
    int i;
    SetWindowText(m_sTitle);

    CComboBox *lbComboChoice = (CComboBox *)GetDlgItem(IDC_COMBO_LIST_CHOICE);
    if (lbComboChoice) {
        for (i = 0; i < m_saListChoice.GetSize(); i++) {
	    lbComboChoice->AddString(m_saListChoice.GetAt(i));
        }
#if 0
		int nRet = lbComboChoice->SelectString(-1, m_saListChoice.GetAt(0));
		if (nRet == CB_ERR)
			AfxMessageBox("Not found !");
#else
		m_sListChoice = m_saListChoice.GetAt(0);
#endif
    }

    bool bInitFirstActive = false;

    for (i = 1; i < 4; i++) {
        CWnd *hWnd = GetDlgItem(uiRadioButtons[i]);
        if (hWnd == 0) continue;
        hWnd->SetWindowText(sRadioButtonsLabels[i]);
        if (bRadioButtonsValidated[i]) {
            hWnd->EnableWindow(TRUE);
            hWnd->ShowWindow(SW_SHOWNORMAL);
            if (! bInitFirstActive) {
                ((CButton *)hWnd)->SetCheck(1);
                bInitFirstActive = true;
				nChoice = i;
            }
        }
        else {
            hWnd->EnableWindow(FALSE);
            hWnd->ShowWindow(SW_HIDE);
        }
    }
    UpdateData(FALSE);
    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}

void CListChoiceDlg::OnListchoiceRadio1() 
{
    // TODO: Add your control notification handler code here
    nChoice = 1;
}

void CListChoiceDlg::OnListchoiceRadio2() 
{
    // TODO: Add your control notification handler code here
    nChoice = 2;
}

void CListChoiceDlg::OnListchoiceRadio3() 
{
    // TODO: Add your control notification handler code here
    nChoice = 3;
}

bool CListChoiceDlg::AddChoice(const CString &sChoice)
{
    m_saListChoice.Add(sChoice);
    return true;
}

bool CListChoiceDlg::SetCheck(UINT nCheck, CString sLabel, bool bValidate)
{
    if ((nCheck < 1) || (nCheck > 3))
        return false;

    sRadioButtonsLabels[nCheck] = sLabel;
    bRadioButtonsValidated[nCheck] = bValidate;

    return true;
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */

