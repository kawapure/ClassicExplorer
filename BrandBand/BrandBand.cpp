/*
 * BrandBand.cpp: Implements the throbber/brand band. This displays the Windows logo
 *                in the upper-right corner of the Explorer window.
 */

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include <commoncontrols.h>
#include "util/util.h"

#include "BrandBand.h"

void CBrandBand::ClearResources()
{
	DeleteObject(m_hBitmap);
	m_pWebBrowser.Release();
}

/*
 * OnPaint: Paint the background and the logo.
 */
LRESULT CBrandBand::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	PAINTSTRUCT paintInfo;
	RECT clientRect;

	HDC dc = BeginPaint(&paintInfo);
	GetClientRect(&clientRect);

	// Calculate destination point:
	POINT destinationPoint;
	destinationPoint.x = (clientRect.right - clientRect.left - m_cxCurBmp) / 2;
	destinationPoint.y = (clientRect.bottom - clientRect.top - m_cyCurBmp) / 2;

	COLORREF background;
	if (m_theme == CLASSIC_EXPLORER_2K)
	{
		background = RGB(0, 0, 0);
	}
	else
	{
		background = RGB(255, 255, 255);
	}

	SetBkColor(dc, background);

	// Draw the background
	HBRUSH bgBrush = CreateSolidBrush(background);
	FillRect(dc, &clientRect, bgBrush);
	DeleteObject(bgBrush);

	HDC sourceDc = CreateCompatibleDC(dc);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(sourceDc, m_hBitmap);

	BitBlt(
		dc, 
		destinationPoint.x, 
		destinationPoint.y, 
		m_cxCurBmp, 
		m_cyCurBmp, 
		sourceDc, 
		0, 
		0, 
		SRCCOPY
	);

	SelectObject(sourceDc, oldBitmap);
	DeleteDC(sourceDc);

	EndPaint(&paintInfo);

	return S_OK;
}

/*
 * OnSize: Handle size messages and reload the desired logo bitmap for the current size.
 */
LRESULT CBrandBand::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LoadBitmapForSize();
	
	return DefWindowProcW(uMsg, wParam, lParam);
}

/*
 * OnEraseBackground: We always paint our own background, so there's no need to ever do this.
 */
LRESULT CBrandBand::OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return 1;
}

LRESULT CBrandBand::OnClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CEUtil::CESettings currentSettings = CEUtil::GetCESettings();

	HMENU hMenu = CreatePopupMenu();

	AppendMenuW(hMenu, (m_theme == CLASSIC_EXPLORER_2K ? MF_CHECKED : MF_UNCHECKED) | MF_STRING, m_theme == CLASSIC_EXPLORER_2K ? 0 : 7000, L"2K Skin");
	AppendMenuW(hMenu, (m_theme == CLASSIC_EXPLORER_XP ? MF_CHECKED : MF_UNCHECKED) | MF_STRING, m_theme == CLASSIC_EXPLORER_XP ? 0 : 7001, L"XP Skin");

	AppendMenuW(hMenu, MF_SEPARATOR, 0, nullptr);

	AppendMenuW(hMenu, (currentSettings.showGoButton ? MF_CHECKED : MF_UNCHECKED) | MF_STRING, 7010, L"Show Go button");
	AppendMenuW(hMenu, (currentSettings.showAddressLabel ? MF_CHECKED : MF_UNCHECKED) | MF_STRING, 7011, L"Show Address label");
	AppendMenuW(hMenu, (currentSettings.showFullAddress ? MF_CHECKED : MF_UNCHECKED) | MF_STRING, 7012, L"Show full address");

	POINT p;
	p.x = GET_X_LPARAM(lParam);
	p.y = GET_Y_LPARAM(lParam);
	HWND hWnd;
	GetWindow(&hWnd);
	ClientToScreen(&p);

	int sel = TrackPopupMenu(hMenu, TPM_RETURNCMD, p.x, p.y, 0, hWnd, nullptr);
	DestroyMenu(hMenu);

	if(sel == 0) // Current theme selected, or outside click, nothing changes
		return S_OK;
	switch (sel)
	{
	case 7000:
		CEUtil::WriteCESettings(CEUtil::CESettings(CLASSIC_EXPLORER_2K, -1, -1,-1));
		break;
	case 7001:
		CEUtil::WriteCESettings(CEUtil::CESettings(CLASSIC_EXPLORER_XP, -1, -1,-1));
		break;
	case 7010:
		CEUtil::WriteCESettings(CEUtil::CESettings(CLASSIC_EXPLORER_NONE, !currentSettings.showGoButton, -1, -1));
		break;
	case 7011:
		CEUtil::WriteCESettings(CEUtil::CESettings(CLASSIC_EXPLORER_NONE, -1, !currentSettings.showAddressLabel, -1));
		break;
	case 7012:
		CEUtil::WriteCESettings(CEUtil::CESettings(CLASSIC_EXPLORER_NONE, -1, -1, !currentSettings.showFullAddress));
		break;
	}
	MessageBeep(0);
	MessageBox(L"Open a new file explorer window to see the changes.");
	return S_OK;
}

