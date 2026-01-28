/*
 * dllmain.cpp: Responsible for DLL exports and COM/ATL initialization.
 */

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

CAddressBarModule g_AtlModule;

// Used to determine whether the DLL can be unloaded by OLE.
_Use_decl_annotations_
STDAPI DllCanUnloadNow(void)
{
	return g_AtlModule.DllCanUnloadNow();
}

// Returns a class factory to create an object of the requested type.
_Use_decl_annotations_
STDAPI DllGetClassObject(_In_ REFCLSID rclsid, _In_ REFIID riid, _Outptr_ LPVOID* ppv)
{
	return g_AtlModule.DllGetClassObject(rclsid, riid, ppv);
}

DEFINE_GUID(CATID_AppContainerCompatible, 0x59FB2056, 0xD625, 0x48D0, 0xA9, 0x44, 0x1A, 0x85, 0xB5, 0xAB, 0x26, 0x40);

// DllRegisterServer - Adds entries to the system registry.
_Use_decl_annotations_
STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = g_AtlModule.DllRegisterServer();

	// Copied from Open-Shell source code:
	// https://github.com/Open-Shell/Open-Shell-Menu/blob/master/Src/ClassicExplorer/ClassicExplorer.cpp#L36-L49
	// This is responsible for the category registration for browser bands, which is required
	// in order to have it display in Explorer.
	if (SUCCEEDED(hr))
	{
		CComPtr<ICatRegister> catRegister;
		catRegister.CoCreateInstance(CLSID_StdComponentCategoriesMgr);

		if (catRegister)
		{
			static CATID rgCatIDs[] = { CATID_AppContainerCompatible, CATID_DeskBand };
			catRegister->RegisterClassImplCategories(CLSID_CAddressBarHostBand, ARRAYSIZE(rgCatIDs), rgCatIDs);
		}
	}

	return hr;
}

// DllUnregisterServer - Removes entries from the system registry.
_Use_decl_annotations_
STDAPI DllUnregisterServer(void)
{
	HRESULT hr = g_AtlModule.DllUnregisterServer();
	return hr;
}

// DllInstall - Adds/Removes entries to the system registry per user per machine.
STDAPI DllInstall(BOOL bInstall, _In_opt_  LPCWSTR pszCmdLine)
{
	HRESULT hr = E_FAIL;
	static const wchar_t szUserSwitch[] = L"user";

	if (pszCmdLine != nullptr)
	{
		if (_wcsnicmp(pszCmdLine, szUserSwitch, _countof(szUserSwitch)) == 0)
		{
			ATL::AtlSetPerUserRegistration(true);
		}
	}

	if (bInstall)
	{
		hr = DllRegisterServer();
		if (FAILED(hr))
		{
			DllUnregisterServer();
		}
	}
	else
	{
		hr = DllUnregisterServer();
	}

	return hr;
}

// DLL entry point, managed by ATL.
extern "C" BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	return g_AtlModule.DllMain(dwReason, lpReserved);
}