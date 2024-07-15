#pragma once

#ifndef _ADDRESSBARHOSTBAND_H
#define _ADDRESSBARHOSTBAND_H

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include "AddressBar.h"

class ATL_NO_VTABLE AddressBarHostBand :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<AddressBarHostBand, &CLSID_AddressBarHostBand>,
	public IObjectWithSiteImpl<AddressBarHostBand>,
	public IDeskBand,
	public IDispEventImpl<1, AddressBarHostBand, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>,
	public IInputObject,
	public IInputObjectSite
{
	protected: // Class members:
		IInputObjectSite *m_pSite = NULL;
		HWND m_parentWindow = NULL;
		AddressBar m_addressBar;
		CComPtr<IWebBrowser2> m_pWebBrowser = NULL;
		static std::wstring m_addressText;

		friend class AddressBar;

	protected: // Internal methods:
		WCHAR GetAddressAccelerator();

	public: // COM class setup:
		DECLARE_REGISTRY_RESOURCEID_V2_WITHOUT_MODULE(IDR_CLASSICEXPLORER, AddressBarHostBand)

		BEGIN_SINK_MAP(AddressBarHostBand)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
		END_SINK_MAP()

		BEGIN_COM_MAP(AddressBarHostBand)
			COM_INTERFACE_ENTRY(IOleWindow)
			COM_INTERFACE_ENTRY(IObjectWithSite)
			COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
			COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
			COM_INTERFACE_ENTRY_IID(IID_IInputObject, IInputObject)
		END_COM_MAP()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct()
		{
			return S_OK;
		}

		void FinalRelease() {}

	public: // COM method implementations:
		
		// implement IDeskBand:
		STDMETHOD(GetBandInfo)(DWORD dwBandId, DWORD dwViewMode, DESKBANDINFO *pDbi);

		// implement IObjectWithSite:
		STDMETHOD(SetSite)(IUnknown *pUnkSite);

		// implement IOleWindow:
		STDMETHOD(GetWindow)(HWND *hWnd);
		STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

		// implement IDockingWindow:
		STDMETHOD(CloseDW)(unsigned long dwReserved);
		STDMETHOD(ResizeBorderDW)(const RECT *pRcBorder, IUnknown *pUnkToolbarSite, BOOL fReserved);
		STDMETHOD(ShowDW)(BOOL fShow);

		// handle DWebBrowserEvents2:
		STDMETHOD(OnNavigateComplete)(IDispatch *pDisp, VARIANT *url);
		STDMETHOD(OnQuit)(void);

		// implement IInputObject:
		STDMETHOD(HasFocusIO)(void);
		STDMETHOD(TranslateAcceleratorIO)(MSG *pMsg);
		STDMETHOD(UIActivateIO)(BOOL fActivate, MSG *pMsg);

		// implement IInputObjectSite:
		STDMETHOD(OnFocusChangeIS)(IUnknown *pUnkObj, BOOL fSetFocus);
};

OBJECT_ENTRY_AUTO(__uuidof(AddressBarHostBand), AddressBarHostBand);

#endif // _ADDRESSBARHOSTBAND_H