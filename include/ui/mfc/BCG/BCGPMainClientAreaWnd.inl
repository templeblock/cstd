
// BCGPMainClientAreaWnd.cpp : implementation file
//

#include "BCGCBProVer.h"
#include "BCGPWorkspace.h"
#include "BCGPMainClientAreaWnd.h"
#include "BCGPMDIFrameWnd.h"
#include "BCGPMDIChildWnd.h"
#include "BCGPMenuBar.h"
#include "BCGPDockingControlBar.h"
#include "BCGPBaseTabbedBar.h"
#include "BCGPVisualManager.h"
#include "RegPath.h"
#include "BCGPRegistry.h"
#include "BCGPLocalResource.h"
#include "bcgprores.h"

extern CBCGPWorkspace* g_pWorkspace;

#define REG_SECTION_FMT						_T("%sMDIClientArea-%d")
#define REG_ENTRY_MDITABS_STATE				_T ("MDITabsState")

static const CString strMDIClientAreaProfile	= _T("MDIClientArea");

UINT BCGM_ON_MOVETOTABGROUP	= ::RegisterWindowMessage (_T("BCGM_ON_MOVETOTABGROUP"));

IMPLEMENT_DYNAMIC(CBCGPMainClientAreaWnd, CWnd)

#define UM_UPDATE_TABS		(WM_USER + 101)
#define RESIZE_MARGIN		40
#define NEW_GROUP_MARGIN	40

/////////////////////////////////////////////////////////////////////////////
// CBCGPMDITabParams

CBCGPMDITabParams::CBCGPMDITabParams ()
{
	m_tabLocation			= CBCGPTabWnd::LOCATION_TOP;
	m_style					= CBCGPTabWnd::STYLE_3D_SCROLLED;
	m_bTabCloseButton		= TRUE;	
	m_bTabCustomTooltips	= FALSE;
	m_bTabIcons				= FALSE;
	m_bAutoColor			= FALSE;
	m_bDocumentMenu			= FALSE;
	m_bEnableTabSwap		= TRUE;
	m_nTabBorderSize		= CBCGPVisualManager::GetInstance ()->GetMDITabsBordersSize ();
	m_bFlatFrame			= TRUE;
	m_bActiveTabCloseButton	= FALSE;
}
void CBCGPMDITabParams::Serialize (CArchive& ar)
{
	if (ar.IsStoring ())
	{
		ar << m_tabLocation;
		ar << m_style;
		ar << m_bTabCloseButton;
		ar << m_bTabIcons;
		ar << m_bAutoColor;
		ar << m_bDocumentMenu;
		ar << m_bEnableTabSwap;
		ar << m_nTabBorderSize;
	}
	else
	{
		int nValue;
		ar >> nValue; 
		m_tabLocation = (CBCGPTabWnd::Location) nValue;

		ar >> nValue;
		m_style = (CBCGPTabWnd::Style) nValue;

		ar >> m_bTabCloseButton;
		ar >> m_bTabIcons;
		ar >> m_bAutoColor;
		ar >> m_bDocumentMenu;
		ar >> m_bEnableTabSwap;
		ar >> m_nTabBorderSize;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CBCGPMainClientAreaWnd

CBCGPMainClientAreaWnd::CBCGPMainClientAreaWnd()
{
	m_bTabIsVisible		= FALSE;
	m_bTabIsEnabled		= FALSE;

	m_bIsMDITabbedGroup	= FALSE;
	m_groupAlignment	= GROUP_NO_ALIGN;
	m_nResizeMargin		= RESIZE_MARGIN;
	m_nNewGroupMargin	= NEW_GROUP_MARGIN;

	m_bDisableUpdateTabs = FALSE;

	m_rectNewTabGroup.SetRectEmpty ();
	m_nTotalResizeRest	= 0;
}
//****
CBCGPMainClientAreaWnd::~CBCGPMainClientAreaWnd()
{
	while (!m_lstTabbedGroups.IsEmpty ())
	{
		delete m_lstTabbedGroups.RemoveTail ();
	}

	while (!m_lstRemovedTabbedGroups.IsEmpty ())
	{
		CBCGPTabWnd* pWnd= DYNAMIC_DOWNCAST(CBCGPTabWnd, m_lstRemovedTabbedGroups.RemoveTail ());
		if (pWnd != NULL)
		{
			delete pWnd;
		}
	}

	if (!m_mapTabIcons.IsEmpty())
	{
		for (POSITION pos = m_mapTabIcons.GetStartPosition(); pos != NULL;)
		{
			CWnd* pWnd = NULL;
			CImageList* pImageList = NULL;

			m_mapTabIcons.GetNextAssoc(pos, pWnd, pImageList);
			if (pImageList != NULL)
			{
				delete pImageList;
			}
		}

		m_mapTabIcons.RemoveAll();
	}
}

//****
void CBCGPMainClientAreaWnd::EnableMDITabs (BOOL bEnable, 
											const CBCGPMDITabParams& params)
{
	if (m_bIsMDITabbedGroup)
	{
		EnableMDITabbedGroups (FALSE, params);
	}

	m_bTabIsEnabled = bEnable;
	m_bTabIsVisible = bEnable;
	
	m_mdiTabParams = params;
	ApplyParams (&m_wndTab);
	
	if (bEnable)
	{
		UpdateTabs ();
		if (!IsKeepClientEdge ())
		{
			ModifyStyleEx (WS_EX_CLIENTEDGE, 0);
		}
	}
	else
	{
		if (!IsKeepClientEdge ())
		{
			ModifyStyleEx (0, WS_EX_CLIENTEDGE);
		}
	}

	if (m_wndTab.GetSafeHwnd () != NULL)
	{
		m_wndTab.ShowWindow (SW_SHOW);
	}

	BringWindowToTop ();

	if (GetSafeHwnd () != NULL && GetParentFrame () != NULL)
	{
		GetParentFrame ()->RecalcLayout ();

		UINT uiRedrawFlags =	RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE | 
								RDW_UPDATENOW | RDW_ERASE;

		if (m_wndTab.GetSafeHwnd () != NULL)
		{
			m_wndTab.RedrawWindow (NULL, NULL, uiRedrawFlags);
		}

		RedrawWindow (NULL, NULL, uiRedrawFlags);
	}
}
//****
void CBCGPMainClientAreaWnd::EnableMDITabbedGroups (BOOL bEnable, const CBCGPMDITabParams& mdiTabParams)
{
	if (m_bTabIsEnabled)
	{
		EnableMDITabs (FALSE, mdiTabParams);
	}

	m_wndTab.ShowWindow (SW_HIDE);

	HWND hwndActive = (HWND) SendMessage (WM_MDIGETACTIVE, 0, 0);

	if (m_bIsMDITabbedGroup != bEnable)
	{
		m_bIsMDITabbedGroup = bEnable;

		if (!bEnable)
		{
			for (POSITION pos = m_lstTabbedGroups.GetHeadPosition (); pos != 0;)
			{
				CBCGPTabWnd* pNextWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
				ASSERT_VALID (pNextWnd);

				pNextWnd->ShowWindow (SW_HIDE);

				for (int i = 0; i < pNextWnd->GetTabsNum (); i++)
				{
					CWnd* pNextChildWnd = pNextWnd->GetTabWnd (i);
					ASSERT_VALID (pNextChildWnd);
					pNextChildWnd->ModifyStyle (0, CBCGPMDIChildWnd::m_dwExcludeStyle | WS_SYSMENU, SWP_NOZORDER | SWP_FRAMECHANGED);
				}
			}
		}
	}

	
	m_bTabIsVisible = bEnable;

	if (!m_bIsMDITabbedGroup)
	{
		if (!IsKeepClientEdge ())
		{
			ModifyStyleEx (0, WS_EX_CLIENTEDGE);
		}

		if (globalData.bIsWindowsVista)
		{
			CWnd* pWndChild = GetWindow (GW_CHILD);
			CList<CBCGPMDIChildWnd*, CBCGPMDIChildWnd*> lst;

			while (pWndChild != NULL)
			{
				ASSERT_VALID (pWndChild);

				CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pWndChild);
				if (pMDIChild != NULL && pMDIChild->CanShowOnMDITabs ())
				{
					lst.AddTail (pMDIChild);
				}

				pWndChild = pWndChild->GetNextWindow ();
			}
			
			m_bDisableUpdateTabs = TRUE;

			for (POSITION pos = lst.GetTailPosition (); pos != NULL;)
			{
				CBCGPMDIChildWnd* pMDIChild = lst.GetPrev (pos);
				pMDIChild->SetWindowPos (NULL, -1, -1, -1, -1,
								SWP_NOZORDER | SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
			}

			m_bDisableUpdateTabs = FALSE;

			UpdateTabs ();
		}

		return;
	}

	m_mdiTabParams = mdiTabParams;

	if (!IsKeepClientEdge ())
	{
		ModifyStyleEx (WS_EX_CLIENTEDGE, 0);
	}

	POSITION pos = NULL;

	for (pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextWnd);
		pNextWnd->ShowWindow (SW_SHOWNA);
		ApplyParams (pNextWnd);
	}

	UpdateMDITabbedGroups (TRUE);	

	for (pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextWnd);
		pNextWnd->RecalcLayout ();
	}

	if (m_bIsMDITabbedGroup)
	{
		SetActiveTab (hwndActive);
	}
}
//****
void CBCGPMainClientAreaWnd::ApplyParams (CBCGPTabWnd* pTabWnd)
{
	ASSERT_VALID (pTabWnd);

	pTabWnd->ModifyTabStyle (m_mdiTabParams.m_style);
	pTabWnd->SetLocation (m_mdiTabParams.m_tabLocation);
	pTabWnd->m_bCloseBtn = m_mdiTabParams.m_bTabCloseButton;
	pTabWnd->m_bActiveTabCloseButton = m_mdiTabParams.m_bActiveTabCloseButton;
	pTabWnd->EnableTabDocumentsMenu (m_mdiTabParams.m_bDocumentMenu);
	pTabWnd->EnableAutoColor (m_mdiTabParams.m_bAutoColor);
	pTabWnd->EnableTabSwap (m_mdiTabParams.m_bEnableTabSwap);
	pTabWnd->SetTabBorderSize (m_mdiTabParams.m_nTabBorderSize);
	pTabWnd->EnableCustomToolTips (m_mdiTabParams.m_bTabCustomTooltips);

	pTabWnd->HideInactiveWindow (FALSE);
	pTabWnd->HideNoTabs ();
	pTabWnd->AutoSizeWindow (FALSE);
	pTabWnd->AutoDestroyWindow (FALSE);
	pTabWnd->SetFlatFrame (m_mdiTabParams.m_bFlatFrame);
	pTabWnd->m_bTransparent = TRUE;
	pTabWnd->m_bTopEdge = TRUE;
	pTabWnd->SetDrawNoPrefix (TRUE, FALSE);
	pTabWnd->SetActiveTabBoldFont ();
	pTabWnd->m_bActivateLastVisibleTab = TRUE;
	pTabWnd->m_bActivateTabOnRightClick = TRUE;

	pTabWnd->m_bIsMDITab = TRUE;
}
//****
BEGIN_MESSAGE_MAP(CBCGPMainClientAreaWnd, CWnd)
	//{{AFX_MSG_MAP(CBCGPMainClientAreaWnd)
	ON_WM_ERASEBKGND()
	ON_WM_STYLECHANGING()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_MDISETMENU,OnSetMenu)
	ON_MESSAGE(WM_MDIREFRESHMENU, OnMDIRefreshMenu)
	ON_MESSAGE(WM_MDIDESTROY, OnMDIDestroy)
	ON_MESSAGE(WM_MDINEXT, OnMDINext)
	ON_MESSAGE(UM_UPDATE_TABS, OnUpdateTabs)
	ON_REGISTERED_MESSAGE(BCGM_GETDRAGBOUNDS, OnGetDragBounds)
	ON_REGISTERED_MESSAGE(BCGM_ON_DRAGCOMPLETE, OnDragComplete)
	ON_REGISTERED_MESSAGE(BCGM_ON_TABGROUPMOUSEMOVE, OnTabGroupMouseMove)
	ON_REGISTERED_MESSAGE(BCGM_ON_CANCELTABMOVE, OnCancelTabMove)
	ON_REGISTERED_MESSAGE(BCGM_ON_MOVETABCOMPLETE, OnMoveTabComplete)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBCGPMainClientAreaWnd message handlers

