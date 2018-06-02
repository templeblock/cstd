//
// BCGPRibbonButton.cpp: implementation of the CBCGPRibbonButton class.
//
//////////////////////////////////////////////////////////////////////

#include "BCGPRibbonCategory.h"
#include "BCGPRibbonButton.h"
#include "BCGPRibbonBar.h"
#include "bcgglobals.h"
#include "BCGPVisualManager.h"
#include "MenuImages.h"
#include "BCGPRibbonButtonsGroup.h"
#include "BCGPRibbonPanelMenu.h"
#include "BCGPToolbarMenuButton.h"
#include "BCGPMDIFrameWnd.h"
#include "BCGPMDIChildWnd.h"

#include "winuser.h"
#include "oleacc.h"

#ifndef BCGP_EXCLUDE_RIBBON

BOOL CBCGPRibbonButton::m_bUseMenuHandle = FALSE;

const int nLargeButtonMarginX = 5;
const int nLargeButtonMarginY = 1;

const int nSmallButtonMarginX = 3;
const int nSmallButtonMarginY = 3;

const int nDefaultPaneButtonMargin = 2;

IMPLEMENT_DYNCREATE(CBCGPRibbonButton, CBCGPBaseRibbonElement)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBCGPRibbonButton::CBCGPRibbonButton()
{
	CommonInit();
}
//****
CBCGPRibbonButton::CBCGPRibbonButton (UINT nID, LPCTSTR lpszText, 
									  int nSmallImageIndex, 
									  int nLargeImageIndex,
									  BOOL bAlwaysShowDescription)
{
	CommonInit();

	m_nID = nID;
	SetText (lpszText);

	m_nSmallImageIndex = nSmallImageIndex;
	m_nLargeImageIndex = nLargeImageIndex;

	m_bAlwaysShowDescription = bAlwaysShowDescription;
}
//****
CBCGPRibbonButton::CBCGPRibbonButton (
		UINT	nID, 
		LPCTSTR lpszText, 
		CBitmap*	hIcon,
		BOOL	bAlwaysShowDescription,
		CBitmap*	hIconSmall,
		BOOL	bAutoDestroyIcon,
		BOOL	bAlphaBlendIcon)
{
	CommonInit();

	m_nID = nID;
	SetText (lpszText);
	m_hIcon = hIcon;
	m_hIconSmall = hIconSmall;
	m_bAlwaysShowDescription = bAlwaysShowDescription;
	m_bAutoDestroyIcon = bAutoDestroyIcon;
	m_bAlphaBlendIcon = bAlphaBlendIcon;
}
//****
void CBCGPRibbonButton::CommonInit()
{
	m_hMenu = NULL;
	m_bRightAlignMenu = FALSE;
	m_bIsDefaultCommand = TRUE;
	m_nMenuArrowMargin = 2;
	m_bAutodestroyMenu = FALSE;

	m_nSmallImageIndex = -1;
	m_nLargeImageIndex = -1;

	m_sizeTextRight = CSize (0, 0);
	m_sizeTextBottom = CSize (0, 0);

	m_szMargin = CSize (nSmallButtonMarginX, nSmallButtonMarginY);

	m_rectMenu.SetRectEmpty();
	m_rectCommand.SetRectEmpty();
	m_bMenuOnBottom = FALSE;
	m_bIsLargeImage = FALSE;

	m_bIsMenuHighlighted = FALSE;
	m_bIsCommandHighlighted = FALSE;

	m_hIcon = NULL;
	m_hIconSmall = NULL;
	m_bForceDrawBorder = FALSE;

	m_bToBeClosed = FALSE;
	m_bAlwaysShowDescription = FALSE;

	m_bCreatedFromMenu = FALSE;
	m_bIsWindowsMenu = FALSE;
	m_nWindowsMenuItems = 0;
	m_nWrapIndex = -1;

	m_bAutoDestroyIcon = FALSE;
	m_bAlphaBlendIcon = FALSE;
}
//****
CBCGPRibbonButton::~CBCGPRibbonButton()
{
	RemoveAllSubItems();

	if (m_bAutodestroyMenu && m_hMenu != NULL)
	{
		::DestroyMenu (m_hMenu);
	}

	if (m_bAutoDestroyIcon && m_hIcon != NULL)
	{
		::DestroyIcon (m_hIcon);
	}

	if (m_bAutoDestroyIcon && m_hIconSmall != NULL)
	{
		::DestroyIcon (m_hIconSmall);
	}
}
//****
void CBCGPRibbonButton::SetText (LPCTSTR lpszText)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetText (lpszText);

	m_sizeTextRight = CSize (0, 0);
	m_sizeTextBottom = CSize (0, 0);

	m_arWordIndexes.RemoveAll();

	for (int nOffset = 0;;)
	{
		int nIndex = m_strText.Find (_T(' '), nOffset);
		if (nIndex >= 0)
		{
			ASSERT (nIndex != 0);
			m_arWordIndexes.Add (nIndex);
			nOffset = nIndex + 1;
		}
		else
		{
			break;
		}
	}
}
//****
void CBCGPRibbonButton::SetDescription (LPCTSTR lpszText)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetDescription (lpszText);

	if (m_bAlwaysShowDescription)
	{
		m_sizeTextRight = CSize (0, 0);
		m_sizeTextBottom = CSize (0, 0);
	}
}
//****
void CBCGPRibbonButton::SetMenu (HMENU hMenu, BOOL bIsDefaultCommand, BOOL bRightAlign)
{
	ASSERT_VALID (this);
	
	m_bIsWindowsMenu = FALSE;
	m_nWindowsMenuItems = 0;

	if (m_bAutodestroyMenu && m_hMenu != NULL)
	{
		::DestroyMenu (m_hMenu);
	}

	m_bAutodestroyMenu = FALSE;

	if (m_bUseMenuHandle)
	{
		m_hMenu = hMenu;
	}
	else
	{
		CMenu* pMenu = CMenu::FromHandle (hMenu);

		for (int i = 0; i < (int) pMenu->GetMenuItemCount(); i++)
		{
			UINT uiID = pMenu->GetMenuItemID (i);

			switch (uiID)
			{
			case 0:
				{
					CBCGPRibbonSeparator* pSeparator = new CBCGPRibbonSeparator (TRUE);
					pSeparator->SetDefaultMenuLook();

					AddSubItem (pSeparator);
					break;
				}

			default:
				{
					CString str;
					pMenu->GetMenuString (i, str, MF_BYPOSITION);

					//-----------------------------------
					// Remove standard aceleration label:
					//-----------------------------------
					int iTabOffset = str.Find (_T('\t'));
					if (iTabOffset >= 0)
					{
						str = m_strText.Left (iTabOffset);
					}

					CBCGPRibbonButton* pItem = new CBCGPRibbonButton (uiID, str);
					pItem->SetDefaultMenuLook();
					pItem->m_pRibbonBar = m_pRibbonBar;

					if (uiID == -1)
					{
						pItem->SetMenu (pMenu->GetSubMenu (i)->GetSafeHmenu(), FALSE, bRightAlign);
					}

					AddSubItem (pItem);

					if (uiID >= AFX_IDM_WINDOW_FIRST && uiID <= AFX_IDM_WINDOW_LAST)
					{
						m_bIsWindowsMenu = TRUE;
					}
				}
			}
		}
	}

	m_bIsDefaultCommand = bIsDefaultCommand;

	if (m_nID == 0 || m_nID == (UINT)-1)
	{
		m_bIsDefaultCommand = FALSE;
	}

	m_bRightAlignMenu = bRightAlign;

	m_sizeTextRight = CSize (0, 0);
	m_sizeTextBottom = CSize (0, 0);

	m_bCreatedFromMenu = TRUE;
}
//****
void CBCGPRibbonButton::SetMenu (UINT uiMenuResID, BOOL bIsDefaultCommand, BOOL bRightAlign)
{
	ASSERT_VALID (this);

	SetMenu (
		::LoadMenu (::AfxGetResourceHandle(), MAKEINTRESOURCE (uiMenuResID)),
		bIsDefaultCommand, bRightAlign);

	m_bAutodestroyMenu = TRUE;
}
//****
void CBCGPRibbonButton::OnCalcTextSize (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_strText.IsEmpty() || IsMainRibbonButton())
	{
		m_sizeTextRight = CSize (0, 0);
		m_sizeTextBottom = CSize (0, 0);
		return;
	}

	if (m_sizeTextRight != CSize (0, 0) && m_sizeTextBottom != CSize (0, 0))
	{
		// Already calculated
		return;
	}

	// Text placed on right will be always single line:

	const CString strDummyAmpSeq = _T("\001\001");
	CString strText = m_strText;
	strText.Replace (_T("&&"), strDummyAmpSeq);
	strText.Remove (_T('&'));
	strText.Replace (strDummyAmpSeq, _T("&"));

	if (m_bAlwaysShowDescription && !m_strDescription.IsEmpty())
	{
		CFont* pOldFont = pDC->SelectObject(globalData.fontBold);
		ASSERT (pOldFont != NULL);

		m_sizeTextRight = pDC->GetTextExtent (strText);

		pDC->SelectObject (pOldFont);

		// Desciption will be draw below the text (in case of text on right only)
		int nTextHeight = 0;
		int nTextWidth = 0;

		CString strText = m_strDescription;

		for (int dx = m_sizeTextRight.w; dx < m_sizeTextRight.w * 10; dx += 10)
		{
			CRect rectText (0, 0, dx, 10000);

			nTextHeight = pDC->DrawText (strText, rectText, 
										DT_WORDBREAK | DT_CALCRECT);

			nTextWidth = rectText.Width();
			
			if (nTextHeight <= 2 * m_sizeTextRight.h)
			{
				break;
			}
		}
	
		m_sizeTextRight.w = max (m_sizeTextRight.w, nTextWidth);
		m_sizeTextRight.h += min (2 * m_sizeTextRight.h, nTextHeight) + 2 * m_szMargin.h;
	}
	else
	{
		// Text placed on right will be always single line:
		m_sizeTextRight = pDC->GetTextExtent (strText);
	}

	CSize sizeImageLarge = GetImageSize (RibbonImageLarge);

	if (sizeImageLarge == CSize (0, 0))
	{
		m_sizeTextBottom = CSize (0, 0);
	}
	else
	{
		// Text placed on bottom will occupy large image size and 1-2 text rows:
		m_sizeTextBottom = DrawBottomText (pDC, TRUE /*bCalcOnly*/);
	}
}
//****
void CBCGPRibbonButton::OnDraw (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	if (m_rect.IsRectEmpty())
	{
		return;
	}

	if (IsDefaultMenuLook() && !IsQATMode() && !m_bIsLargeImage)
	{
		CBCGPToolbarMenuButton dummy;

		dummy.m_strText = m_strText;
		dummy.m_nID = m_nID;
		dummy.m_bMenuMode = TRUE;
		dummy.m_pWndParent = GetParentWnd();
		dummy.m_bIsRadio = m_bIsRadio;

		if (IsChecked())
		{
			dummy.m_nStyle |= TBBS_CHECKED;
		}

		if (HasMenu())
		{
			dummy.m_bDrawDownArrow = TRUE;
		}

		BOOL bIsHighlighted = m_bIsHighlighted;

		if (IsDisabled())
		{
			dummy.m_nStyle |= TBBS_DISABLED;

			bIsHighlighted = IsFocused();
		}

		dummy.OnDraw (pDC, m_rect, NULL, TRUE, FALSE, bIsHighlighted || m_bIsFocused);
		return;
	}

	BOOL bIsDisabled = m_bIsDisabled;
	BOOL bIsDroppedDown = m_bIsDroppedDown;
	BOOL bIsHighlighted = m_bIsHighlighted;
	BOOL bMenuHighlighted = m_bIsMenuHighlighted;
	BOOL bCommandHighlighted = m_bIsCommandHighlighted;

	const int cxDropDown = GetDropDownImageWidth();

	if (m_bIsDisabled && HasMenu())
	{
		if (m_bIsDefaultCommand || 
			(!m_bIsDefaultCommand && !(m_nID == 0 || m_nID == (UINT)-1)))
		{
			m_bIsHighlighted = FALSE;
		}
		else
		{
			m_bIsDisabled = FALSE;
		}
	}

	if (m_bToBeClosed)
	{
		m_bIsDroppedDown = FALSE;
	}

	if (m_bIsFocused)
	{
		m_bIsHighlighted = TRUE;
		m_bIsMenuHighlighted = TRUE;
		m_bIsCommandHighlighted = TRUE;
	}

	CRect rectMenuArrow;
	rectMenuArrow.SetRectEmpty();

	if (HasMenu())
	{
		rectMenuArrow = m_rect;

		rectMenuArrow.l = rectMenuArrow.r - cxDropDown - m_nMenuArrowMargin;
		if (m_sizeTextRight.w == 0 && !m_bQuickAccessMode)
		{
			rectMenuArrow.l -= 2;
		}

		rectMenuArrow.b -= m_nMenuArrowMargin;

		if (m_bIsDefaultCommand)
		{
			m_rectMenu = m_rect;

			m_rectMenu.l = m_rectMenu.r - cxDropDown - m_nMenuArrowMargin - 1;
			
			m_rectCommand = m_rect;
			m_rectCommand.r = m_rectMenu.l;

			m_bMenuOnBottom = FALSE;
		}
	}

	CSize sizeImageLarge = GetImageSize (RibbonImageLarge);
	CSize sizeImageSmall = GetImageSize (RibbonImageSmall);

	CRect rectText = m_rect;
	BOOL bDrawText = !IsMainRibbonButton() && !m_bQuickAccessMode && !m_bFloatyMode;

	if (m_bQuickAccessMode || m_bFloatyMode)
	{
		bDrawText = FALSE;
	}
	else if (m_bCompactMode)
	{
		bDrawText = FALSE;
	}
	else if (sizeImageLarge != CSize (0, 0) && !m_bMenuOnBottom && m_bIsLargeImage)
	{
		if (!m_rectMenu.IsRectEmpty())
		{
			m_rectMenu.l -= cxDropDown;
			m_rectCommand.r = m_rectMenu.l;
		}

		rectMenuArrow.OffsetRect (-cxDropDown / 2, 0);
	}

	const RibbonImageType imageType = 
		m_bIsLargeImage ? RibbonImageLarge : RibbonImageSmall;

	CSize sizeImage = GetImageSize (imageType);
	BOOL bDrawDefaultImage = FALSE;

	if ((m_bQuickAccessMode || m_bFloatyMode) && sizeImage == CSize (0, 0))
	{
		// Use default image:
		sizeImage = CSize (16, 16);

		if (globalData.GetRibbonImageScale() != 1.)
		{
			sizeImage.w = (int) (.5 + globalData.GetRibbonImageScale() * sizeImage.w);
			sizeImage.h = (int) (.5 + globalData.GetRibbonImageScale() * sizeImage.h);
		}

		bDrawDefaultImage = TRUE;
	}
	
	CRect rectImage = m_rect;
	rectImage.DeflateRect (m_szMargin);

	if (IsMainRibbonButton())
	{
		if (globalData.GetRibbonImageScale() != 1.)
		{
			sizeImage.w = (int) (.8 * globalData.GetRibbonImageScale() * sizeImage.w);
			sizeImage.h = (int) (.8 * globalData.GetRibbonImageScale() * sizeImage.h);
		}

		rectImage.l += (rectImage.Width() - sizeImage.w) / 2;
		rectImage.t  += (rectImage.Height() - sizeImage.h) / 2;

		rectImage.OffsetRect (CBCGPVisualManager::GetInstance()->GetRibbonMainImageOffset());
	}
	else if (m_bIsLargeImage && !m_bTextAlwaysOnRight)
	{
		rectImage.l = rectImage.CenterPoint().x - sizeImage.w / 2;
		rectImage.t += m_szMargin.h + 1;

		if (!bDrawText)
		{
			rectImage.t = rectImage.CenterPoint().y - sizeImage.h / 2;
		}
	}
	else
	{
		rectImage.t = rectImage.CenterPoint().y - sizeImage.h / 2;
	}

	rectImage.r = rectImage.l + sizeImage.w;
	rectImage.b = rectImage.t + sizeImage.h;

	if (m_bIsLargeImage && !m_bTextAlwaysOnRight && HasMenu() && m_bIsDefaultCommand)
	{
		m_rectMenu = m_rect;
		m_rectMenu.t = rectImage.b + 3;
		
		m_rectCommand = m_rect;
		m_rectCommand.b = m_rectMenu.t;

		m_bMenuOnBottom = TRUE;
	}

	COLORREF clrText = (COLORREF)-1;

	if (!IsMainRibbonButton())
	{
		clrText = OnFillBackground (pDC);
	}

	if (IsMenuMode() && IsChecked() && sizeImage != CSize (0, 0))
	{
		CBCGPVisualManager::GetInstance()->OnDrawRibbonMenuCheckFrame (pDC,
			this, rectImage);
	}

	//------------
	// Draw image:
	//------------
	if (bDrawDefaultImage)
	{
		CBCGPVisualManager::GetInstance()->OnDrawDefaultRibbonImage (
			pDC, rectImage, m_bIsDisabled, m_bIsPressed, m_bIsHighlighted);
	}
	else
	{
		BOOL bIsRibbonImageScale = globalData.IsRibbonImageScaleEnabled();

		if (IsMenuMode() && !m_bIsLargeImage)
		{
			if (m_pParentMenu == NULL || !m_pParentMenu->IsMainPanel())
			{
				globalData.EnableRibbonImageScale (FALSE);
			}
		}

		DrawImage (pDC, imageType, rectImage);
		globalData.EnableRibbonImageScale (bIsRibbonImageScale);
	}

	//-----------
	// Draw text:
	//-----------
	if (bDrawText)
	{
		CFont* pOldFont = NULL;

		CRect rectText = m_rect;
		UINT uiDTFlags = 0;

		COLORREF clrTextOld = (COLORREF)-1;

		if (bIsDisabled && 
			(m_bIsDefaultCommand ||
			(!m_bIsDefaultCommand && !(m_nID == 0 || m_nID == (UINT)-1))))
		{
			if (m_bQuickAccessMode)
			{
				clrText = CBCGPVisualManager::GetInstance()->GetRibbonQATTextColor (TRUE);
			}
			else
			{
				clrTextOld = pDC->SetTextColor (
					clrText == (COLORREF)-1 ? 
						CBCGPVisualManager::GetInstance()->GetToolbarDisabledTextColor() : clrText);
			}
		}
		else if (clrText != (COLORREF)-1)
		{
			clrTextOld = pDC->SetTextColor (clrText);
		}

		if (m_bIsLargeImage && !m_bTextAlwaysOnRight)
		{
			DrawBottomText (pDC, FALSE);
			rectMenuArrow.SetRectEmpty();
		}
		else
		{
			rectText.l = rectImage.r;

			if (m_nImageOffset > 0)
			{
				rectText.l = m_rect.l + m_nImageOffset + 3 * m_szMargin.w;
			}
			else if (sizeImage.w != 0)
			{
				rectText.l += GetTextOffset();
			}

			uiDTFlags = DT_SINGLELINE | DT_END_ELLIPSIS;
			
			if (!m_bAlwaysShowDescription || m_strDescription.IsEmpty())
			{
				uiDTFlags |= DT_VCENTER;
			}
			else
			{
				pOldFont = pDC->SelectObject(globalData.fontBold);
				ASSERT (pOldFont != NULL);

				rectText.t += max (0, (m_rect.Height() - m_sizeTextRight.h) / 2);
			}

			if (m_bIsTabElement)
			{
				rectText.OffsetRect (0, 2);
			}

			int nTextHeight = DoDrawText (pDC, m_strText, rectText, uiDTFlags);

			if (pOldFont != NULL)
			{
				pDC->SelectObject (pOldFont);
			}

			if (m_bAlwaysShowDescription && !m_strDescription.IsEmpty())
			{
				rectText.t += nTextHeight + m_szMargin.h;
				rectText.r = m_rect.r - m_szMargin.w;

				pDC->DrawText (m_strDescription, rectText, DT_WORDBREAK | DT_END_ELLIPSIS);
			}

			if (nTextHeight == m_sizeTextRight.h &&
				m_bIsLargeImage && HasMenu())
			{
				rectMenuArrow = m_rect;
				rectMenuArrow.DeflateRect (m_nMenuArrowMargin, m_nMenuArrowMargin * 2);
				rectMenuArrow.r -= 2;

				int cyMenu = CBCGPMenuImages::Size().h;

				rectMenuArrow.t = rectMenuArrow.b - cyMenu;
				rectMenuArrow.b = rectMenuArrow.t + CBCGPMenuImages::Size().h;
			}
		}

		if (clrTextOld != (COLORREF)-1)
		{
			pDC->SetTextColor (clrTextOld);
		}
	}

	if (!IsMainRibbonButton())
	{
		if (!rectMenuArrow.IsRectEmpty())
		{
			CBCGPMenuImages::IMAGES_IDS id = CBCGPMenuImages::IdArowDown;

			if (IsMenuMode())
			{
				BOOL bIsRTL = FALSE;

				CBCGPRibbonBar* pTopLevelRibbon = GetTopLevelRibbonBar();
				if (pTopLevelRibbon->GetSafeHwnd() != NULL)
				{
					bIsRTL = (pTopLevelRibbon->GetExStyle() & WS_EX_LAYOUTRTL);
				}

				id = bIsRTL ? CBCGPMenuImages::IdArowLeftLarge : CBCGPMenuImages::IdArowRightLarge;
			}

			CRect rectWhite = rectMenuArrow;
			rectWhite.OffsetRect (0, 1);

			CBCGPMenuImages::Draw (pDC, id, rectWhite, CBCGPMenuImages::ImageWhite);
			CBCGPMenuImages::Draw (pDC, id, rectMenuArrow, 
				m_bIsDisabled ? CBCGPMenuImages::ImageGray : CBCGPMenuImages::ImageBlack);
		}

		OnDrawBorder (pDC);
	}

	m_bIsDisabled = bIsDisabled;
	m_bIsDroppedDown = bIsDroppedDown;
	m_bIsHighlighted = bIsHighlighted;
	m_bIsMenuHighlighted = bMenuHighlighted;
	m_bIsCommandHighlighted = bCommandHighlighted;
}
//****
void CBCGPRibbonButton::OnDrawOnList (CDC* pDC, CString strText,
									  int nTextOffset, CRect rect,
									  BOOL /*bIsSelected*/,
									  BOOL bHighlighted)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	BOOL bIsDisabled = m_bIsDisabled;
	m_bIsDisabled = FALSE;

	CRect rectImage = rect;
	rectImage.r = rect.l + nTextOffset;

	CSize sizeImageSmall = GetImageSize (RibbonImageSmall);
	if (sizeImageSmall != CSize (0, 0))
	{
		rectImage.DeflateRect (1, 0);
		rectImage.t += max (0, (rectImage.Height() - sizeImageSmall.h) / 2);
		rectImage.b = rectImage.t + sizeImageSmall.h;

		DrawImage (pDC, RibbonImageSmall, rectImage);
	}
	else if (m_bDrawDefaultIcon)
	{
		CBCGPVisualManager::GetInstance()->OnDrawDefaultRibbonImage (
			pDC, rectImage);
	}

	CRect rectText = rect;

	if (HasMenu())
	{
		CRect rectMenuArrow = rect;
		rectMenuArrow.l = rectMenuArrow.r - rectMenuArrow.Height();

		CRect rectWhite = rectMenuArrow;
		rectWhite.OffsetRect (0, 1);

		BOOL bIsDarkMenu = TRUE;

		if (bHighlighted)
		{
			COLORREF clrText = _GetSysColor(CLR_HIGHLIGHTTEXT);
			
			if (GetRV (clrText) > 128 &&
				GetGV (clrText) > 128 &&
				GetBV (clrText) > 128)
			{
				bIsDarkMenu = FALSE;
			}
		}

		CBCGPMenuImages::IMAGES_IDS id = 
			globalData.GetRibbonImageScale() > 1. ? 
			CBCGPMenuImages::IdArowRightLarge : CBCGPMenuImages::IdArowRight;

		CBCGPMenuImages::Draw (pDC, id, rectWhite,
			bIsDarkMenu ? CBCGPMenuImages::ImageWhite : CBCGPMenuImages::ImageBlack);

		CBCGPMenuImages::Draw (pDC, id, rectMenuArrow,
			bIsDarkMenu ? CBCGPMenuImages::ImageBlack : CBCGPMenuImages::ImageWhite);

		rectText.r = rectMenuArrow.l;
	}

	rectText.l += nTextOffset;

	const int xMargin = 3;
	rectText.DeflateRect (xMargin, 0);

	pDC->DrawText (strText, rectText, DT_VCENTER | DT_SINGLELINE | DT_NOPREFIX);
	m_bIsDisabled = bIsDisabled;
}
//****
CSize CBCGPRibbonButton::GetRegularSize (CDC* pDC)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		ASSERT_VALID (m_arSubItems [i]);
		m_arSubItems [i]->SetParentCategory (m_pParent);
	}

	if (m_bQuickAccessMode || m_bFloatyMode)
	{
		return GetCompactSize (pDC);
	}

	if (!HasLargeMode())
	{
		return GetIntermediateSize (pDC);
	}

	CSize sizeImageLarge = GetImageSize (RibbonImageLarge);
	CSize sizeImageSmall = GetImageSize (RibbonImageSmall);

	if (IsMainRibbonButton())
	{
		return sizeImageLarge;
	}

	const int cxExtra = GetGroupButtonExtraWidth();

	if (sizeImageLarge == CSize (0, 0) || m_bTextAlwaysOnRight)
	{
		if (m_bTextAlwaysOnRight && sizeImageLarge != CSize (0, 0))
		{
			sizeImageSmall = CSize (sizeImageLarge.w + 2, sizeImageLarge.h + 2);
			m_szMargin.h = 5;
		}

		int cx = sizeImageSmall.w + 2 * m_szMargin.w;
		
		if (m_sizeTextRight.w > 0)
		{
			cx += m_szMargin.w + m_sizeTextRight.w;

			if (sizeImageLarge != CSize (0, 0) && m_bTextAlwaysOnRight)
			{
				cx += m_szMargin.w;
			}
		}

		int cy = max (sizeImageSmall.h, m_sizeTextRight.h) + 2 * m_szMargin.h;

		if (sizeImageSmall.h == 0)
		{
			cy += 2 * m_szMargin.h;
		}

		if (HasMenu())
		{
			cx += GetDropDownImageWidth();

			if (m_bIsDefaultCommand && m_nID != -1 && m_nID != 0 &&
				m_sizeTextRight.w > 0)
			{
				cx += m_nMenuArrowMargin;
			}
		}

		if (IsDefaultMenuLook() && !IsQATMode())
		{
			cx += 2 * TEXT_MARGIN;
		}

		return CSize (cx + cxExtra, cy);
	}

	SetMargin (CSize (nLargeButtonMarginX, nLargeButtonMarginY));

	if (IsDefaultPanelButton())
	{
		sizeImageLarge.w += 2 * (m_szMargin.w + 1);
	}

	int cx = max (sizeImageLarge.w + 2 * m_szMargin.w, m_sizeTextBottom.w + 5);

	if (IsDefaultPanelButton())
	{
		cx += nDefaultPaneButtonMargin;
	}

	if (IsDefaultMenuLook())
	{
		cx += 2 * TEXT_MARGIN;
	}

	TEXTMETRIC tm;
	pDC->GetTextMetrics (&tm);

	int cyExtra = (tm.tmHeight < 15 || globalData.GetRibbonImageScale() != 1) ? 1 : 5;

	const int cyText = max (m_sizeTextBottom.h, sizeImageLarge.h + 1);
	const int cy = sizeImageLarge.h + cyText + cyExtra;

	return CSize (cx + cxExtra, cy);
}
//****
CSize CBCGPRibbonButton::GetCompactSize (CDC* /*pDC*/)
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		ASSERT_VALID (m_arSubItems [i]);
		m_arSubItems [i]->SetParentCategory (m_pParent);
	}

	CSize sizeImageSmall = GetImageSize (RibbonImageSmall);

	if (IsMainRibbonButton())
	{
		return sizeImageSmall;
	}

	const int cxDropDown = GetDropDownImageWidth();

	int cxExtra = 0;

	if (m_bQuickAccessMode || m_bFloatyMode)
	{
		SetMargin (globalData.GetRibbonImageScale() != 1. ? 
			CSize (nSmallButtonMarginX, nSmallButtonMarginY - 1) : CSize (nSmallButtonMarginX, nSmallButtonMarginY));
	
		if (sizeImageSmall == CSize (0, 0))
		{
			sizeImageSmall = CSize (16, 16);

			if (globalData.GetRibbonImageScale() != 1.)
			{
				sizeImageSmall.w = (int) (.5 + globalData.GetRibbonImageScale() * sizeImageSmall.w);
				sizeImageSmall.h = (int) (.5 + globalData.GetRibbonImageScale() * sizeImageSmall.h);
			}
		}
	}
	else
	{
		SetMargin (CSize (nSmallButtonMarginX, nSmallButtonMarginY));
		cxExtra = GetGroupButtonExtraWidth();

		if (IsDefaultMenuLook())
		{
			cxExtra += 2 * TEXT_MARGIN;
		}
	}

	int nMenuArrowWidth = 0;
	
	if (HasMenu())
	{
		if (m_bIsDefaultCommand)
		{
			nMenuArrowWidth = cxDropDown + m_szMargin.w / 2 + 1;
		}
		else
		{
			nMenuArrowWidth = cxDropDown - m_szMargin.w / 2 - 1;
		}
	}

	int cx = sizeImageSmall.w + 2 * m_szMargin.w + nMenuArrowWidth + cxExtra;
	int cy = sizeImageSmall.h + 2 * m_szMargin.h;

	return CSize (cx, cy);
}
//****
CSize CBCGPRibbonButton::GetIntermediateSize (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		ASSERT_VALID (m_arSubItems [i]);
		m_arSubItems [i]->SetParentCategory (m_pParent);
	}

	if (m_bQuickAccessMode || m_bFloatyMode)
	{
		return GetCompactSize (pDC);
	}

	SetMargin (CSize (nSmallButtonMarginX, nSmallButtonMarginY));

	const int nMenuArrowWidth = HasMenu() ? GetDropDownImageWidth() : 0;

	CSize sizeImageSmall = GetImageSize (RibbonImageSmall);

	sizeImageSmall.h = max (16, sizeImageSmall.h);

	int cy = max (sizeImageSmall.h, m_sizeTextRight.h) + 2 * m_szMargin.h;
	int cx = sizeImageSmall.w + 2 * m_szMargin.w + nMenuArrowWidth + m_sizeTextRight.w + GetTextOffset() + 
		GetGroupButtonExtraWidth() + 1;

	if (IsDefaultMenuLook())
	{
		cx += 2 * TEXT_MARGIN;
		cy += TEXT_MARGIN / 2 - 1;
	}

	return CSize (cx, cy);
}
//****
void CBCGPRibbonButton::OnLButtonDown (CPoint point)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnLButtonDown (point);

	if (!HasMenu() || IsMenuMode())
	{
		return;
	}

	if (!m_rectMenu.IsRectEmpty() && !m_rectMenu.PtInRect (point))
	{
		return;
	}

	if (m_bIsDefaultCommand && m_bIsDisabled)
	{
		return;
	}

	if (m_bIsDisabled && m_rectCommand.IsRectEmpty())
	{
		return;
	}

	OnShowPopupMenu();
}
//****
void CBCGPRibbonButton::OnLButtonUp (CPoint point)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnLButtonUp (point);

	BOOL bIsPressed = m_bIsPressed || IsMenuMode();

	if (m_bIsDisabled || !bIsPressed || !m_bIsHighlighted)
	{
		return;
	}

	if (m_bIsDroppedDown)
	{
		if (!m_rectCommand.IsRectEmpty() && m_rectCommand.PtInRect (point) && IsMenuMode())
		{
			OnClick (point);
		}

		return;
	}

	if (!m_rectCommand.IsRectEmpty() && !m_rectCommand.PtInRect (point))
	{
		return;
	}

	OnClick (point);
}
//****
void CBCGPRibbonButton::OnMouseMove (CPoint point)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnMouseMove (point);

	if (!HasMenu() || m_nID == -1 || m_nID == 0)
	{
		return;
	}

	BOOL bMenuWasHighlighted = m_bIsMenuHighlighted;
	BOOL bCommandWasHighlighted = m_bIsCommandHighlighted;

	m_bIsMenuHighlighted = m_rectMenu.PtInRect (point);
	m_bIsCommandHighlighted = m_rectCommand.PtInRect (point);

	if (bMenuWasHighlighted != m_bIsMenuHighlighted ||
		bCommandWasHighlighted != m_bIsCommandHighlighted)
	{
		Redraw();

		if (m_pParentMenu != NULL)
		{
			ASSERT_VALID (m_pParentMenu);
			m_pParentMenu->OnChangeHighlighted (this);
		}
	}
}
//****
void CBCGPRibbonButton::OnClick (CPoint point)
{
	ASSERT_VALID (this);

	if (IsMenuMode() && HasMenu() && m_rectCommand.IsRectEmpty())
	{
		return;
	}

	if (m_pParentMenu != NULL)
	{
		ASSERT_VALID (m_pParentMenu);
		m_pParentMenu->OnClickButton (this, point);
		return;
	}

	NotifyCommand();
}
//****
void CBCGPRibbonButton::OnShowPopupMenu()
{
	ASSERT_VALID (this);

	CWnd* pWndParent = GetParentWnd();
	if (pWndParent->GetSafeHwnd() == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	CBCGPRibbonBar* pTopLevelRibbon = GetTopLevelRibbonBar();
	if (pTopLevelRibbon->GetSafeHwnd() == NULL)
	{
		ASSERT (FALSE);
		return;
	}

	CBCGPBaseRibbonElement::OnShowPopupMenu();

	const BOOL bIsRTL = (pTopLevelRibbon->GetExStyle() & WS_EX_LAYOUTRTL);

	CWnd* pWndOwner = pTopLevelRibbon->GetSafeOwner();

	if (m_arSubItems.GetSize() > 0)
	{
		if (m_bIsWindowsMenu)
		{
			FillWindowList();
		}

		//--------------------------------
		// Build popup menu from subitems:
		//--------------------------------
		CBCGPRibbonPanelMenu* pMenu = new CBCGPRibbonPanelMenu
			(pTopLevelRibbon, m_arSubItems);

		pMenu->SetParentRibbonElement (this);

		pMenu->SetMenuMode();

		BOOL bIsPopupDefaultMenuLook = IsPopupDefaultMenuLook();

		for (int i = 0; bIsPopupDefaultMenuLook && i < m_arSubItems.GetSize(); i++)
		{
			ASSERT_VALID (m_arSubItems [i]);
			
			if (!m_arSubItems [i]->IsDefaultMenuLook())
			{
				bIsPopupDefaultMenuLook = FALSE;
			}
		}

		pMenu->SetDefaultMenuLook (bIsPopupDefaultMenuLook);

		if (m_pOriginal != NULL && m_pOriginal->GetParentPanel() != NULL &&
			m_pOriginal->GetParentPanel()->IsMainPanel())
		{
			pMenu->SetDefaultMenuLook (FALSE);
		}
		
		CRect rectBtn = GetRect();
		pWndParent->ClientToScreen (&rectBtn);

		int x = m_bRightAlignMenu || bIsRTL ? rectBtn.r : rectBtn.l;

		int y = rectBtn.b;

		if (m_bCreatedFromMenu && m_bRightAlignMenu && !bIsRTL)
		{
			pMenu->SetRightAlign();
		}

		if (IsMenuMode())
		{
			x = bIsRTL ? rectBtn.l : rectBtn.r;
			y = rectBtn.t;
		}

		CRect rectMenuLocation;
		rectMenuLocation.SetRectEmpty();

		CBCGPRibbonPanel* pPanel = GetParentPanel();

		if (pPanel != NULL && 
			pPanel->GetPreferedMenuLocation (rectMenuLocation))
		{
			pWndParent->ClientToScreen (&rectMenuLocation);

			x = bIsRTL ? rectMenuLocation.r : rectMenuLocation.l;
			y = rectMenuLocation.t;

			pMenu->SetPreferedSize (rectMenuLocation.Size());
			pMenu->SetDefaultMenuLook (FALSE);
		}

		pMenu->Create (pWndOwner, x, y, (HMENU) NULL);

		SetDroppedDown (pMenu);
		return;
	}

	HMENU hPopupMenu = GetMenu();
	if (hPopupMenu == NULL)
	{
		return;
	}

	CRect rectBtn = GetRect();
	pWndParent->ClientToScreen (&rectBtn);

	CBCGPPopupMenu* pPopupMenu = new CBCGPPopupMenu;

	pPopupMenu->SetAutoDestroy (FALSE);
	pPopupMenu->SetRightAlign (m_bRightAlignMenu && !bIsRTL);

	pPopupMenu->SetParentRibbonElement (this);

	CBCGPPopupMenu* pMenuActive = CBCGPPopupMenu::GetActiveMenu();
	if (pMenuActive != NULL &&
		pMenuActive->GetSafeHwnd() != pWndParent->GetParent()->GetSafeHwnd())
	{
		pMenuActive->SendMessage (WM_CLOSE);
	}

	int x = m_bRightAlignMenu || bIsRTL ? rectBtn.r : rectBtn.l;
	int y = rectBtn.b;

	pPopupMenu->Create (pWndOwner, x, y, hPopupMenu, FALSE);

	SetDroppedDown (pPopupMenu);
}
//****
void CBCGPRibbonButton::SetParentCategory (CBCGPRibbonCategory* pParent)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetParentCategory (pParent);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pSubItem = m_arSubItems [i];
		ASSERT_VALID (pSubItem);

		pSubItem->SetParentCategory (m_pParent);
		pSubItem->SetDefaultMenuLook (!m_bUseMenuHandle && !pSubItem->HasLargeMode());
	}
}
//****
void CBCGPRibbonButton::CopyFrom (const CBCGPBaseRibbonElement& s)
{
	ASSERT_VALID (this);

	if (m_bAutodestroyMenu && m_hMenu != NULL)
	{
		::DestroyMenu (m_hMenu);
	}

	if (m_bAutoDestroyIcon && m_hIcon != NULL)
	{
		::DestroyIcon (m_hIcon);
	}

	if (m_bAutoDestroyIcon && m_hIconSmall != NULL)
	{
		::DestroyIcon (m_hIconSmall);
	}

	RemoveAllSubItems();

	CBCGPBaseRibbonElement::CopyFrom (s);

	CBCGPRibbonButton& src = (CBCGPRibbonButton&) s;

	m_nSmallImageIndex = src.m_nSmallImageIndex;
	m_nLargeImageIndex = src.m_nLargeImageIndex;
	m_hMenu = src.m_hMenu;
	m_bAutodestroyMenu = FALSE;
	m_bRightAlignMenu = src.m_bRightAlignMenu;
	m_bIsDefaultCommand = src.m_bIsDefaultCommand;
	m_szMargin = src.m_szMargin;
	m_hIcon = src.m_hIcon;
	m_hIconSmall = src.m_hIconSmall;
	m_bAutoDestroyIcon = FALSE;
	m_bAlphaBlendIcon = src.m_bAlphaBlendIcon;
	m_bForceDrawBorder = src.m_bForceDrawBorder;
	m_bAlwaysShowDescription = src.m_bAlwaysShowDescription;
	m_bCreatedFromMenu = src.m_bCreatedFromMenu;
	m_bIsWindowsMenu = src.m_bIsWindowsMenu;
	m_nWindowsMenuItems = src.m_nWindowsMenuItems;

	int i = 0;

	for (i = 0; i < src.m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pSrcElem = src.m_arSubItems [i];
		ASSERT_VALID (pSrcElem);

		CBCGPBaseRibbonElement* pElem =
			(CBCGPBaseRibbonElement*) pSrcElem->GetRuntimeClass()->CreateObject();
		ASSERT_VALID (pElem);

		pElem->CopyFrom (*pSrcElem);
		m_arSubItems.Add (pElem);
	}

	m_nWrapIndex = src.m_nWrapIndex;

	m_arWordIndexes.RemoveAll();

	for (i = 0; i < src.m_arWordIndexes.GetSize(); i++)
	{
		m_arWordIndexes.Add (src.m_arWordIndexes [i]);
	}
}
//****
void CBCGPRibbonButton::SetOriginal (CBCGPBaseRibbonElement* pOriginal)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetOriginal (pOriginal);

	CBCGPRibbonButton* pOriginalButton =
		DYNAMIC_DOWNCAST (CBCGPRibbonButton, pOriginal);

	if (pOriginalButton == NULL)
	{
		return;
	}

	ASSERT_VALID (pOriginalButton);

	if (pOriginalButton->m_arSubItems.GetSize() != m_arSubItems.GetSize())
	{
		return;
	}

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arSubItems [i];
		ASSERT_VALID (pButton);

		pButton->SetOriginal (pOriginalButton->m_arSubItems [i]);
	}
}
//****
void CBCGPRibbonButton::DrawImage (CDC* pDC, RibbonImageType type, 
								   CRect rectImage)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CBCGPRibbonButton* pOrigButton = DYNAMIC_DOWNCAST (
		CBCGPRibbonButton, m_pOriginal);

	if (pOrigButton != NULL)
	{
		ASSERT_VALID (pOrigButton);

		BOOL bIsDisabled = pOrigButton->m_bIsDisabled;
		pOrigButton->m_bIsDisabled = m_bIsDisabled;

		CRect rect = pOrigButton->m_rect;
		pOrigButton->m_rect = m_rect;

		pOrigButton->DrawImage (pDC, type, rectImage);

		pOrigButton->m_bIsDisabled = bIsDisabled;
		pOrigButton->m_rect = rect;
		return;
	}

	if (m_hIcon != NULL)
	{
		CBitmap* hIcon = type == RibbonImageLarge || m_hIconSmall == NULL ? m_hIcon : m_hIconSmall;

		CSize sizeIcon = type == RibbonImageLarge ? CSize (32, 32) : CSize (16, 16);

		if (globalData.GetRibbonImageScale() != 1.)
		{
			sizeIcon.w = (int) (.5 + globalData.GetRibbonImageScale() * sizeIcon.w);
			sizeIcon.h = (int) (.5 + globalData.GetRibbonImageScale() * sizeIcon.h);
		}

		if (m_bIsDisabled)
		{
			CBCGPToolBarImages icon;
			icon.SetImageSize (type == RibbonImageLarge ? CSize (32, 32) : CSize (16, 16));

			icon.AddIcon (hIcon, m_bAlphaBlendIcon);
			
			CBCGPDrawState ds;
			icon.PrepareDrawImage (ds, sizeIcon);
			icon.Draw (pDC, rectImage.l, rectImage.t, 0, FALSE, TRUE);
			icon.EndDrawImage (ds);
		}
		else
		{
			UINT diFlags = DI_NORMAL;

			CWnd* pWndParent = GetParentWnd();
			if (pWndParent != NULL && (pWndParent->GetExStyle() & WS_EX_LAYOUTRTL))
			{
				diFlags |= 0x0010 /*DI_NOMIRROR*/;
			}

			::DrawIconEx (pDC, 
				rectImage.l, 
				rectImage.t,
				hIcon, sizeIcon.w, sizeIcon.h, 0, NULL,
				diFlags);
		}
		return;
	}

	if (m_pParentGroup != NULL)
	{
		ASSERT_VALID (m_pParentGroup);

		if (m_pParentGroup->HasImages())
		{
			m_pParentGroup->OnDrawImage (pDC, rectImage, this, m_nSmallImageIndex);
			return;
		}
	}

	if (m_pParent == NULL || rectImage.Width() == 0 || rectImage.Height() == 0)
	{
		return;
	}

	ASSERT_VALID (m_pParent);

	m_pParent->OnDrawImage (
		pDC, rectImage, this, 
		type == RibbonImageLarge, 
		type == RibbonImageLarge ? m_nLargeImageIndex : m_nSmallImageIndex,
		FALSE /* no center */);
}
//****
CSize CBCGPRibbonButton::GetImageSize (RibbonImageType type) const
{
	ASSERT_VALID (this);

	CBCGPRibbonButton* pOrigButton = DYNAMIC_DOWNCAST (
		CBCGPRibbonButton, m_pOriginal);

	if (pOrigButton != NULL)
	{
		ASSERT_VALID (pOrigButton);
		return pOrigButton->GetImageSize (type);
	}

	if (m_hIcon != NULL)
	{
		CSize sizeIcon = type == RibbonImageLarge ? CSize (32, 32) : CSize (16, 16);

		if (globalData.GetRibbonImageScale() != 1.)
		{
			sizeIcon.w = (int) (.5 + globalData.GetRibbonImageScale() * sizeIcon.w);
			sizeIcon.h = (int) (.5 + globalData.GetRibbonImageScale() * sizeIcon.h);
		}

		return sizeIcon;
	}

	const int nImageIndex = type == RibbonImageLarge  ? 
		m_nLargeImageIndex : m_nSmallImageIndex;

	if (nImageIndex < 0)
	{
		return CSize (0, 0);
	}

	if (m_pParentGroup != NULL)
	{
		ASSERT_VALID (m_pParentGroup);

		if (m_pParentGroup->HasImages())
		{
			return m_pParentGroup->GetImageSize();
		}
	}

	if (m_pParent == NULL)
	{
		return CSize (0, 0);
	}

	ASSERT_VALID (m_pParent);

	const int nImageCount = m_pParent->GetImageCount (type == RibbonImageLarge);

	if (nImageIndex >= nImageCount)
	{
		return CSize (0, 0);
	}

	return m_pParent->GetImageSize (type == RibbonImageLarge);
}
//****
BOOL CBCGPRibbonButton::CanBeStretched()
{
	ASSERT_VALID (this);
	return GetImageSize (RibbonImageLarge) != CSize (0, 0);
}
//****
void CBCGPRibbonButton::AddSubItem (CBCGPBaseRibbonElement* pSubItem, int nIndex)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pSubItem);

	pSubItem->SetParentCategory (m_pParent);
	pSubItem->SetDefaultMenuLook (!m_bUseMenuHandle && !pSubItem->HasLargeMode());

	if (nIndex == -1)
	{
		m_arSubItems.Add (pSubItem);
	}
	else
	{
		m_arSubItems.InsertAt (nIndex, pSubItem);
	}

	pSubItem->OnAfterAddToParent (this);
}
//****
int CBCGPRibbonButton::FindSubItemIndexByID (UINT uiID) const
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		ASSERT_VALID (m_arSubItems [i]);
		
		if (m_arSubItems [i]->GetID() == uiID)
		{
			return i;
		}
	}

	return -1;
}
//****
BOOL CBCGPRibbonButton::RemoveSubItem (int nIndex)
{
	ASSERT_VALID (this);

	if (nIndex < 0 || nIndex >= m_arSubItems.GetSize())
	{
		return FALSE;
	}

	ASSERT_VALID (m_arSubItems [nIndex]);
	delete m_arSubItems [nIndex];

	m_arSubItems.RemoveAt (nIndex);

	return TRUE;
}
//****
void CBCGPRibbonButton::RemoveAllSubItems()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		ASSERT_VALID (m_arSubItems [i]);
		delete m_arSubItems [i];
	}

	m_arSubItems.RemoveAll();
}
//****
COLORREF CBCGPRibbonButton::OnFillBackground (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	const BOOL bIsDisabled = m_bIsDisabled;

	if (m_bIsDisabled && HasMenu())
	{
		m_bIsDisabled = FALSE;
	}

	COLORREF clrText = CBCGPVisualManager::GetInstance()->
		OnFillRibbonButton (pDC, this);

	m_bIsDisabled = bIsDisabled;

	return clrText;
}
//****
void CBCGPRibbonButton::OnDrawBorder (CDC* pDC)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	const BOOL bIsDisabled = m_bIsDisabled;

	if (m_bIsDisabled && HasMenu())
	{
		m_bIsDisabled = FALSE;
	}

	CBCGPVisualManager::GetInstance()->OnDrawRibbonButtonBorder (pDC, this);

	m_bIsDisabled = bIsDisabled;
}
//****
int CBCGPRibbonButton::AddToListBox (CBCGPRibbonCommandsListBox* pWndListBox, BOOL bDeep)
{
	ASSERT_VALID (this);

	int nIndex = CBCGPBaseRibbonElement::AddToListBox (pWndListBox, bDeep);

	if (bDeep && !m_bCreatedFromMenu)
	{
		for (int i = 0; i < m_arSubItems.GetSize(); i++)
		{
			ASSERT_VALID (m_arSubItems [i]);

			if (m_arSubItems [i]->GetID() != 0)	// Don't add separators
			{
				nIndex = m_arSubItems [i]->AddToListBox (pWndListBox, TRUE);
			}
		}
	}

	return nIndex;
}
//****
void CBCGPRibbonButton::ClosePopupMenu()
{
	ASSERT_VALID (this);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		ASSERT_VALID (m_arSubItems [i]);
		m_arSubItems [i]->ClosePopupMenu();
	}

	CBCGPBaseRibbonElement::ClosePopupMenu();
}
//****
CBCGPBaseRibbonElement* CBCGPRibbonButton::FindByID (UINT uiCmdID)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement* pElem = CBCGPBaseRibbonElement::FindByID (uiCmdID);
	if (pElem != NULL)
	{
		ASSERT_VALID (pElem);
		return pElem;
	}

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arSubItems [i];
		ASSERT_VALID (pButton);

		pElem = pButton->FindByID (uiCmdID);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//****
