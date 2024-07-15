#include "stdafx.h"

DECLARE_INTERFACE_IID_(IFolderType, IUnknown, "053B4A86-0DC9-40A3-B7ED-BC6A2E951F48")
{
	public:
		virtual HRESULT STDMETHODCALLTYPE GetFolderType(FOLDERTYPEID * pTypeId) = 0;
};

static GUID IID_IFolderType = __uuidof(IFolderType);