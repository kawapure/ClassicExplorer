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

class CSettingsManager
{
private:
	bool m_fShowGoButton = false;
	bool m_fShowAddressLabel = false;
	bool m_fShowFullAddress = false;
	ITheme *m_pAppTheme;

	static CSettingsManager *s_pInstance;

public:
	static HRESULT CreateInstance()
	{
		s_pInstance = new CSettingsManager();
		s_pInstance->Initialize();
		return S_OK;
	}

	HRESULT Initialize();

	static inline CSettingsManager *GetInstance()
	{
		return s_pInstance;
	}

	inline bool ShouldShowGoButton()
	{
		return m_fShowGoButton;
	}

	inline bool ShouldShowAddressLabel()
	{
		return m_fShowAddressLabel;
	}

	inline bool ShouldShowFullAddress()
	{
		return m_fShowFullAddress;
	}

	inline ITheme *GetAppTheme()
	{
		return m_pAppTheme;
	}
};

ITheme *GetAppTheme();

CESettings GetCESettings();
void WriteCESettings(CESettings& toWrite);

}