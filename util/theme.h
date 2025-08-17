#pragma once

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include <memory>
#include <tuple>
#include <vector>
#include <string>
#include "wil/resource.h"
#include <map>
#include <simpleini/SimpleIni.h>

enum class EThemePartType : int
{
	Bitmap = 1,
	Color = 1025,
	Bool = 2049,
};

enum class EThemeColor : int
{
	Unknown = 0,
	ThrobberBackground = EThemePartType::Color,

	// This value must always be the last in the enum:
	End
};

enum class EThemeBitmap : int
{
	Unknown = 0,
	GoActive = EThemePartType::Bitmap,
	GoInactive,
	ThrobberSmall,
	ThrobberMedium,
	ThrobberLarge,
	WatermarkMusic,
	WatermarkSearch,
	WatermarkVideos,
	WatermarkPictures,

	// This value must always be the last in the enum:
	End
};

enum class EThemeBool : int
{
	Unknown = 0,
	EnableListViewWatermarks = EThemePartType::Bool,

	// This value must always be the last in the enum:
	End
};

enum class EThemePartProperty : int
{
	Color,
	MaskColor,
	Width,
	Height,
};

enum class EMappedPropertyDataType : int
{
	Integer,
	Float,
	String,
};

struct ThemePropertyMap
{
	int ePartName;
	EThemePartProperty ePropertyName;
	EMappedPropertyDataType eType;

	// Stores the byte count if pvData is set. Otherwise, this stores any arbitrary value.
	long int iData;

	void *pvData = nullptr;
};

interface ITheme
{
	virtual LPCWSTR GetThemeName() PURE;
	virtual LPCWSTR GetThemeAuthor() PURE;
	virtual LPCWSTR GetThemeDescription() PURE;
	virtual LPCWSTR GetThemeVersion() PURE;

	virtual HINSTANCE GetResourceInstance() PURE;
	virtual bool GetBool(EThemeBool eBool) PURE;
	virtual COLORREF GetColor(EThemeColor colorName) PURE;
	virtual wil::shared_hbitmap GetBitmap(EThemeBitmap bitmapName) PURE;
	virtual SIZE GetBitmapSize(EThemeBitmap eBitmap) PURE;
	virtual COLORREF GetBitmapMaskColor(EThemeBitmap eBitmap) PURE;
	virtual HIMAGELIST LoadImageListFromBitmap(EThemeBitmap eBitmap) PURE;
	virtual LPCWSTR FindBitmapResource(EThemeBitmap bitmapName) PURE;
};

class CThemeBase
{
protected:
	std::tuple<HRESULT, std::unique_ptr<ThemePropertyMap> >
		FindProperty(int ePartName, EThemePartProperty eProperty);

	std::tuple<HRESULT, int> GetIntegerProperty(int ePartName, EThemePartProperty eProperty);
	std::tuple<HRESULT, LPCWSTR> GetStringProperty(int ePartName, EThemePartProperty eProperty);

	HRESULT SetProperty(int ePartName, EThemePartProperty eProperty, int iValue);

	HRESULT SetProperty(int ePartName, EThemePartProperty eProperty, COLORREF crValue)
	{
		return SetProperty(ePartName, eProperty, (int)crValue);
	}

	HRESULT SetProperty(int ePartName, EThemePartProperty eProperty, LPCWSTR szValue);

	std::vector<ThemePropertyMap> _properties;
};

// Used by the theme loader to diffuse behaviour.
enum class EForeignThemeType
{
	Dll,
	Folder,
};

/*
 * CForeignThemeBase: Shared foreign theme code between DLL and folder loader methods.
 */
class CForeignThemeBase
	: public ITheme
	, private CThemeBase
{
public:
	CForeignThemeBase();

	virtual EForeignThemeType GetForeignThemeType() PURE;

	LPCWSTR GetThemeName() override { return _spszName.c_str(); }
	LPCWSTR GetThemeAuthor() override { return _spszAuthor.c_str(); }
	LPCWSTR GetThemeDescription() override { return _spszDescription.c_str(); }
	LPCWSTR GetThemeVersion() override { return _spszVersion.c_str(); }

	bool GetBool(EThemeBool eBool) override;

	COLORREF GetColor(EThemeColor colorName) override;

	SIZE GetBitmapSize(EThemeBitmap eBitmap) override;
	COLORREF GetBitmapMaskColor(EThemeBitmap eBitmap) override;

protected:
	// Foreign theme properties
	std::wstring _spszName;
	std::wstring _spszAuthor;
	std::wstring _spszDescription;
	std::wstring _spszVersion;
	bool _fExplorerWatermarksEnabled = false;

	friend class CThemeLoader;
};

