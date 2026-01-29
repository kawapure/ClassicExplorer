/*
 * AddressBarHostBand.cpp: Implements the host rebar band for the address bar toolbar.
 * 
 * For the address bar implementation proper, see AddressBar.cpp.
 * 
 * As usual, the window itself is created in the SetSite method.
 * 
 * ================================================================================================
 * ---- IMPORTANT FUNCTIONS ----
 * 
 *  - SetSite: Installs the toolbar.
 *  - GetBandInfo: Provides important metadata about the toolbar.
 */

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include <commoncontrols.h>

#include "AddressBarHostBand.h"

std::wstring CAddressBarHostBand::m_addressText = L"";

//================================================================================================================
// implement IDeskBand:
//

WCHAR CAddressBarHostBand::GetAddressAccelerator()
{
	for (int i = 0; i < m_addressText.length(); i++)
	{
		if (m_addressText[i] == L'\0')
			break;

		if (m_addressText[i - 1] == L'&')
			return m_addressText[i];
	}

	return L'\0';
}
/*
 * CanRenderComposited: Indicates whether the Desk Band can render with DWM composition.
 */
STDMETHODIMP CAddressBarHostBand::CanRenderComposited(BOOL* pfCanRenderComposited)
{
	*pfCanRenderComposited = TRUE;
	return S_OK;
}

/*
 * SetCompositionState: Informs the Desk Band of the current DWM composition state.
 */
STDMETHODIMP CAddressBarHostBand::SetCompositionState(BOOL fCompositionEnabled)
{
	UNREFERENCED_PARAMETER(fCompositionEnabled);
	return S_OK;
}

/*
 * GetCompositionState: Queries the Desk Band for its DWM composition capabilities.
 */
STDMETHODIMP CAddressBarHostBand::GetCompositionState(BOOL* pfCompositionEnabled)
{
	*pfCompositionEnabled = TRUE;
	return S_OK;
}

/*
 * GetBandInfo: This is queried by the Shell and must return relevant information about
 *              the Desk Band.
 */
STDMETHODIMP CAddressBarHostBand::GetBandInfo(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pDbi)
{
	RECT rc;
	SendMessage(m_addressBar.GetToolbar(), TB_GETITEMRECT, 0, (LPARAM)&rc);
	int minSize = rc.right;

	if (pDbi)
	{
		if (pDbi->dwMask & DBIM_MINSIZE)
		{
			RECT rcComboBox;
			GetWindowRect(m_addressBar.m_comboBox, &rcComboBox);

			pDbi->ptMinSize.x = 150;
			pDbi->ptMinSize.y = rcComboBox.bottom - rcComboBox.top;
		}
		if (pDbi->dwMask & DBIM_MAXSIZE)
		{
			pDbi->ptMaxSize.x = 0; // 0 = ignored
			pDbi->ptMaxSize.y = -1; // -1 = unlimited
		}
		if (pDbi->dwMask & DBIM_INTEGRAL)
		{
			// Should not be sizeable.
			pDbi->ptIntegral.x = 0;
			pDbi->ptIntegral.y = 1;
		}
		if (pDbi->dwMask & DBIM_ACTUAL)
		{
			pDbi->ptActual.x = rc.right;
			pDbi->ptActual.y = rc.bottom;
		}
		if (pDbi->dwMask & DBIM_TITLE)
		{
			CEUtil::CESettings cS = CEUtil::GetCESettings();
			if (cS.showAddressLabel == 0) //Show no label
			{
				wcscpy_s(pDbi->wszTitle, L"");
				return S_OK;
			}
			wcscpy_s(pDbi->wszTitle, m_addressText.c_str());
			return S_OK;
		}
		if (pDbi->dwMask & DBIM_BKCOLOR)
		{
			// use default:
			pDbi->dwMask &= ~DBIM_BKCOLOR;
		}
	}

	return S_OK;
}

//================================================================================================================
// implement IOleWindow:
//
STDMETHODIMP CAddressBarHostBand::GetWindow(HWND *hWnd)
{
	if (!hWnd)
	{
		return E_INVALIDARG;
	}

	*hWnd = m_addressBar.GetToolbar();
	return S_OK;
}

STDMETHODIMP CAddressBarHostBand::ContextSensitiveHelp(BOOL fEnterMode)
{
	return S_OK;
}

//================================================================================================================
// implement IDockingWindow:
//
STDMETHODIMP CAddressBarHostBand::CloseDW(unsigned long dwReserved)
{
	return ShowDW(FALSE);
}

STDMETHODIMP CAddressBarHostBand::ResizeBorderDW(const RECT *pRcBorder, IUnknown *pUnkToolbarSite, BOOL fReserved)
{
	return E_NOTIMPL;
}

