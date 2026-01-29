/*
 * AddressBar.cpp: Implements the address bar toolbar.
 */

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"
#include <commoncontrols.h>
#include "util/shell_helpers.h"
#include "util/util.h"

#include "AddressBar.h"
#include "winreg.h"

std::wstring CAddressBar::m_goText = L"";

/*
 * OnCreate: Handle the WM_CREATE message sent out and create the address bar controls.
 */
LRESULT CAddressBar::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	HINSTANCE moduleInstance = _AtlBaseModule.GetModuleInstance();

	m_showGoButton = true;

	CEUtil::CESettings cS = CEUtil::GetCESettings();
	m_showGoButton = cS.showGoButton;
	m_theme = cS.theme;

	if (m_goText == L"")
	{
		HMODULE explorerframe = LoadLibrary(L"explorerframe.dll");
		if (explorerframe)
		{
			WCHAR* goText = new WCHAR[32];
			LoadStringW(explorerframe, 12656, goText, 32);
			m_goText = goText;
			delete[] goText;
			FreeLibrary(explorerframe);

			m_goText.erase(m_goText.begin());
			auto it = m_goText.begin();
			while (it != m_goText.end())
			{
				if (*it == L' ')
				{
					m_goText.erase(it, m_goText.end());
					break;
				}
				++it;
			}
		}
		else
			m_goText = L"Go";
	}

	m_toolbar = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		WC_COMBOBOXEXW,
		nullptr,
		WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_TABSTOP | CCS_NODIVIDER | CCS_NOMOVEY | CBS_OWNERDRAWFIXED,
		0, 0, 500, 250,
		m_hWnd,
		nullptr,
		moduleInstance,
		nullptr
	);

	if (m_toolbar == nullptr)
	{
		return E_FAIL;
	}

	SetWindowSubclass(m_toolbar, ComboboxSubclassProc, (UINT_PTR)this, (DWORD_PTR)this);

	::SendMessageW(m_toolbar, CB_SETITEMHEIGHT, -1, 16);
	::SendMessageW(
		m_toolbar,
		CBEM_SETEXTENDEDSTYLE,
		CBES_EX_CASESENSITIVE | CBES_EX_NOSIZELIMIT,
		CBES_EX_CASESENSITIVE | CBES_EX_NOSIZELIMIT
	);
	m_comboBox = (HWND)::SendMessageW(m_toolbar, CBEM_GETCOMBOCONTROL, 0, 0);
	m_comboBoxEditCtl = (HWND)::SendMessageW(m_toolbar, CBEM_GETEDITCONTROL, 0, 0);

	SetWindowSubclass(m_comboBox, RealComboboxSubclassProc, (UINT_PTR)this, (DWORD_PTR)this);
	SetWindowSubclass(m_comboBoxEditCtl, RealComboboxSubclassProc, (UINT_PTR)this, (DWORD_PTR)this);

	// Set the address bar combobox to use the shell image list.
	// This is required in order for icons to be able to render in the address bar.
	IImageList *piml;
	SHGetImageList(SHIL_SMALL, IID_IImageList, (void**)&piml);
	if (piml)
	{
		::SendMessageW(
			m_toolbar,
			CBEM_SETIMAGELIST,
			0,
			(LPARAM)piml
		);
	}

	// Provides autocomplete capabilities to the combobox editor.
	// This is a standard shell API, surprisingly enough.
	SHAutoComplete(m_comboBoxEditCtl, SHACF_FILESYSTEM | SHACF_URLALL | SHACF_USETAB);

	if (m_showGoButton)
	{
		CreateGoButton();
	}

	return S_OK;
}

/*
 * CreateGoButton: Creates the control window for the "Go" button.
 * 
 * TODO: Implement the ability to properly toggle this feature.
 */
