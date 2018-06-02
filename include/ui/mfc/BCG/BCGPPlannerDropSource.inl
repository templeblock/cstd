
//
// BCGPPlannerDropSource.cpp : implementation file
//

#include "BCGPPlannerDropSource.h"

#ifndef BCGP_EXCLUDE_PLANNER

/////////////////////////////////////////////////////////////////////////////
// CBCGPPlannerDropSource

CBCGPPlannerDropSource::CBCGPPlannerDropSource()
	: m_bEscapePressed (FALSE)
	, m_bDragStarted   (FALSE)
	, m_hcurMove       (NULL)
	, m_hcurCopy       (NULL)
{
}

CBCGPPlannerDropSource::~CBCGPPlannerDropSource()
{
	Empty ();
}

void CBCGPPlannerDropSource::Empty ()
{
	m_bEscapePressed = FALSE;
	m_bDragStarted = FALSE;

	if (m_hcurMove != NULL)
	{
		::DestroyCursor (m_hcurMove);
		m_hcurMove = NULL;
	}

	if (m_hcurCopy != NULL)
	{
		::DestroyCursor (m_hcurCopy);
		m_hcurCopy = NULL;
	}
}

SCODE CBCGPPlannerDropSource::GiveFeedback(DROPEFFECT dropEffect) 
{
	HCURSOR hcur = NULL;

	switch (dropEffect)
	{
	case DROPEFFECT_MOVE:
		hcur = m_hcurMove;
		break;

	case DROPEFFECT_COPY:
		hcur = m_hcurCopy;
		break;
	}

	if (hcur == NULL)
	{
		return COleDropSource::GiveFeedback(dropEffect);
	}

	::SetCursor (hcur);
	
	return S_OK;
}

SCODE CBCGPPlannerDropSource::QueryContinueDrag(BOOL bEscapePressed, DWORD dwKeyState) 
{
	m_bEscapePressed = bEscapePressed;

	return COleDropSource::QueryContinueDrag(bEscapePressed, dwKeyState);
}

BOOL CBCGPPlannerDropSource::OnBeginDrag(CWnd* pWnd) 
{
/*
	if (m_hcurMove == NULL && m_hcurCopy == NULL)
	{
//		CBCGPLocalResource locaRes;
		m_hcurMove = AfxGetApp ()->LoadCursor (IDC_BCGBARRES_MOVE);
		m_hcurCopy = AfxGetApp ()->LoadCursor (IDC_BCGBARRES_COPY);
	}
*/
	m_bDragStarted = COleDropSource::OnBeginDrag(pWnd);
	
	return m_bDragStarted;
}

#endif // BCGP_EXCLUDE_PLANNER