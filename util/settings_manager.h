#pragma once

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "dllmain.h"
#include <memory>

#include "theme.h"

namespace CEUtil
{

enum ClassicExplorerTheme
{
	CLASSIC_EXPLORER_NONE = -1,
	CLASSIC_EXPLORER_2K = 0,
	CLASSIC_EXPLORER_XP = 1,
	CLASSIC_EXPLORER_10 = 2,
	CLASSIC_EXPLORER_MEMPHIS = 3
};

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

class CSettingsManager;

extern CSettingsManager *g_pSettingsManagerInstance;

class CSettingsManager
{
private:
	bool _fShowGoButton = false;
	bool _fShowAddressLabel = false;
	bool _fShowFullAddress = false;
	ITheme *_pAppTheme;

public:
	static HRESULT CreateInstance()
	{
		g_pSettingsManagerInstance = new CSettingsManager();
		g_pSettingsManagerInstance->Initialize();
		return S_OK;
	}

	HRESULT Initialize();

	static inline CSettingsManager *GetInstance()
	{
		return g_pSettingsManagerInstance;
	}

	inline bool ShouldShowGoButton()
	{
		return _fShowGoButton;
	}

	inline bool ShouldShowAddressLabel()
	{
		return _fShowAddressLabel;
	}

	inline bool ShouldShowFullAddress()
	{
		return _fShowFullAddress;
	}

	inline ITheme *GetAppTheme()
	{
		return _pAppTheme;
	}
};

ITheme *GetAppTheme();

}