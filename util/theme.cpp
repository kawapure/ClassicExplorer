/*
 * theme.cpp: Implementation of application theming functionality.
 */

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include <memory>
#include <string>
#include <tuple>
#include "wil/resource.h"
#include "simpleini/SimpleIni.h"

#include "theme.h"

#define DEFINE_WIDTH_HEIGHT_PROPERTIES(PROPERTY, WIDTH, HEIGHT)                          \
	{ PROPERTY, EThemePartProperty::Width, EMappedPropertyDataType::Integer, WIDTH },    \
	{ PROPERTY, EThemePartProperty::Height, EMappedPropertyDataType::Integer, HEIGHT }

#define GEN_INI_NAME_MAP(CLASS, NAME) case (int)CLASS::NAME: return L#NAME

#define USE_THEME_DEFINITION_SCOPE                                                 \
	constexpr EThemePartProperty Width = EThemePartProperty::Width;                \
	constexpr EThemePartProperty Height = EThemePartProperty::Height;              \
	constexpr EThemePartProperty Color = EThemePartProperty::Color;                \
	constexpr EMappedPropertyDataType Integer = EMappedPropertyDataType::Integer;  \
	constexpr EMappedPropertyDataType Float = EMappedPropertyDataType::Float;      \
	constexpr EMappedPropertyDataType String = EMappedPropertyDataType::String;

static const ThemePropertyMap kThemePropertyDefaults[] = {
	{ (int)EThemeColor::ThrobberBackground, EThemePartProperty::Color, EMappedPropertyDataType::Integer, (int)RGB(0, 0, 0) },
	DEFINE_WIDTH_HEIGHT_PROPERTIES((int)EThemeBitmap::GoActive, 20, 20),
	DEFINE_WIDTH_HEIGHT_PROPERTIES((int)EThemeBitmap::GoInactive, 20, 20),
	DEFINE_WIDTH_HEIGHT_PROPERTIES((int)EThemeBitmap::ThrobberLarge, 38, 38),
	DEFINE_WIDTH_HEIGHT_PROPERTIES((int)EThemeBitmap::ThrobberMedium, 26, 26),
	DEFINE_WIDTH_HEIGHT_PROPERTIES((int)EThemeBitmap::ThrobberSmall, 22, 22),
	DEFINE_WIDTH_HEIGHT_PROPERTIES((int)EThemeBitmap::WatermarkMusic, 150, 150),
};

static LPCWSTR GetIniPartName(int iPartId)
{
	switch (iPartId)
	{
		GEN_INI_NAME_MAP(EThemeColor, ThrobberBackground);
		GEN_INI_NAME_MAP(EThemeBitmap, GoActive);
		GEN_INI_NAME_MAP(EThemeBitmap, GoInactive);
		GEN_INI_NAME_MAP(EThemeBitmap, ThrobberLarge);
		GEN_INI_NAME_MAP(EThemeBitmap, ThrobberMedium);
		GEN_INI_NAME_MAP(EThemeBitmap, ThrobberSmall);
		GEN_INI_NAME_MAP(EThemeBitmap, WatermarkMusic);
		GEN_INI_NAME_MAP(EThemeBitmap, WatermarkSearch);
		GEN_INI_NAME_MAP(EThemeBitmap, WatermarkVideos);
		GEN_INI_NAME_MAP(EThemeBitmap, WatermarkPictures);
	}
	
	return nullptr;
}

static LPCWSTR GetIniPropertyName(EThemePartProperty eProperty)
{
	switch (eProperty)
	{
		case EThemePartProperty::Color:
			return L"Color";
		case EThemePartProperty::MaskColor:
			return L"MaskColor";
		case EThemePartProperty::Width:
			return L"Width";
		case EThemePartProperty::Height:
			return L"Height";
	}

	return nullptr;
}

std::tuple<HRESULT, std::unique_ptr<ThemePropertyMap> >
CThemeBase::FindProperty(int ePartName, EThemePartProperty eProperty)
{
	for (ThemePropertyMap &propMap : m_properties)
	{
		if (propMap.ePartName == ePartName && propMap.ePropertyName == eProperty)
		{
			return { S_OK, std::make_unique<ThemePropertyMap>(propMap) };
		}
	}

	return { E_FAIL, nullptr };
}