/*
 * LoadBitmapForSize: Load the desired throbber icon for the current size of the band.
 */
LRESULT CBrandBand::LoadBitmapForSize()
{
	RECT curRect;
	GetClientRect(&curRect);

	DeleteObject(m_hBitmap);

	int cySelf = curRect.bottom - curRect.top;
	int resourceId;

	if (cySelf >= 38)
	{
		switch (m_theme)
		{
		default:
		case CLASSIC_EXPLORER_2K:
			resourceId = IDB_2K_THROBBER_SIZE_LARGE;
			break;
		case CLASSIC_EXPLORER_XP:
			resourceId = IDB_XP_THROBBER_SIZE_LARGE;
			break;
		}
	}
	else if (cySelf >= 26)
	{
		switch (m_theme)
		{
		default:
		case CLASSIC_EXPLORER_2K:
			resourceId = IDB_2K_THROBBER_SIZE_MID;
			break;
		case CLASSIC_EXPLORER_XP:
			resourceId = IDB_XP_THROBBER_SIZE_MID;
			break;
		}
	}
	else
	{
		switch (m_theme)
		{
		default:
		case CLASSIC_EXPLORER_2K:
			resourceId = IDB_2K_THROBBER_SIZE_SMALL;
			break;
		case CLASSIC_EXPLORER_XP:
			resourceId = IDB_XP_THROBBER_SIZE_SMALL;
			break;
		}
	}

	m_hBitmap = LoadBitmapW(
		_AtlBaseModule.GetResourceInstance(),
		MAKEINTRESOURCEW(resourceId)
	);

	BITMAP bmp;
	GetObject(m_hBitmap, sizeof(bmp), &bmp);

	m_cxCurBmp = bmp.bmWidth;
	m_cyCurBmp = bmp.bmHeight;

	return S_OK;
}

/* 
 * CorrectBandSize: Correct the height of the band to be displayed.
 *
 * This may seem like a hacky solution, but it is actually necessary, as in my testing, using
 * the default integral scaling would only scale up and never scale back down to fit sibling
 * toolbars on the same row.
 *
 * Using this solution, the band is resized by sending some messages to the ReBar API which is
 * owned by Explorer.
 *
 * I spent several days straight trying to figure out how to do this; please don't make fun of
 * me too much...
 */
LRESULT CBrandBand::CorrectBandSize()
{
	RECT curRect;
	GetClientRect(&curRect);

	UINT initialTrHeight = ::SendMessageW(m_parentRebar, RB_GETROWHEIGHT, 0, 0);

	REBARBANDINFOW curBandInfo;
	curBandInfo.cbSize = sizeof(REBARBANDINFOW);
	curBandInfo.fMask = RBBIM_CHILD;

	// This was never an officially-supported feature of shell rebars in Windows, so there's
	// no function to just get the interface for our own rebar band.
	for (int i = 0; i < ::SendMessageW(m_parentRebar, RB_GETBANDCOUNT, 0, 0); i++)
	{
		::SendMessageW(m_parentRebar, RB_GETBANDINFO, i, (LPARAM)&curBandInfo);

		if (::IsWindow(curBandInfo.hwndChild) && curBandInfo.hwndChild == m_hWnd)
		{
			// Get the rebar info:
			REBARBANDINFOW curBandInfo2;
			curBandInfo2.cbSize = sizeof(REBARBANDINFOW);
			curBandInfo2.fMask = RBBIM_SIZE | RBBIM_CHILDSIZE;
			::SendMessageW(m_parentRebar, RB_GETBANDINFO, i, (LPARAM)&curBandInfo2);

			// Set the bar to the minimum size possible.
			// Yes, it was necessary to define all of these, or it would half in horizontal size
			// every time this function was called.
			curBandInfo2.fMask = RBBIM_CHILDSIZE;
			curBandInfo2.cxMinChild = 38;
			curBandInfo2.cyMinChild = 22;
			curBandInfo2.cyChild = 22;
			curBandInfo2.cyIntegral = 1;
			::SendMessageW(m_parentRebar, RB_SETBANDINFOW, i, (LPARAM)&curBandInfo2);

			// Resize the bar back up to what it should be: the size of the topmost row.
			UINT topRowHeight = ::SendMessageW(m_parentRebar, RB_GETROWHEIGHT, 0, 0);
			curBandInfo2.cyChild = topRowHeight;
			::SendMessageW(m_parentRebar, RB_SETBANDINFOW, i, (LPARAM)&curBandInfo2);

			// Explorer isn't notified of this resize, so we need to manually invalidate the
			// visual or a vertical gap may be left under the rebar.
			if (initialTrHeight > topRowHeight)
			{
				m_shouldManuallyCorrectHeight = true;
			}
		}
	}

	return S_OK;
}

