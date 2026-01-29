#pragma once

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

#include <vector>

class ATL_NO_VTABLE BrowserHelperObject :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<BrowserHelperObject, &CLSID_BrowserHelperObject>,
	public IDispEventImpl<1, BrowserHelperObject, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>,
	public IObjectWithSiteImpl<BrowserHelperObject>
{
	protected: // Class members:
		IInputObjectSite *m_pSite = nullptr;
		HWND m_parentWindow = nullptr;
		CComPtr<IWebBrowser2> m_pWebBrowser = nullptr;
		CComPtr<IShellBrowser> m_pShellBrowser = nullptr;
		HHOOK m_hHook = nullptr;

	public: // COM class setup:
		DECLARE_REGISTRY_RESOURCEID_V2_WITHOUT_MODULE(IDR_CLASSICEXPLORER, BrowserHelperObject)

		BEGIN_SINK_MAP(BrowserHelperObject)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
		END_SINK_MAP()

		BEGIN_COM_MAP(BrowserHelperObject)
			COM_INTERFACE_ENTRY(IObjectWithSite)
		END_COM_MAP()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct()
		{
			return S_OK;
		}

		void FinalRelease() {}

	protected: // Miscellaneous methods:
		void BrowserHelperObject::Cleanup();
		HRESULT BrowserHelperObject::UpdateWatermark();
		HRESULT BrowserHelperObject::_DoUpdateWatermark(HWND listView);
		static LRESULT CALLBACK BrowserHelperObject::s_HookProc(int nCode, WPARAM wParam, LPARAM lParam);

	public: // COM method implementations:

		// implement IObjectWithSite:
		STDMETHOD(SetSite)(IUnknown *pUnkSite);

		// handle DWebBrowserEvents2:
		STDMETHOD(OnNavigateComplete)(IDispatch *pDisp, VARIANT *url);
		STDMETHOD(OnQuit)(void);
};

OBJECT_ENTRY_AUTO(__uuidof(BrowserHelperObject), BrowserHelperObject);