STDMETHODIMP CAddressBarHostBand::ShowDW(BOOL fShow)
{
	ShowWindow(m_addressBar.GetToolbar(), fShow ? SW_SHOW : SW_HIDE);
	return S_OK;
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
STDMETHODIMP CAddressBarHostBand::SetSite(IUnknown *pUnkSite)
{
	IObjectWithSiteImpl<CAddressBarHostBand>::SetSite(pUnkSite);

	if (m_addressBar.IsWindow())
	{
		m_addressBar.DestroyWindow();
	}

	// If pUnkSite is not nullptr, then the site is being set.
	// Otherwise, the site is being removed.
	if (pUnkSite) // hook:
	{
		if (FAILED(pUnkSite->QueryInterface(IID_IInputObjectSite, (void **)&m_pSite)))
		{
			return E_FAIL;
		}

		HWND hWndParent = nullptr;

		CComQIPtr<IOleWindow> pOleWindow = pUnkSite;
		if (pOleWindow)
			pOleWindow->GetWindow(&hWndParent);

		if (!IsWindow(hWndParent))
		{
			return E_FAIL;
		}

		m_parentWindow = GetAncestor(hWndParent, GA_ROOT);

		// Create the toolbar window proper:
		m_addressBar.Create(hWndParent, nullptr, nullptr, WS_CHILD);
		//m_addressBar.CreateBand(hWndParent);

		if (!m_addressBar.IsWindow())
		{
			return E_FAIL;
		}

		CComQIPtr<IServiceProvider> pProvider = pUnkSite;

		if (pProvider)
		{
			CComPtr<IShellBrowser> pShellBrowser;
			pProvider->QueryService(SID_SShellBrowser, IID_IShellBrowser, (void **)&pShellBrowser);
			pProvider->QueryService(SID_SWebBrowserApp, IID_IWebBrowser2, (void **)&m_pWebBrowser);

			if (m_pWebBrowser)
			{
				if (m_dwEventCookie == 0xFEFEFEFE)
				{
					DispEventAdvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
				}
			}

			m_addressBar.SetBrowsers(pShellBrowser, m_pWebBrowser);
			m_addressBar.InitComboBox();
		}
		if (m_addressText == L"")
		{
			HMODULE explorerframe = LoadLibrary(L"explorerframe.dll");
			if (explorerframe)
			{
				WCHAR* addressText = new WCHAR[32];
				LoadStringW(explorerframe, 12896, addressText, 32);
				m_addressText = addressText;
				delete[] addressText;
				FreeLibrary(explorerframe);
			}
			else
				m_addressText = L"Error";
		}
	}
	else // unhook:
	{
		m_pSite = nullptr;
		m_parentWindow = nullptr;
	}

	return S_OK;
}

//================================================================================================================
// handle DWebBrowserEvents2:
//

STDMETHODIMP CAddressBarHostBand::OnNavigateComplete(IDispatch *pDisp, VARIANT *url)
{
	m_addressBar.HandleNavigate();

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
STDMETHODIMP CAddressBarHostBand::OnQuit()
{
	if (m_pWebBrowser && m_dwEventCookie != 0xFEFEFEFE)
	{
		return DispEventUnadvise(m_pWebBrowser, &DIID_DWebBrowserEvents2);
	}

	return S_OK;
}

//================================================================================================================
// implement IInputObject:
//

STDMETHODIMP CAddressBarHostBand::HasFocusIO()
{
	if (
		GetFocus() == m_addressBar.m_comboBoxEditCtl || 
		::SendMessage(m_addressBar.m_comboBox, CB_GETDROPPEDSTATE, 0, 0)
	)
	{
		return S_OK;
	}
	else
	{
		return S_FALSE;
	}
}

STDMETHODIMP CAddressBarHostBand::TranslateAcceleratorIO(MSG *pMsg)
{
	switch (pMsg->message)
	{
		case WM_SYSCHAR:
		{
			WCHAR szInput[2] = L"\0";
			szInput[0] = (char)pMsg->wParam;

			WCHAR szAccelerator[2] = L"\0";
			szAccelerator[0] = GetAddressAccelerator();

			if (lstrcmpiW(szInput, szAccelerator) == 0)
			{
				//MessageBoxW(nullptr, L"FUCK", L"KILL YOURSELF", MB_OK);
				::SetFocus(m_addressBar.m_comboBoxEditCtl);
				return S_OK;
			}
		}
		break;
	}

	/*if (pMsg->hwnd == m_addressBar.m_comboBoxEditCtl)
	{
		switch (pMsg->message)
		{
			case WM_SYSKEYDOWN:
			case WM_SYSKEYUP:
			case WM_SYSCOMMAND:
			case WM_SYSDEADCHAR:
			case WM_SYSCHAR:
				return S_FALSE;
		}

		TranslateMessage(pMsg);
		DispatchMessageW(pMsg);

		return S_OK;
	}*/

	return S_FALSE;
}

STDMETHODIMP CAddressBarHostBand::UIActivateIO(BOOL fActivate, MSG *pMsg)
{
	if (fActivate)
	{
		m_pSite->OnFocusChangeIS((IDeskBand *)this, fActivate);
		::SetFocus(m_addressBar.m_comboBoxEditCtl);
	}

	return S_OK;
}

//================================================================================================================
// implement IInputObjectSite:
//

STDMETHODIMP CAddressBarHostBand::OnFocusChangeIS(IUnknown *pUnkObj, BOOL fSetFocus)
{
	return E_NOTIMPL;
}