/*
 * CForeignThemeBaseDll: Implementation of themes stored in foreign modules.
 */
class CForeignThemeDll : public CForeignThemeBase
{
public:
	HINSTANCE GetResourceInstance() override
	{
		return _hModule;
	}

	EForeignThemeType GetForeignThemeType() override { return EForeignThemeType::Dll; }

	wil::shared_hbitmap GetBitmap(EThemeBitmap bitmapName) override;

	LPCWSTR FindBitmapResource(EThemeBitmap bitmapName) override;

	HIMAGELIST LoadImageListFromBitmap(EThemeBitmap eBitmap) override;

private:
	HMODULE _hModule = nullptr;
	std::map<std::wstring, std::wstring> _mapFiles;

	friend class CThemeLoader;
};

class CForeignThemeFolder : public CForeignThemeBase
{
public:
	HINSTANCE GetResourceInstance() override
	{
		// Not possible.
		return NULL;
	}

	EForeignThemeType GetForeignThemeType() override { return EForeignThemeType::Folder; }

	wil::shared_hbitmap GetBitmap(EThemeBitmap bitmapName) override;

	LPCWSTR FindBitmapResource(EThemeBitmap bitmapName) override;

	HIMAGELIST LoadImageListFromBitmap(EThemeBitmap eBitmap) override;
	
private:
	wil::shared_hbitmap _rgshbmBitmapParts[(int)EThemeBitmap::End - (int)EThemePartType::Bitmap];
	WCHAR _rgszBitmapPaths[(int)EThemeBitmap::End - (int)EThemePartType::Bitmap][MAX_PATH];

	friend class CThemeLoader;
};

/*
 * CNativeTheme: Implementation of the default theme embedded in the main module.
 */
class CNativeTheme 
	: public ITheme
	//, private CThemeBase // not currently necessary as native theme properties are hardcoded
{
public:
	CNativeTheme();

	/*
	 * Even though this isn't a "real" theme, we still need to meet the signature, so here are
	 * some hardcoded strings to account for theme metadata.
	 */

	LPCWSTR GetThemeName() override
	{
		return L"Built-in theme (Windows XP Luna)";
	}

	LPCWSTR GetThemeAuthor() override
	{
		return L"Microsoft Corporation";
	}

	LPCWSTR GetThemeDescription() override
	{
		return L"The built-in theme of ClassicExplorer.";
	}

	LPCWSTR GetThemeVersion() override
	{
		return L"1.0.0";
	}

	HINSTANCE GetResourceInstance() override
	{
		// Return ClassicExplorer's own HMODULE; it is, after all, what we use for themes.
		return _AtlBaseModule.GetResourceInstance();
	}

	bool GetBool(EThemeBool eBool) override;

	COLORREF GetColor(EThemeColor colorName) override;

	wil::shared_hbitmap GetBitmap(EThemeBitmap bitmapName) override;

	SIZE GetBitmapSize(EThemeBitmap eBitmap) override;

	COLORREF GetBitmapMaskColor(EThemeBitmap eBitmap) override;

	HIMAGELIST LoadImageListFromBitmap(EThemeBitmap eBitmap) override;

	LPCWSTR FindBitmapResource(EThemeBitmap bitmapName) override;
};

class CThemeLoader
{
public:
	CThemeLoader();
	HRESULT LoadForeignTheme(LPCWSTR szThemePath);

	inline CForeignThemeBase *ObtainTheme()
	{
		return _spTheme.release();
	}

private:
	HRESULT LoadForeignThemeFromDll(LPCWSTR szThemePath);
	HRESULT LoadForeignThemeFromFolder(LPCWSTR szThemeManifestPath);

	HBITMAP LoadThemeBitmap(LPCWSTR szBitmapPath);

	HRESULT ParseManifest(LPCWSTR szManifest);
	std::tuple<HRESULT, COLORREF> ParseManifestColor(LPCWSTR szColor);
	HRESULT InstallProperty(int ePart, EThemePartProperty eProperty);
	HRESULT _InstallColorProperty(int ePart, std::unique_ptr<std::wstring> pIniPath);
	HRESULT _InstallIntegerProperty(int ePart, EThemePartProperty eProperty, std::unique_ptr<std::wstring> pIniPath);

	LPCWSTR _szThemeModulePath = nullptr;
	HMODULE _hModule = nullptr;
	std::unique_ptr<CForeignThemeBase> _spTheme = nullptr;
	std::unique_ptr<CSimpleIniW> _spIniReader = nullptr;
};