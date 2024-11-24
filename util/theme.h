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

enum class EThemePartType : int
{
	Bitmap = 1,
	Color = 1025,
};

enum class EThemeColor : int
{
	Unknown = 0,
	ThrobberBackground = EThemePartType::Color,

	// Mask colours:
	GoActive,
	GoInactive,

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

	// Stores the byte count if pvData is set.
	long int iData;

	void *pvData = nullptr;
};

struct ITheme
{
	virtual HINSTANCE GetResourceInstance() = 0;
	virtual COLORREF GetColor(EThemeColor colorName) = 0;
	virtual wil::shared_hbitmap GetBitmap(EThemeBitmap bitmapName) = 0;
	virtual SIZE GetBitmapSize(EThemeBitmap eBitmap) = 0;
	virtual LPCWSTR FindBitmapResource(EThemeBitmap bitmapName) = 0;
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

	std::vector<ThemePropertyMap> m_properties;
};

/*
 * CForeignTheme: Implementation of themes stored in foreign modules.
 */
class CForeignTheme 
	: public ITheme
	, private CThemeBase
{
public:
	CForeignTheme();

	HINSTANCE GetResourceInstance() override
	{
		return m_hModule;
	}

	COLORREF GetColor(EThemeColor colorName) override;

	wil::shared_hbitmap GetBitmap(EThemeBitmap bitmapName) override;

	SIZE GetBitmapSize(EThemeBitmap eBitmap) override;

	LPCWSTR FindBitmapResource(EThemeBitmap bitmapName) override;

private:
	HMODULE m_hModule = nullptr;

	// Foreign theme properties
	bool m_fExplorerWatermarksEnabled = false;

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

	HINSTANCE GetResourceInstance() override
	{
		return _AtlBaseModule.GetResourceInstance();
	}

	COLORREF GetColor(EThemeColor colorName) override;

	wil::shared_hbitmap GetBitmap(EThemeBitmap bitmapName) override;

	SIZE GetBitmapSize(EThemeBitmap eBitmap) override;

	LPCWSTR FindBitmapResource(EThemeBitmap bitmapName) override;
};

class CThemeLoader
{
public:
	CThemeLoader();
	HRESULT LoadForeignTheme(LPCWSTR szThemePath);

private:
	HRESULT ParseManifest(LPCWSTR szManifest);
	std::tuple<HRESULT, COLORREF> ParseManifestColor(LPCWSTR szColor);
	HRESULT InstallProperty(int ePart, EThemePartProperty eProperty);
	HRESULT _InstallColorProperty(int ePart, std::unique_ptr<std::wstring> pIniPath);
	HRESULT _InstallIntegerProperty(int ePart, EThemePartProperty eProperty, std::unique_ptr<std::wstring> pIniPath);

	LPCWSTR m_szThemeModulePath = nullptr;
	HMODULE m_hModule = nullptr;
	std::unique_ptr<CForeignTheme> m_pTheme = nullptr;
	std::unique_ptr<CSimpleIniW> m_pIniReader = nullptr;
};