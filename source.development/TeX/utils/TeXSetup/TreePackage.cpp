// TreePackage.cpp : implementation file
//

#include "stdafx.h"
#include "texsetup.h"
#include "TransBmp.h"
#include "TPM.h"
#include "TreePackage.h"
#include "TeXInstall.h"
#include "PackagesPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

enum SetupType {
    NOTSELECTED = 1,
    SELECTED,
    DESELECTED,
    NEWSELECTED,
    NETNEEDED
};

static CTransBmp bmpIExplorer;
static CSize csIExplorer;

/////////////////////////////////////////////////////////////////////////////
// CTreePackage

IMPLEMENT_DYNCREATE(CTreePackage, CTreeCtrl)

    CTreePackage::CTreePackage()
{
    m_cImageList.Create(IDB_TREE_BOXES, 16, 8, RGB(255, 255, 255));

    bmpIExplorer.LoadBitmap(IDB_CONNECT);
    csIExplorer = CSize(bmpIExplorer.GetWidth(), bmpIExplorer.GetHeight());

}

CTreePackage::~CTreePackage()
{
}


BEGIN_MESSAGE_MAP(CTreePackage, CTreeCtrl)
    //{{AFX_MSG_MAP(CTreePackage)
    ON_WM_LBUTTONDOWN()
    ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
    ON_NOTIFY_REFLECT(TVN_KEYDOWN, OnKeydown)
    ON_WM_CREATE()
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
// CTreePackage message handlers

void CTreePackage::SwitchState(const HTREEITEM hItem)
{
    TPM *tpm = reinterpret_cast<TPM*>(GetItemData(hItem));
    CString sLabel = GetItemText(hItem);
  
    if (tpm == 0 || _stricmp((LPCTSTR)sLabel, "Unknown") == 0) {
        // do nothing !
    }
    else if (tpm != 0) {
        if (tpm->IsSelected()) {
            tpm->DeSelect();
        }
        else {
            tpm->Select();
        }
    }
}

void CTreePackage::UpdateStateImage (const HTREEITEM h)
{
    HTREEITEM hChild;
	TPM *tpm;
    CString sKey;
    if (h !=0 ) {
        tpm = reinterpret_cast<TPM*>(GetItemData(h));
	if (tpm != 0) {
            SetItemState(h,
			 INDEXTOSTATEIMAGEMASK(tpm->IsSelected() ? ::SELECTED : ::NOTSELECTED), 
                         TVIS_STATEIMAGEMASK);
//              if (! tpm->IsOnCD())
//                  SetItemImage(h, ::NETNEEDED, ::NETNEEDED);
        }
        hChild = GetChildItem(h);
        while (hChild != 0) {
            UpdateStateImage(hChild);
            hChild = GetNextSiblingItem(hChild);
        }
    }
}

void CTreePackage::Insert(const HTREEITEM hFather, TPM *t)
{
    HTREEITEM hChild = InsertItem(t->GetName(), 0, 0, hFather);
    SetItemState(hChild, 
                 INDEXTOSTATEIMAGEMASK(t->m_bSelected ? ::SELECTED : ::NOTSELECTED), 
                 TVIS_STATEIMAGEMASK);
//      if (! t->IsOnCD())
//          SetItemImage(hChild, ::NETNEEDED, ::NETNEEDED);
    SetItemData(hChild, (DWORD)t);
    for (int i = 0; i < t->m_saTPMRequires.GetSize(); i++) {
        TPM *tReq;
        CString sKey = t->m_saTPMRequires.GetAt(i);
        if (g_vSourceTPM.Lookup(sKey, tReq)) {
            Insert(hChild, tReq);
        }
    }
    SortChildren(hFather);
}

void CTreePackage::Initialize()
{
    // TODO: Add your specialized creation code here
    // Populate the tree, collections first, then packages

    SetImageList(&m_cImageList, TVSIL_STATE);
    //    SetImageList(&m_cImageList, TVSIL_NORMAL);

    if (g_tpmRoot) {
        Insert(0, g_tpmRoot);
    }
    else {
        // Fall back
        HTREEITEM hTreeItemRoot = InsertItem("Unknown", 0, 0);
        SetItemState(hTreeItemRoot, INDEXTOSTATEIMAGEMASK(0), 
                     TVIS_STATEIMAGEMASK);
        SetItemData(hTreeItemRoot, 0);

        POSITION pos = g_vSourceTPM.GetStartPosition();
        CString sKey;
        TPM *tpm;
        while (pos != 0) {
            g_vSourceTPM.GetNextAssoc(pos, sKey, tpm);
            if (_stricmp(sKey.Left(11), "collection/") == 0 && tpm != g_tpmRoot) {
                Insert(hTreeItemRoot, tpm);
            }
        }
        SortChildren(hTreeItemRoot);
    }
}

void CTreePackage::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    // TODO: Add your control notification handler code here
    HTREEITEM hItem = pNMTreeView->itemNew.hItem;
    DWORD dwData = GetItemData(hItem);

    UpdateParent(hItem);

    *pResult = 0;
}

