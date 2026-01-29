#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

#include "util.h"

namespace ShellHelpers
{

/*
 * GetLocalizedDisplayPath: Get the path of the given folder, respecting translated
 *                          file names.
 * 
 * I wish there were a better approach to doing this, but I couldn't ever figure
 * anything out. This simply traverses the file system from the requested file up,
 * and rebuilds the path manually.
 */
HRESULT GetLocalizedDisplayPath(const WCHAR *path, WCHAR *out, long length)
{
	HRESULT hr = S_OK;

	ZeroMemory(out, length);

	// Avoid modifying the input pointer:
	WCHAR copyPath[2048];
	wcscpy_s(copyPath, path);

	WCHAR truePath[2048] = { 0 };

	WCHAR *buffer = nullptr;
	WCHAR *token = wcstok_s(copyPath, L"\\", &buffer);
	int i = 0;

	while (token)
	{
		int tokenLen = wcslen(token);

		// If the final character of the token is the colon character, then it represents
		// a drive path. Just copy it and ignore.
		if (tokenLen != 0 && token[tokenLen - 1] == L':')
		{
			wcscat_s(out, length, token);
			wcscat_s(truePath, token);
			token = wcstok_s(nullptr, L"\\", &buffer);
			continue;
		}

		// Concatenate working path to truePath to lookup:
		wcscat_s(truePath, L"\\");
		wcscat_s(truePath, token);

		CComPtr<IShellFolder> pShellFolder;

		PIDLIST_ABSOLUTE pidl;
		PCITEMID_CHILD pidlChild;
		hr = SHParseDisplayName(
			truePath,
			nullptr,
			&pidl,
			0,
			nullptr
		);

		if (hr != S_OK)
			goto iteration_cleanup;

		hr = SHBindToParent(
			pidl,
			IID_IShellFolder,
			(void **)&pShellFolder,
			&pidlChild
		);

		if (hr != S_OK)
			return hr;

		STRRET ret;
		hr = pShellFolder->GetDisplayNameOf(
			pidlChild,
			SHGDN_NORMAL,
			&ret
		);

		if (hr != S_OK)
			goto iteration_cleanup;

		WCHAR pszName[MAX_PATH];
		hr = StrRetToBuf(&ret, pidlChild, pszName, MAX_PATH);

		wcscat_s(out, length, L"\\");
		wcscat_s(out, length, pszName);

		// Grab the next token:
		token = wcstok_s(nullptr, L"\\", &buffer);

		i++;

		iteration_cleanup:
		{
			pShellFolder.Release();

			if (hr != S_OK)
			{
				return hr;
			}
		}
	}

	// If we only echoed the drive letter, then place a semantic "\" anyway.
	if (i == 0)
	{
		wcscat_s(out, length, L"\\");
	}

	return hr;
}

/*
 * IsStringPath: Checks if a string may be a path.
 * 
 * This does not check if the string is a valid path. In fact, it only compares
 * the second and third characters and checks if they form the sequence ":\".
 */
bool IsStringPath(const WCHAR *str)
{
	return wcslen(str) != 0 && str[1] == L':' && str[2] == L'\\';
}

} // namespace ShellHelpers