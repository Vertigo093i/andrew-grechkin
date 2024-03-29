﻿/**
	filever: File Version FAR plugin
	Displays version information from file resource in dialog
	FAR3 plugin

	© 2013 Andrew Grechkin

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
**/

#include "fileversion.hpp"

#include <libbase/io.hpp>
#include <libbase/memory.hpp>
#include <libbase/pcstr.hpp>

#include <stdio.h>

#define NTSIGNATURE(a) ((LPVOID)((BYTE *)(a) + ((PIMAGE_DOS_HEADER)(a))->e_lfanew))

using namespace Base;

version_dll & version_dll::inst() {
	static version_dll ret;
	return ret;
}

bool version_dll::is_valid() const {
	return DynamicLibrary::is_valid() && GetFileVersionInfoSizeW && VerLanguageNameW && GetFileVersionInfoW && VerQueryValueW;
}

version_dll::version_dll():
	DynamicLibrary(L"version.dll") {
	GET_DLL_FUNC(GetFileVersionInfoSizeW);
	GET_DLL_FUNC(VerLanguageNameW);
	GET_DLL_FUNC(GetFileVersionInfoW);
	GET_DLL_FUNC(VerQueryValueW);
}

FileVersion::~FileVersion() {
	Memory::free(m_data);
}

FileVersion::FileVersion(PCWSTR path): m_data(nullptr) {
	Memory::zero(this, sizeof(*this));

	DWORD hndl;
	DWORD size = version_dll::inst().GetFileVersionInfoSizeW(path, &hndl);
	if (size && Memory::realloc(m_data, size) && version_dll::inst().GetFileVersionInfoW(path, hndl, size, m_data)) {
		UINT buf_len;
		VS_FIXEDFILEINFO * ffi;
		if (version_dll::inst().VerQueryValueW(m_data, (PWSTR)L"\\", (PCWSTR*)&ffi, &buf_len)) {
			m_MajorVersion = HIWORD(ffi->dwFileVersionMS);
			m_MinorVersion = LOWORD(ffi->dwFileVersionMS);
			m_BuildNumber = HIWORD(ffi->dwFileVersionLS);
			m_RevisionNumber = LOWORD(ffi->dwFileVersionLS);
			_snwprintf(m_ver, lengthof(m_ver), L"%d.%d.%d.%d", m_MajorVersion, m_MinorVersion, m_BuildNumber, m_RevisionNumber);
		}
		struct LANGANDCODEPAGE {
			WORD wLanguage;
			WORD wCodePage;
		} * translate;
		if (version_dll::inst().VerQueryValueW(m_data, (PWSTR)L"\\VarFileInfo\\Translation", (PCWSTR*)&translate, &buf_len)) {
			version_dll::inst().VerLanguageNameW(translate->wLanguage, m_lng, lengthof(m_lng));
			_snwprintf(m_lngId, lengthof(m_lngId), L"%04x%04x", translate->wLanguage, translate->wCodePage);
			WCHAR tmp[4] = {0};
			DWORD err = 0;
			_snwprintf(tmp, lengthof(tmp), L"%04x", translate->wCodePage);
			err = Base::Str::as_uint32(tmp);
			_snwprintf(m_lngIderr, lengthof(m_lngIderr), L"%04x%04x", translate->wLanguage, err);
		}
	} else {
		return;
	}

	file_map_t fmap(path, sizeof(IMAGE_DOS_HEADER));
	if (fmap.is_ok() && (fmap.size() == sizeof(IMAGE_DOS_HEADER))) {
		PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)fmap.data();
		if (dosHeader->e_magic != IMAGE_DOS_SIGNATURE) {
			return;
		}
		PIMAGE_NT_HEADERS pPEHeader = (PIMAGE_NT_HEADERS)NTSIGNATURE(dosHeader);

		if (pPEHeader->Signature != IMAGE_NT_SIGNATURE) {
			return;
		}
		m_machine = pPEHeader->FileHeader.Machine;
		m_flags = pPEHeader->FileHeader.Characteristics;
	}
}

FVI::FVI(const FileVersion & in) {
	FileVerInfo_ tmp[] = {
		{L"", L"FileDescription", MtxtFileDesc},
		{L"", L"LegalCopyright", MtxtFileCopyright},
		{L"", L"Comments", MtxtFileComment},
		{L"", L"CompanyName", MtxtFileCompany},
		{L"", L"FileVersion", MtxtFileVer},
		{L"", L"InternalName", MtxtFileInternal},
		{L"", L"LegalTrademarks", MtxtFileTrade},
		{L"", L"OriginalFilename", MtxtFileOriginal},
		{L"", L"PrivateBuild", MtxtFilePrivate},
		{L"", L"ProductName", MtxtFileProductName},
		{L"", L"ProductVersion", MtxtFileProductVer},
		{L"", L"SpecialBuild", MtxtFileSpecial},
	};
	m_data = tmp;
	m_size = lengthof(tmp);
	if (in.is_ok()) {
		WCHAR QueryString[128] = {0};
		UINT bufLen;
		for (size_t i = 0; i < m_size; ++i) {
			_snwprintf(QueryString, lengthof(QueryString), L"\\StringFileInfo\\%s\\%s", in.lngID(), m_data[i].SubBlock);
			if (!version_dll::inst().VerQueryValueW(in.GetData(), QueryString, &m_data[i].data, &bufLen)) {
				_snwprintf(QueryString, lengthof(QueryString), L"\\StringFileInfo\\%s\\%s", in.lngIDerr(), m_data[i].SubBlock);
				if (!version_dll::inst().VerQueryValueW(in.GetData(), QueryString, &m_data[i].data, &bufLen))
					m_data[i].data = L"";
			}
		}
	}
}