void CTreePackage::OnLButtonDown(UINT nFlags, CPoint point) 
{
    // TODO: Add your message handler code here and/or call default
    UINT uFlags=0;
    HTREEITEM hItem = HitTest(point,&uFlags);

    if (!hItem)
        return;

    if( uFlags & TVHT_ONITEMSTATEICON )	{
        SwitchState(hItem);
        UpdateStateImage(GetRootItem());
        UpdateParent(hItem);
    }

    CTreeCtrl::OnLButtonDown(nFlags, point);	
}

void CTreePackage::OnKeydown(NMHDR* pNMHDR, LRESULT* pResult) 
{
    TV_KEYDOWN* pTVKeyDown = (TV_KEYDOWN*)pNMHDR;
    // TODO: Add your control notification handler code here
    HTREEITEM hItem = GetSelectedItem();

    if (!hItem)
        return;

    if( pTVKeyDown->wVKey == VK_SPACE )	{
        SwitchState(hItem);
        UpdateStateImage(GetRootItem());
        UpdateParent(hItem);
    }
    
    *pResult = 0;
}

void CTreePackage::UpdateParent(HTREEITEM hItem)
{
    CPackagesPage *hPage = reinterpret_cast<CPackagesPage *>(GetParent());
    TPM *tpm = reinterpret_cast<TPM*>(GetItemData(hItem));
    
    if (tpm) {
        hPage->m_sDescription = tpm->m_sDescription;
        hPage->UpdateSizes(tpm->GetSize(true, g_dwClusterSize, true, true) / 1024);
    }
}

int CTreePackage::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
    if (CTreeCtrl::OnCreate(lpCreateStruct) == -1)
        return -1;
	
    // TODO: Add your specialized creation code here
	
    return 0;
}

void CTreePackage::OnPaint() 
{
    CPaintDC dc(this); // device context for painting
	
    // TODO: Add your message handler code here
	
    // Do not call CTreeCtrl::OnPaint() for painting messages
    // Create a memory DC compatible with the paint DC
    CDC memDC;
    memDC.CreateCompatibleDC( &dc );

    CRect rcClip, rcClient;
    dc.GetClipBox( &rcClip );
    GetClientRect(&rcClient);

    // Select a compatible bitmap into the memory DC
    CBitmap bitmap;
    bitmap.CreateCompatibleBitmap( &dc, rcClient.Width(), rcClient.Height() );
    memDC.SelectObject( &bitmap );
	
    // Set clip region to be same as that in paint DC
    CRgn rgn;
    rgn.CreateRectRgnIndirect( &rcClip );
    memDC.SelectClipRgn(&rgn);
    rgn.DeleteObject();
	


    // First let the control do its default drawing.
    CWnd::DefWindowProc( WM_PAINT, (WPARAM)memDC.m_hDC, 0 );


    HTREEITEM hItem = GetFirstVisibleItem();

    int n = GetVisibleCount()+1;
    while( hItem && n--) {
        CRect rect;
        TPM *tpm = reinterpret_cast<TPM*>(GetItemData(hItem));
        
        if (tpm) {

            // Do not meddle with selected items or drop highlighted items
            //        UINT selflag = TVIS_DROPHILITED;
            UINT selflag = TVIS_DROPHILITED | TVIS_SELECTED;
            UINT uiItemState = GetItemState(hItem, selflag);
            
            CFont fontDC;
            LOGFONT logfont;
            CFont *pFontDC;
            // No font specified, so use window font
            CFont *pFont = GetFont();
            pFont->GetLogFont( &logfont );
            // if( GetItemBold( hItem ) )
            //    logfont.lfWeight = 700;
            fontDC.CreateFontIndirect( &logfont );
            pFontDC = memDC.SelectObject( &fontDC );
            
            // Get Size of the item label
            GetItemRect(hItem, &rect, TRUE);
            
            if (! tpm->IsOnCD()) {
                CPoint cpOrig(rect.right+2, rect.top+1);
                memDC.DrawState(cpOrig, csIExplorer, &bmpIExplorer, DSS_NORMAL);
            }
            
            if (!(uiItemState & selflag)) {
                if (! tpm->IsToBeInstalled()) {
                    memDC.SetTextColor(RGB(0,192,0));
                }
                else if (tpm->IsUpToDate()) {
                    memDC.SetTextColor(RGB(0,0,0));
                }
                else {
                    memDC.SetTextColor(RGB(192,0,0));
                }

                CString sItem = GetItemText( hItem );
                
                memDC.SetBkColor( GetSysColor( COLOR_WINDOW ) );
                memDC.TextOut( rect.left+2, rect.top+1, sItem );
            }

            memDC.SelectObject( pFontDC );
        }

        hItem = GetNextVisibleItem( hItem );
    }
    
    
    dc.BitBlt( rcClip.left, rcClip.top, rcClip.Width(), rcClip.Height(), &memDC, 
               rcClip.left, rcClip.top, SRCCOPY );
}

/* Local Variables:      */
/* mode: c++             */
/* c-basic-offset: 4     */
/* indent-tabs-mode: nil */
/* End:                  */