std::tuple<HRESULT, int> CThemeBase::GetIntegerProperty(int ePartName, EThemePartProperty eProperty)
{
	HRESULT hr = E_FAIL;
	std::unique_ptr<ThemePropertyMap> pPropertyMap = nullptr;

	std::tie(hr, pPropertyMap) = FindProperty(ePartName, eProperty);

	if (SUCCEEDED(hr) && pPropertyMap->eType == EMappedPropertyDataType::Integer)
	{
		hr = S_OK;
		return { hr, pPropertyMap->iData };
	}

	return { E_FAIL, 0 };
}

std::tuple<HRESULT, LPCWSTR> CThemeBase::GetStringProperty(int ePartName, EThemePartProperty eProperty)
{
	HRESULT hr = E_FAIL;
	std::unique_ptr<ThemePropertyMap> pPropertyMap = nullptr;

	std::tie(hr, pPropertyMap) = FindProperty(ePartName, eProperty);

	if (SUCCEEDED(hr) && pPropertyMap->eType == EMappedPropertyDataType::String)
	{
		if (pPropertyMap->pvData)
		{
			return { S_OK, (LPCWSTR)pPropertyMap->pvData };
		}
	}

	return { E_FAIL, nullptr };
}

HRESULT CThemeBase::SetProperty(int ePartName, EThemePartProperty eProperty, int iValue)
{
	HRESULT hr = E_FAIL;
	std::unique_ptr<ThemePropertyMap> pPropertyMap = nullptr;

	std::tie(hr, pPropertyMap) = FindProperty(ePartName, eProperty);

	if (SUCCEEDED(hr))
	{
		pPropertyMap->iData = iValue;
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CThemeBase::SetProperty(int ePartName, EThemePartProperty eProperty, LPCWSTR szValue)
{
	HRESULT hr = E_FAIL;
	std::unique_ptr<ThemePropertyMap> pPropertyMap = nullptr;

	std::tie(hr, pPropertyMap) = FindProperty(ePartName, eProperty);

	if (SUCCEEDED(hr))
	{
		pPropertyMap->iData = (wcslen(szValue) + 1) * 2;
		pPropertyMap->pvData = (void *)szValue;
	}

	return E_FAIL;
}

CNativeTheme::CNativeTheme()
{
}

COLORREF CNativeTheme::GetColor(EThemeColor colorName)
{
	switch (colorName)
	{
		case EThemeColor::ThrobberBackground:
		{
			// Windows XP default background.
			return RGB(0, 0, 0);
			break;
		}
	}

	return RGB(0, 0, 0);
}

wil::shared_hbitmap CNativeTheme::GetBitmap(EThemeBitmap bitmapName)
{
	LPCWSTR szBitmapName = FindBitmapResource(bitmapName);

	if (szBitmapName)
	{
		return wil::shared_hbitmap(LoadBitmapW(
			GetResourceInstance(),
			szBitmapName
		));
	}

	return nullptr;
}

/**
 * GetBitmapSize: Gets the dimensions of a theme bitmap.
 *
 * For the native theme, we just hardcode the size of bitmaps.
 */
SIZE CNativeTheme::GetBitmapSize(EThemeBitmap eBitmap)
{
	switch (eBitmap)
	{
		case EThemeBitmap::GoActive:
		case EThemeBitmap::GoInactive:
		{
			return { 20, 20 };
		}

		case EThemeBitmap::ThrobberLarge:
		{
			return { 38, 38 };
		}

		case EThemeBitmap::ThrobberMedium:
		{
			return { 26, 26 };
		}

		case EThemeBitmap::ThrobberSmall:
		{
			return { 22, 22 };
		}

		case EThemeBitmap::WatermarkMusic:
		case EThemeBitmap::WatermarkPictures:
		case EThemeBitmap::WatermarkSearch:
		case EThemeBitmap::WatermarkVideos:
		{
			return { 150, 150 };
		}
	}

	return { 0, 0 };
}

LPCWSTR CNativeTheme::FindBitmapResource(EThemeBitmap bitmapName)
{
	switch (bitmapName)
	{
		case EThemeBitmap::GoActive:
		{
			return MAKEINTRESOURCEW(IDB_XP_GO_ACTIVE);
		}

		case EThemeBitmap::GoInactive:
		{
			return MAKEINTRESOURCEW(IDB_XP_GO_INACTIVE);
		}

		case EThemeBitmap::ThrobberLarge:
		{
			return MAKEINTRESOURCEW(IDB_XP_THROBBER_SIZE_LARGE);
		}

		case EThemeBitmap::ThrobberMedium:
		{
			return MAKEINTRESOURCEW(IDB_XP_THROBBER_SIZE_MID);
		}

		case EThemeBitmap::ThrobberSmall:
		{
			return MAKEINTRESOURCEW(IDB_XP_THROBBER_SIZE_SMALL);
		}

		case EThemeBitmap::WatermarkMusic:
		{
			return MAKEINTRESOURCEW(IDB_BG_MUSIC_BLUE);
		}

		case EThemeBitmap::WatermarkSearch:
		{
			return MAKEINTRESOURCEW(IDB_BG_SEARCH_BLUE);
		}

		case EThemeBitmap::WatermarkVideos:
		{
			return MAKEINTRESOURCEW(IDB_BG_VIDEOS_BLUE);
		}

		case EThemeBitmap::WatermarkPictures:
		{
			return MAKEINTRESOURCEW(IDB_BG_PICTURES_BLUE);
		}
	}

	return nullptr;
}

CForeignTheme::CForeignTheme()
{
	USE_THEME_DEFINITION_SCOPE;

	m_properties.insert(
		m_properties.end(), 
		&kThemePropertyDefaults[0], 
		&kThemePropertyDefaults[ARRAYSIZE(kThemePropertyDefaults)]
	);
}

COLORREF CForeignTheme::GetColor(EThemeColor colorName)
{
	switch (colorName)
	{
		case EThemeColor::ThrobberBackground:
		{
			HRESULT hr = E_FAIL;
			int iValue = 0;
			std::tie(hr, iValue) = GetIntegerProperty((int)EThemeColor::ThrobberBackground, EThemePartProperty::Color);

			if (SUCCEEDED(hr))
			{
				return (COLORREF)iValue;
			}

			break;
		}
	}

	return RGB(0, 0, 0);
}

wil::shared_hbitmap CForeignTheme::GetBitmap(EThemeBitmap bitmapName)
{
	LPCWSTR szBitmapName = FindBitmapResource(bitmapName);

	if (szBitmapName)
	{
		return wil::shared_hbitmap(LoadBitmapW(
			GetResourceInstance(),
			szBitmapName
		));
	}

	return nullptr;
}

LPCWSTR CForeignTheme::FindBitmapResource(EThemeBitmap bitmapName)
{
	LPCWSTR szResourceName = nullptr;

	switch (bitmapName)
	{
		case EThemeBitmap::GoActive:
		{
			szResourceName = L"CETHEME_BMP_GO_ACTIVE";
			break;
		}

		case EThemeBitmap::GoInactive:
		{
			szResourceName = L"CETHEME_BMP_GO_INACTIVE";
			break;
		}

		case EThemeBitmap::ThrobberLarge:
		{
			szResourceName = L"CETHEME_BMP_THROBBER_SIZE_LARGE";
			break;
		}

		case EThemeBitmap::ThrobberMedium:
		{
			szResourceName = L"CETHEME_BMP_THROBBER_SIZE_MID";
			break;
		}

		case EThemeBitmap::ThrobberSmall:
		{
			szResourceName = L"CETHEME_BMP_THROBBER_SIZE_SMALL";
			break;
		}

		case EThemeBitmap::WatermarkMusic:
		{
			szResourceName = L"CETHEME_BMP_BG_MUSIC";
			break;
		}

		case EThemeBitmap::WatermarkSearch:
		{
			szResourceName = L"CETHEME_BMP_BG_SEARCH";
			break;
		}

		case EThemeBitmap::WatermarkVideos:
		{
			szResourceName = L"CETHEME_BMP_BG_VIDEOS";
			break;
		}

		case EThemeBitmap::WatermarkPictures:
		{
			szResourceName = L"CETHEME_BMP_BG_PICTURES";
			break;
		}
	}

	return szResourceName;
}

CThemeLoader::CThemeLoader()
{
	m_pTheme = std::make_unique<CForeignTheme>();
	m_pIniReader = std::make_unique<CSimpleIniW>();
}

HRESULT CThemeLoader::LoadForeignTheme(LPCWSTR szThemePath)
{
	m_hModule = LoadLibraryExW(szThemePath, nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);

	if (!m_hModule)
	{
		return E_FAIL;
	}

	HRSRC rcManifest = FindResourceW(m_hModule, L"CETHEME_MANIFEST", RT_RCDATA);

	if (!rcManifest)
	{
		return E_FAIL;
	}

	HGLOBAL hManifest = LoadResource(m_hModule, rcManifest);

	if (!hManifest)
	{
		return E_FAIL;
	}

	LPCWSTR szManifest = (LPCWSTR)LockResource(hManifest);

	if (FAILED(ParseManifest(szManifest)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CThemeLoader::ParseManifest(LPCWSTR szManifest)
{
	SI_Error rc = m_pIniReader->LoadData((LPCSTR)szManifest);

	if (FAILED(rc))
	{
		return E_FAIL;
	}

	DWORD dwVersion = m_pIniReader->GetLongValue(
		L"ClassicExplorerThemeManifest",
		L"version",
		0
	);

	if (dwVersion == 0)
	{
		// Invalid manifest.
		return E_FAIL;
	}

	bool fEnableWatermarks = m_pIniReader->GetBoolValue(
		L"Properties",
		L"EnableListViewWatermarks",
		false
	);

	m_pTheme->m_fExplorerWatermarksEnabled = fEnableWatermarks;

	// Iterate over EThemeColor values:
	for (int eColor = (int)EThemePartType::Color; eColor < (int)EThemeColor::End; eColor++)
	{
		InstallProperty(eColor, EThemePartProperty::Color);
	}

	// Iterate over EThemeBitmap values:
	for (int eBitmap = (int)EThemePartType::Bitmap; eBitmap < (int)EThemeBitmap::End; eBitmap++)
	{
		InstallProperty(eBitmap, EThemePartProperty::Width);
		InstallProperty(eBitmap, EThemePartProperty::Height);
	}

	return S_OK;
}

HRESULT CThemeLoader::InstallProperty(int ePart, EThemePartProperty eProperty)
{
	LPCWSTR szPartName = GetIniPartName(ePart);

	if (!szPartName)
	{
		return E_FAIL;
	}

	LPCWSTR szPropertyName = GetIniPropertyName(eProperty);

	if (!szPropertyName)
	{
		return E_FAIL;
	}

	std::wstring fullIniPath = std::wstring(szPartName).append(L".").append(szPropertyName);

	switch (eProperty)
	{
		case EThemePartProperty::Color:
		{
			return _InstallColorProperty(ePart, std::make_unique<std::wstring>(fullIniPath));
		}

		case EThemePartProperty::Width:
		{
			return _InstallIntegerProperty(
				ePart, 
				EThemePartProperty::Width, 
				std::make_unique<std::wstring>(fullIniPath)
			);
		}

		case EThemePartProperty::Height:
		{
			return _InstallIntegerProperty(
				ePart,
				EThemePartProperty::Height,
				std::make_unique<std::wstring>(fullIniPath)
			);
		}
	}

	return E_FAIL;
}

HRESULT CThemeLoader::_InstallColorProperty(int ePart, std::unique_ptr<std::wstring> pIniPath)
{
	LPCWSTR szIniColorValue = m_pIniReader->GetValue(
		L"Properties",
		pIniPath->c_str(),
		nullptr
	);

	if (!szIniColorValue)
	{
		return E_FAIL;
	}

	HRESULT hrColor;
	COLORREF crColor;
	std::tie(hrColor, crColor) = ParseManifestColor(szIniColorValue);

	if (SUCCEEDED(hrColor))
	{
		m_pTheme->SetProperty(ePart, EThemePartProperty::Color, crColor);
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CThemeLoader::_InstallIntegerProperty(int ePart, EThemePartProperty eProperty, std::unique_ptr<std::wstring> pIniPath)
{
	if (!m_pIniReader->KeyExists(L"Properties", pIniPath->c_str()))
	{
		return E_FAIL;
	}

	int iIniValue = m_pIniReader->GetLongValue(
		L"Properties",
		pIniPath->c_str(),
		0
	);

	m_pTheme->SetProperty(ePart, eProperty, iIniValue);

	return S_OK;
}

std::tuple<HRESULT, COLORREF> CThemeLoader::ParseManifestColor(LPCWSTR szColor)
{
	if (szColor[0] != L'#' || wcslen(szColor) != wcslen(L"#000000"))
	{
		// Invalid hex code; fail.
		return { E_FAIL, 0 };
	}

	DWORD dwColor = std::stol(&szColor[1], nullptr, 16);

	DWORD red   = dwColor & 0xFF0000 >> 16;
	DWORD green = dwColor & 0x00FF00 >> 8;
	DWORD blue  = dwColor & 0x0000FF;

	return { S_OK, RGB(red, green, blue) };
}