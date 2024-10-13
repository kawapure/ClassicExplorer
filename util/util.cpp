#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

#include "util.h"

namespace CEUtil
{

CESettings GetCESettings()
{
	DWORD showGoButton = 1;
	DWORD showAddressLabel = 1;
	DWORD showFullAddress = 1;
	ClassicExplorerTheme theme = CLASSIC_EXPLORER_2K;
	HKEY hKey;
	LSTATUS ls = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\ClassicExplorer\\AddressAndThrobber", 0, KEY_READ, &hKey);
	if (ls != ERROR_SUCCESS)
	{
		// Key doesn't exist, make it
		ls = RegCreateKeyW(HKEY_CURRENT_USER, L"SOFTWARE\\ClassicExplorer\\AddressAndThrobber", &hKey);
		if (ls != ERROR_SUCCESS) // sum ting wong
			return CESettings(CLASSIC_EXPLORER_NONE, -1, -1,-1);

		// Open the new key with write access
		RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\ClassicExplorer\\AddressAndThrobber", 0, KEY_WRITE, &hKey);

		// Write default values and return default settings
		WCHAR themeDef[] = L"2K";
		RegSetValueExW(hKey, L"Theme", 0, REG_SZ, (BYTE*)themeDef, 4);
		RegSetValueExW(hKey, L"ShowGoButton", 0, REG_DWORD, (BYTE*)&showGoButton, 4);
		RegSetValueExW(hKey, L"ShowAddressLabel", 0, REG_DWORD, (BYTE*)&showAddressLabel, 4);
		RegSetValueExW(hKey, L"ShowFullAddress", 0, REG_DWORD, (BYTE*)&showFullAddress, 4);
		return CESettings(CLASSIC_EXPLORER_2K, 1, 1,1);
	}
	// Read settings
	//WCHAR themeRead[8];
	WCHAR themeRead[9];
	//DWORD size = 8;
	DWORD size = sizeof(themeRead);

	RegGetValueW(hKey, NULL, L"Theme", RRF_RT_REG_SZ, NULL, themeRead, &size);
	if (wcscmp(themeRead, L"XP") == 0)
	{
		theme = CLASSIC_EXPLORER_XP;
	}
	else if (wcscmp(themeRead, L"98") == 0)
	{
		theme = CLASSIC_EXPLORER_MEMPHIS;
	}
	else if (wcscmp(themeRead, L"10") == 0)
	{
		theme = CLASSIC_EXPLORER_10;
	}
	
	DWORD dwValueSize = sizeof(DWORD);
	RegGetValueW(hKey, NULL, L"ShowGoButton", RRF_RT_REG_DWORD, NULL, &showGoButton, &dwValueSize);
	RegGetValueW(hKey, NULL, L"ShowAddressLabel", RRF_RT_REG_DWORD, NULL, &showAddressLabel, &dwValueSize);
	RegGetValueW(hKey, NULL, L"ShowFullAddress", RRF_RT_REG_DWORD, NULL, &showFullAddress, &dwValueSize);

	RegCloseKey(hKey);

	return CESettings(theme, showGoButton, showAddressLabel,showFullAddress);
}

void WriteCESettings(CESettings& toWrite)
{
	HKEY hKey;
	LSTATUS ls = RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\ClassicExplorer\\AddressAndThrobber", 0, KEY_WRITE, &hKey);
	if (ls != ERROR_SUCCESS)
	{
		// Key doesn't exist, make it
		ls = RegCreateKeyW(HKEY_CURRENT_USER, L"SOFTWARE\\ClassicExplorer\\AddressAndThrobber", &hKey);
		if (ls != ERROR_SUCCESS)
			return;

		// Open the new key with write access
		RegOpenKeyExW(HKEY_CURRENT_USER, L"SOFTWARE\\ClassicExplorer\\AddressAndThrobber", 0, KEY_WRITE, &hKey);
	}
	if (toWrite.theme != CLASSIC_EXPLORER_NONE)
	{
		WCHAR theme[] = L"2K";
		if (toWrite.theme == CLASSIC_EXPLORER_XP)
			wcscpy_s(theme, L"XP");
		if (toWrite.theme == CLASSIC_EXPLORER_MEMPHIS)
			wcscpy_s(theme, L"98");
		if (toWrite.theme == CLASSIC_EXPLORER_10)
			wcscpy_s(theme, L"10");
		RegSetValueExW(hKey, L"Theme", 0, REG_SZ, (BYTE*)theme, 4);
	}
	if (toWrite.showGoButton != -1)
	{
		RegSetValueExW(hKey, L"ShowGoButton", 0, REG_DWORD, (BYTE*)&toWrite.showGoButton, 4);
	}
	if (toWrite.showAddressLabel != -1)
	{
		RegSetValueExW(hKey, L"ShowAddressLabel", 0, REG_DWORD, (BYTE*)&toWrite.showAddressLabel, 4);
	}
	if (toWrite.showFullAddress != -1)
	{
		RegSetValueExW(hKey, L"ShowFullAddress", 0, REG_DWORD, (BYTE*)&toWrite.showFullAddress, 4);
	}
	RegCloseKey(hKey);
}

/*
 * GetCurrentFolderPidl: Get the current folder or shell view as a pointer to
 *                       an absolute item ID list.
 *
 * That is, a unique identifier of the current location used internally by the
 * Shell API.
 *
 * See: https://learn.microsoft.com/en-us/previous-versions/windows/desktop/legacy/cc144089(v=vs.85)
 */
HRESULT GetCurrentFolderPidl(CComPtr<IShellBrowser> pShellBrowser, PIDLIST_ABSOLUTE *pidlOut)
{
	CComPtr<IShellView> pView;

	if (pShellBrowser)
	{
		pShellBrowser->QueryActiveShellView(&pView);

		if (pView)
		{
			CComQIPtr<IFolderView> pView2(pView);

			if (pView2)
			{
				CComPtr<IPersistFolder2> pFolder;
				pView2->GetFolder(IID_IPersistFolder2, (void **)&pFolder);

				if (pFolder)
				{
					return pFolder->GetCurFolder(pidlOut); // should return S_OK
				}
			}
		}
	}

	return E_FAIL;
}

/*
 * FixExplorerSizes: Manually correct the sizes of all children in the explorer
 *                   window.
 * 
 * This is useful for avoiding graphical inconsistencies after certain operations
 * which don't immediately sync with explorer.
 * 
 * Make sure to be mindful of redraw loops in calling this. Avoid calling this function
 * from within size handlers, unless you are absolutely sure that the visual needs to be
 * revalidated, or else it will softlock explorer (and probably every other program too).
 * 
 * See: FixExplorerSizesIfNecessary
 * 
 * NOTE: Find a better way of invalidating the explorer visual?
 */
HRESULT FixExplorerSizes(HWND hWndExplorerChild)
{
	HWND hWndExplorerRoot = GetAncestor(hWndExplorerChild, GA_ROOTOWNER);
	if (!IsWindow(hWndExplorerRoot))
		return E_FAIL;

	HWND hWndTabWindow = FindWindowExW(hWndExplorerRoot, NULL, L"ShellTabWindowClass", NULL);
	if (!hWndTabWindow)
		return E_FAIL;

	RECT rcTabWindow;
	GetClientRect(hWndTabWindow, &rcTabWindow);

	int cxTabWindow = rcTabWindow.right - rcTabWindow.left;
	int cyTabWindow = rcTabWindow.bottom - rcTabWindow.top;

	/*
	 * For some reason, Explorer *may* initialise the tab window at 0x0. This isn't always the
	 * case, but I have experienced it occasionally (and consistently thereafter) during testing.
	 * My guess is that it should be initially scaled to fit the last rebar configuration, but
	 * perhaps this cached value gets discarded or corrupted by something.
	 * 
	 * The size is corrected shortly after the call, but it can result in a race condition where
	 * the rebar bands are almost always coerced into being on their own separate row. During my
	 * testing, the bands would only be correctly positioned on the initial sizing if MessageBox
	 * was called, due to the specific way it blocks the thread which allows the normal Explorer
	 * logic to continue executing just enough to set the size but not calculate the rebar band
	 * positions yet.
	 * 
	 * As a result, I was having difficulties with the throbber band being positioned on its own
	 * row instead of on the right side of whichever nearby band. Explorer would correct the
	 * position the moment you sized the explorer window in any way, but not any sooner.
	 * 
	 * Fortunately, this wasn't entirely difficult to correct. Simply overriding the initial
	 * sizing (which we determine by checking if the width of the tab window is 0) does the job
	 * excellently.
	 */
	bool isInitialSizing = cxTabWindow <= 0;

	SetWindowPos(
		hWndTabWindow,
		NULL,
		NULL,
		NULL,
		isInitialSizing ? 1300 : cxTabWindow + 1,
		isInitialSizing ?  900 : cyTabWindow + 1,
		SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER
	);

	if (!isInitialSizing)
	{
		SetWindowPos(
			hWndTabWindow,
			NULL,
			NULL,
			NULL,
			cxTabWindow,
			cyTabWindow,
			SWP_NOACTIVATE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_NOMOVE | SWP_NOOWNERZORDER
		);
	}

	RedrawWindow(hWndExplorerRoot, NULL, NULL, RDW_INVALIDATE);

	return S_OK;
}

HRESULT FixExplorerSizesIfNecessary(HWND explorerChild)
{
	bool shouldResize = false;

	HWND explorerRoot = GetAncestor(explorerChild, GA_ROOTOWNER);
	if (!IsWindow(explorerRoot))
		return E_FAIL;

	//------------------------------------------------------------------------------
	// BEGIN CHECKS
	//------------------------------------------------------------------------------

	// A manual resize is necessary if the height of the ReBar host shell worker is
	// different from the height of the ReBar:
	{
		HWND tab = FindWindowExW(explorerRoot, NULL, L"ShellTabWindowClass", NULL);
		HWND worker = FindWindowExW(tab, NULL, L"WorkerW", NULL);
		HWND rebar = FindWindowExW(worker, NULL, L"ReBarWindow32", NULL);

		// Worker must exist if this can be true.
		if (IsWindow(rebar))
		{
			RECT rcWorker, rcRebar;
			GetWindowRect(worker, &rcWorker);
			GetWindowRect(rebar, &rcRebar);

			int cyWorker = rcWorker.bottom - rcWorker.top;
			int cyRebar = rcRebar.bottom - rcRebar.top;

			if (cyWorker > cyRebar)
			{
				shouldResize = true;
			}
		}
	}

	//------------------------------------------------------------------------------
	// END CHECKS
	//------------------------------------------------------------------------------

	if (shouldResize)
	{
		FixExplorerSizes(explorerChild);
	}

	return S_OK;
}

} // namespace CEUtil