/*
 * ShouldRefreshVisual: Determines if the visual needs manual correction from our code.
 */
bool CBrandBand::ShouldRefreshVisual()
{
	if (::IsWindow(m_parentRebar))
	{
		int numRebars = ::SendMessageW(m_parentRebar, RB_GETBANDCOUNT, 0, 0);

		// If there is only one rebar control for whatever reason, then we don't want to try
		// redrawing or a redraw loop may occur.
		if (numRebars > 1)
		{
			RECT rcOffset;
			GetClientRect(&rcOffset);
			::MapWindowPoints(m_hWnd, m_parentRebar, (LPPOINT)&rcOffset, 2);

			WCHAR buf[512];
			wsprintf(buf, L"%d, %d", rcOffset.left, rcOffset.top);

			//MessageBoxW(buf, L"hi!", MB_OK);

			if (rcOffset.left < 12)
			{
				return true;
			}
		}
	}

	return false;
}

void CBrandBand::PerformRedrawCheck()
{
	const int checkDuration = 500;
	const int allowedRedrawsInTimeSpan = 50;

	unsigned int previousRedrawTime = m_latestRedrawTime;
	m_latestRedrawTime = GetTickCount64();

	if (m_latestRedrawTime < previousRedrawTime + checkDuration)
	{
		m_redrawCounter++;

		if (m_redrawCounter > allowedRedrawsInTimeSpan)
		{
			m_disableRedraws = true;
			#ifdef DEBUG
			MessageBoxW(L"Redraw loop mitigated.");
			#endif
		}
	}
	else
	{
		m_redrawCounter = 0;
	}
}

/*
 * RebarParentSubclassProc: This reads notifications (not messages) of the rebar, which is done by hooking
 *                          its parent (the shell WorkerW).
 *
 * This is used to initialise the size-correction routine, since it works in a predictable manner, sending
 * notifications about changes to the parent before enacting those changes on the child.
 */
