#pragma once
#ifndef _UTIL_H
#define _UTIL_H

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
//#include "ClassicExplorer_i.h"
#include "dllmain.h"

namespace CEUtil
{
	HRESULT GetCurrentFolderPidl(CComPtr<IShellBrowser> pShellBrowser, PIDLIST_ABSOLUTE *pidlOut);
	HRESULT FixExplorerSizes(HWND explorerChild);
	HRESULT FixExplorerSizesIfNecessary(HWND explorerChild);
}

#endif