LRESULT CAddressBar::CreateGoButton()
{
	HINSTANCE moduleInstance = _AtlBaseModule.GetModuleInstance();

	const TBBUTTON goButtonInfo[] = { {0, 1, TBSTATE_ENABLED, 0} };
	HINSTANCE resourceInstance = _AtlBaseModule.GetResourceInstance();

	int go_inactive_bitmap = IDB_2K_GO_INACTIVE;
	int go_active_bitmap = IDB_2K_GO_ACTIVE;
	if (m_theme == CLASSIC_EXPLORER_2K)
	{
		go_inactive_bitmap = IDB_2K_GO_INACTIVE;
		go_active_bitmap = IDB_2K_GO_ACTIVE;
	}
	else if (m_theme == CLASSIC_EXPLORER_XP)
	{
		go_inactive_bitmap = IDB_XP_GO_INACTIVE;
		go_active_bitmap = IDB_XP_GO_ACTIVE;
	}

	m_himlGoInactive = ImageList_LoadImageW(
		resourceInstance,
		MAKEINTRESOURCEW(go_inactive_bitmap),
		20,
		0,
		RGB(0, 0, 0),
		IMAGE_BITMAP,
		LR_CREATEDIBSECTION
	);

	m_himlGoActive = ImageList_LoadImageW(
		resourceInstance,
		MAKEINTRESOURCEW(go_active_bitmap),
		20,
		0,
		RGB(0, 0, 0),
		IMAGE_BITMAP,
		LR_CREATEDIBSECTION
	);

	m_goButton = CreateWindowEx(
		WS_EX_TOOLWINDOW,
		TOOLBARCLASSNAMEW,
		nullptr,
		WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TBSTYLE_LIST | TBSTYLE_FLAT | TBSTYLE_TOOLTIPS |
		CCS_NODIVIDER | CCS_NOPARENTALIGN | CCS_NORESIZE,
		0,
		0,
		50,
		50,
		m_toolbar,
		nullptr,
		moduleInstance,
		nullptr
	);

	if (!m_goButton)
	{
		return E_FAIL;
	}

	::SendMessage(m_goButton, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0); // 1 button
	::SendMessage(m_goButton, TB_SETMAXTEXTROWS, 1, 0);

	if (m_himlGoInactive)
		::SendMessage(m_goButton, TB_SETIMAGELIST, 0, (LPARAM)m_himlGoInactive);

	if (m_himlGoActive)
		::SendMessage(m_goButton, TB_SETHOTIMAGELIST, 0, (LPARAM)m_himlGoActive);

	WCHAR pwszGoLabel[255];

	::SendMessage(m_goButton, TB_ADDSTRINGW, 0, (LPARAM)m_goText.c_str());

	// add the go button:
	::SendMessage(m_goButton, TB_ADDBUTTONSW, 1, (LPARAM)&goButtonInfo);
	::SendMessage(m_goButton, TB_AUTOSIZE, 0, 0);
	::ShowWindow(m_goButton, TRUE);

	return S_OK;
}

/*
 * InitComboBox: Initialise the address bar combobox.
 * 
 * This is only called once every time a new explorer window is opened.
 */
HRESULT CAddressBar::InitComboBox()
{
	RefreshCurrentAddress();

	return S_OK;
}

/*
 * OnDestroy: Handle WM_DESTROY messages.
 */
LRESULT CAddressBar::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	return 0;
}

/*
 * OnComponentNotifyClick: Handle click events sent out to other components:
 *                         notably, the go button.
 */
LRESULT CAddressBar::OnComponentNotifyClick(WPARAM wParam, LPNMHDR notifyHeader, BOOL &bHandled)
{
	if (notifyHeader->hwndFrom == m_goButton)
	{
		this->Execute();
	}

	return 0;
}

/*
 * OnNotify: Handle WM_NOTIFY messages.
 * 
 * This is used in order to respond to events sent out from the combobox for UX
 * reasons, such as switching the display text with the full path when clicked.
 */
LRESULT CAddressBar::OnNotify(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL &bHandled)
{
	LPNMHDR hdr = (LPNMHDR)lParam;

	if (hdr->code == CBEN_ENDEDITW)
	{
		NMCBEENDEDITW *endEdit = (NMCBEENDEDITW *)lParam;
		if (endEdit->iWhy == CBENF_RETURN)
		{
			this->Execute();
		}
		else if (endEdit->iWhy = CBENF_ESCAPE)
		{
			RefreshCurrentAddress();
		}
	}
	else if (hdr->code == CBEN_BEGINEDIT)
	{
		//PNMCBEENDEDITW *beginEdit = (PNMCBEENDEDITW *)lParam;

		if (m_locationHasPhysicalPath)
		{
			::SetWindowTextW(m_comboBoxEditCtl, m_currentPath);
			::SendMessageW(m_comboBoxEditCtl, EM_SETSEL, 0, -1);
		}
	}

	return S_OK;
}

