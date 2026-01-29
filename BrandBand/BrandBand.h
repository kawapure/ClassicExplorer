#pragma once

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

class ATL_NO_VTABLE CBrandBand :
	public CWindowImpl<CBrandBand, CWindow, CControlWinTraits>,
	public CComObjectRootEx<CComMultiThreadModelNoCS>,
	public CComCoClass<CBrandBand, &CLSID_CBrandBand>,
	public IObjectWithSiteImpl<CBrandBand>,
	public IDeskBand,
	public IDispEventImpl<1, CBrandBand, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>
{
	private: // Class members:
		CComPtr<IWebBrowser2> m_pWebBrowser = nullptr;
		HWND m_parentRebar = nullptr;
		HBITMAP m_hBitmap = nullptr;
		bool m_subclassedRebar = false;
		bool m_alreadyDeletedSelf = false;
		bool m_shouldManuallyCorrectHeight = false;
		
		ClassicExplorerTheme m_theme = CLASSIC_EXPLORER_2K;

		// Width of the current bitmap.
		int m_cxCurBmp = 0;

		// Height of the current bitmap.
		int m_cyCurBmp = 0;

		int m_redrawCounter = 0;
		unsigned int m_latestRedrawTime = 0;
		bool m_disableRedraws = false;

	public: // COM class setup:
		DECLARE_WND_CLASS(L"ClassicExplorer.BrandBand")

		~CBrandBand(void)
		{
			ClearResources();
		}

		DECLARE_REGISTRY_RESOURCEID_V2_WITHOUT_MODULE(IDR_CLASSICEXPLORER, CBrandBand)

		BEGIN_MSG_MAP(CBrandBand)
			MESSAGE_HANDLER(WM_PAINT, OnPaint)
			MESSAGE_HANDLER(WM_SIZE, OnSize)
			MESSAGE_HANDLER(WM_ERASEBKGND, OnEraseBackground)
			MESSAGE_HANDLER(WM_LBUTTONUP, OnClick)
			//MESSAGE_HANDLER(WM_COMMAND, OnCommand)
		END_MSG_MAP()

		BEGIN_COM_MAP(CBrandBand)
			COM_INTERFACE_ENTRY(IOleWindow)
			COM_INTERFACE_ENTRY(IObjectWithSite)
			COM_INTERFACE_ENTRY_IID(IID_IDockingWindow, IDockingWindow)
			COM_INTERFACE_ENTRY_IID(IID_IDeskBand, IDeskBand)
		END_COM_MAP()

		BEGIN_SINK_MAP(CBrandBand)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATECOMPLETE2, OnNavigateComplete)
			SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit)
		END_SINK_MAP()

		DECLARE_PROTECT_FINAL_CONSTRUCT()

		HRESULT FinalConstruct()
		{
			return S_OK;
		}

		void FinalRelease() {}

	protected: // Message handlers:
		LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnEraseBackground(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled);
		LRESULT OnClick(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected: // Miscellaneous functions:
		void ClearResources();

		static LRESULT CALLBACK RebarParentSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
		static LRESULT CALLBACK RebarSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

		void PerformRedrawCheck();
		LRESULT CorrectBandSize();
		bool ShouldRefreshVisual();

		LRESULT LoadBitmapForSize();

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
};

OBJECT_ENTRY_AUTO(__uuidof(CBrandBand), CBrandBand);