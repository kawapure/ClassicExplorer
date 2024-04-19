// dllmain.h : Declaration of module class.

#pragma once


class CAddressBarModule : public ATL::CAtlDllModuleT<CAddressBarModule>
{
public :
	DECLARE_LIBID(LIBID_AddressBarLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_ADDRESSBAR, "{205f9779-62d5-4e06-8dad-8e150ffa38db}")
};

extern class CAddressBarModule g_AtlModule;