afx_msg LRESULT CBCGPMainClientAreaWnd::OnSetMenu (WPARAM wp, LPARAM lp)
{
	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParentFrame ());
	if (pMainFrame != NULL && ::IsWindow (pMainFrame->GetSafeHwnd ()))
	{
		if (pMainFrame->OnSetMenu ((HMENU) wp))
		{
			wp = NULL;
		}
	}
	else
	{
		wp = NULL;
	}

	return DefWindowProc (WM_MDISETMENU, wp, lp);
}
//****
LRESULT CBCGPMainClientAreaWnd::OnMDIRefreshMenu (WPARAM /*wp*/, LPARAM /*lp*/)
{
	LRESULT lRes = Default ();

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParentFrame ());
	if (pMainFrame != NULL && pMainFrame->GetMenuBar () != NULL)
	{
		pMainFrame->m_hmenuWindow = 
			pMainFrame->GetWindowMenuPopup (pMainFrame->GetMenuBar ()->GetHMenu ());
	}

	return lRes;
}
//****
BOOL CBCGPMainClientAreaWnd::OnEraseBkgnd(CDC* pDC) 
{
	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParentFrame ());
	if (pMainFrame != NULL && pMainFrame->OnEraseMDIClientBackground (pDC))
	{
		return TRUE;
	}

	CRect rectClient;
	GetClientRect (rectClient);

	if (CBCGPVisualManager::GetInstance ()->OnEraseMDIClientArea (pDC, rectClient))
	{
		return TRUE;
	}

	return CWnd::OnEraseBkgnd(pDC);
}
//****
LRESULT CBCGPMainClientAreaWnd::OnMDIDestroy(WPARAM wParam, LPARAM)
{
	LRESULT lRes = 0;
	CBCGPMDIFrameWnd* pParentFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParentFrame ());
	
	CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, CWnd::FromHandle ((HWND)wParam));
	BOOL bTabHeightChanged = FALSE;

	if (!pParentFrame->m_bClosing && !CBCGPMDIFrameWnd::m_bDisableSetRedraw)
	{
		SetRedraw (FALSE);
	}

	HWND hwndActive = NULL;
	if (pMDIChild != NULL)
	{
		CBCGPTabWnd* pTabWnd = pMDIChild->GetRelatedTabGroup ();
		pMDIChild->SetRelatedTabGroup (NULL);
		if (pTabWnd != NULL)
		{
			int nTabsHeight = pTabWnd->GetTabsHeight ();

			int iTab = pTabWnd->GetTabFromHwnd ((HWND)wParam);
			if (iTab >= 0)
			{
				pMDIChild->m_bToBeDestroyed = TRUE;
			}
			pTabWnd->RemoveTab (iTab);
			
			if (pTabWnd->GetTabsNum () == 0)
			{
				POSITION pos = m_lstTabbedGroups.Find (pTabWnd);
				
				if (pos != NULL)
				{
					// find window to activate next group after the current group has been destroyed
					// we should find the window to activate only if the active group is being destroyed
					if (m_lstTabbedGroups.GetCount () > 1 && 
						pTabWnd->IsActiveInMDITabGroup ())
					{
						m_lstTabbedGroups.GetNext (pos);
						if (pos == NULL)
						{
							pos = m_lstTabbedGroups.GetHeadPosition ();
						}

						if (pos != NULL)
						{
							CBCGPTabWnd* pNextTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetAt (pos));
							if (pNextTabWnd != NULL)
							{
								int iActiveTab = pNextTabWnd->GetActiveTab ();
								if (iActiveTab == -1)
								{
									iActiveTab = 0;
								}
								CWnd* pActiveWnd = pNextTabWnd->GetTabWnd (iActiveTab);
								if (pActiveWnd != NULL)
								{
									ASSERT_VALID (pActiveWnd);
									hwndActive = pActiveWnd->GetSafeHwnd ();
								}
							}
						}
					}
					RemoveTabGroup (pTabWnd);
				}
			}
			else
			{
				bTabHeightChanged = (nTabsHeight != pTabWnd->GetTabsHeight ());
			}
		}
	}

	
	if (m_wndTab.GetSafeHwnd () != NULL)
	{
		int nTabsHeight = m_wndTab.GetTabsHeight ();
		int iTab = m_wndTab.GetTabFromHwnd ((HWND)wParam);
		if (iTab >= 0)
		{
			CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, m_wndTab.GetTabWnd (iTab));
			if (pMDIChild != NULL)
			{
				pMDIChild->m_bToBeDestroyed = TRUE;
			}

			m_wndTab.RemoveTab (iTab);
		}
		bTabHeightChanged = (nTabsHeight != m_wndTab.GetTabsHeight ());
	}
	

	lRes = Default ();
	if (bTabHeightChanged && pParentFrame != NULL)
	{
		pParentFrame->RecalcLayout ();
	}

	if (!pParentFrame->m_bClosing)
	{
		if (IsWindow (hwndActive))
		{
			SetActiveTab (hwndActive);
		}
	}
	if (!pParentFrame->m_bClosing && !CBCGPMDIFrameWnd::m_bDisableSetRedraw)
	{
		SetRedraw (TRUE);
		GetParent ()->RedrawWindow (NULL, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_ALLCHILDREN);
	}

	return lRes;
}
//****
void CBCGPMainClientAreaWnd::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
	if (m_bDisableUpdateTabs)
	{
		return;
	}
	if (m_bIsMDITabbedGroup)
	{
		CalcWindowRectForMDITabbedGroups (lpClientRect, nAdjustType);
		CWnd::CalcWindowRect(lpClientRect, nAdjustType);
		return;
	}

	if (m_wndTab.GetSafeHwnd () != NULL)
	{
		BOOL bRedraw = FALSE;

		if (m_bTabIsVisible)
		{
			CRect rectOld;
			m_wndTab.GetWindowRect (rectOld);

			m_wndTab.SetWindowPos (NULL, 
				lpClientRect->left, lpClientRect->top,
				lpClientRect->right - lpClientRect->left,
				lpClientRect->bottom - lpClientRect->top,
				SWP_NOZORDER | SWP_NOACTIVATE);

			CRect rectTabClient;
			m_wndTab.GetClientRect (rectTabClient);

			CRect rectTabWnd;
			m_wndTab.GetWndArea (rectTabWnd);

			lpClientRect->top += (rectTabWnd.top - rectTabClient.top);
			lpClientRect->bottom += (rectTabWnd.bottom - rectTabClient.bottom);
			lpClientRect->left += (rectTabWnd.left - rectTabClient.left);
			lpClientRect->right += (rectTabWnd.right - rectTabClient.right);

			m_wndTab.ShowWindow (SW_SHOWNA);

			CRect rectNew;
			m_wndTab.GetWindowRect (rectNew);

			bRedraw = (rectOld != rectNew);
		}
		else
		{
			m_wndTab.ShowWindow (SW_HIDE);
		}

		CRect rectOld;
		GetWindowRect (rectOld);
		int nHeightDelta = lpClientRect->bottom - lpClientRect->top - 
							rectOld.Height ();

		SetWindowPos (NULL, 
				lpClientRect->left, lpClientRect->top,
				lpClientRect->right - lpClientRect->left,
				lpClientRect->bottom - lpClientRect->top,
				SWP_NOZORDER | SWP_NOACTIVATE);

		CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParentFrame ());
		if (pMainFrame != NULL)
		{
			pMainFrame->OnSizeMDIClient (rectOld, lpClientRect);
		}

		if (!m_bTabIsVisible)
		{
			CRect rectClient;
			GetClientRect (&rectClient);
			CBCGPMDIFrameWnd* pFrame = (CBCGPMDIFrameWnd*) GetParentFrame ();
			ASSERT_VALID (pFrame);
			HWND hwndT = ::GetWindow(pFrame->m_hWndMDIClient, GW_CHILD);
			
			while (hwndT != NULL)
			{
				DWORD dwStyle = ::GetWindowLong (hwndT, GWL_STYLE);
				if (dwStyle & WS_MAXIMIZE)
				{
					break; // nothing to move;
				}
				if (dwStyle & WS_MINIMIZE)
				{
					CRect rectWnd;
					::GetWindowRect (hwndT, rectWnd);
					ScreenToClient (&rectWnd);

					rectWnd.OffsetRect (0, nHeightDelta);

					if (rectWnd.top < rectClient.top)
					{
						rectWnd.top = rectClient.top;
					}

					::SetWindowPos (hwndT, NULL, 
									rectWnd.left, rectWnd.top, 0, 0, 
									SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE);
				}

				hwndT=::GetWindow(hwndT,GW_HWNDNEXT);
			}
		}
	}

	CWnd::CalcWindowRect(lpClientRect, nAdjustType);

	int nActiveTab = m_wndTab.GetActiveTab ();
	for (int i = 0; i < m_wndTab.GetTabsNum (); i++)
	{
		CWnd* pWnd = m_wndTab.GetTabWnd (i);
		if (pWnd->GetSafeHwnd () == 0)
		{
			continue;
		}

		// only applies to MDI children in "always maximize" mode
		if ((pWnd->GetStyle () & WS_MINIMIZE) != 0 && 
			((pWnd->GetStyle () & WS_SYSMENU) == 0))
		{
			pWnd->ShowWindow (SW_RESTORE);
		}

		DWORD dwFlags = SWP_NOACTIVATE;
		if (i != nActiveTab)
		{
			dwFlags |= SWP_NOZORDER | SWP_NOREDRAW;
		}

		CRect rect (0, 0, lpClientRect->right - lpClientRect->left, 
							lpClientRect->bottom - lpClientRect->top);
		
	
		CRect rectClient;
		pWnd->GetClientRect (rectClient);
		pWnd->ClientToScreen (rectClient);

		CRect rectScreen;
		pWnd->GetWindowRect (rectScreen);

		rect.left -= rectClient.left - rectScreen.left;
		rect.top -= rectClient.top - rectScreen.top;
		rect.right += rectScreen.right - rectClient.right;
		rect.bottom += rectScreen.bottom - rectClient.bottom;

		if (rectClient == rect)
		{
			break;
		}
		
		if (pWnd != NULL && ((pWnd->GetStyle () & WS_SYSMENU) == 0))
		{
			pWnd->SetWindowPos (&wndTop, rect.left, rect.top, 
								rect.Width (), rect.Height (), dwFlags);
		}
	}

}
//****
void CBCGPMainClientAreaWnd::CalcWindowRectForMDITabbedGroups(LPRECT lpClientRect, UINT /*nAdjustType*/) 
{
	SetWindowPos (&wndBottom, 
				lpClientRect->left, lpClientRect->top,
				lpClientRect->right - lpClientRect->left,
				lpClientRect->bottom - lpClientRect->top,
				SWP_NOACTIVATE); 

	if (m_lstTabbedGroups.IsEmpty ())
	{
		return;
	}
	// special processing for single tab
	
	if (m_lstTabbedGroups.GetCount () == 1)
	{
		CBCGPTabWnd* pNextTab = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetHead ());
		ASSERT_VALID (pNextTab);
		pNextTab->ShowWindow (SW_SHOWNA);
		pNextTab->SetWindowPos (NULL, 0, 0, 
								lpClientRect->right - lpClientRect->left, 
								lpClientRect->bottom - lpClientRect->top, 
								SWP_NOZORDER | SWP_NOACTIVATE);

		AdjustMDIChildren (pNextTab);		
		return;
	}


	ASSERT (m_groupAlignment != GROUP_NO_ALIGN);

	int nTotalSize = 0;
	POSITION pos = NULL;

	for (pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextTab = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextTab);

		CRect rect;
		pNextTab->GetWindowRect (rect);

		nTotalSize += (m_groupAlignment == GROUP_VERT_ALIGN) ? rect.Width () : rect.Height (); 
	}

	int nClientAreaWndSize = (m_groupAlignment == GROUP_VERT_ALIGN) ? 
								lpClientRect->right - lpClientRect->left : 
								lpClientRect->bottom - lpClientRect->top;

	int nDelta = (nClientAreaWndSize - nTotalSize) / (int) m_lstTabbedGroups.GetCount ();
	int nRest  = (nClientAreaWndSize - nTotalSize) % (int) m_lstTabbedGroups.GetCount ();

	m_nTotalResizeRest += nRest;
	if (abs (m_nTotalResizeRest) >= m_lstTabbedGroups.GetCount ())
	{
		m_nTotalResizeRest > 0 ? nDelta ++ : nDelta --;
		m_nTotalResizeRest = 0;
	}

	int nOffset = 0;
	for (pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextTab = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextTab);

		if (pNextTab->GetSafeHwnd () != NULL)
		{
			CRect rect;
			pNextTab->GetWindowRect (rect);
			ScreenToClient (rect);

			if (m_groupAlignment == GROUP_VERT_ALIGN)
			{
				int nFinalWidth = rect.Width () + nDelta;
				if (pos == NULL && nClientAreaWndSize - nOffset + nFinalWidth != 0)
				{
					nFinalWidth = nClientAreaWndSize - nOffset;
				}
				pNextTab->SetWindowPos (NULL, nOffset, 
										0, 
										nFinalWidth, 
										lpClientRect->bottom - lpClientRect->top, 
										SWP_NOZORDER | SWP_NOACTIVATE);
				nOffset += rect.Width () + nDelta;
			}
			else
			{
				int nFinalHeight = rect.Height () + nDelta;
				if (pos == NULL && nClientAreaWndSize - nOffset + nFinalHeight != 0)
				{
					nFinalHeight = nClientAreaWndSize - nOffset;
				}
				pNextTab->SetWindowPos (NULL, 0, 
										nOffset, 
										lpClientRect->right - lpClientRect->left,
										nFinalHeight, 
										SWP_NOZORDER | SWP_NOACTIVATE);
				nOffset += rect.Height () + nDelta;
			}

			AdjustMDIChildren (pNextTab);		
		}
	}
}
//****
void CBCGPMainClientAreaWnd::AdjustMDIChildren (CBCGPTabWnd* pTabWnd)
{
	if (!pTabWnd->IsWindowVisible () && CBCGPMDIFrameWnd::m_bDisableSetRedraw)
	{
		return;
	}

	CRect rectTabWnd;
	pTabWnd->GetWndArea (rectTabWnd);
	pTabWnd->MapWindowPoints (this, rectTabWnd);

	int nActiveTab = pTabWnd->GetActiveTab ();
	
	for (int i = 0; i < pTabWnd->GetTabsNum (); i++)
	{
		CWnd* pWnd = pTabWnd->GetTabWnd (i);
		if (pWnd->GetSafeHwnd () == 0)
		{
			continue;
		}

		DWORD dwStyle = ::GetWindowLong (pWnd->GetSafeHwnd (), GWL_STYLE);
		if ((dwStyle & WS_MINIMIZE) != 0)
		{
			pWnd->ShowWindow (SW_RESTORE);
		}

		DWORD dwFlags = SWP_NOACTIVATE;
		if (i != nActiveTab)
		{
			dwFlags |= SWP_NOZORDER | SWP_NOREDRAW;
		}
		
		if (pWnd != NULL)
		{
			pWnd->SetWindowPos (&wndTop, rectTabWnd.left, rectTabWnd.top, 
								rectTabWnd.Width (), rectTabWnd.Height (), dwFlags);
		}
	}
}
//****
void CBCGPMainClientAreaWnd::SetActiveTab (HWND hwnd)
{
	if (m_bDisableUpdateTabs)
	{
		return;
	}
	if (m_bIsMDITabbedGroup)
	{
		CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, CWnd::FromHandle (hwnd));
		if (pMDIChild != NULL)
		{
			ASSERT_VALID (pMDIChild);
			CBCGPTabWnd* pTabWnd = pMDIChild->GetRelatedTabGroup ();
			if (pTabWnd != NULL)
			{
				ASSERT_VALID (pTabWnd);

				int iTab = pTabWnd->GetTabFromHwnd (hwnd);
				if (iTab >= 0)
				{

					CRect rectTabWnd; 
					pTabWnd->GetClientRect (rectTabWnd);

					if (rectTabWnd.IsRectEmpty ())
					{
						CFrameWnd* pMainFrame = pMDIChild->GetTopLevelFrame ();

						if (pMainFrame != NULL)
						{
							ASSERT_VALID (pMainFrame);
							pMainFrame->RecalcLayout ();
						}
					}

					CBCGPTabWnd* pPrevActiveWnd = FindActiveTabWnd ();
					if (pPrevActiveWnd != NULL)
					{
						pPrevActiveWnd->SetActiveInMDITabGroup (FALSE);
						pPrevActiveWnd->InvalidateTab (pPrevActiveWnd->GetActiveTab ());
					}

					pTabWnd->SetActiveInMDITabGroup (TRUE);
					pTabWnd->SetActiveTab (iTab);
					pTabWnd->InvalidateTab (pTabWnd->GetActiveTab ());
				}
			}
		}
	}
	else
	{
		if (m_bTabIsVisible)
		{
			int iTab = m_wndTab.GetTabFromHwnd (hwnd);
			if (iTab >= 0)
			{
				m_wndTab.SetActiveTab (iTab);
			}
		}
	}
}