LRESULT CALLBACK CBrandBand::RebarParentSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CBrandBand *self = (CBrandBand *)dwRefData;

	if (uMsg == WM_NOTIFY)
	{
		LPNMHDR hdr = (LPNMHDR)lParam;

		switch (hdr->code)
		{
			case RBN_HEIGHTCHANGE:
			case RBN_LAYOUTCHANGED:
			{
				self->CorrectBandSize();
				break;
			}
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

/*
 * RebarSubclassProc: This reads messages sent to the rebar itself, which is used to send out the sizing
 *                    command when it is needed.
 */
LRESULT CALLBACK CBrandBand::RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CBrandBand *self = (CBrandBand *)dwRefData;
	
	if (uMsg == WM_SIZE)
	{
		if (self->m_shouldManuallyCorrectHeight)
		{
			// Mark the correction as no longer necessary as we handle it here:
			self->m_shouldManuallyCorrectHeight = false;

			LRESULT result = DefSubclassProc(hWnd, uMsg, wParam, lParam);
			CEUtil::FixExplorerSizesIfNecessary(self->m_parentRebar);
			return result;
		}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

//================================================================================================================
// implement IDeskBand:
//
STDMETHODIMP CBrandBand::GetBandInfo(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pDbi)
{
	/*if (!m_subclassedRebar)
	{
		m_parentRebar = GetParent();

		SetWindowSubclass(::GetParent(m_parentRebar), RebarParentSubclassProc, (UINT_PTR)this, (UINT_PTR)this);
		SetWindowSubclass(m_parentRebar, RebarSubclassProc, (UINT_PTR)this, (UINT_PTR)this);

		m_subclassedRebar = true;
	}*/

	if (pDbi)
	{
		if (pDbi->dwMask & DBIM_MINSIZE)
		{
			pDbi->ptMinSize.x = 38;
			pDbi->ptMinSize.y = 22;
		}
		if (pDbi->dwMask & DBIM_MAXSIZE)
		{
			// The throbber should be able to adjust to the size of any sibling rebar, but
			// should always be locked to a size of 38 pixels horizontally.
			pDbi->ptMaxSize.x = 38;
			pDbi->ptMaxSize.y = -1;
		}
		if (pDbi->dwMask & DBIM_INTEGRAL)
		{
			// We want the throbber to grow organically alongside sibling rebars, without any
			// blank spaces. As such, it is to grow every 1 pixel vertically.
			pDbi->ptIntegral.x = 0;
			pDbi->ptIntegral.y = 1;
		}
		if (pDbi->dwMask & DBIM_ACTUAL)
		{
			pDbi->ptActual.x = 38;
			pDbi->ptActual.y = -1;
		}
		if (pDbi->dwMask & DBIM_TITLE)
		{
			// No title.
			wcscpy_s(pDbi->wszTitle, L"");
		}
		if (pDbi->dwMask & DBIM_MODEFLAGS)
		{
			pDbi->dwModeFlags = DBIMF_FIXED | DBIMF_TOPALIGN | DBIMF_VARIABLEHEIGHT;
		}
		if (pDbi->dwMask & DBIM_BKCOLOR)
		{
			// We draw the background ourselves (in OnPaint), so there's no need to bother
			// with it here.
			pDbi->crBkgnd = 0;
		}
	}

	return S_OK;
}

//================================================================================================================
// implement IOleWindow:
//
STDMETHODIMP CBrandBand::GetWindow(HWND *hWnd)
{
	if (!hWnd)
	{
		return E_INVALIDARG;
	}

	*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CBrandBand::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}

//================================================================================================================
// implement IDockingWindow:
//
STDMETHODIMP CBrandBand::CloseDW(unsigned long dwReserved)
{
	ShowDW(FALSE);

	if (IsWindow())
		DestroyWindow();

	m_hWnd = nullptr;

	return S_OK;
}

STDMETHODIMP CBrandBand::ResizeBorderDW(const RECT *pRcBorder, IUnknown *pUnkToolbarSite, BOOL fReserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP CBrandBand::ShowDW(BOOL fShow)
{
	if (m_hWnd)
		ShowWindow(fShow ? SW_SHOW : SW_HIDE);

	return S_OK;
}

//================================================================================================================
// implement IObjectWithSite:
//

/**
 * SetSite: Responsible for installation or removal of the toolbar band from a location
 *          provided by Explorer.
 *
 * This function is additionally responsible for obtaining the shell control APIs and creating
 * the inner toolbar window.
 */
STDMETHODIMP CBrandBand::SetSite(IUnknown *pUnkSite)
{
	IObjectWithSiteImpl<CBrandBand>::SetSite(pUnkSite);

	HRESULT hr;
	CComPtr<IOleWindow> oleWindow;

	if (pUnkSite == nullptr)
	{
		ClearResources();
		return S_OK;
	}

	HWND hWndParent = nullptr;

	CComQIPtr<IOleWindow> pOleWindow = pUnkSite;
	if (pOleWindow)
		pOleWindow->GetWindow(&hWndParent);

	if (!::IsWindow(hWndParent))
	{
		return E_FAIL;
	}

	this->Create(
		hWndParent,
		nullptr,
		nullptr,
		WS_VISIBLE | WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN
	);
	
	if (!IsWindow())
		return E_FAIL;

	CComQIPtr<IServiceProvider> pProvider = pUnkSite;
	if (pProvider)
	{
		// No error handling; this can fail safely.
		pProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void **)&m_pWebBrowser);

		if (m_pWebBrowser)
		{
			if (m_dwEventCookie == 0xFEFEFEFE)
			{
				//MessageBox(L"d");
				DispEventAdvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
			}
		}
	}

	LoadBitmapForSize();


	// Subclass helpers:
	m_parentRebar = GetParent();

	SetWindowSubclass(::GetParent(m_parentRebar), RebarParentSubclassProc, (UINT_PTR)this, (UINT_PTR)this);
	SetWindowSubclass(m_parentRebar, RebarSubclassProc, (UINT_PTR)this, (UINT_PTR)this);

	m_subclassedRebar = true;

	// Read settings from registry
	CEUtil::CESettings cS = CEUtil::GetCESettings();
	m_theme = cS.theme;

	// Explorer may initialise our position onto a separate rebar until the sizes are
	// invalidated, so let's manually invalidate to correct the position:
	CEUtil::FixExplorerSizes(this->m_hWnd);

	return S_OK;
}

//================================================================================================================
// handle DWebBrowserEvents2:
//
STDMETHODIMP CBrandBand::OnNavigateComplete(IDispatch *pDisp, VARIANT *url)
{
	//MessageBox(L"fuck you");
	CEUtil::FixExplorerSizes(this->m_hWnd);
	//::SendMessageW(m_parentRebar, WM_SIZE, 0, 1);

	return S_OK;
}

/**
 * OnQuit: Called when the user attempts to quit the browser.
 *
 * Copied from Open-Shell implementation here:
 * https://github.com/Open-Shell/Open-Shell-Menu/blob/master/Src/ClassicExplorer/ExplorerBand.cpp#L2280-L2285
 */
STDMETHODIMP CBrandBand::OnQuit()
{
	if (m_pWebBrowser && m_dwEventCookie != 0xFEFEFEFE)
	{
		return DispEventUnadvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
	}

	return S_OK;
}