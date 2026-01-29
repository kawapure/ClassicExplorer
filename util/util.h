#pragma once
#ifndef _UTIL_H
#define _UTIL_H

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
//#include "ClassicExplorer_i.h"
#include "dllmain.h"

enum ClassicExplorerTheme
{
	CLASSIC_EXPLORER_NONE = -1,
	CLASSIC_EXPLORER_2K = 0,
	CLASSIC_EXPLORER_XP = 1,
};

namespace CEUtil
{
	struct CESettings
	{
		ClassicExplorerTheme theme = CLASSIC_EXPLORER_NONE;
		DWORD showGoButton = -1;
		DWORD showAddressLabel = -1;
		DWORD showFullAddress = -1;

		CESettings(ClassicExplorerTheme t, int a, int b, int f)
		{
			theme = t;
			showGoButton = a;
			showAddressLabel = b;
			showFullAddress = f;
		}
	};
	CESettings GetCESettings();
	void WriteCESettings(CESettings& toWrite);
	HRESULT GetCurrentFolderPidl(CComPtr<IShellBrowser> pShellBrowser, PIDLIST_ABSOLUTE *pidlOut);
	HRESULT FixExplorerSizes(HWND explorerChild);
	HRESULT FixExplorerSizesIfNecessary(HWND explorerChild);
}

#endif