/*
 * ComboboxSubclassProc: Overrides the handling of messages sent to the combobox.
 * 
 * This is used for properly sizing and positioning the address bar controls,
 * among other things.
 */
LRESULT CALLBACK CAddressBar::ComboboxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CAddressBar *self = (CAddressBar *)dwRefData;

	if (uMsg == WM_SIZE || uMsg == WM_WINDOWPOSCHANGING)
	{
		RECT rcGoButton;
		RECT rcComboBox;
		RECT rcRefresh;

		long newHeight;
		long newWidth;

		if (uMsg == WM_SIZE)
		{
			newHeight = HIWORD(lParam);
			newWidth = LOWORD(lParam);
		}
		else if (uMsg == WM_WINDOWPOSCHANGING)
		{
			WINDOWPOS pos = *(WINDOWPOS *)lParam;
			newHeight = pos.cy;
			newWidth = pos.cx;
		}

		// Declare margins:
		constexpr int addressRightMargin = 2;
		int goRightMargin = 0; // Assuming go button is not displayed.

		long goButtonWidth = 0;
		long goButtonHeight = 0;
		if (self->m_showGoButton)
		{
			::SendMessageW(self->m_goButton, TB_GETITEMRECT, 0, (LPARAM)&rcGoButton);
			goButtonWidth = rcGoButton.right - rcGoButton.left;
			goButtonHeight = rcGoButton.bottom - rcGoButton.top;

			// Set the right margin of the go button to also display.
			goRightMargin = 2;
		}

		if (uMsg == WM_SIZE)
		{
			DefSubclassProc(hWnd, WM_SIZE, wParam, MAKELONG(newWidth - goButtonWidth - 2, newHeight));
		}
		else if (uMsg == WM_WINDOWPOSCHANGING)
		{
			WINDOWPOS pos = *(WINDOWPOS *)lParam;
			pos.cx = newWidth - addressRightMargin - goButtonWidth - goRightMargin;

			DefSubclassProc(hWnd, WM_WINDOWPOSCHANGING, wParam, (LPARAM)&pos);
		}

		if (self->m_showGoButton)
		{
			::GetWindowRect(self->m_comboBox, &rcComboBox);
			::SetWindowPos(
				self->m_goButton,
				nullptr,
				newWidth - goButtonWidth - goRightMargin,
				(rcComboBox.bottom - rcComboBox.top - goButtonHeight) / 2,
				goButtonWidth,
				goButtonHeight,
				SWP_NOOWNERZORDER | SWP_SHOWWINDOW | SWP_NOACTIVATE | SWP_NOZORDER
			);
		}

		return 0;
	}
	else if (uMsg == WM_ERASEBKGND)
	{
		POINT pt = { 0, 0 }, ptOrig;
		HWND parentWindow = self->GetParent();
		::MapWindowPoints(hWnd, parentWindow, &pt, 1);
		OffsetWindowOrgEx((HDC)wParam, pt.x, pt.y, &ptOrig);

		LRESULT result = SendMessage(parentWindow, WM_ERASEBKGND, wParam, 0);
		SetWindowOrgEx((HDC)wParam, ptOrig.x, ptOrig.y, nullptr);

		return result;
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

/*
 * RealComboboxSubclassProc: todo?
 */
LRESULT CALLBACK CAddressBar::RealComboboxSubclassProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CAddressBar *self = (CAddressBar *)dwRefData;

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

/*
 * SetBrowsers: Set the browser interfaces to use for navigation purposes.
 */
void CAddressBar::SetBrowsers(CComPtr<IShellBrowser> pShellBrowser, CComPtr<IWebBrowser2> pWebBrowser)
{
	m_pShellBrowser = pShellBrowser;
	m_pWebBrowser = pWebBrowser;
}

/*
 * HandleNavigate: Handle a browser navigation event.
 */
HRESULT CAddressBar::HandleNavigate()
{
	RefreshCurrentAddress();

	return S_OK;
}

/*
 * RefreshCurrentAddress: Get the respective display name or address of the
 *                        current explorer page and configure the address
 *                        bar respectively.
 * 
 * TODO: Split this up into smaller utility functions for code reuse purposes.
 */
HRESULT CAddressBar::RefreshCurrentAddress()
{
	HRESULT hr = S_OK;
	PIDLIST_ABSOLUTE pidlCurrentFolder;
	CComPtr<IKnownFolderManager> pKnownFolderManager;
	CComPtr<IKnownFolder> pKnownFolder;

	hr = GetCurrentFolderPidl(&pidlCurrentFolder);

	if (hr != S_OK)
		return hr;

	// Get the display text (current path or folder display name)
	ZeroMemory(m_displayName, ARRAYSIZE(m_displayName));
	ZeroMemory(m_currentPath, ARRAYSIZE(m_currentPath));

	hr = pKnownFolderManager.CoCreateInstance(CLSID_KnownFolderManager);

	if (hr != S_OK)
		return hr;

	HRESULT isKnownFolder = pKnownFolderManager->FindFolderFromIDList(
		pidlCurrentFolder,
		&pKnownFolder
	);

	m_locationHasPhysicalPath = SHGetPathFromIDListW(pidlCurrentFolder, m_currentPath);

	// If the check if it's a known folder failed, then the path must be physical:
	if (FAILED(isKnownFolder))
	{
		if (CEUtil::GetCESettings().showFullAddress)
		{
			BOOL hasPath = SUCCEEDED(
				ShellHelpers::GetLocalizedDisplayPath(
					m_currentPath,
					m_displayName,
					ARRAYSIZE(m_displayName)
				)
			);

			// In the case this fails, just put the display name of the folder in its place.
			if (!m_locationHasPhysicalPath)
			{
				hr = GetCurrentFolderName(m_displayName, ARRAYSIZE(m_displayName));

				if (hr != S_OK)
					return hr;
			}
		}
		else
		{
			hr = GetCurrentFolderName(m_displayName, ARRAYSIZE(m_displayName));

			if (hr != S_OK)
				return hr;
		}
	}
	else
	{
		// Known folders can also return a display name that is just a path. In order for
		// localized display paths to work, this needs to be accounted for.
		WCHAR buf[1024];
		hr = GetCurrentFolderName(buf, ARRAYSIZE(buf));

		if (hr != S_OK)
			return hr;

		BOOL shouldCopy = TRUE;

		if (ShellHelpers::IsStringPath(buf) && m_locationHasPhysicalPath)
		{
			// If this fails, then it will just copy the original.
			shouldCopy = !SUCCEEDED(
				ShellHelpers::GetLocalizedDisplayPath(
					m_currentPath,
					m_displayName,
					ARRAYSIZE(m_displayName)
				)
			);
		}

		if (shouldCopy)
		{
			wcscpy_s(m_displayName, buf);
		}
	}

	// Get the folder icon
	CComPtr<IShellFolder> pShellFolder;
	PCITEMID_CHILD pidlChild;

	hr = SHBindToParent(
		pidlCurrentFolder,
		IID_IShellFolder,
		(void **)&pShellFolder,
		&pidlChild
	);

	// Create the combobox item
	COMBOBOXEXITEMW item = { CBEIF_IMAGE | CBEIF_SELECTEDIMAGE | CBEIF_TEXT };
	item.iItem = -1; // -1 = selected item
	item.iImage = SHMapPIDLToSystemImageListIndex(
		pShellFolder,
		pidlChild,
		&item.iSelectedImage
	);
	item.pszText = m_displayName;

	::SendMessageW(
		m_toolbar,
		CBEM_SETITEMW,
		0,
		(LPARAM)&item
	);

	pShellFolder.Release();
	pKnownFolder.Release();
	pKnownFolderManager.Release();

	return S_OK;
}

/*
 * GetCurrentAddressText: Get the current text in the address bar.
 */
BOOL CAddressBar::GetCurrentAddressText(CComHeapPtr<WCHAR> &pszText)
{
	pszText.Free();

	INT cchMax = ::GetWindowTextLengthW(m_comboBoxEditCtl) + sizeof(UNICODE_NULL);
	if (!pszText.Allocate(cchMax))
		return FALSE;

	return ::GetWindowTextW(m_comboBoxEditCtl, pszText, cchMax);
}

HRESULT STDMETHODCALLTYPE CAddressBar::ShowFileNotFoundError(HRESULT hRet)
{
	CComHeapPtr<WCHAR> input;
	if (!GetCurrentAddressText(input))
		return E_FAIL;


	WCHAR szFormat[512];
	WCHAR szMessage[1024];
	WCHAR szTitle[512];
	HINSTANCE hInst = _AtlBaseModule.GetModuleInstance();
	if (m_theme == CLASSIC_EXPLORER_2K)
	{
		LoadStringW(hInst, IDS_NOTFOUND_TEXT_2K, szFormat, 512);
		LoadStringW(hInst, IDS_NOTFOUND_TITLE_2K, szTitle, 512);
	}
	else
	{
		LoadStringW(hInst, IDS_NOTFOUND_TEXT_XP, szFormat, 512);
		LoadStringW(hInst, IDS_NOTFOUND_TITLE_XP, szTitle, 512);

		// The XP error is from IE, and shows a file:// URI.
		// Swap out \ for /.
		int len = wcslen(input);
		for (int i = 0; i < len; i++)
		{
			if (input[i] == L'\\')
				input[i] = L'/';
		}
	}

	swprintf_s(szMessage, szFormat, input);
	::MessageBoxW(m_comboBoxEditCtl, szMessage, szTitle, MB_OK | MB_ICONERROR);

	return hRet;
}

/*
 * Execute: Perform the browse action with the requested address.
 */
HRESULT CAddressBar::Execute()
{
	HRESULT hr = E_FAIL;
	PIDLIST_RELATIVE parsedPidl = nullptr;
	
	// have to be initialised before goto cleanup...
	CComPtr<IShellFolder> pShellFolder;
	CComPtr<IShellFolder> pShellFolder2;

	hr = ParseAddress(&parsedPidl);

	if (hr == HRESULT_FROM_WIN32(ERROR_INVALID_DRIVE) || hr == HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND))
	{
		ILFree(parsedPidl);

		if (SUCCEEDED(ExecuteCommandLine()))
			return S_OK;

		return ShowFileNotFoundError(hr);
	}

	if (!parsedPidl)
	{
		return E_FAIL;
	}

	PIDLIST_ABSOLUTE pidlCurrentFolder;
	hr = GetCurrentFolderPidl(&pidlCurrentFolder);
	if (FAILED(hr))
		goto cleanup;

	
	hr = SHGetDesktopFolder(&pShellFolder);
	if (FAILED(hr))
		goto cleanup;

	hr = pShellFolder->CompareIDs(0, pidlCurrentFolder, parsedPidl);

	SHFree(pidlCurrentFolder);
	if (hr == 0)
		goto cleanup;

	hr = m_pShellBrowser->BrowseObject(parsedPidl, 0);
	if (SUCCEEDED(hr))
		goto cleanup;

	HWND topLevelWindow;
	hr = IUnknown_GetWindow(m_pShellBrowser, &topLevelWindow);
	if (FAILED(hr))
		goto cleanup;

	PCUITEMID_CHILD pidlChild;
	hr = SHBindToParent(
		(PCIDLIST_ABSOLUTE)parsedPidl,
		IID_IShellFolder,
		(void **)&pShellFolder2,
		&pidlChild
	);
	if (FAILED(hr))
		goto cleanup;

	// TODO: steal SHInvokeDefaultCommand stuff from WinAPI and ReactOS...

	cleanup:
		ILFree(parsedPidl);

	return hr;
}

