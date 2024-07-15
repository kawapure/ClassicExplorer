#pragma once
#ifndef _SHELL_HELPERS_H
#define _SHELL_HELPERS_H

#include "stdafx.h"
#include "framework.h"
#include "resource.h"
#include "ClassicExplorer_i.h"
#include "dllmain.h"

namespace ShellHelpers
{
	HRESULT GetLocalizedDisplayPath(const WCHAR *path, WCHAR *out, long length);
	bool IsStringPath(const WCHAR *str);
}

#endif // _SHELL_HELPERS_H