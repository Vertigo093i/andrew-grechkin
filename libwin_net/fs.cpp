﻿#include "file.h"
#include "reg.h"
#include <libwin_def/priv.h>
#include <libwin_def/link.h>

#include <winioctl.h>

namespace	FileSys {
	HANDLE	HandleRead(PCWSTR path) {
		// Obtain backup/restore privilege in case we don't have it
		Privilege priv(SE_BACKUP_NAME);

		return CheckHandle(::CreateFileW(path, GENERIC_READ, FILE_SHARE_DELETE | FILE_SHARE_READ,
										 nullptr, OPEN_EXISTING,
										 FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
										 nullptr));
	}

	HANDLE	HandleWrite(PCWSTR path) {
		Privilege priv(SE_RESTORE_NAME);

		return CheckHandle(::CreateFileW(path, GENERIC_READ | GENERIC_WRITE, 0, nullptr,
										 OPEN_EXISTING, FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS,
										 nullptr));
	}
}

void copy_file_security(PCWSTR path, PCWSTR dest) {
	WinSDW sd(path);
	SetSecurity(dest, sd, SE_FILE_OBJECT);
}

bool del_by_mask(PCWSTR mask) {
	bool Result = false;
	WIN32_FIND_DATAW wfd;
	HANDLE hFind = ::FindFirstFileW(mask, &wfd);
	if (hFind != INVALID_HANDLE_VALUE) {
		Result = true;
		AutoUTF fullpath = get_path_from_mask(mask);
		do {
			if (!is_valid_filename(wfd.cFileName))
				continue;
			AutoUTF path = MakePath(fullpath, wfd.cFileName);
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
				Result = delete_link(path);
			}
			if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
				del_by_mask(MakePath(path, L"*"));
				Result = delete_dir(path.c_str());
			} else {
				Result = delete_file(path.c_str());
			}
		} while (::FindNextFileW(hFind, &wfd));
		::FindClose(hFind);
	}
	return Result;
}

bool ensure_dir_exist(PCWSTR path, LPSECURITY_ATTRIBUTES lpsa) {
	if (is_exists(path) && is_dir(path))
		return true;
	CheckApiError(::SHCreateDirectoryExW(nullptr, path, lpsa));
	return true;
}

bool remove_dir(PCWSTR path, bool follow_links) {
	bool Result = false;
	if (is_path_mask(path)) {
		Result = del_by_mask(path);
	} else {
		if (!is_exists(path))
			return true;
		if (is_dir(path)) {
			if (!follow_links && is_link(path)) {
				Result = delete_link(path);
			} else {
				del_by_mask(MakePath(path, L"*"));
				Result = delete_dir(path);
			}
		} else {
			Result = delete_file(path);
		}
	}
	return Result;
}

void SetOwnerRecur(const AutoUTF &path, PSID owner, SE_OBJECT_TYPE type) {
	try {
		SetOwner(path, owner, type);
	} catch (...) {
	}
	if (is_dir(path)) {
		WinDir dir(path);
		for (WinDir::iterator it = dir.begin(); it != dir.end(); ++it) {
			if (it.is_dir() || it.is_link_dir()) {
				SetOwnerRecur(it.path(), owner, type);
			} else {
				try {
					SetOwner(it.path(), owner, type);
				} catch (...) {
				}
			}
		}
	}
}

///========================================================================================== WinVol
void WinVol::Close() {
	if (m_hnd != INVALID_HANDLE_VALUE) {
		::FindVolumeClose(m_hnd);
		m_hnd = INVALID_HANDLE_VALUE;
	}
}

bool WinVol::Next() {
	WCHAR buf[MAX_PATH];
	if (m_hnd != INVALID_HANDLE_VALUE) {
		ChkSucc(::FindNextVolumeW(m_hnd, buf, sizeofa(buf)));
	} else {
		m_hnd = ::FindFirstVolumeW(buf, sizeofa(buf));
		ChkSucc(m_hnd != INVALID_HANDLE_VALUE);
	}
	if (IsOK()) {
		name = buf;
	}
	return IsOK();
}

AutoUTF WinVol::GetPath() const {
	AutoUTF Result;
	if (IsOK()) {
		DWORD size;
		::GetVolumePathNamesForVolumeNameW(name.c_str(), nullptr, 0, &size);
		if (::GetLastError() == ERROR_MORE_DATA) {
			auto_array<WCHAR> buf(size);
			::GetVolumePathNamesForVolumeNameW(name.c_str(), buf, size, &size);
			Result = buf.data();
			CutWord(Result, L"\\");
		}
	}
	return Result;
}

AutoUTF WinVol::GetDevice() const {
	auto_array<WCHAR> Result(MAX_PATH);
	::QueryDosDeviceW(GetPath().c_str(), Result, Result.size());
	return AutoUTF(Result);
}

bool WinVol::GetSize(uint64_t &uiUserFree, uint64_t &uiTotalSize, uint64_t &uiTotalFree) const {
	UINT mode = ::SetErrorMode(SEM_FAILCRITICALERRORS);
	bool Result = ::GetDiskFreeSpaceExW(name.c_str(), (PULARGE_INTEGER)&uiUserFree,
	                                    (PULARGE_INTEGER)&uiTotalSize,
	                                    (PULARGE_INTEGER)&uiTotalFree);
	::SetErrorMode(mode);
	return Result;
}

///=================================================================================================
bool FileWipe(PCWSTR path) {
//	{
//		DWORD attr = get_attributes(path);
//		if (!set_attributes(path, FILE_ATTRIBUTE_NORMAL))
//			return false;
//		WinFile WipeFile;
//		if (!WipeFile.Open(path, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ, nullptr,
//		                   OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH | FILE_FLAG_SEQUENTIAL_SCAN)) {
//			set_attributes(path, attr);
//			return false;
//		}
//
//		uint64_t size = WipeFile.size();
//		{
//			const uint64_t BufSize = 65536;
//			char *buf[BufSize];
//			WinMem::Fill(buf, BufSize, (char)'\0'); // используем символ заполнитель
//
//			DWORD Written;
//			while (size > 0) {
//				DWORD WriteSize = std::min(BufSize, size);
//				WipeFile.Write(buf, WriteSize, Written);
//				size -= WriteSize;
//			}
//			WipeFile.Write(buf, BufSize, Written);
//		}
//		WipeFile.Pointer(0, FILE_BEGIN);
//		WipeFile.SetEnd();
//	}
//	AutoUTF TmpName(TempFile(ExtractPath(path).c_str()));
//	if (!move_file(path, TmpName.c_str(), MOVEFILE_REPLACE_EXISTING))
//		return delete_file(path);
//	return delete_file(TmpName);
	return path;
}
