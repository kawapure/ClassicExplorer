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

//CESettings GetCESettings()
//{
//	bool fShowGoButton = true;
//	bool fShowAddressLabel = true;
//	bool fShowFullAddress = true;
//
//	ClassicExplorerTheme theme = CLASSIC_EXPLORER_2K;
//	HKEY hKey;
//	LSTATUS ls = RegOpenKeyExW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, 0, KEY_READ, &hKey);
//	if (ls != ERROR_SUCCESS)
//	{
//		// Key doesn't exist, make it
//		ls = RegCreateKeyW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, &hKey);
//		if (ls != ERROR_SUCCESS) // sum ting wong
//			return CESettings(CLASSIC_EXPLORER_NONE, -1, -1, -1);
//
//		// Open the new key with write access
//		RegOpenKeyExW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, 0, KEY_WRITE, &hKey);
//
//		// Write default values and return default settings
//		WCHAR themeDef[] = L"2K";
//		RegSetValueExW(hKey, L"Theme", 0, REG_SZ, (BYTE*)themeDef, 4);
//		RegSetValueExW(hKey, L"ShowGoButton", 0, REG_DWORD, (BYTE*)&fShowGoButton, 4);
//		RegSetValueExW(hKey, L"ShowAddressLabel", 0, REG_DWORD, (BYTE*)&fShowAddressLabel, 4);
//		RegSetValueExW(hKey, L"ShowFullAddress", 0, REG_DWORD, (BYTE*)&fShowFullAddress, 4);
//		return CESettings(CLASSIC_EXPLORER_2K, 1, 1, 1);
//	}
//	// Read settings
//	//WCHAR themeRead[8];
//	WCHAR themeRead[9];
//	//DWORD size = 8;
//	DWORD size = sizeof(themeRead);
//
//	RegGetValueW(hKey, NULL, L"Theme", RRF_RT_REG_SZ, NULL, themeRead, &size);
//	if (wcscmp(themeRead, L"XP") == 0)
//	{
//		theme = CLASSIC_EXPLORER_XP;
//	}
//	else if (wcscmp(themeRead, L"98") == 0)
//	{
//		theme = CLASSIC_EXPLORER_MEMPHIS;
//	}
//	else if (wcscmp(themeRead, L"10") == 0)
//	{
//		theme = CLASSIC_EXPLORER_10;
//	}
//
//	DWORD dwValueSize = sizeof(DWORD);
//	RegGetValueW(hKey, NULL, L"ShowGoButton", RRF_RT_REG_DWORD, NULL, &fShowGoButton, &dwValueSize);
//	RegGetValueW(hKey, NULL, L"ShowAddressLabel", RRF_RT_REG_DWORD, NULL, &fShowAddressLabel, &dwValueSize);
//	RegGetValueW(hKey, NULL, L"ShowFullAddress", RRF_RT_REG_DWORD, NULL, &fShowFullAddress, &dwValueSize);
//
//	RegCloseKey(hKey);
//
//	return CESettings(theme, fShowGoButton, fShowAddressLabel, fShowFullAddress);
//}
//
//void WriteCESettings(CESettings& toWrite)
//{
//	HKEY hKey;
//	LSTATUS ls = RegOpenKeyExW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, 0, KEY_WRITE, &hKey);
//	if (ls != ERROR_SUCCESS)
//	{
//		// Key doesn't exist, make it
//		ls = RegCreateKeyW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, &hKey);
//		if (ls != ERROR_SUCCESS)
//			return;
//
//		// Open the new key with write access
//		RegOpenKeyExW(HKEY_CURRENT_USER, CE_REGISTRY_PATH, 0, KEY_WRITE, &hKey);
//	}
//	if (toWrite.theme != CLASSIC_EXPLORER_NONE)
//	{
//		WCHAR theme[] = L"2K";
//		if (toWrite.theme == CLASSIC_EXPLORER_XP)
//			wcscpy_s(theme, L"XP");
//		if (toWrite.theme == CLASSIC_EXPLORER_MEMPHIS)
//			wcscpy_s(theme, L"98");
//		if (toWrite.theme == CLASSIC_EXPLORER_10)
//			wcscpy_s(theme, L"10");
//		RegSetValueExW(hKey, L"Theme", 0, REG_SZ, (BYTE*)theme, 4);
//	}
//	if (toWrite.showGoButton != -1)
//	{
//		RegSetValueExW(hKey, L"ShowGoButton", 0, REG_DWORD, (BYTE*)&toWrite.showGoButton, 4);
//	}
//	if (toWrite.showAddressLabel != -1)
//	{
//		RegSetValueExW(hKey, L"ShowAddressLabel", 0, REG_DWORD, (BYTE*)&toWrite.showAddressLabel, 4);
//	}
//	if (toWrite.showFullAddress != -1)
//	{
//		RegSetValueExW(hKey, L"ShowFullAddress", 0, REG_DWORD, (BYTE*)&toWrite.showFullAddress, 4);
//	}
//	RegCloseKey(hKey);
//}

ITheme *GetAppTheme()
{
	return CSettingsManager::GetInstance()->GetAppTheme();
}

}