/*
 * ParseAddress: Parse the browser address into m_lastParsedPidl.
 */
HRESULT CAddressBar::ParseAddress(PIDLIST_RELATIVE *pidlOut)
{
	HRESULT hr = E_FAIL;

	ULONG eaten = 0, attributes = 0;
	HWND topLevelWindow = nullptr;
	PIDLIST_RELATIVE relativePidl = nullptr;

	// Must be initialised before any gotos...
	CComPtr<IShellFolder> pCurrentFolder = nullptr;
	PIDLIST_ABSOLUTE currentPidl = nullptr;

	// TODO: what is IBrowserService, how to replicate with newer code?
	// I think it is only used for the current PIDL.

	hr = IUnknown_GetWindow(m_pShellBrowser, &topLevelWindow);
	if (FAILED(hr))
		return hr;

	CComHeapPtr<WCHAR> input, address;
	if (!GetCurrentAddressText(input))
		return E_FAIL;

	int addressLength = (wcschr(input, L'%')) ? ::ExpandEnvironmentStringsW(input, nullptr, 0) : 0;
	if (
		addressLength <= 0 ||
		!address.Allocate(addressLength + 1) ||
		!::ExpandEnvironmentStringsW(input, address, addressLength)
	)
	{
		address.Free();
		address.Attach(input.Detach());
	}

	CComPtr<IShellFolder> psfDesktop;
	hr = SHGetDesktopFolder(&psfDesktop);
	if (FAILED(hr))
		goto cleanup;

	// hr = pBrowserService->GetPidl(&currentPidl); // REPLICATE WITHOUT IBrowserService.
	hr = GetCurrentFolderPidl(&currentPidl);
	if (FAILED(hr))
		goto parse_absolute;

	hr = psfDesktop->BindToObject(
		currentPidl,
		nullptr,
		IID_IShellFolder,
		(void **)&pCurrentFolder
	);
	if (FAILED(hr))
		goto parse_absolute;

	hr = pCurrentFolder->ParseDisplayName(
		topLevelWindow,
		nullptr,
		address,
		&eaten,
		&relativePidl,
		&attributes
	);
	if (SUCCEEDED(hr))
	{
		*pidlOut = ILCombine(currentPidl, relativePidl);
		ILFree(relativePidl);
		goto cleanup;
	}

	parse_absolute:
		// Used in case a relative path could not be parsed:
		hr = psfDesktop->ParseDisplayName(
			topLevelWindow,
			nullptr,
			address,
			&eaten,
			pidlOut,
			&attributes
		);

	cleanup:
		if (currentPidl)
			ILFree(currentPidl);

	return hr;
}

