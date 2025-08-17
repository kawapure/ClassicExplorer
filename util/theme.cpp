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

#define IS_VALID_PROPERTY_ENUM(e, ENUM, BASE) ((int)e >= (int)ENUM::BASE && (int)e < (int)ENUM::End)
#define IS_VALID_BITMAP(e) IS_VALID_PROPERTY_ENUM(e, EThemeBitmap, GoActive)

struct ThemeBitmapInfo
{
	EThemeBitmap eVal;
	LPCWSTR szValName;
	LPCWSTR szDefaultResourceName;
};

#define DEFINE_THEME_BITMAP(eBitmap, szDefaultResourceName) \
	{ EThemeBitmap:: eBitmap, L#eBitmap, L##szDefaultResourceName },

static const ThemeBitmapInfo c_rgThemeBitmapInfo[(int)EThemeBitmap::End - (int)EThemePartType::Bitmap] = {
	DEFINE_THEME_BITMAP(GoActive,          "CETHEME_BMP_GO_ACTIVE")
	DEFINE_THEME_BITMAP(GoInactive,        "CETHEME_BMP_GO_INACTIVE")
	DEFINE_THEME_BITMAP(ThrobberSmall,     "CETHEME_BMP_THROBBER_SIZE_SMALL")
	DEFINE_THEME_BITMAP(ThrobberMedium,    "CETHEME_BMP_THROBBER_SIZE_MID")
	DEFINE_THEME_BITMAP(ThrobberLarge,     "CETHEME_BMP_THROBBER_SIZE_LARGE")
	DEFINE_THEME_BITMAP(WatermarkMusic,    "CETHEME_BMP_BG_MUSIC")
	DEFINE_THEME_BITMAP(WatermarkSearch,   "CETHEME_BMP_BG_SEARCH")
	DEFINE_THEME_BITMAP(WatermarkVideos,   "CETHEME_BMP_BG_VIDEOS")
	DEFINE_THEME_BITMAP(WatermarkPictures, "CETHEME_BMP_BG_PICTURES")
};

static const ThemePropertyMap c_rgThemePropertyDefaults[] = {
	{ (int)EThemeColor::ThrobberBackground, EThemePartProperty::Color, EMappedPropertyDataType::Integer, (int)RGB(0, 0, 0) },
	{ (int)EThemeBitmap::GoActive, EThemePartProperty::MaskColor, EMappedPropertyDataType::Integer, (int)RGB(0, 0, 0) },
	{ (int)EThemeBitmap::GoInactive, EThemePartProperty::MaskColor, EMappedPropertyDataType::Integer, (int)RGB(0, 0, 0) },
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
	for (ThemePropertyMap &propMap : _properties)
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

bool CNativeTheme::GetBool(EThemeBool eBool)
{
	switch (eBool)
	{
		case EThemeBool::EnableListViewWatermarks:
		{
			// Windows XP default:
			return true;
		}
	}

	return false;
}

COLORREF CNativeTheme::GetColor(EThemeColor colorName)
{
	switch (colorName)
	{
		case EThemeColor::ThrobberBackground:
		{
			// Windows XP default background.
			return RGB(255, 255, 255);
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

/**
 * GetBitmapMaskColor: Gets the mask colour of a theme bitmap.
 *
 * For the native theme, we just hardcode the size of bitmaps.
 */
COLORREF CNativeTheme::GetBitmapMaskColor(EThemeBitmap eBitmap)
{
	switch (eBitmap)
	{
		case EThemeBitmap::GoActive:
		case EThemeBitmap::GoInactive:
		{
			return RGB(0, 0, 0);
		}
	}

	return RGB(0, 0, 0);
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

HIMAGELIST CNativeTheme::LoadImageListFromBitmap(EThemeBitmap eBitmap)
{
	return ImageList_LoadImageW(
		GetResourceInstance(),
		FindBitmapResource(eBitmap),
		GetBitmapSize(eBitmap).cx,
		0,
		GetBitmapMaskColor(eBitmap),
		IMAGE_BITMAP,
		LR_CREATEDIBSECTION
	);
}

CForeignThemeBase::CForeignThemeBase()
{
	USE_THEME_DEFINITION_SCOPE;

	_properties.insert(
		_properties.end(),
		&c_rgThemePropertyDefaults[0],
		&c_rgThemePropertyDefaults[ARRAYSIZE(c_rgThemePropertyDefaults)]
	);
}

bool CForeignThemeBase::GetBool(EThemeBool eBool)
{
	switch (eBool)
	{
		case EThemeBool::EnableListViewWatermarks:
		{
			return _fExplorerWatermarksEnabled;
		}
	}

	return false;
}

COLORREF CForeignThemeBase::GetColor(EThemeColor colorName)
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

SIZE CForeignThemeBase::GetBitmapSize(EThemeBitmap eBitmap)
{
	HRESULT hr = E_FAIL;
	int iWidth = 0;
	std::tie(hr, iWidth) = GetIntegerProperty((int)eBitmap, EThemePartProperty::Width);

	HRESULT hr2 = E_FAIL;
	int iHeight = 0;
	std::tie(hr2, iHeight) = GetIntegerProperty((int)eBitmap, EThemePartProperty::Height);

	if (SUCCEEDED(hr) || SUCCEEDED(hr2))
	{
		return { iWidth, iHeight };
	}

	return { 0, 0 };
}

COLORREF CForeignThemeBase::GetBitmapMaskColor(EThemeBitmap eBitmap)
{
	HRESULT hr = E_FAIL;
	int iValue = 0;
	std::tie(hr, iValue) = GetIntegerProperty((int)eBitmap, EThemePartProperty::MaskColor);

	if (SUCCEEDED(hr))
	{
		return (COLORREF)iValue;
	}

	return RGB(0, 0, 0);
}

wil::shared_hbitmap CForeignThemeDll::GetBitmap(EThemeBitmap bitmapName)
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

LPCWSTR CForeignThemeDll::FindBitmapResource(EThemeBitmap bitmapName)
{
	LPCWSTR szResourceName = nullptr;

	// Default fallback names:
	for (const ThemeBitmapInfo &rbi : c_rgThemeBitmapInfo) if (rbi.eVal == bitmapName)
	{
		szResourceName = rbi.szDefaultResourceName;
		break;
	}

	// If the INI part has a mapped name from the INI file, then we'll use
	// that instead.
	auto pUserDefinedFile = _mapFiles.find(GetIniPartName((int)bitmapName));
	if (pUserDefinedFile != _mapFiles.end())
	{
		szResourceName = pUserDefinedFile->second.c_str();
	}

	return szResourceName;
}

HIMAGELIST CForeignThemeDll::LoadImageListFromBitmap(EThemeBitmap eBitmap)
{
	// Same as CNativeTheme implementation.
	return ImageList_LoadImageW(
		GetResourceInstance(),
		FindBitmapResource(eBitmap),
		GetBitmapSize(eBitmap).cx,
		0,
		GetBitmapMaskColor(eBitmap),
		IMAGE_BITMAP,
		LR_CREATEDIBSECTION
	);
}

LPCWSTR CForeignThemeFolder::FindBitmapResource(EThemeBitmap bitmapName)
{
	if (IS_VALID_BITMAP(bitmapName))
	{
		return _rgszBitmapPaths[(int)bitmapName - 1];
	}

	return nullptr;
}

wil::shared_hbitmap CForeignThemeFolder::GetBitmap(EThemeBitmap bitmapName)
{
	if (IS_VALID_BITMAP(bitmapName))
	{
		return _rgshbmBitmapParts[(int)bitmapName - 1];
	}

	return nullptr;
}

HIMAGELIST CForeignThemeFolder::LoadImageListFromBitmap(EThemeBitmap eBitmap)
{
	return ImageList_LoadImageW(
		NULL,
		FindBitmapResource(eBitmap),
		GetBitmapSize(eBitmap).cx,
		0,
		GetBitmapMaskColor(eBitmap),
		IMAGE_BITMAP,
		LR_CREATEDIBSECTION | LR_LOADFROMFILE
	);
}

CThemeLoader::CThemeLoader()
{
	_spIniReader = std::make_unique<CSimpleIniW>();
}

HRESULT CThemeLoader::LoadForeignTheme(LPCWSTR szThemePath)
{
	LPCWSTR szExt = PathFindExtensionW(szThemePath);
	
	if (wcscmp(szExt, L".dll") == 0)
	{
		return LoadForeignThemeFromDll(szThemePath);
	}

	return LoadForeignThemeFromFolder(szThemePath);
}

HRESULT CThemeLoader::LoadForeignThemeFromFolder(LPCWSTR szThemeManifestPath)
{
	_szThemeModulePath = szThemeManifestPath;
	_spTheme = std::make_unique<CForeignThemeFolder>();

	wil::unique_hfile shFile = wil::unique_hfile(CreateFileW(
		szThemeManifestPath,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL
	));

	RETURN_HR_IF_MSG(E_ACCESSDENIED, shFile.get() == INVALID_HANDLE_VALUE, "Failed to open foreign theme manifest file.");

	LARGE_INTEGER liFileSize;
	if (!GetFileSizeEx(shFile.get(), &liFileSize))
	{
		RETURN_HR_MSG(E_FAIL, "Failed to get manifest file size.");
	}

	// Ensure 2 byte null termination in case we directly get a UTF-16 string.
	std::unique_ptr<BYTE[]> spszManifestRaw = std::make_unique<BYTE[]>(liFileSize.QuadPart + 2);

	if (!spszManifestRaw)
	{
		RETURN_HR(E_OUTOFMEMORY);
	}

	ZeroMemory(spszManifestRaw.get(), liFileSize.QuadPart + 2);

	if (!ReadFile(shFile.get(), spszManifestRaw.get(), liFileSize.QuadPart, NULL, NULL))
	{
		RETURN_HR_MSG(E_FAIL, "Failed to read manifest file.");
	}

	//// We now need to convert the manifest file to an encoding the operating system can work with
	//// (in our case, UTF-16).
	//std::unique_ptr<WCHAR[]> spwszManifest = nullptr;
	//LPCWSTR pwszManifest = nullptr;
	//if (!IsTextUnicode(spszManifestRaw.get(), liFileSize.QuadPart, nullptr))
	//{
	//	int cch = MultiByteToWideChar(CP_UTF8, 0, (LPCCH)spszManifestRaw.get(), -1, nullptr, 0);

	//	spwszManifest = std::make_unique<WCHAR[]>(cch + 1);

	//	if (!spwszManifest)
	//	{
	//		RETURN_HR(E_OUTOFMEMORY);
	//	}

	//	MultiByteToWideChar(CP_UTF8, 0, (LPCCH)spszManifestRaw.get(), -1, spwszManifest.get(), cch);

	//	// Avoid the extra memory allocation if we don't need it (already UTF-16 input).
	//	pwszManifest = spwszManifest.get();
	//}
	//else
	//{
	//	// We're already working with a wide string.
	//	pwszManifest = (LPCWSTR)spszManifestRaw.get();
	//}

	//// Unicode in SimpleIni means "UTF-8", which we are not working with. Since we're working
	//// with UTF-16 text, we make sure SimpleIni is not expecting UTF-8.
	//_spIniReader->SetUnicode(false);

	if (FAILED(ParseManifest((LPCWSTR)spszManifestRaw.get())))
	{
		// ParseManifest logs failures itself, so we don't bother here.
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CThemeLoader::LoadForeignThemeFromDll(LPCWSTR szThemePath)
{
	_szThemeModulePath = szThemePath;
	_spTheme = std::make_unique<CForeignThemeDll>();
	_hModule = LoadLibraryExW(szThemePath, nullptr, LOAD_LIBRARY_AS_IMAGE_RESOURCE | LOAD_LIBRARY_AS_DATAFILE);

	if (!_hModule)
	{
		return HRESULT_FROM_WIN32(GetLastError());
	}

	HRSRC rcManifest = FindResourceW(_hModule, L"CETHEME_MANIFEST", RT_RCDATA);

	if (!rcManifest)
	{
		RETURN_HR_MSG(HRESULT_FROM_WIN32(GetLastError()), "Failed to find CETHEME_MANIFEST resource in theme.");
	}

	HGLOBAL hManifest = LoadResource(_hModule, rcManifest);

	if (!hManifest)
	{
		RETURN_HR_MSG(HRESULT_FROM_WIN32(GetLastError()), "Failed to load CETHEME_MANIFEST resource.");
	}

	LPCWSTR szManifest = (LPCWSTR)LockResource(hManifest);

	if (FAILED(ParseManifest(szManifest)))
	{
		// ParseManifest logs failures itself, so we don't bother here.
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CThemeLoader::ParseManifest(LPCWSTR szManifest)
{
	SI_Error rc = _spIniReader->LoadData((LPCSTR)szManifest, strlen((LPCSTR)szManifest) * 2 + 2 /* zero terminator */);

	if (FAILED(rc))
	{
		RETURN_HR_MSG(rc == SI_NOMEM ? E_OUTOFMEMORY : E_FAIL, "Failed to parse manifest INI.");
	}

	DWORD dwVersion = _spIniReader->GetLongValue(
		L"ClassicExplorerThemeManifest",
		L"ManifestVersion",
		0
	);

	if (dwVersion == 0)
	{
		// Invalid manifest.
		RETURN_HR_MSG(E_FAIL, "Attempted to load a theme with an invalid version.");
	}

	//
	// Parse theme metadata:
	//

	LPCWSTR pszThemeName = _spIniReader->GetValue(
		L"ClassicExplorerThemeManifest",
		L"Name",
		L"(unspecified)"
	);
	_spTheme->_spszName = pszThemeName; // Copy so the lifetime exceeds INI parser.

	LPCWSTR pszThemeDescription = _spIniReader->GetValue(
		L"ClassicExplorerThemeManifest",
		L"Description",
		L"(unspecified)"
	);
	_spTheme->_spszDescription = pszThemeDescription;

	LPCWSTR pszThemeAuthor = _spIniReader->GetValue(
		L"ClassicExplorerThemeManifest",
		L"Author",
		L"(unspecified)"
	);
	_spTheme->_spszAuthor = pszThemeAuthor;

	LPCWSTR pszThemeVersion = _spIniReader->GetValue(
		L"ClassicExplorerThemeManifest",
		L"Version",
		L"0" // Looks nicer as a fallback value.
	);
	_spTheme->_spszVersion = pszThemeVersion;

	//
	// If the theme has a "Files" section, then we must copy all entries of it to the
	// foreign theme in order to be able to access anything declared under there after
	// the theme loader is out of the picture.
	//

	if (_spIniReader->SectionExists(L"Files"))
	{
		if (_spTheme->GetForeignThemeType() == EForeignThemeType::Dll)
		{
			auto pSections = _spIniReader->GetSection(L"Files");

			for (auto section = pSections->begin(); section != pSections->end(); ++section)
			{
				// We need to make copies of these strings so that they outlast the INI parser.
				std::wstring spszKey = section->first.pItem;
				std::wstring spszValue = section->second;

				((CForeignThemeDll *)_spTheme.get())->_mapFiles.emplace(std::move(spszKey), std::move(spszValue));
			}
		}
		else // Folder theme
		{
			CForeignThemeFolder *pFolderTheme = (CForeignThemeFolder *)_spTheme.get();

			// Load every bitmap from the bitmap list:
			for (const ThemeBitmapInfo &rbi : c_rgThemeBitmapInfo)
			{
				LPCWSTR szBitmapFile = _spIniReader->GetValue(L"Files", rbi.szValName);

				if (!szBitmapFile)
				{
					LOG_HR_MSG(E_FAIL, "Nonfatal failure: Theme does not specify bitmap %s", rbi.szValName);
					continue;
				}

				WCHAR szFilePath[MAX_PATH] = { 0 };
				wcscpy_s(szFilePath, _szThemeModulePath);
				PathRemoveFileSpecW(szFilePath);

				PathAppendW(szFilePath, szBitmapFile);

				wil::shared_hbitmap shbm = wil::shared_hbitmap(LoadThemeBitmap(szFilePath));

				// Our enum is one-indexed, but the array is zero-indexed.
				pFolderTheme->_rgshbmBitmapParts[(int)rbi.eVal - 1] = std::move(shbm);
				wcscpy_s(pFolderTheme->_rgszBitmapPaths[(int)rbi.eVal - 1], szFilePath);
			}
		}
	}
	else if (_spTheme->GetForeignThemeType() == EForeignThemeType::Folder)
	{
		// It is illegal for a folder theme to not have a file map.
		RETURN_HR_MSG(E_UNEXPECTED, "Attempted to load folder theme without a file map.");
	}

	//
	// Parse theme properties:
	//

	bool fEnableWatermarks = _spIniReader->GetBoolValue(
		L"Properties",
		L"EnableListViewWatermarks",
		false
	);

	_spTheme->_fExplorerWatermarksEnabled = fEnableWatermarks;

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
		InstallProperty(eBitmap, EThemePartProperty::MaskColor);
	}

	return S_OK;
}

HBITMAP CThemeLoader::LoadThemeBitmap(LPCWSTR szBitmapPath)
{
	return (HBITMAP)LoadImageW(NULL, szBitmapPath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
}

HRESULT CThemeLoader::InstallProperty(int ePart, EThemePartProperty eProperty)
{
	LPCWSTR szPartName = GetIniPartName(ePart);

	//LOG_HR_MSG(S_OK, "InstallProperty PartName: %s", szPartName);

	if (!szPartName)
	{
		//LOG_HR_MSG(E_FAIL, "InstallProperty Failed to find PartName");
		return E_FAIL;
	}

	LPCWSTR szPropertyName = GetIniPropertyName(eProperty);

	//LOG_HR_MSG(S_OK, "InstallProperty PropertyName: %s", szPropertyName);

	if (!szPropertyName)
	{
		//LOG_HR_MSG(E_FAIL, "InstallProperty Failed to find PropertyName");
		return E_FAIL;
	}

	std::wstring spszFullIniPath = std::wstring(szPartName).append(L".").append(szPropertyName);

	switch (eProperty)
	{
		case EThemePartProperty::Color:
		case EThemePartProperty::MaskColor:
		{
			return _InstallColorProperty(ePart, std::make_unique<std::wstring>(spszFullIniPath));
		}

		case EThemePartProperty::Width:
		case EThemePartProperty::Height:
		{
			return _InstallIntegerProperty(
				ePart, 
				eProperty, 
				std::make_unique<std::wstring>(spszFullIniPath)
			);
		}
	}

	return E_FAIL;
}

HRESULT CThemeLoader::_InstallColorProperty(int ePart, std::unique_ptr<std::wstring> pIniPath)
{
	LPCWSTR szIniColorValue = _spIniReader->GetValue(
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
		_spTheme->SetProperty(ePart, EThemePartProperty::Color, crColor);
		return S_OK;
	}

	return E_FAIL;
}

HRESULT CThemeLoader::_InstallIntegerProperty(int ePart, EThemePartProperty eProperty, std::unique_ptr<std::wstring> pIniPath)
{
	if (!_spIniReader->KeyExists(L"Properties", pIniPath->c_str()))
	{
		return E_FAIL;
	}

	OutputDebugString(pIniPath->c_str());

	int iIniValue = _spIniReader->GetLongValue(
		L"Properties",
		pIniPath->c_str(),
		0
	);

	_spTheme->SetProperty(ePart, eProperty, iIniValue);

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