CBCGPBaseRibbonElement* CBCGPRibbonButton::FindByData (DWORD_PTR dwData)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement* pElem = CBCGPBaseRibbonElement::FindByData (dwData);
	if (pElem != NULL)
	{
		ASSERT_VALID (pElem);
		return pElem;
	}

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arSubItems [i];
		ASSERT_VALID (pButton);

		pElem = pButton->FindByData (dwData);
		if (pElem != NULL)
		{
			ASSERT_VALID (pElem);
			return pElem;
		}
	}
	
	return NULL;
}
//****
CString CBCGPRibbonButton::GetToolTipText() const
{
	ASSERT_VALID (this);

	if (!m_bQuickAccessMode && m_bAlwaysShowDescription && !m_strDescription.IsEmpty())
	{
		return _T("");
	}

	return CBCGPBaseRibbonElement::GetToolTipText();
}
//****
void CBCGPRibbonButton::SetParentRibbonBar (CBCGPRibbonBar* pRibbonBar)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::SetParentRibbonBar (pRibbonBar);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arSubItems [i];
		ASSERT_VALID (pButton);

		pButton->SetParentRibbonBar (pRibbonBar);
	}
}
//****
CRect CBCGPRibbonButton::GetKeyTipRect (CDC* pDC, BOOL bIsMenu)
{
	ASSERT_VALID (this);
	ASSERT_VALID (pDC);

	CSize sizeKeyTip = GetKeyTipSize (pDC);
	CRect rectKeyTip (0, 0, 0, 0);

	if (sizeKeyTip == CSize (0, 0) || m_rect.IsRectEmpty())
	{
		return rectKeyTip;
	}

	rectKeyTip.l = bIsMenu ? m_rect.r - sizeKeyTip.w / 2 : m_rect.l + 10;
	rectKeyTip.t = m_rect.b - sizeKeyTip.h / 2;

	CRect rectPanel;
	rectPanel.SetRectEmpty();

	CBCGPRibbonPanel* pPanel = GetParentPanel();
	if (pPanel != NULL && !IsMenuMode() && !(m_bQuickAccessMode && m_bFloatyMode) && !IsDefaultPanelButton())
	{
		ASSERT_VALID (pPanel);

		rectPanel = pPanel->GetRect();

		if (!rectPanel.IsRectEmpty())
		{
			rectPanel.b -= pPanel->GetCaptionHeight();
			rectKeyTip.t = rectPanel.b - sizeKeyTip.h / 2;
		}
	}

	if (IsDefaultPanelButton() && !m_bQuickAccessMode && !m_bFloatyMode)
	{
		rectKeyTip.t = m_rect.b;
		rectKeyTip.l = m_rect.CenterPoint().x - sizeKeyTip.w / 2;
	}
	else if (IsMainRibbonButton())
	{
		// Center key tip:
		rectKeyTip.t = m_rect.CenterPoint().y - sizeKeyTip.h / 2;
		rectKeyTip.l = m_rect.CenterPoint().x - sizeKeyTip.w / 2;
	}
	else if (m_bIsLargeImage || m_bFloatyMode)
	{
		if (m_bTextAlwaysOnRight)
		{
			if (!bIsMenu)
			{
				rectKeyTip.l = m_rect.l + GetImageSize (RibbonImageLarge).w - sizeKeyTip.w + 4;
			}

			rectKeyTip.t = m_rect.b - sizeKeyTip.h - 4;
		}
		else if (!bIsMenu)
		{
			rectKeyTip.l = m_rect.CenterPoint().x - sizeKeyTip.w / 2;
		}
	}
	else if (IsMenuMode())
	{
		rectKeyTip.t = m_rect.CenterPoint().y;
	}
	else
	{
		if (m_bQuickAccessMode)
		{
			rectKeyTip.l = m_rect.CenterPoint().x - sizeKeyTip.w / 2;
			rectKeyTip.t = m_rect.CenterPoint().y;
		}

		if (!rectPanel.IsRectEmpty())
		{
			if (m_rect.t < rectPanel.CenterPoint().y &&
				m_rect.b > rectPanel.CenterPoint().y)
			{
				rectKeyTip.t = m_rect.CenterPoint().y - sizeKeyTip.h / 2;
			}
			else if (m_rect.t < rectPanel.CenterPoint().y)
			{
				rectKeyTip.t = m_rect.t - sizeKeyTip.h / 2;
			}
		}
	}

	if (m_bIsTabElement)
	{
		rectKeyTip.t += 2;
	}

	rectKeyTip.r = rectKeyTip.l + sizeKeyTip.w;
	rectKeyTip.b = rectKeyTip.t + sizeKeyTip.h;

	return rectKeyTip;
}
//****
BOOL CBCGPRibbonButton::OnKey (BOOL bIsMenuKey)
{
	ASSERT_VALID (this);

	if (IsDisabled())
	{
		return FALSE;
	}

	if (m_rect.IsRectEmpty())
	{
		return CBCGPBaseRibbonElement::OnKey (bIsMenuKey);
	}

	CBCGPRibbonBar* pTopLevelRibbon = GetTopLevelRibbonBar();

	if (HasMenu() && (bIsMenuKey || m_strMenuKeys.IsEmpty()))
	{
		if (IsDroppedDown())
		{
			return TRUE;
		}

		if (pTopLevelRibbon != NULL)
		{
			pTopLevelRibbon->HideKeyTips();
		}

		CBCGPRibbonPanel* pPanel = GetParentPanel();
		if (pPanel != NULL)
		{
			ASSERT_VALID (pPanel);
			pPanel->SetFocused (this);
		}

		OnShowPopupMenu();

		if (m_pPopupMenu != NULL)
		{
			ASSERT_VALID (m_pPopupMenu);
			m_pPopupMenu->SendMessage (WM_KEYDOWN, VK_HOME);
		}

		return m_hMenu != NULL;
	}

	if (pTopLevelRibbon != NULL && pTopLevelRibbon->GetTopLevelFrame() != NULL)
	{
		pTopLevelRibbon->GetTopLevelFrame()->SetFocus();
	}

	OnClick (m_rect.TopLeft());
	return TRUE;
}
//****
void CBCGPRibbonButton::GetElementsByID (UINT uiCmdID,
		CArray <CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElements)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::GetElementsByID (uiCmdID, arElements);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arSubItems [i];
		ASSERT_VALID (pButton);

		pButton->GetElementsByID (uiCmdID, arElements);
	}
}
//****
void CBCGPRibbonButton::GetElements (
		CArray <CBCGPBaseRibbonElement*, CBCGPBaseRibbonElement*>& arElements)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::GetElements (arElements);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		CBCGPBaseRibbonElement* pButton = m_arSubItems [i];
		ASSERT_VALID (pButton);

		pButton->GetElements (arElements);
	}
}
//****
void CBCGPRibbonButton::OnAfterChangeRect (CDC* pDC)
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::OnAfterChangeRect (pDC);

	if (IsMainRibbonButton())
	{
		m_bIsLargeImage = TRUE;
		return;
	}

	m_bIsLargeImage = FALSE;

	if (m_bQuickAccessMode || m_bFloatyMode)
	{
		return;
	}

	CSize sizeImageLarge = GetImageSize (RibbonImageLarge);
	CSize sizeImageSmall = GetImageSize (RibbonImageSmall);

	if (m_bCompactMode || m_bIntermediateMode)
	{
		m_bIsLargeImage = FALSE;

		if (sizeImageLarge != CSize (0, 0) &&
			sizeImageSmall == CSize (0, 0))
		{
			m_bIsLargeImage = TRUE;
		}
	}
	else
	{
		BOOL bIsSmallIcon = FALSE;

		if (m_hIcon != NULL)
		{
			CSize sizeIcon = CSize (32, 32);

			if (globalData.GetRibbonImageScale() != 1.)
			{
				sizeIcon.w = (int) (.5 + globalData.GetRibbonImageScale() * sizeIcon.w);
				sizeIcon.h = (int) (.5 + globalData.GetRibbonImageScale() * sizeIcon.h);
			}

			bIsSmallIcon =	sizeIcon.w > m_rect.Width() ||
							sizeIcon.h > m_rect.Height();
		}

		if (sizeImageLarge != CSize (0, 0) && !bIsSmallIcon)
		{
			m_bIsLargeImage = TRUE;
		}
	}

	if (m_bIsLargeImage)
	{
		SetMargin (CSize (nLargeButtonMarginX, nLargeButtonMarginY));
	}
	else if (m_szMargin == CSize (nLargeButtonMarginX, nLargeButtonMarginY))
	{
		SetMargin (CSize (nSmallButtonMarginX, nSmallButtonMarginY));
	}
}
//****
void CBCGPRibbonButton::FillWindowList()
{
	if (m_nWindowsMenuItems > 0)
	{
		for (int i = 0; i < m_nWindowsMenuItems; i++)
		{
			int nIndex = (int) m_arSubItems.GetSize() - 1;

			delete m_arSubItems [nIndex];
			m_arSubItems.RemoveAt (nIndex);
		}
	}

	m_nWindowsMenuItems = 0;

	CBCGPRibbonBar* pTopLevelRibbon = GetTopLevelRibbonBar();
	if (pTopLevelRibbon == NULL)
	{
		return;
	}

	CBCGPMDIFrameWnd* pMDIFrameWnd = DYNAMIC_DOWNCAST (
		CBCGPMDIFrameWnd, pTopLevelRibbon->GetTopLevelFrame());
	if (pMDIFrameWnd == NULL)
	{
		return;
	}

	const int nMaxFiles = 9;

	HWND hwndT = ::GetWindow (pMDIFrameWnd->m_hWndMDIClient, GW_CHILD);
	int i = 0;

	for (i = 0; hwndT != NULL && i < nMaxFiles; i++)
	{
		CBCGPMDIChildWnd* pFrame = DYNAMIC_DOWNCAST (CBCGPMDIChildWnd, 
			CWnd::FromHandle (hwndT));
		if (pFrame == NULL)
		{
			hwndT = ::GetWindow(hwndT,GW_HWNDNEXT);
			continue;
		}

		if (!pFrame->CanShowOnWindowsList())
		{
			hwndT = ::GetWindow(hwndT,GW_HWNDNEXT);
			continue;
		}

		if (i == 0)
		{
			CBCGPRibbonSeparator* pSeparator = new CBCGPRibbonSeparator (TRUE);
			pSeparator->SetDefaultMenuLook();

			AddSubItem (pSeparator);
			m_nWindowsMenuItems = 1;
		}

		TCHAR	szWndTitle[256];
		::GetWindowText(hwndT,szWndTitle,sizeof(szWndTitle)/sizeof(szWndTitle[0]));

		CString strItem;
		strItem.Format (_T("&%d %s"), i + 1, szWndTitle);

		CBCGPRibbonButton* pItem = new CBCGPRibbonButton (AFX_IDM_FIRST_MDICHILD, strItem);
		pItem->SetData ((DWORD_PTR) hwndT);
		pItem->SetDefaultMenuLook();
		pItem->m_pRibbonBar = m_pRibbonBar;

		AddSubItem (pItem);

		hwndT = ::GetWindow (hwndT,GW_HWNDNEXT);
		m_nWindowsMenuItems++;
	}

	if (pMDIFrameWnd->m_uiWindowsDlgMenuId != 0 &&
		(i == nMaxFiles || pMDIFrameWnd->m_bShowWindowsDlgAlways))
	{
		//-------------------------
		// Add "Windows..." dialog:
		//-------------------------
		CBCGPRibbonButton* pItem = new CBCGPRibbonButton (
			pMDIFrameWnd->m_uiWindowsDlgMenuId, pMDIFrameWnd->m_strWindowsDlgMenuText);
		pItem->SetDefaultMenuLook();
		pItem->m_pRibbonBar = m_pRibbonBar;

		AddSubItem (pItem);
		m_nWindowsMenuItems++;
	}
}
//****
int CBCGPRibbonButton::GetGroupButtonExtraWidth()
{
	if (m_pParentGroup == NULL)
	{
		return 0;
	}

	ASSERT_VALID (m_pParentGroup);

	switch (m_pParentGroup->GetCount())
	{
	case 1:
		return 2;

	case 2:
		if (m_Location != RibbonElementFirstInGroup)
		{
			return 0;
		}
		break;
	}

	return m_Location == RibbonElementFirstInGroup || m_Location == RibbonElementLastInGroup ? 1 : 2;
}
//****
CSize CBCGPRibbonButton::DrawBottomText (CDC* pDC, BOOL bCalcOnly)
{
	ASSERT_VALID (this);

	if (m_pParent == NULL)
	{
		return CSize (0, 0);
	}

	if (m_strText.IsEmpty())
	{
		return CSize (0, 0);
	}

	ASSERT_VALID (m_pParent);

	const CSize sizeImageLarge = m_pParent->GetImageSize (TRUE);
	if (sizeImageLarge == CSize (0, 0))
	{
		ASSERT (FALSE);
		return CSize (0, 0);
	}

	CSize sizeText = pDC->GetTextExtent (m_strText);

	const int nTextLineHeight = sizeText.h;
	int nMenuArrowWidth = (HasMenu() || IsDefaultPanelButton()) ? (CBCGPMenuImages::Size().w) : 0;

	if (nMenuArrowWidth != 0 && globalData.GetRibbonImageScale() > 1.)
	{
		nMenuArrowWidth = (int)(.5 + globalData.GetRibbonImageScale() * nMenuArrowWidth);
	}

	if (bCalcOnly)
	{
		const CString strDummyAmpSeq = _T("\001\001");

		m_nWrapIndex = -1;
		int nTextWidth = 0;

		if (m_arWordIndexes.GetSize() == 0) // 1 word
		{
			nTextWidth = sizeText.w;
		}
		else
		{
			nTextWidth = 32767;

			for (int i = 0; i < m_arWordIndexes.GetSize(); i++)
			{
				int nIndex = m_arWordIndexes [i];

				CString strLineOne = m_strText.Left (nIndex);

				if (!IsDefaultPanelButton())
				{
					strLineOne.Replace (_T("&&"), strDummyAmpSeq);
					strLineOne.Remove (_T('&'));
					strLineOne.Replace (strDummyAmpSeq, _T("&"));
				}

				const int cx1 = pDC->GetTextExtent (strLineOne).w;

				CString strLineTwo = m_strText.Mid (nIndex + 1);

				if (!IsDefaultPanelButton())
				{
					strLineTwo.Replace (_T("&&"), strDummyAmpSeq);
					strLineTwo.Remove (_T('&'));
					strLineTwo.Replace (strDummyAmpSeq, _T("&"));
				}

				const int cx2 = pDC->GetTextExtent (strLineTwo).w + nMenuArrowWidth;

				int nWidth = max (cx1, cx2);

				if (nWidth < nTextWidth)
				{
					nTextWidth = nWidth;
					m_nWrapIndex = nIndex;
				}
			}
		}

		if (nTextWidth % 2)
		{
			nTextWidth--;
		}

		CSize size (nTextWidth, nTextLineHeight * 2);
		return size;
	}

	int y = m_rect.t + nLargeButtonMarginY + sizeImageLarge.h + 5;
	CRect rectMenuArrow (0, 0, 0, 0);

	if (IsDefaultPanelButton())
	{
		y += nDefaultPaneButtonMargin;
	}

	CRect rectText = m_rect;
	rectText.t = y;

	UINT uiDTFlags = DT_SINGLELINE | DT_CENTER;
	if (IsDefaultPanelButton())
	{
		uiDTFlags |= DT_NOPREFIX;
	}

	if (m_nWrapIndex == -1)
	{
		// Single line text
		pDC->DrawText (m_strText, rectText, uiDTFlags);

		if (HasMenu() || IsDefaultPanelButton())
		{
			rectMenuArrow = m_rect;

			rectMenuArrow.t = y + nTextLineHeight + 2;
			rectMenuArrow.l = m_rect.CenterPoint().x - CBCGPMenuImages::Size().w / 2 - 1;
		}
	}
	else
	{
		CString strLineOne = m_strText.Left (m_nWrapIndex);
		pDC->DrawText (strLineOne, rectText, uiDTFlags);

		rectText.t = y + nTextLineHeight;
		rectText.r -= nMenuArrowWidth;

		CString strLineTwo = m_strText.Mid (m_nWrapIndex + 1);
		pDC->DrawText (strLineTwo, rectText, uiDTFlags);

		if (HasMenu() || IsDefaultPanelButton())
		{
			rectMenuArrow = rectText;

			rectMenuArrow.t += 2;
			rectMenuArrow.l = rectText.r - (rectText.Width() - pDC->GetTextExtent (strLineTwo).w) / 2;
		}
	}

	if (!rectMenuArrow.IsRectEmpty())
	{
		int nMenuArrowHeight = CBCGPMenuImages::Size().h;

		rectMenuArrow.b = rectMenuArrow.t + nMenuArrowHeight;
		rectMenuArrow.r = rectMenuArrow.l + nMenuArrowWidth;

		CRect rectWhite = rectMenuArrow;
		rectWhite.OffsetRect (0, 1);

		CBCGPMenuImages::IMAGES_IDS id = 
			globalData.GetRibbonImageScale() > 1. ? 
			CBCGPMenuImages::IdArowDownLarge : CBCGPMenuImages::IdArowDown;

		CBCGPMenuImages::Draw (pDC, id, rectWhite, CBCGPMenuImages::ImageWhite);
		CBCGPMenuImages::Draw (pDC, id, rectMenuArrow, 
			m_bIsDisabled ? CBCGPMenuImages::ImageGray : CBCGPMenuImages::ImageBlack);
	}

	return CSize (0, 0);
}
//****
BOOL CBCGPRibbonButton::SetACCData (CWnd* pParent, CBCGPAccessibilityData& data)
{
	ASSERT_VALID (this);

	if (!CBCGPBaseRibbonElement::SetACCData (pParent, data))
	{
		return FALSE;
	}

	if (HasMenu())
	{
		data.m_nAccRole = IsCommandAreaHighlighted() ? ROLE_SYSTEM_SPLITBUTTON : ROLE_SYSTEM_BUTTONDROPDOWN;
		data.m_nAccRole = ROLE_SYSTEM_BUTTONDROPDOWN;
	
		if (!IsCommandAreaHighlighted())
		{
			data.m_bAccState |= STATE_SYSTEM_HASPOPUP;
			data.m_strAccDefAction = _T("Open");

			if (IsDroppedDown())
			{
				data.m_bAccState |= STATE_SYSTEM_PRESSED;
				data.m_strAccDefAction = _T("Close");
			}
		}
	}

	return TRUE;
}
//****
void CBCGPRibbonButton::GetItemIDsList (CList<UINT,UINT>& lstItems) const
{
	ASSERT_VALID (this);

	CBCGPBaseRibbonElement::GetItemIDsList (lstItems);

	for (int i = 0; i < m_arSubItems.GetSize(); i++)
	{
		ASSERT_VALID (m_arSubItems [i]);
		m_arSubItems [i]->GetItemIDsList (lstItems);
	}
}

#endif // BCGP_EXCLUDE_RIBBON