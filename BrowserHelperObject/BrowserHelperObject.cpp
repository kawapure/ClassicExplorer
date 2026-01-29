/*
 * BrowserHelperObject.cpp: Manages general/background tasks for Explorer.
 */
#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include <commoncontrols.h>
#include "util/util.h"

#include "util/shell_undoc.h"
#include "BrowserHelperObject.h"

void BrowserHelperObject::Cleanup()
{
	m_pSite = nullptr;
	m_parentWindow = nullptr;
	//UnhookWindowsHookEx(m_hHook);
}

HRESULT BrowserHelperObject::_DoUpdateWatermark(HWND listView)
{
	HRESULT hr = S_OK;

	int resourceId = 0;
	bool shouldUse = false;

	CComPtr<IShellView> pView;
	m_pShellBrowser->QueryActiveShellView(&pView);
	CComPtr<IFolderType> pType;
	pView->QueryInterface(IID_IFolderType, (void **)&pType);

	FOLDERTYPEID curFolderType;
	pType->GetFolderType(&curFolderType);

	if (curFolderType == FOLDERTYPEID_Pictures)
	{
		resourceId = IDB_BG_PICTURES_BLUE;
		shouldUse = true;
	}
	else if (curFolderType == FOLDERTYPEID_Music)
	{
		resourceId = IDB_BG_MUSIC_BLUE;
		shouldUse = true;
	}
	else if (curFolderType == FOLDERTYPEID_Videos)
	{
		resourceId = IDB_BG_VIDEOS_BLUE;
		shouldUse = true;
	}

	if (shouldUse)
	{
		HBITMAP bitmap = LoadBitmapW(
			_AtlBaseModule.GetResourceInstance(),
			MAKEINTRESOURCEW(resourceId)
		);

		LVBKIMAGEW bkImage = { 0 };
		bkImage.ulFlags = LVBKIF_TYPE_WATERMARK;
		bkImage.xOffsetPercent = 100;
		bkImage.yOffsetPercent = 100;
		bkImage.hbm = bitmap;

		if (!SendMessageW(listView, LVM_SETBKIMAGEW, 0, (LPARAM)&bkImage))
		{
			DeleteObject(bitmap);
			WCHAR buffer[128];
			swprintf_s(buffer, L"Failed to apply background image on HWND: %x", listView);
			MessageBoxW(nullptr, buffer, L"Fuck you", MB_OK);
		}
	}
	else
	{
		// Remove the background
		SendMessageW(listView, LVM_SETBKIMAGEW, 0, 0);
	}

	return hr;
}

HRESULT BrowserHelperObject::UpdateWatermark()
{
	bool found = false;

	EnumChildWindows(m_parentWindow, [](HWND hWnd, LPARAM lParam) -> BOOL CALLBACK {
		BrowserHelperObject *pThis = (BrowserHelperObject *)lParam;
		WCHAR className[MAX_PATH];
		if (GetClassName(hWnd, (LPWSTR)&className, MAX_PATH) && (_wcsicmp(className, L"SysListView32") == 0))
		{
			HWND parent = GetParent(hWnd);
			if (parent)
			{
				WCHAR className2[MAX_PATH];
				GetClassNameW(parent, (LPWSTR)&className2, MAX_PATH);
				if (wcscmp(className2, L"SHELLDLL_DefView") != 0)
				{
					// not the right ListView
					return TRUE;
				}
			}

			pThis->_DoUpdateWatermark(hWnd);

			return FALSE;
		}
		return TRUE;
	}, (LPARAM)this);

	if (!found)
	{
		// hmm
	}

	return S_OK;
}


LRESULT CALLBACK BrowserHelperObject::s_HookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HCBT_CREATEWND)
	{
		HWND hWnd = (HWND)wParam;
		CBT_CREATEWNDW *pCreate = (CBT_CREATEWNDW *)lParam;


	}

	return CallNextHookEx(nullptr, nCode, wParam, lParam);
}

//================================================================================================================
// implement IObjectWithSite:
//

/*
 * SetSite: Responsible for installation or removal of the toolbar band from a location provided
 *          by the Shell.
 *
 * This function is additionally responsible for obtaining the shell control APIs and creating
 * the actual toolbar control window.
 */
STDMETHODIMP BrowserHelperObject::SetSite(IUnknown *pUnkSite)
{
	IObjectWithSiteImpl<BrowserHelperObject>::SetSite(pUnkSite);

	// If pUnkSite is not nullptr, then the site is being set.
	// Otherwise, the site is being removed.
	if (pUnkSite) // hook:
	{
		CComQIPtr<IServiceProvider> pProvider = pUnkSite;

		// Set Windows hooks for additional things:
		//m_hHook = SetWindowsHookExW(WH_CBT, s_HookProc, nullptr, GetCurrentThreadId());

		if (pProvider)
		{
			CComPtr<IShellBrowser> pShellBrowser;
			pProvider->QueryService(SID_SShellBrowser, IID_IShellBrowser, (void **)&m_pShellBrowser);
			pProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void **)&m_pWebBrowser);

			if (m_pWebBrowser)
			{
				if (m_dwEventCookie == 0xFEFEFEFE)
				{
					DispEventAdvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
				}
			}

			HWND parentWindow = nullptr;
			if (SUCCEEDED(m_pShellBrowser->GetWindow(&parentWindow)))
			{
				m_parentWindow = GetAncestor(parentWindow, GA_ROOT);
			}
			else
			{
				MessageBox(nullptr, nullptr, L"could not get parent window", MB_OK);
			}
		}
	}
	else // unhook:
	{
		Cleanup();
	}

	return S_OK;
}

//================================================================================================================
// handle DWebBrowserEvents2:
//

STDMETHODIMP BrowserHelperObject::OnNavigateComplete(IDispatch *pDisp, VARIANT *url)
{
	UpdateWatermark();
	return S_OK;
}

/**
 * OnQuit: Called when the user attempts to quit the Shell browser.
 * 
 * This detaches the event listener we installed in order to listen for navigation
 * events.
 * 
 * Copied from Open-Shell implementation here:
 * https://github.com/Open-Shell/Open-Shell-Menu/blob/master/Src/ClassicExplorer/ExplorerBand.cpp#L2280-L2285
 */
STDMETHODIMP BrowserHelperObject::OnQuit()
{
	if (m_pWebBrowser && m_dwEventCookie != 0xFEFEFEFE)
	{
		return DispEventUnadvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
	}

	return S_OK;
}