//****
LRESULT CBCGPMainClientAreaWnd::OnUpdateTabs (WPARAM, LPARAM)
{
	UpdateTabs ();
	return 0;
}
//****
void CBCGPMainClientAreaWnd::PreSubclassWindow() 
{
	CWnd::PreSubclassWindow();
	CreateTabGroup (&m_wndTab);
}
//****
CBCGPTabWnd* CBCGPMainClientAreaWnd::CreateTabGroup (CBCGPTabWnd* pWndTab)
{
	if (pWndTab == NULL)
	{
		pWndTab = new CBCGPTabWnd;
	}

	if (m_mdiTabParams.m_bTabCustomTooltips)
	{
		pWndTab->EnableCustomToolTips ();
	}

	CWnd* pParent = m_bIsMDITabbedGroup ? this : (CWnd*) GetParentFrame ();

	//-------------------------
	// Create MDI tabs control:
	//-------------------------
    if (!pWndTab->Create (m_mdiTabParams.m_style, CRect (0, 0, 0, 0), 
		pParent, (UINT)-1, m_mdiTabParams.m_tabLocation, m_mdiTabParams.m_bTabCloseButton))
	{
		TRACE(_T("CBCGPMainClientAreaWnd::OnCreate: can't create tabs window\n"));
		delete pWndTab;
		return NULL;
	}

	ApplyParams (pWndTab);

	if (!m_bTabIsVisible)
	{
		pWndTab->ShowWindow (SW_HIDE);
	}

	//------------------
	// Create tab icons:
	//------------------
	
	if (!m_bIsMDITabbedGroup)
	{
		m_TabIcons.Create (
			globalData.m_sizeSmallIcon.cx, globalData.m_sizeSmallIcon.cy, 
			ILC_COLOR32 | ILC_MASK, 0, 1);
	}
	else 
	{
		CImageList* pImageList = NULL;
		if (m_mapTabIcons.Lookup (pWndTab, pImageList) && 
			pImageList != NULL)
		{
			pImageList->DeleteImageList ();
		}
		else
		{
			pImageList = new CImageList;
			m_mapTabIcons.SetAt (pWndTab, pImageList);
		}

		pImageList->Create (
			globalData.m_sizeSmallIcon.cx, globalData.m_sizeSmallIcon.cy, 
			ILC_COLOR32 | ILC_MASK, 0, 1);
	}

	return pWndTab;
}
//****
void CBCGPMainClientAreaWnd::UpdateTabs (BOOL bSetActiveTabVisible/* = FALSE*/)
{
	if (m_bDisableUpdateTabs)
	{
		return;
	}

	if (m_bIsMDITabbedGroup)
	{
		UpdateMDITabbedGroups (bSetActiveTabVisible);
		return;
	}

	if (m_wndTab.GetSafeHwnd () == NULL || !m_bTabIsVisible)
	{
		return;
	}

	BOOL bRecalcLayout = FALSE;
	BOOL bTabWndEmpty = m_wndTab.GetTabsNum () == 0;

	CWnd* pWndChild = GetWindow (GW_CHILD);
	while (pWndChild != NULL)
	{
		ASSERT_VALID (pWndChild);

		CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pWndChild);

		BOOL bIsShowTab = TRUE;
		if (pMDIChild != NULL)
		{
			bIsShowTab = pMDIChild->CanShowOnMDITabs ();
		}
		else if (pWndChild->IsKindOf (RUNTIME_CLASS (CBCGPTabWnd)))
		{
			pWndChild = pWndChild->GetNextWindow ();
			continue;
		}


		//--------------
		// Get tab icon:
		//--------------
		int iIcon = -1;
		if (m_mdiTabParams.m_bTabIcons)
		{
			HICON hIcon = NULL;
			if (pMDIChild != NULL)
			{
				hIcon = pMDIChild->GetFrameIcon ();
			}
			else
			{
				if ((hIcon = pWndChild->GetIcon (FALSE)) == NULL)
				{
					hIcon = (HICON)(LONG_PTR) GetClassLongPtr (*pWndChild, GCLP_HICONSM);
				}
			}

			if (hIcon != NULL)
			{
				if (!m_mapIcons.Lookup (hIcon, iIcon))
				{
					iIcon = m_TabIcons.Add (hIcon);
					m_mapIcons.SetAt (hIcon, iIcon);

					if (m_TabIcons.GetImageCount () == 1)
					{
						m_wndTab.SetImageList (m_TabIcons.GetSafeHandle ());
					}
				}
			}
		}
		else
		{
			m_wndTab.ClearImageList ();
			m_mapIcons.RemoveAll ();

			while (m_TabIcons.GetImageCount () > 0)
			{
				m_TabIcons.Remove (0);
			}
		}

		//--------------------------------
		// Get tab label (window caption):
		//--------------------------------
		CString strTabLabel;
		if (pMDIChild != NULL)
		{
			strTabLabel = pMDIChild->GetFrameText ();
		}
		else
		{
			pWndChild->GetWindowText (strTabLabel);
		}

		int iTabIndex = m_wndTab.GetTabFromHwnd (pWndChild->GetSafeHwnd ());
		if (iTabIndex >= 0)
		{
			//---------------------------------
			// Tab is already exist, update it:
			//---------------------------------
			if (pWndChild->GetStyle () & WS_VISIBLE)
			{
				CString strCurTabLabel;
				m_wndTab.GetTabLabel (iTabIndex, strCurTabLabel);

				if (strCurTabLabel != strTabLabel)
				{
					//-----------------------------
					// Text was changed, update it:
					//-----------------------------
					m_wndTab.SetTabLabel (iTabIndex, strTabLabel);
					bRecalcLayout = TRUE;
				}

				if (m_wndTab.GetTabIcon (iTabIndex) != (UINT) iIcon)
				{
					//-----------------------------
					// Icon was changed, update it:
					//-----------------------------
					m_wndTab.SetTabIcon (iTabIndex, iIcon);
					bRecalcLayout = TRUE;
				}
			}
			else
			{
				//----------------------------------
				// Window is hidden now, remove tab:
				//----------------------------------
				m_wndTab.RemoveTab (iTabIndex);
				bRecalcLayout = TRUE;
			}
		}
		else if ((pMDIChild == NULL || !pMDIChild->m_bToBeDestroyed) && bIsShowTab)
		{
			//----------------------
			// New item, add it now:
			//----------------------
			m_wndTab.AddTab (pWndChild, strTabLabel, iIcon);
			m_wndTab.SetActiveTab (m_wndTab.GetTabsNum () - 1);

			bRecalcLayout = TRUE;
		}

		pWndChild = pWndChild->GetNextWindow ();
    }

	if (bRecalcLayout && GetParentFrame () != NULL)
	{
		GetParentFrame ()->RecalcLayout ();
	}

	if (bSetActiveTabVisible)
	{
		m_wndTab.EnsureVisible (m_wndTab.GetActiveTab ());
	}

	if (bTabWndEmpty && m_wndTab.GetTabsNum () > 0 || m_wndTab.GetTabsNum () == 0)
	{
		GetParentFrame ()->RecalcLayout ();
		RedrawWindow (NULL, NULL, 
						RDW_FRAME | RDW_INVALIDATE | RDW_UPDATENOW |
						RDW_ERASE | RDW_INTERNALPAINT);
	}
}
//****
void CBCGPMainClientAreaWnd::UpdateMDITabbedGroups (BOOL bSetActiveTabVisible)
{
	if (m_bDisableUpdateTabs)
	{
		return;
	}

	BOOL bRecalcLayout = FALSE;
	CWnd* pWndChild = GetWindow (GW_CHILD);
	HWND hwndActive = NULL;

	while (pWndChild != NULL)
	{
		ASSERT_VALID (pWndChild);

		CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pWndChild);

		if (pMDIChild == NULL)
		{
			pWndChild = pWndChild->GetNextWindow ();
			continue;
		}

		// always modify style
		pMDIChild->ModifyStyle (CBCGPMDIChildWnd::m_dwExcludeStyle | WS_MAXIMIZE | WS_SYSMENU, 0, SWP_NOZORDER);	

		BOOL bIsShowTab = pMDIChild->CanShowOnMDITabs () && (pWndChild->GetStyle () & WS_VISIBLE);
		CString strTabLabel = pMDIChild->GetFrameText ();		

		CBCGPTabWnd* pRelatedTabWnd = pMDIChild->GetRelatedTabGroup ();

		BOOL bRemoved = FALSE;
		if (pRelatedTabWnd == NULL && !pMDIChild->m_bToBeDestroyed && bIsShowTab)
		{
			if (m_lstTabbedGroups.IsEmpty ())
			{
				pRelatedTabWnd = CreateTabGroup (NULL);
				m_lstTabbedGroups.AddTail (pRelatedTabWnd);
			}
			else
			{
				// new window to be added			
				pRelatedTabWnd = FindActiveTabWnd ();

				if (pRelatedTabWnd == NULL)
				{
					pRelatedTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetHead ());
				}
			}

			ASSERT_VALID (pRelatedTabWnd);

			pMDIChild->SetRelatedTabGroup (pRelatedTabWnd);
			pRelatedTabWnd->AddTab (pWndChild, strTabLabel);

			if (!pRelatedTabWnd->IsWindowVisible ())
			{
				pRelatedTabWnd->ShowWindow (SW_SHOWNA);
			}
			
			hwndActive = pWndChild->GetSafeHwnd ();
			
		}
		else if (pRelatedTabWnd != NULL)
		{
			int iTabIndex = pRelatedTabWnd->GetTabFromHwnd (pWndChild->GetSafeHwnd ());
			if (iTabIndex >= 0)
			{
				//---------------------------------
				// Tab is already exist, update it:
				//---------------------------------
				if (pWndChild->GetStyle () & WS_VISIBLE)
				{
					CString strCurTabLabel;
					pRelatedTabWnd->GetTabLabel (iTabIndex, strCurTabLabel);

					if (strCurTabLabel != strTabLabel)
					{
						//-----------------------------
						// Text was changed, update it:
						//-----------------------------
						pRelatedTabWnd->SetTabLabel (iTabIndex, strTabLabel);
						bRecalcLayout = TRUE;
					}
				}
				else
				{
					//----------------------------------
					// Window is hidden now, remove tab:
					//----------------------------------
					pRelatedTabWnd->RemoveTab (iTabIndex);
					if (pRelatedTabWnd->GetTabsNum () == 0)
					{
						RemoveTabGroup (pRelatedTabWnd, FALSE);
					}
					bRecalcLayout = TRUE;
					bRemoved = TRUE;
				}
			}
		}

		CImageList* pImageList = NULL;
		m_mapTabIcons.Lookup (pRelatedTabWnd, pImageList);

		if (pImageList != NULL)
		{
			ASSERT_VALID (pImageList);

			int iIcon = -1;
			if (m_mdiTabParams.m_bTabIcons)
			{
				HICON hIcon = NULL;
				if (pMDIChild != NULL)
				{
					hIcon = pMDIChild->GetFrameIcon ();
				}

				if (hIcon != NULL)
				{
					if (!pRelatedTabWnd->IsIconAdded (hIcon, iIcon))
					{
						iIcon = pImageList->Add (hIcon);
						pRelatedTabWnd->AddIcon (hIcon, iIcon);
					}

					if (pRelatedTabWnd->GetImageList () != pImageList)
					{
						pRelatedTabWnd->SetImageList (pImageList->GetSafeHandle ());
					}
				}

				if (!bRemoved)
				{
					int iTabIndex = pRelatedTabWnd->GetTabFromHwnd (pMDIChild->GetSafeHwnd ());
					if (pRelatedTabWnd->GetTabIcon (iTabIndex) != (UINT) iIcon)
					{
						//-----------------------------
						// Icon was changed, update it:
						//-----------------------------
						pRelatedTabWnd->SetTabIcon (iTabIndex, iIcon);
					}
				}
			}
			else
			{
				pRelatedTabWnd->ResetImageList ();
				m_mapIcons.RemoveAll ();

				while (pImageList->GetImageCount () > 0)
				{
					pImageList->Remove (0);
				}

				bRecalcLayout = TRUE;
			}
		}

		pWndChild = pWndChild->GetNextWindow ();
    }

	for (POSITION pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextTab = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextTab);
		AdjustMDIChildren (pNextTab);
	}

	if (bRecalcLayout && GetParentFrame () != NULL)
	{
		GetParentFrame ()->RecalcLayout ();
	}
	
	if (hwndActive != NULL)
	{
		SetActiveTab (hwndActive);
	}

	if (bSetActiveTabVisible)
	{
		CBCGPTabWnd* pActiveWnd = FindActiveTabWnd ();
		if (pActiveWnd != NULL)
		{
			ASSERT_VALID (pActiveWnd);
			pActiveWnd->EnsureVisible (pActiveWnd->GetActiveTab ());
		}
	}
}
//****
void CBCGPMainClientAreaWnd::OnStyleChanging (int nStyleType, LPSTYLESTRUCT lpStyleStruct)
{
	if (nStyleType == GWL_EXSTYLE && !IsKeepClientEdge ())
	{
		lpStyleStruct->styleNew = lpStyleStruct->styleOld & ~WS_EX_CLIENTEDGE;
	}
	
	CWnd::OnStyleChanging (nStyleType, lpStyleStruct);
}
//****
BOOL CBCGPMainClientAreaWnd::IsKeepClientEdge () 
{
	BOOL bKeepEdge = FALSE;
	HWND hwndActive = (HWND) SendMessage (WM_MDIGETACTIVE, 0, 0);
	if (hwndActive != NULL)
	{
		CWnd* pWnd = CWnd::FromHandle (hwndActive);
		if (pWnd != NULL && ::IsWindow (pWnd->GetSafeHwnd ()))
		{
			bKeepEdge = (pWnd->GetStyle () & WS_SYSMENU) != 0;
		}
	}

	return !m_bIsMDITabbedGroup && bKeepEdge;
}
//****
LRESULT CBCGPMainClientAreaWnd::OnGetDragBounds (WPARAM wp, LPARAM lp)
{
	if (!m_bIsMDITabbedGroup || m_lstTabbedGroups.IsEmpty ())
	{
		return 0;
	}

	CBCGPTabWnd* pTabWndToResize = (CBCGPTabWnd*) (wp);
	LPRECT lpRectBounds = (LPRECT) (lp);

	if (pTabWndToResize == NULL)
	{
		return 0;
	}

	ASSERT_VALID (pTabWndToResize);
	CBCGPTabWnd* pNextTabWnd = GetNextTabWnd (pTabWndToResize);

	if (pNextTabWnd == NULL)
	{
		return 0;
	}

	ASSERT (m_groupAlignment != GROUP_NO_ALIGN);

	CRect rectTabWndToResize;
	CRect rectNextTabWnd;

	pTabWndToResize->GetWindowRect (rectTabWndToResize);
	pNextTabWnd->GetWindowRect (rectNextTabWnd);

	rectTabWndToResize.UnionRect (rectTabWndToResize, rectNextTabWnd);

	if (m_groupAlignment == GROUP_VERT_ALIGN)
	{
		rectTabWndToResize.left += m_nResizeMargin;
		rectTabWndToResize.right -= m_nResizeMargin;
	}
	else
	{
		rectTabWndToResize.top += m_nResizeMargin;
		rectTabWndToResize.bottom -= m_nResizeMargin;
	}

	CopyRect (lpRectBounds, &rectTabWndToResize);
	return TRUE;
}
//****
LRESULT CBCGPMainClientAreaWnd::OnDragComplete (WPARAM wp, LPARAM lp)
{
	if (!m_bIsMDITabbedGroup || m_lstTabbedGroups.IsEmpty ())
	{
		return 0;
	}
	CBCGPTabWnd* pTabWndToResize = (CBCGPTabWnd*) (wp);
	LPRECT lpRectResized = (LPRECT) (lp);

	ASSERT_VALID (pTabWndToResize);
	CBCGPTabWnd* pNextTabWnd = GetNextTabWnd (pTabWndToResize);

	if (pNextTabWnd == NULL)
	{
		return 0;
	}


	ASSERT (m_groupAlignment != GROUP_NO_ALIGN);

	ScreenToClient (lpRectResized);
	pTabWndToResize->SetWindowPos (NULL, -1, -1, 
								   lpRectResized->right - lpRectResized->left,
								   lpRectResized->bottom - lpRectResized->top, 
								   SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);

	CRect rectNextWnd;
	pNextTabWnd->GetWindowRect (rectNextWnd);
	ScreenToClient (rectNextWnd);

	m_groupAlignment == GROUP_VERT_ALIGN ?  rectNextWnd.left = lpRectResized->right : 
											rectNextWnd.top = lpRectResized->bottom;

	pNextTabWnd->SetWindowPos (NULL, rectNextWnd.left, rectNextWnd.top, 
							   rectNextWnd.Width (), rectNextWnd.Height (), 
							   SWP_NOZORDER | SWP_NOACTIVATE);

	AdjustMDIChildren (pTabWndToResize);
	AdjustMDIChildren (pNextTabWnd);

	return TRUE;
}
//****
LRESULT CBCGPMainClientAreaWnd::OnTabGroupMouseMove (WPARAM /*wp*/, LPARAM lp)
{
	CBCGPTabWnd* pTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, GetCapture ());
	if (pTabWnd == NULL)
	{
		return 0;
	}

	if (m_lstTabbedGroups.GetCount () == 1 && pTabWnd->GetTabsNum () == 1)
	{
		return 0;
	}

	POINTS pt = MAKEPOINTS (lp);
	CPoint point (pt.x, pt.y); 

	if (pTabWnd->IsPtInTabArea (point))
	{
		::SetCursor (AfxGetApp ()->LoadStandardCursor (IDC_ARROW));
		DrawNewGroupRect (NULL, m_rectNewTabGroup);
		m_rectNewTabGroup.SetRectEmpty ();
		return 0;
	}

	CPoint pointScreen = point;
	pTabWnd->ClientToScreen (&pointScreen);

	CRect rectWnd;
	GetClientRect (rectWnd);

	ClientToScreen (rectWnd);

	if (globalData.m_hcurMoveTab == NULL)
	{
		CBCGPLocalResource locaRes;

		globalData.m_hcurMoveTab = AfxGetApp ()->LoadCursor (IDC_BCGBARRES_MOVE_TAB);
		globalData.m_hcurNoMoveTab = AfxGetApp ()->LoadCursor (IDC_BCGBARRES_NO_MOVE_TAB);
	}

	if (!rectWnd.PtInRect (pointScreen))
	{
		::SetCursor (globalData.m_hcurNoMoveTab);

		DrawNewGroupRect (NULL, m_rectNewTabGroup);
		m_rectNewTabGroup.SetRectEmpty ();
		return TRUE;
	}

	::SetCursor (globalData.m_hcurMoveTab);

	CBCGPTabWnd* pHoveredTabWnd = TabWndFromPoint (pointScreen);

	if (pHoveredTabWnd == NULL)
	{
		DrawNewGroupRect (NULL, m_rectNewTabGroup);
		m_rectNewTabGroup.SetRectEmpty ();
		return 0;
	}

	CRect rectScreenHoveredWnd;
	pHoveredTabWnd->GetWindowRect (rectScreenHoveredWnd);

	CRect rectMargin = rectScreenHoveredWnd;

	BOOL bCalcVertRect = TRUE;

	if (m_groupAlignment == GROUP_NO_ALIGN)
	{
		bCalcVertRect = rectScreenHoveredWnd.right - pointScreen.x < 
						rectScreenHoveredWnd.bottom - pointScreen.y;
	}
	else
	{
		bCalcVertRect = m_groupAlignment == GROUP_VERT_ALIGN;
	}

	if (m_groupAlignment == GROUP_VERT_ALIGN || bCalcVertRect)
	{
		rectMargin.left = rectScreenHoveredWnd.right - m_nNewGroupMargin;
		bCalcVertRect = TRUE;
	}
	else if (m_groupAlignment == GROUP_HORZ_ALIGN || !bCalcVertRect)
	{
		rectMargin.top  = rectScreenHoveredWnd.bottom - m_nNewGroupMargin; 
		bCalcVertRect = FALSE;
	}

	CRect rectNew = rectScreenHoveredWnd;

	bCalcVertRect ? rectNew.left = rectScreenHoveredWnd.right - rectScreenHoveredWnd.Width () / 2:
					rectNew.top = rectScreenHoveredWnd.bottom - rectScreenHoveredWnd.Height () / 2; 

	if (!rectMargin.PtInRect (pointScreen)) 
	{
		if (pHoveredTabWnd == pTabWnd)
		{
			rectNew.SetRectEmpty ();
		}
		else
		{
			CPoint pointClient = pointScreen;
			pHoveredTabWnd->ScreenToClient (&pointClient);
			if (pHoveredTabWnd->IsPtInTabArea (pointClient))
			{
				pHoveredTabWnd->GetWndArea (rectNew);
				pHoveredTabWnd->ClientToScreen (rectNew);
			}
			else
			{
				rectNew.SetRectEmpty ();
			}
		}
	}
	else if (pHoveredTabWnd == pTabWnd && pTabWnd->GetTabsNum () == 1)
	{
		rectNew.SetRectEmpty ();
	}

	DrawNewGroupRect (rectNew, m_rectNewTabGroup);
	m_rectNewTabGroup = rectNew;
	m_bNewVericalGroup = bCalcVertRect;

	return TRUE;
}
//****
LRESULT CBCGPMainClientAreaWnd::OnMoveTabComplete(WPARAM wp, LPARAM lp)
{
	CBCGPTabWnd* pTabWnd = (CBCGPTabWnd*) wp;

	CRect rectNewTabGroup = m_rectNewTabGroup;
	DrawNewGroupRect (NULL, m_rectNewTabGroup);
	m_rectNewTabGroup.SetRectEmpty ();

	if (pTabWnd == NULL)
	{
		return 0;
	}

	ASSERT_VALID (pTabWnd);

	POINTS pt = MAKEPOINTS (lp);
	CPoint point (pt.x, pt.y); 

	CPoint pointScreen = point;
	pTabWnd->ClientToScreen (&pointScreen);


	CBCGPTabWnd* pHoveredTabWnd = TabWndFromPoint (pointScreen);

	if (pHoveredTabWnd == NULL)
	{
		return 0;
	}

	ASSERT_VALID (pHoveredTabWnd);

	BOOL bMenuResult = TRUE;
	if (rectNewTabGroup.IsRectEmpty ())
	{
		CBCGPMDIFrameWnd* pMDIFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParent ());
		ASSERT_VALID (pMDIFrame);

		CPoint point;
		GetCursorPos (&point);

		DWORD dwAllowedItems = GetMDITabsContextMenuAllowedItems ();

		if (dwAllowedItems != 0)
		{
			bMenuResult = pMDIFrame->OnShowMDITabContextMenu (point, dwAllowedItems, TRUE);
		}
	}

	CRect rectHoveredWnd;
	pHoveredTabWnd->GetWndArea (rectHoveredWnd);
	pHoveredTabWnd->ClientToScreen (rectHoveredWnd);

	if (!bMenuResult)
	{
		if (pTabWnd != pHoveredTabWnd)
		{
			MoveWindowToTabGroup (pTabWnd, pHoveredTabWnd);
		}
		else if (pTabWnd->GetTabsNum () > 1)
		{
			BOOL bVertGroup = FALSE;
			if (m_groupAlignment == GROUP_HORZ_ALIGN)
			{
				rectHoveredWnd.top = rectHoveredWnd.bottom - rectHoveredWnd.Height () / 2;
			}
			else
			{
				rectHoveredWnd.left = rectHoveredWnd.right - rectHoveredWnd.Width () / 2;
				bVertGroup = TRUE;
			}

			CBCGPTabWnd* pNewTabWnd = CreateNewTabGroup (pHoveredTabWnd, 
					rectHoveredWnd, bVertGroup); 
			MoveWindowToTabGroup (pTabWnd, pNewTabWnd);
		}
	}
	else if (!rectNewTabGroup.IsRectEmpty ())
	{
		if (rectNewTabGroup == rectHoveredWnd)
		{
			MoveWindowToTabGroup (pTabWnd, pHoveredTabWnd);
		}
		else
		{
			CBCGPTabWnd* pNewTabWnd = CreateNewTabGroup (pHoveredTabWnd, 
					rectNewTabGroup, m_bNewVericalGroup); 
			MoveWindowToTabGroup (pTabWnd, pNewTabWnd);
		}
	}

	
	return 0;
}
//****
BOOL CBCGPMainClientAreaWnd::MoveWindowToTabGroup (CBCGPTabWnd* pTabWndFrom, CBCGPTabWnd* pTabWndTo, int nIdxFrom)
{
	ASSERT_VALID (pTabWndFrom);
	ASSERT_VALID (pTabWndTo);

	HWND hwndFrom = pTabWndFrom->GetSafeHwnd ();
	HWND hwndTo	  = pTabWndTo->GetSafeHwnd ();

	int nIdx = nIdxFrom;
	if (nIdx == -1)
	{
		nIdx = pTabWndFrom->GetActiveTab ();
	}

	if (nIdx == -1)
	{
		return FALSE;
	}

	CBCGPMDIChildWnd* pWnd = DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, pTabWndFrom->GetTabWnd (nIdx));

	if (pWnd == NULL)
	{
		return FALSE;
	}

	ASSERT_VALID (pWnd);

	CString strTabLabel = pWnd->GetFrameText ();		

	pTabWndFrom->RemoveTab (nIdx, TRUE);
	pWnd->SetRelatedTabGroup (pTabWndTo);
	pTabWndTo->AddTab (pWnd, strTabLabel);
	

	if (pTabWndFrom->GetTabsNum () == 0)
	{
		RemoveTabGroup (pTabWndFrom);
		UpdateMDITabbedGroups(TRUE);
		pTabWndTo->RecalcLayout ();
	}
	else
	{
		AdjustMDIChildren (pTabWndFrom);
	}

	AdjustMDIChildren (pTabWndTo);
	SetActiveTab (pWnd->GetSafeHwnd ());

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParentFrame ());
	ASSERT_VALID (pMainFrame);

	if (pMainFrame != NULL)
	{
		pMainFrame->SendMessage (BCGM_ON_MOVETOTABGROUP, (WPARAM) hwndFrom, (LPARAM) hwndTo);
	}

	return TRUE;
}
//****
CBCGPTabWnd* CBCGPMainClientAreaWnd::CreateNewTabGroup (CBCGPTabWnd* pTabWndAfter, CRect rectGroup, 
												BOOL bVertical)
{
	ASSERT_VALID (pTabWndAfter);
	POSITION pos = m_lstTabbedGroups.Find (pTabWndAfter);

	if (pos == NULL)
	{
		return NULL;
	}

	CBCGPTabWnd* pNewTabWnd = CreateTabGroup (NULL);
	pTabWndAfter->SetResizeMode (bVertical ? CBCGPTabWnd::RESIZE_VERT : CBCGPTabWnd::RESIZE_HORIZ);

	m_lstTabbedGroups.InsertAfter (pos, pNewTabWnd);
	if (pNewTabWnd != m_lstTabbedGroups.GetTail ())
	{
		pNewTabWnd->SetResizeMode (bVertical ? CBCGPTabWnd::RESIZE_VERT : CBCGPTabWnd::RESIZE_HORIZ);
	}

	m_groupAlignment = bVertical ? GROUP_VERT_ALIGN : GROUP_HORZ_ALIGN;

	CRect rectWndAfter;
	pTabWndAfter->GetWindowRect (rectWndAfter);

	ScreenToClient (rectGroup);
	ScreenToClient (rectWndAfter);
	
	if (bVertical)		
	{
		rectWndAfter.right -= rectGroup.Width ();
		rectGroup.top = rectWndAfter.top;
		rectGroup.bottom = rectWndAfter.bottom;
	}
	else
	{
		rectWndAfter.bottom -= rectGroup.Height ();
		rectGroup.left = rectWndAfter.left;
		rectGroup.right = rectWndAfter.right;
	}

	pTabWndAfter->SetWindowPos (NULL, -1, -1, rectWndAfter.Width (), rectWndAfter.Height (),
									SWP_NOZORDER | SWP_NOMOVE | SWP_NOACTIVATE);
	pNewTabWnd->SetWindowPos (NULL, rectGroup.left, rectGroup.top, 
									rectGroup.Width (), rectGroup.Height (),
									SWP_NOZORDER |  SWP_NOACTIVATE);

	AdjustMDIChildren (pTabWndAfter);

	return pNewTabWnd;
}
//****
void CBCGPMainClientAreaWnd::RemoveTabGroup (CBCGPTabWnd* pTabWnd, BOOL /*bRecalcLayout*/)
{
	ASSERT_VALID (pTabWnd);
	ASSERT (pTabWnd->GetTabsNum () == 0);

	POSITION pos = m_lstTabbedGroups.Find (pTabWnd);
	if (pos == NULL)
	{
		TRACE0 ("Attempt to remove non-existing tab group");
		return;
	}

	CBCGPTabWnd* pSiblingTabWndToResize = NULL;
	POSITION posNextPrev = pos;
	BOOL bNext = FALSE;

	if (m_lstTabbedGroups.GetHeadPosition () == pos)
	{
		m_lstTabbedGroups.GetNext (posNextPrev);
		bNext = TRUE;
	}
	else
	{
		m_lstTabbedGroups.GetPrev (posNextPrev);
	}

	if (posNextPrev != NULL)
	{
		pSiblingTabWndToResize = DYNAMIC_DOWNCAST (CBCGPTabWnd, 
										m_lstTabbedGroups.GetAt (posNextPrev));
	}

	
	m_lstTabbedGroups.RemoveAt (pos);
	pTabWnd->ShowWindow (SW_HIDE);

	if (m_lstTabbedGroups.GetCount () > 0)
	{
		CBCGPTabWnd* pLastTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetTail ());
		ASSERT_VALID (pLastTabWnd);
		pLastTabWnd->SetResizeMode (CBCGPTabWnd::RESIZE_NO);
	}

	if (m_lstTabbedGroups.GetCount () <= 1)
	{
		m_groupAlignment = GROUP_NO_ALIGN;
	}

	if (pSiblingTabWndToResize != NULL)
	{
		CRect rectTabWndToRemove;
		pTabWnd->GetWindowRect (rectTabWndToRemove);
		CRect rectSiblingWnd;
		pSiblingTabWndToResize->GetWindowRect (rectSiblingWnd);
		
		rectSiblingWnd.UnionRect (rectSiblingWnd, rectTabWndToRemove);
		ScreenToClient (rectSiblingWnd);
		pSiblingTabWndToResize->SetWindowPos (NULL, rectSiblingWnd.left, rectSiblingWnd.top, 
							rectSiblingWnd.Width (), rectSiblingWnd.Height (), 
							SWP_NOZORDER);
		
		AdjustMDIChildren (pSiblingTabWndToResize);
	}

	CImageList* pImageList = NULL;
	if (m_mapTabIcons.Lookup (pTabWnd, pImageList) && 
		pImageList != NULL)
	{
		delete pImageList;
		m_mapTabIcons.RemoveKey (pTabWnd);
	}

	m_lstRemovedTabbedGroups.AddTail (pTabWnd);
	pTabWnd->ShowWindow(SW_HIDE);

	return;
}
//****
LRESULT CBCGPMainClientAreaWnd::OnCancelTabMove (WPARAM, LPARAM)
{
	DrawNewGroupRect (NULL, m_rectNewTabGroup);
	m_rectNewTabGroup.SetRectEmpty ();
	return 0;
}
//****
DWORD CBCGPMainClientAreaWnd::GetMDITabsContextMenuAllowedItems ()
{
	CBCGPTabWnd* pActiveTabWnd = FindActiveTabWndByActiveChild ();
	if (pActiveTabWnd == NULL)
	{
		return 0;
	}

	DWORD dwAllowedItems = 0;
	int nTabCount = pActiveTabWnd->GetTabsNum ();

	if (nTabCount > 1)
	{
		if (m_lstTabbedGroups.GetCount () > 1)
		{
			dwAllowedItems = (m_groupAlignment == GROUP_VERT_ALIGN) ? 
										 BCGP_MDI_CREATE_VERT_GROUP : 
										 BCGP_MDI_CREATE_HORZ_GROUP;
		}
		else
		{
			dwAllowedItems = BCGP_MDI_CREATE_VERT_GROUP | BCGP_MDI_CREATE_HORZ_GROUP;
		}
	}

	if (pActiveTabWnd != m_lstTabbedGroups.GetHead ())
	{
		dwAllowedItems |= BCGP_MDI_CAN_MOVE_PREV;
	}

	if (pActiveTabWnd != m_lstTabbedGroups.GetTail ())
	{
		dwAllowedItems |= BCGP_MDI_CAN_MOVE_NEXT;
	}

	CBCGPMDIChildWnd* pMDIChildFrame = DYNAMIC_DOWNCAST (
		CBCGPMDIChildWnd, pActiveTabWnd->GetActiveWnd ());

	CBCGPMDIFrameWnd* pMainFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParentFrame ());
	if (pMDIChildFrame != NULL && pMDIChildFrame->IsTabbedControlBar () && pMainFrame != NULL && 
		!pMainFrame->IsFullScreen ())
	{
		dwAllowedItems |= BCGP_MDI_CAN_BE_DOCKED;
	}

	return dwAllowedItems;
}
//****
CBCGPTabWnd* CBCGPMainClientAreaWnd::FindActiveTabWndByActiveChild ()
{
	HWND hwndActive = (HWND) SendMessage (WM_MDIGETACTIVE, 0, 0);
	if (hwndActive == NULL)
	{
		return NULL;
	}

	for (POSITION pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextTabWnd);
		if (pNextTabWnd->GetTabFromHwnd (hwndActive) >= 0)
		{
			return pNextTabWnd;
		}
	}
	return NULL;
}
//****
CBCGPTabWnd* CBCGPMainClientAreaWnd::FindActiveTabWnd ()
{
	for (POSITION pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextTabWnd);
		if (pNextTabWnd->IsActiveInMDITabGroup ())
		{
			return pNextTabWnd;
		}
	}
	return NULL;
}
//****
CBCGPTabWnd* CBCGPMainClientAreaWnd::GetFirstTabWnd ()
{
	if (m_lstTabbedGroups.IsEmpty ())
	{
		return NULL;
	}
	return DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetHead ());
}
//****
BOOL CBCGPMainClientAreaWnd::IsMemberOfMDITabGroup (CWnd* pWnd)
{
	if (!IsMDITabbedGroup ())
	{
		return FALSE;
	}

	return (m_lstTabbedGroups.Find (pWnd) != NULL);
}
//****
CBCGPTabWnd* CBCGPMainClientAreaWnd::GetNextTabWnd (CBCGPTabWnd* pOrgTabWnd, BOOL /*bWithoutAsserts*/)
{
	POSITION pos = m_lstTabbedGroups.Find (pOrgTabWnd);

	if (pos == NULL)
	{
		ASSERT(FALSE);
		TRACE0("Trying to resize a member of tabbed group which is not in the list of groups.\n");
		return NULL;
	}
	
	m_lstTabbedGroups.GetNext (pos);
	if (pos == NULL)
	{
		ASSERT(FALSE);
		TRACE0("Trying to resize a last member of tabbed group, which should not be resizable.\n");
		return NULL;
	}

	CBCGPTabWnd* pNextTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetAt (pos));

	if (pNextTabWnd == NULL)
	{
		ASSERT(FALSE);
		TRACE0("Next member of tabbed group is NULL, or not a tab window.\n");
		return NULL;
	}

	return pNextTabWnd;
}
//****
CBCGPTabWnd* CBCGPMainClientAreaWnd::TabWndFromPoint (CPoint ptScreen)
{
	for (POSITION pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
	{
		CBCGPTabWnd* pNextTab = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
		ASSERT_VALID (pNextTab);
		CRect rectWnd;
		pNextTab->GetWindowRect (rectWnd);
		if (rectWnd.PtInRect (ptScreen))
		{
			return pNextTab;
		}
	}
	return NULL;
}
//****
void CBCGPMainClientAreaWnd::DrawNewGroupRect (LPCRECT lpRectNew, LPCRECT lpRectOld)
{
	CWindowDC dc (GetDesktopWindow ());
	CSize size (4, 4);
	CRect rectNew; rectNew.SetRectEmpty ();
	CRect rectOld; rectOld.SetRectEmpty ();
	if (lpRectNew != NULL)
	{
		rectNew = lpRectNew;
	}
	if (lpRectOld != NULL)
	{
		rectOld = lpRectOld;
	}
	dc.DrawDragRect (rectNew, size, rectOld, size);
}
//****
void CBCGPMainClientAreaWnd::MDITabMoveToNextGroup (BOOL bNext) 
{
	CBCGPTabWnd* pActiveWnd = FindActiveTabWndByActiveChild ();
	if (pActiveWnd == NULL)
	{
		return;
	}
	ASSERT_VALID (pActiveWnd);

	POSITION pos = m_lstTabbedGroups.Find (pActiveWnd);
	bNext ? m_lstTabbedGroups.GetNext (pos) : m_lstTabbedGroups.GetPrev (pos);

	if (pos == NULL)
	{
		return;
	}

	CBCGPTabWnd* pNextTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetAt (pos));
	ASSERT_VALID (pNextTabWnd);
	
	MoveWindowToTabGroup (pActiveWnd, pNextTabWnd);
}
//****
void CBCGPMainClientAreaWnd::MDITabNewGroup (BOOL bVert) 
{
	CBCGPTabWnd* pActiveWnd = FindActiveTabWndByActiveChild ();
	if (pActiveWnd == NULL)
	{
		return;
	}
	ASSERT_VALID (pActiveWnd);

	CRect rect;
	pActiveWnd->GetWindowRect (rect);

	if (bVert)
	{
		rect.left += rect.Width () / 2;
	}
	else
	{
		rect.top += rect.Height () / 2;
	}

	CBCGPTabWnd* pNewTabWnd = CreateNewTabGroup (pActiveWnd, rect, bVert); 
	MoveWindowToTabGroup (pActiveWnd, pNewTabWnd);
}
//****
void CBCGPMainClientAreaWnd::CloseAllWindows (CBCGPTabWnd* pTabWnd)
{
	if (pTabWnd != NULL)
	{
		ASSERT_VALID (pTabWnd);

		for (int i = pTabWnd->GetTabsNum () - 1; i >= 0; i--)
		{
			CBCGPMDIChildWnd* pNextWnd = DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, pTabWnd->GetTabWnd (i));
			if (pNextWnd != NULL)
			{
				ASSERT_VALID (pNextWnd);
				pNextWnd->SendMessage (WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
			}
		}
	}
	else
	{
		CObList lstWindows;
		CWnd* pWndChild = GetWindow (GW_CHILD);

		while (pWndChild != NULL)
		{
			ASSERT_VALID (pWndChild);

			CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pWndChild);
			if (pMDIChild != NULL)
			{
				ASSERT_VALID (pMDIChild);
				lstWindows.AddTail (pMDIChild);
			}

			pWndChild = pWndChild->GetNextWindow ();
		}

		for (POSITION pos = lstWindows.GetHeadPosition (); pos != NULL;)
		{
			CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, 
															lstWindows.GetNext (pos));
			ASSERT_VALID (pMDIChild);
			pMDIChild->SendMessage (WM_CLOSE, (WPARAM) 0, (LPARAM) 0);
		}

	}
}
//****
void CBCGPMainClientAreaWnd::SerializeTabGroup (CArchive& ar, CBCGPTabWnd* pTabWnd, BOOL bSetRelation)
{
	ASSERT_VALID (pTabWnd);
	if (ar.IsStoring ())
	{
		int nTabsNum = pTabWnd->GetTabsNum ();
		ar << nTabsNum;

		int nActiveTab = pTabWnd->GetActiveTab ();
		ar << nActiveTab;
		ar << pTabWnd->IsActiveInMDITabGroup ();

		int i = 0;

		for (i = 0; i < nTabsNum; i++)
		{
			CBCGPMDIChildWnd* pNextWnd = DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, pTabWnd->GetTabWnd (i));
			ASSERT_VALID (pNextWnd);

			CObject* pObject = NULL;
			CString strDocumentName = pNextWnd->GetDocumentName (&pObject);
			ar << strDocumentName;

			BOOL bObjPresent = (pObject != NULL);
			ar << bObjPresent;
			if (bObjPresent)
			{
				ar << pObject;
				delete pObject;
			}

			CString strLabel; 
			pTabWnd->GetTabLabel (i, strLabel);
			ar << strLabel;

			ar << pTabWnd->GetResizeMode ();
			ar << pTabWnd->GetTabBkColor (i);

			int nBarID = -1;
			
			if (pNextWnd->IsTabbedControlBar ())
			{
				CBCGPDockingControlBar* pBar = pNextWnd->GetTabbedControlBar ();
				if (pBar != NULL && pBar->GetSafeHwnd () != NULL)
				{
					nBarID = pBar->GetDlgCtrlID ();
				}
			}

			ar << nBarID;
		}

		ar << pTabWnd->IsAutoColor ();

		const CArray<COLORREF, COLORREF>& arColors = pTabWnd->GetAutoColors (); 
		ar << (int) arColors.GetSize ();

		for (i = 0; i < arColors.GetSize (); i++)
		{
			ar << arColors [i];
		}

		ar << pTabWnd->IsTabDocumentsMenu ();
		ar << pTabWnd->IsTabSwapEnabled ();
		ar << pTabWnd->GetTabBorderSize ();

		CRect rectWindow;
		pTabWnd->GetWindowRect (rectWindow);
		ar << rectWindow;
	}
	else
	{
		int nTabsNum = 0;
		ar >> nTabsNum;

		int nActiveTab = -1;
		ar >> nActiveTab;

		BOOL bIsActiveInMDITabGroup = FALSE;
		ar >> bIsActiveInMDITabGroup;

		if (bIsActiveInMDITabGroup)
		{
			CBCGPTabWnd* pPrevActiveWnd = FindActiveTabWnd ();
			if (pPrevActiveWnd != NULL)
			{
				pPrevActiveWnd->SetActiveInMDITabGroup (FALSE);
				pPrevActiveWnd->InvalidateTab (pPrevActiveWnd->GetActiveTab ());
			}
		}
		pTabWnd->SetActiveInMDITabGroup (bIsActiveInMDITabGroup);

		CBCGPMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParent ());
		ASSERT_VALID (pFrame);

		int i = 0;

		for (i = 0; i < nTabsNum; i++)
		{
			CString strDocumentName;
			ar >> strDocumentName;

			BOOL bObjectPresent = FALSE;
			CObject* pObj = NULL;

			ar >> bObjectPresent;
			if (bObjectPresent)
			{
				ar >> pObj;
			}

			CString strLabel; 
			ar >> strLabel;

			int nValue;
			ar >> nValue;
			pTabWnd->SetResizeMode ((CBCGPTabWnd::ResizeMode) nValue);

			COLORREF clrTab = (COLORREF) -1;
			ar >> clrTab;

			int nBarID = -1;
			if (g_pWorkspace != NULL && g_pWorkspace->GetDataVersion () >= 0x90200)
			{
				ar >> nBarID; 
			}


			CBCGPMDIChildWnd* pNextWnd = NULL;

			if (!strDocumentName.IsEmpty ())
			{
				if (m_lstLoadedTabDocuments.Find (strDocumentName) == NULL)
				{
					pNextWnd = pFrame->CreateDocumentWindow (strDocumentName, pObj);
					m_lstLoadedTabDocuments.AddTail (strDocumentName);
				}
				else
				{
					pNextWnd = pFrame->CreateNewWindow (strDocumentName, pObj);
				}
			}
			else if (nBarID != -1)
			{

				CBCGPDockingControlBar* pBar = DYNAMIC_DOWNCAST (CBCGPDockingControlBar, pFrame->GetControlBar (nBarID));
				if (pBar != NULL)
				{
					CBCGPBaseTabbedBar* pTabbedBar = pBar->GetParentTabbedBar ();
					if (pTabbedBar != NULL)
					{
						pBar->StoreRecentTabRelatedInfo ();
					}
					pNextWnd = pFrame->ControlBarToTabbedDocument (pBar);
					if (pTabbedBar != NULL)
					{
						pTabbedBar->RemoveControlBar (pBar);
					}
				}
			}

			if (pNextWnd != NULL)
			{
				ASSERT_VALID (pNextWnd);
				pTabWnd->AddTab (pNextWnd, strLabel);
				pTabWnd->SetTabBkColor (i, clrTab);
				if (bSetRelation)
				{
					pNextWnd->SetRelatedTabGroup (pTabWnd);
				}
				
			}
			
			if (pObj != NULL)
			{
				delete pObj;
			}
		}

		BOOL bIsAutoColor = FALSE;
		ar >> bIsAutoColor;

		int nAutoColorSize = 0;
		ar >> nAutoColorSize;

		CArray<COLORREF, COLORREF> arColors;
		
		for (i = 0; i < nAutoColorSize; i++)
		{
			COLORREF clr = (COLORREF) -1;
			ar >> clr;
			arColors.SetAtGrow (i, clr);
		}

		pTabWnd->EnableAutoColor (bIsAutoColor);
		pTabWnd->SetAutoColors (arColors);
		m_mdiTabParams.m_bAutoColor = bIsAutoColor;

		BOOL bValue = FALSE;

		ar >> bValue; 
		pTabWnd->EnableTabDocumentsMenu (bValue);
		m_mdiTabParams.m_bDocumentMenu = bValue;

		ar >> bValue;
		pTabWnd->EnableTabSwap (bValue);
		m_mdiTabParams.m_bEnableTabSwap = bValue;

		int nTabBorderSize = 1;
		ar >> nTabBorderSize; 
		pTabWnd->SetTabBorderSize (nTabBorderSize);
		m_mdiTabParams.m_nTabBorderSize = nTabBorderSize;

		CRect rectWindow;
		ar >> rectWindow;

		pTabWnd->GetParent ()->ScreenToClient (rectWindow);

		pTabWnd->SetWindowPos (NULL, rectWindow.left, rectWindow.right, 
									 rectWindow.Width (), rectWindow.Height (), 
									 SWP_NOZORDER | SWP_NOACTIVATE);

		if (pTabWnd->GetTabsNum () > 0)
		{
			if (nActiveTab > pTabWnd->GetTabsNum () - 1)
			{
				nActiveTab = pTabWnd->GetTabsNum () - 1;
			}

			pTabWnd->SetActiveTab (nActiveTab);
		}
	}
}
//****
void CBCGPMainClientAreaWnd::SerializeOpenChildren (CArchive& ar)
{
	if (ar.IsStoring ())
	{
		CObList lstWindows;

		CWnd* pWndChild = GetWindow (GW_CHILD);
		while (pWndChild != NULL)
		{
			ASSERT_VALID (pWndChild);

			CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, pWndChild);
			if (pMDIChild != NULL)
			{
				ASSERT_VALID (pWndChild);

				CObject* pObj = NULL;
				CString str = pMDIChild->GetDocumentName (&pObj);
				if (pObj != NULL)
				{
					delete pObj;
				}
				if (!str.IsEmpty ())
				{
					lstWindows.AddHead (pMDIChild);
				}
			}

			pWndChild = pWndChild->GetNextWindow ();
		}

		HWND hwndActive = (HWND) SendMessage (WM_MDIGETACTIVE);

		ar << (int) lstWindows.GetCount ();

		for (POSITION pos = lstWindows.GetHeadPosition (); pos != NULL;)
		{
			CBCGPMDIChildWnd* pMDIChild = DYNAMIC_DOWNCAST(CBCGPMDIChildWnd, 
															lstWindows.GetNext (pos));
			
			ASSERT_VALID (pMDIChild);

			CObject* pObj = NULL;
			CString str = pMDIChild->GetDocumentName (&pObj);
			ar << str;

			BOOL bObjPresent = (pObj != NULL);
			ar << bObjPresent;
			if (bObjPresent)
			{
				ar << pObj;
				delete pObj;
			}

			WINDOWPLACEMENT wp;
			pMDIChild->GetWindowPlacement (&wp);

			ar << wp.flags;
			ar << wp.length;
			ar << wp.ptMaxPosition;
			ar << wp.ptMinPosition;
			ar << wp.rcNormalPosition;
			ar << wp.showCmd;

			BOOL bActive = (pMDIChild->GetSafeHwnd () == hwndActive);
			ar << bActive;

			int nBarID = -1;
			
			if (pMDIChild->IsTabbedControlBar ())
			{
				CBCGPDockingControlBar* pBar = pMDIChild->GetTabbedControlBar ();
				if (pBar != NULL && pBar->GetSafeHwnd () != NULL)
				{
					nBarID = pBar->GetDlgCtrlID ();
				}
			}

			ar << nBarID;
		}
		
	}
	else
	{
		CBCGPMDIFrameWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPMDIFrameWnd, GetParent ());
		ASSERT_VALID (pFrame);


		int nCount = 0;
		ar >> nCount;

		HWND hwndActive = NULL;
		BOOL bMaximize = FALSE;
		for (int i = 0; i < nCount; i++)
		{
			CString strDocName;
			ar >> strDocName;

			BOOL bObjPresent = FALSE;
			CObject* pObj = NULL;

			ar >> bObjPresent;
			if (bObjPresent)
			{
				ar >> pObj;
			}

			WINDOWPLACEMENT wp;
			ar >> wp.flags;
			ar >> wp.length;
			ar >> wp.ptMaxPosition;
			ar >> wp.ptMinPosition;
			ar >> wp.rcNormalPosition;
			ar >> wp.showCmd;

			BOOL bActive = FALSE;
			ar >> bActive;

			int nBarID = -1;
			if (g_pWorkspace != NULL && g_pWorkspace->GetDataVersion () >= 0x90200)
			{
				ar >> nBarID; 
			}


			if (bMaximize)
			{
				wp.showCmd = SW_SHOWMAXIMIZED;
			}

			CBCGPMDIChildWnd* pNextWnd = NULL;



			if (!strDocName.IsEmpty () && nBarID == -1)
			{
				pNextWnd = pFrame->CreateNewWindow (strDocName, pObj);
			}
			else if (nBarID != -1)
			{

				CBCGPDockingControlBar* pBar = DYNAMIC_DOWNCAST (CBCGPDockingControlBar, pFrame->GetControlBar (nBarID));
				if (pBar != NULL)
				{
					CBCGPBaseTabbedBar* pTabbedBar = pBar->GetParentTabbedBar ();
					if (pTabbedBar != NULL)
					{
						pBar->StoreRecentTabRelatedInfo ();
					}
					pNextWnd = pFrame->ControlBarToTabbedDocument (pBar);
					if (pTabbedBar != NULL)
					{
						pTabbedBar->RemoveControlBar (pBar);
					}
				}
			}

			if (pObj != NULL)
			{
				delete pObj;
			}
			if (pNextWnd != NULL)
			{
				ASSERT_VALID (pNextWnd);

				pNextWnd->SetWindowPlacement (&wp);	
				if (wp.showCmd == SW_SHOWMAXIMIZED)
				{
					ShowWindow (wp.showCmd);
					bMaximize = TRUE;
				}

				if (bActive)
				{
					hwndActive = pNextWnd->GetSafeHwnd ();
				}
			}
		}

		if (hwndActive != NULL)
		{
			SendMessage (WM_MDIACTIVATE, (WPARAM) hwndActive);
		}
	}
	
}
//****
BOOL CBCGPMainClientAreaWnd::SaveState (LPCTSTR lpszProfileName, UINT nFrameID)
{
	BOOL bResult = FALSE;
	CString strProfileName = ::BCGPGetRegPath (strMDIClientAreaProfile, lpszProfileName);

	CString strSection;
	strSection.Format (REG_SECTION_FMT, strProfileName, nFrameID);
	
	try
	{
		CMemFile file;
		{
			CArchive ar (&file, CArchive::store);

			Serialize (ar);
			ar.Flush ();
		}

		UINT uiDataSize = (UINT) file.GetLength ();
		LPBYTE lpbData = file.Detach ();

		if (lpbData != NULL)
		{
			CBCGPRegistrySP regSP;
			CBCGPRegistry& reg = regSP.Create (FALSE, FALSE);

			if (reg.CreateKey (strSection))
			{
				bResult = reg.Write (REG_ENTRY_MDITABS_STATE, lpbData, uiDataSize);
			}

			free (lpbData);
		}
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T ("Memory exception in CBCGPMainClientAreaWnd::SaveState ()!\n"));
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		TRACE(_T ("CArchiveException exception in CBCGPMainClientAreaWnd::SaveState ()!\n"));
	}
	catch (...)
	{
		TRACE(_T ("Unknown exception in CBCGPMainClientAreaWnd::SaveState ()!\n"));
	}


	return bResult;
}
//****
BOOL CBCGPMainClientAreaWnd::LoadState (LPCTSTR lpszProfileName, UINT nFrameID)
{
	BOOL bResult = FALSE;

	CString strProfileName = ::BCGPGetRegPath (strMDIClientAreaProfile, lpszProfileName);

	CString strSection;
	strSection.Format (REG_SECTION_FMT, strProfileName, nFrameID);

	LPBYTE	lpbData = NULL;
	UINT	uiDataSize;

	CBCGPRegistrySP regSP;
	CBCGPRegistry& reg = regSP.Create (FALSE, TRUE);

	if (!reg.Open (strSection))
	{
		return FALSE;
	}

	if (!reg.Read (REG_ENTRY_MDITABS_STATE, &lpbData, &uiDataSize))
	{
		return FALSE;
	}

	try
	{
		CMemFile file (lpbData, uiDataSize);
		CArchive ar (&file, CArchive::load);

		Serialize (ar);
		bResult = TRUE;
	}
	catch (CMemoryException* pEx)
	{
		pEx->Delete ();
		TRACE(_T ("Memory exception in CBCGPMainClientAreaWnd::LoadState!\n"));
	}
	catch (CArchiveException* pEx)
	{
		pEx->Delete ();
		TRACE(_T ("CArchiveException exception in CBCGPMainClientAreaWnd::LoadState ()!\n"));
	}
	catch (...)
	{
		TRACE(_T ("Unknown exception in CBCGPMainClientAreaWnd::LoadState ()!\n"));
	}

	if (lpbData != NULL)
	{
		delete [] lpbData;
	}

	if (!bResult)
	{
		m_bDisableUpdateTabs = FALSE;
		CloseAllWindows (NULL);
	}

	return bResult;
}
//****
void CBCGPMainClientAreaWnd::Serialize (CArchive& ar)
{
	m_mdiTabParams.Serialize (ar);

	if (ar.IsStoring ())
	{
		ar << m_bTabIsEnabled;
		ar << m_bIsMDITabbedGroup;

		ar << m_bTabIsVisible;
		ar << m_groupAlignment;
		ar << m_nResizeMargin;
		ar << m_nNewGroupMargin;

		if (m_bTabIsEnabled)
		{
			SerializeTabGroup (ar, &m_wndTab);
		}
		else if (m_bIsMDITabbedGroup)
		{
			int nCountTabbedGroups = (int) m_lstTabbedGroups.GetCount ();
			ar << nCountTabbedGroups;

			if (nCountTabbedGroups > 0)
			{
				for (POSITION pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
				{
					CBCGPTabWnd* pNextWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
					SerializeTabGroup (ar, pNextWnd);
				}
			}
		}
		else
		{
			SerializeOpenChildren (ar);
		}
	}
	else if (ar.IsLoading ())
	{
		CloseAllWindows (NULL);
		m_lstLoadedTabDocuments.RemoveAll ();

		m_bDisableUpdateTabs = TRUE;

		ar >> m_bTabIsEnabled;
		ar >> m_bIsMDITabbedGroup;
		ar >> m_bTabIsVisible;

		int nValue;		
		ar >> nValue; 
		m_groupAlignment = (GROUP_ALIGNMENT) nValue;

		ar >> m_nResizeMargin;
		ar >> m_nNewGroupMargin;

		
		if (m_bTabIsEnabled)
		{
			SerializeTabGroup (ar, &m_wndTab);
			EnableMDITabs (TRUE, m_mdiTabParams);
		}
		else if (m_bIsMDITabbedGroup)
		{

			int nCountTabbedGroups = 0;
			ar >> nCountTabbedGroups;

			for (int i = 0; i < nCountTabbedGroups; i++)
			{
				CBCGPTabWnd* pNewTabWnd = CreateTabGroup (NULL);
				ASSERT_VALID (pNewTabWnd);
				SerializeTabGroup (ar, pNewTabWnd, TRUE);

				if (pNewTabWnd->GetTabsNum () == 0)
				{
					pNewTabWnd->DestroyWindow ();
					delete pNewTabWnd;
				}
				else
				{
					m_lstTabbedGroups.AddTail (pNewTabWnd);
				}
			}

			// snaity check for resize mode - the last group might have been removed
			// because the document could not be opened
			if (m_lstTabbedGroups.GetCount () > 0)
			{
				CBCGPTabWnd* pLastTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetTail ());
				ASSERT_VALID (pLastTabWnd);
				pLastTabWnd->SetResizeMode (CBCGPTabWnd::RESIZE_NO);
			}
		
			EnableMDITabbedGroups (TRUE, m_mdiTabParams);
		}
		else
		{
			SerializeOpenChildren (ar);
		}

		m_bDisableUpdateTabs = FALSE;

		if (m_bIsMDITabbedGroup)
		{
			UpdateMDITabbedGroups (TRUE);
			for (POSITION pos = m_lstTabbedGroups.GetHeadPosition (); pos != NULL;)
			{
				CBCGPTabWnd* pNextWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (pos));
				ASSERT_VALID (pNextWnd);
				pNextWnd->RecalcLayout ();

				if (pNextWnd->IsActiveInMDITabGroup ())
				{
					CWnd* pWnd = pNextWnd->GetTabWnd (pNextWnd->GetActiveTab ());
					PostMessage (WM_MDIACTIVATE, (WPARAM) pWnd->GetSafeHwnd ());
				}
			}
		}
		else if (m_bTabIsEnabled)
		{
			UpdateTabs (TRUE);
			m_wndTab.RecalcLayout ();
		}

		((CFrameWnd*) GetParent ())->RecalcLayout ();
	}
}
//****
LRESULT CBCGPMainClientAreaWnd::OnMDINext(WPARAM wp, LPARAM lp)
{
	if (!m_bIsMDITabbedGroup && !m_bTabIsEnabled)
	{
		return Default ();
	}

	BOOL bNext = (lp == 0);

	CBCGPTabWnd* pActiveTabWnd = NULL;
	int nActiveTab = -1;

	if (m_bIsMDITabbedGroup)
	{
		pActiveTabWnd = FindActiveTabWnd ();
	}
	else
	{
		pActiveTabWnd = &m_wndTab;
	}


	ASSERT_VALID (pActiveTabWnd);

	POSITION posActive = m_bIsMDITabbedGroup ? m_lstTabbedGroups.Find (pActiveTabWnd) : NULL;
	int nGroupCount = m_bIsMDITabbedGroup ? (int) m_lstTabbedGroups.GetCount () : 0;

	if (m_bIsMDITabbedGroup)
	{
		ASSERT (posActive != NULL);
	}

	nActiveTab = pActiveTabWnd->GetActiveTab ();

	bNext ? nActiveTab++ : nActiveTab--;

	if (nActiveTab < 0)
	{
		if (nGroupCount > 0)
		{
			m_lstTabbedGroups.GetPrev (posActive);
			if (posActive != NULL)
			{
				pActiveTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetAt (posActive));
			}
			else
			{
				pActiveTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetTail ());
			}
		}

		ASSERT (pActiveTabWnd != NULL);

		nActiveTab = pActiveTabWnd->GetTabsNum () - 1;
	}

	if (nActiveTab >= pActiveTabWnd->GetTabsNum ())
	{
		if (nGroupCount > 0)
		{
			pActiveTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetNext (posActive));
			if (posActive != NULL)
			{
				pActiveTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetAt (posActive));
			}
			else
			{
				pActiveTabWnd = DYNAMIC_DOWNCAST (CBCGPTabWnd, m_lstTabbedGroups.GetHead ());
			}
		}

		ASSERT (pActiveTabWnd != NULL);

		nActiveTab = 0;
	}
	

	CWnd* pWnd = pActiveTabWnd->GetTabWnd (nActiveTab);
	ASSERT_VALID (pWnd);

	if (pWnd->GetSafeHwnd () != (HWND) wp)
	{
		SetActiveTab (pWnd->GetSafeHwnd ());
	}	
	return 0L;
}

#undef REG_SECTION_FMT