/*
 * ExecuteCommandLine: Run the text in the address bar as if it is a shell
 *                     command.
 */
HRESULT CAddressBar::ExecuteCommandLine()
{
	HRESULT hr = E_FAIL;

	CComHeapPtr<WCHAR> pszCmdLine;
	if (!GetCurrentAddressText(pszCmdLine))
		return E_FAIL;

	PWCHAR args = PathGetArgsW(pszCmdLine);
	PathRemoveArgsW(pszCmdLine);
	PathUnquoteSpacesW(pszCmdLine);

	SHELLEXECUTEINFOW info = {
		sizeof(info),
		SEE_MASK_FLAG_NO_UI,
		m_hWnd
	};
	info.lpFile = pszCmdLine;
	info.lpParameters = args;
	info.nShow = SW_SHOWNORMAL;

	WCHAR dir[MAX_PATH] = L"";
	PIDLIST_ABSOLUTE pidl;
	if (SUCCEEDED(GetCurrentFolderPidl(&pidl)))
	{
		if (SHGetPathFromIDListW(pidl, dir) && PathIsDirectoryW(dir))
		{
			info.lpDirectory = dir;
		}
	}

	if (!::ShellExecuteExW(&info))
		return E_FAIL;

	// We want to put the original text of the address bar back once the command
	// is done executing.
	RefreshCurrentAddress();

	return S_OK;
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
HRESULT CAddressBar::GetCurrentFolderPidl(PIDLIST_ABSOLUTE *pidlOut)
{
	return CEUtil::GetCurrentFolderPidl(m_pShellBrowser, pidlOut);
}

/*
 * GetCurrentFolderName: Get the display name of the current folder.
 * 
 * This is used as a fallback in case the path of the folder cannot be obtained,
 * or in the case of Known Folders which should always show their display name
 * (for example, the user's Documents folder).
 */
HRESULT CAddressBar::GetCurrentFolderName(WCHAR *pszName, long length)
{
	HRESULT hr = S_OK;

	CComPtr<IShellFolder> pShellFolder;
	PCITEMID_CHILD pidlChild;

	PIDLIST_ABSOLUTE pidlCurFolder;
	hr = GetCurrentFolderPidl(&pidlCurFolder);

	if (hr != S_OK)
		return hr;

	hr = SHBindToParent(
		pidlCurFolder,
		IID_IShellFolder,
		(void **)&pShellFolder,
		&pidlChild
	);

	if (hr != S_OK)
		return hr;

	STRRET ret;
	hr = pShellFolder->GetDisplayNameOf(
		pidlChild,
		CEUtil::GetCESettings().showFullAddress ? SHGDN_FORADDRESSBAR | SHGDN_FORPARSING : 0,
		&ret
	);

	if (hr != S_OK)
		return hr;

	hr = StrRetToBuf(&ret, pidlChild, pszName, length);

	if (hr != S_OK)
		return hr;

	return S_OK;
}