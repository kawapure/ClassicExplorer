/*
 * settings_manager.cpp: Implementation of the settings manager.
 */

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include "wil/resource.h"
#include "wil/win32_helpers.h"
#include "wil/registry.h"

#include "theme.h"

#include "settings_manager.h"

namespace CEUtil
{

#define CE_REGISTRY_PATH L"SOFTWARE\\kawapure\\ClassicExplorer"

HRESULT CSettingsManager::Initialize()
{
	m_pAppTheme = new CNativeTheme();

	HKEY hKey;
	LSTATUS ls = RegOpenKeyExW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, 0, KEY_READ, &hKey);

	if (ls != ERROR_SUCCESS)
	{
		// Key doesn't exist; make it.
		ls = RegCreateKeyW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, &hKey);

		if (ls != ERROR_SUCCESS)
		{
			// If we failed to create the key, then just fail here.
			return E_FAIL;
		}
	}

	size_t curBufferSize = MAX_PATH;

	std::unique_ptr<WCHAR> szTextBuffer = std::make_unique<WCHAR>(curBufferSize);
	DWORD dwTextSize = 0;

	ls = RegGetValueW(hKey, NULL, L"Theme", RRF_RT_REG_SZ, NULL, nullptr, &dwTextSize);

	if (ls != ERROR_SUCCESS)
	{
		return E_FAIL;
	}

	if (dwTextSize > curBufferSize)
	{
		curBufferSize += dwTextSize * 2;
		szTextBuffer.reset();
		szTextBuffer = std::make_unique<WCHAR>(curBufferSize);
	}

	ls = RegGetValueW(hKey, NULL, L"Theme", RRF_RT_REG_SZ, NULL, szTextBuffer.get(), &dwTextSize);

	DWORD dwCurBoolProp;
	DWORD dwValueSize = sizeof(DWORD);

	RegGetValueW(hKey, NULL, L"ShowGoButton", RRF_RT_REG_DWORD, NULL, &dwCurBoolProp, &dwValueSize);
	m_fShowGoButton = (bool)dwCurBoolProp;

	RegGetValueW(hKey, NULL, L"ShowAddressLabel", RRF_RT_REG_DWORD, NULL, &dwCurBoolProp, &dwValueSize);
	m_fShowAddressLabel = (bool)dwCurBoolProp;

	RegGetValueW(hKey, NULL, L"ShowFullAddress", RRF_RT_REG_DWORD, NULL, &dwCurBoolProp, &dwValueSize);
	m_fShowFullAddress = (bool)dwCurBoolProp;

	RegCloseKey(hKey);

	return S_OK;
}

ITheme *GetAppTheme()
{
	return CSettingsManager::GetInstance()->GetAppTheme();
}

}