﻿#include "win_def.h"

#include <psapi.h>
#include <winioctl.h>

extern "C" {
	INT WINAPI		SHCreateDirectoryExA(HWND, PCSTR, PSECURITY_ATTRIBUTES);
	INT WINAPI		SHCreateDirectoryExW(HWND, PCWSTR, PSECURITY_ATTRIBUTES);
	BOOL WINAPI		SHGetSpecialFolderPathW(HWND, LPWSTR, int, BOOL);
}

#ifndef CSIDL_WINDOWS
#define CSIDL_WINDOWS 0x0024
#endif
#ifndef CSIDL_SYSTEM
#define CSIDL_SYSTEM 0x0025
#endif

#ifndef MAXIMUM_REPARSE_DATA_BUFFER_SIZE
#define MAXIMUM_REPARSE_DATA_BUFFER_SIZE 16384
#endif
#ifndef IO_REPARSE_TAG_RESERVED_ZERO
#define IO_REPARSE_TAG_RESERVED_ZERO 0
#endif
#ifndef IO_REPARSE_TAG_RESERVED_ONE
#define IO_REPARSE_TAG_RESERVED_ONE 1
#endif
#ifndef IO_REPARSE_TAG_RESERVED_RANGE
#define IO_REPARSE_TAG_RESERVED_RANGE IO_REPARSE_TAG_RESERVED_ONE
#endif
#ifndef IO_REPARSE_TAG_VALID_VALUES
#define IO_REPARSE_TAG_VALID_VALUES 0xE000FFFF
#endif
#ifndef IsReparseTagValid
#define IsReparseTagValid(x) (!((x)&~IO_REPARSE_TAG_VALID_VALUES)&&((x)>IO_REPARSE_TAG_RESERVED_RANGE))
#endif

struct TMN_REPARSE_DATA_BUFFER {
	DWORD	ReparseTag;
	WORD	ReparseDataLength;
	WORD	Reserved;

	// IO_REPARSE_TAG_MOUNT_POINT specifics follow
	WORD	SubstituteNameOffset;
	WORD	SubstituteNameLength;
	WORD	PrintNameOffset;
	WORD	PrintNameLength;
	WCHAR	PathBuffer[1];

	// Some helper functions
	//BOOL	Init(PCSTR szJunctionPoint);
	//BOOL	Init(PCWSTR wszJunctionPoint);
	//int	BytesForIoControl() const;
};


///============================================================================================ path
AutoUTF				Secure(PCWSTR path) {
	AutoUTF	Result(path);
	ReplaceAll(Result, L"..", L"");
	ReplaceAll(Result, L"%", L"");
	return	Validate(Result);
}
AutoUTF				Secure(const AutoUTF& path) {
	return	Secure(path.c_str());
}
AutoUTF				UnExpand(PCWSTR path) {
//	bool	unx = IsPathUnix(path);
//	if (unx)
//		Result.PathWin();
	WCHAR	buf[MAX_PATH_LENGTH];
	if (::PathUnExpandEnvStringsW(path, buf, sizeofa(buf))) {
		return	buf;
	}
//	return	unx ? Result.PathUnix() : Result;
	return	AutoUTF();
}
AutoUTF				UnExpand(const AutoUTF &path) {
	return	UnExpand(path.c_str());
}
AutoUTF				Validate(PCWSTR path) {
	AutoUTF	Result(Canonicalize(Expand(path)));
	ReplaceAll(Result, L"...", L"");
	ReplaceAll(Result, L"\\\\", L"\\");
	if (Result == L"\\")
		Result.clear();
	return	Result;
}
AutoUTF				Validate(const AutoUTF &path) {
	return	Validate(path.c_str());
}

/*
AutoUTF				SlashAddNec(PCWSTR path) {
#ifndef NoStlString
	AutoUTF	Result(path);
	return	Result.SlashAddNec();
#endif
}
AutoUTF				SlashAddNec(const AutoUTF &path) {
	return	SlashAddNec(path.c_str());
}
*/
AutoUTF				SlashDel(PCWSTR path) {
	AutoUTF	Result(path);
	if (!Result.empty()) {
		size_t	pos = Result.size() - 1;
		if (Result.at(pos) == L'\\' || Result.at(pos) == L'/')
			Result.erase(pos);
	}
	return	Result;
}
AutoUTF				SlashDel(const AutoUTF &path) {
	return	SlashDel(path.c_str());
}

bool				IsPathMask(PCWSTR path) {
	PCWSTR	pos = CharLastOf(path, L"?*");
	return	(pos && pos != (path + 2));
}
bool				IsPathMask(const AutoUTF &path) {
	return	IsPathMask(path.c_str());
}
bool				IsPathUnix(PCWSTR path) {
	return	CharFirst(path, L'/') != NULL;
}
bool				IsPathUnix(const AutoUTF &path) {
	return	IsPathUnix(path.c_str());
}

AutoUTF				ExtractFile(PCWSTR path, WCHAR sep) {
	size_t	len = Len(path);
	PWSTR	ch = CharLast((PWSTR)path, PATH_SEPARATOR_C);
	if (ch && ++ch < (path + len)) {
		return	AutoUTF(ch);
	}
	return	L"";
}
AutoUTF				ExtractFile(const AutoUTF &path, WCHAR sep) {
	return	ExtractFile(path.c_str());
}
AutoUTF				ExtractPath(PCWSTR path, WCHAR sep) {
	size_t	len = Len(path);
	PWSTR	ch = CharLast((PWSTR)path, PATH_SEPARATOR_C);
	if (ch && ch < (path + len)) {
		return	AutoUTF(path, ch - path);
	}
	return	L"";
}
AutoUTF				ExtractPath(const AutoUTF &path, WCHAR sep) {
	return	ExtractPath(path.c_str());
}
AutoUTF				GetPathFromMask(PCWSTR mask) {
	AutoUTF	Result = L"\\\\?\\";
	AutoUTF	tmp = mask;
	if (tmp.Find(Result))
		tmp.substr(Result.size());

	size_t	pos = tmp.find_first_of(L"?*");
	if (pos != AutoUTF::npos) {
		tmp.erase(pos);
		pos = tmp.find_last_of(L"\\/");
		if (pos != AutoUTF::npos)
			tmp.erase(pos);
	}
	Result += tmp;
	return	Result;
}
AutoUTF				GetPathFromMask(const AutoUTF &mask) {
	return	GetPathFromMask(mask.c_str());
}
AutoUTF				GetSpecialPath(int csidl, bool create) {
	WCHAR	buf[MAX_PATH];
	if (::SHGetSpecialFolderPathW(NULL, buf, csidl, create))
		return	buf;
	return	AutoUTF();
}

AutoUTF				PathUnix(PCWSTR path) {
	AutoUTF	Result(path);
	return	ReplaceAll(Result, L"\\", L"/");
}
AutoUTF				PathUnix(const AutoUTF &path) {
	return	PathUnix(path.c_str());
}
AutoUTF				PathWin(PCWSTR path) {
	AutoUTF	Result(path);
	return	ReplaceAll(Result, L"/", L"\\");
}
AutoUTF				PathWin(const AutoUTF &path) {
	return	PathWin(path.c_str());
}

AutoUTF				GetWorkDirectory() {
	WCHAR	Result[::GetCurrentDirectory(0, NULL)];
	::GetCurrentDirectoryW(sizeofa(Result), Result);
	return	Result;
}
bool				SetWorkDirectory(PCWSTR path) {
	if (Empty(path))
		return	false;
	return	::SetCurrentDirectoryW(path);
}
bool				SetWorkDirectory(const AutoUTF &path) {
	return	SetWorkDirectory(path.c_str());
}

///========================================================================================= SysPath
namespace	SysPath {
AutoUTF	Winnt() {
	return	SlashAdd(GetSpecialPath(CSIDL_WINDOWS));
}
AutoUTF	Sys32() {
	return	SlashAdd(GetSpecialPath(CSIDL_SYSTEM));
}

AutoUTF	SysNative() {
	AutoUTF	Result = Win64::IsWOW64() ? Validate(AutoUTF(L"%systemroot%\\sysnative\\")) : Validate(AutoUTF(L"%systemroot%\\system32\\"));
	return	SlashAdd(Result);
}
AutoUTF	InetSrv() {
	return	MakePath(Sys32().c_str(), L"inetsrv\\");
}
AutoUTF	Dns() {
	return	MakePath(SysNative().c_str(), L"dns\\");
}

AutoUTF	Users() {
	AutoUTF	Result = Validate(AutoUTF(L"%PUBLIC%\\..\\"));
	if (Result.empty() || (Result == L"\\"))
		Result = Validate(AutoUTF(L"%ALLUSERSPROFILE%\\..\\"));
	return	SlashAdd(Result);
}
AutoUTF	UsersRoot() {
	return	MakePath(Users().c_str(), L"ISPmgrUsers\\");
}
AutoUTF	FtpRoot() {
	return	MakePath(Users().c_str(), L"LocalUser\\");
}

AutoUTF	Temp() {
	return	Validate(AutoUTF(L"%TEMP%\\"));
}

#ifdef ISPMGR
AutoUTF			UserRecycle() {
	AutoUTF	Result(SysPath::UsersRoot());
	Result += L"$Recycle.Bin\\";
	return	Result.SlashAdd();
}
AutoUTF			UserHome(const AutoUTF &name) {
	AutoUTF	Result(SysPath::UsersRoot());
	Result += Sid::AsStr(name.c_str());
	return	Result.SlashAdd();
}
AutoUTF			FtpUserHome(const AutoUTF &name) {
	AutoUTF	Result(SysPath::FtpRoot());
	Result += name;
	return	Result.SlashAdd();
}
#endif
}

///========================================================================================== SysApp
namespace	SysApp {
AutoUTF			appcmd() {
	return	SysPath::InetSrv() + L"appcmd.exe ";
}
AutoUTF			dnscmd() {
	return	SysPath::SysNative() + L"dnscmd.exe ";
}
#ifdef ISPMGR
AutoUTF			openssl() {
	AutoUTF	Result(L"bin\\openssl.exe");
	return	Result;
}
AutoUTF			openssl_conf() {
	AutoUTF	Result(SysPath::UsersRoot());
	Result += L"openssl.cnf";
	return	Result;
}
#endif
}

///===================================================================================== File system
bool				IsDirEmpty(PCWSTR path) {
	return	::PathIsDirectoryEmptyW(path);
}

bool				DirCreate(PCWSTR path) {
	return	::SHCreateDirectoryExW(NULL, path, NULL) == NO_ERROR;
}
bool				FileCreate(PCWSTR path, PCWSTR name, PCSTR content) {
	DWORD	dwBytesToWrite = Len(content);
	DWORD	dwBytesWritten = 0;
	AutoUTF	fpath = MakePath(path, name);
	HANDLE	hFile = ::CreateFileW(fpath.c_str(),
								 GENERIC_WRITE,
								 FILE_SHARE_READ,
								 NULL,
								 CREATE_NEW,
								 FILE_ATTRIBUTE_NORMAL,
								 NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		HRESULT err = ::GetLastError();
		if (err != ERROR_FILE_EXISTS) {
			::WriteFile(hFile, (PCVOID)content, dwBytesToWrite, &dwBytesWritten, NULL);
		}
		::CloseHandle(hFile);
		if (dwBytesToWrite == dwBytesWritten)
			return	true;
	}
	return	false;
}
bool				DirDel(PCWSTR path) {
	::SetFileAttributesW(path, FILE_ATTRIBUTE_NORMAL);
	return	::RemoveDirectoryW(path) != 0;
}
bool				Del2(PCWSTR path) {
	SHFILEOPSTRUCTW sh;

	sh.hwnd = NULL; //Для BCB sh.hwnd=FormX->Handle;
	sh.wFunc = FO_DELETE;
	sh.pFrom = path;
	sh.pTo = NULL;
	sh.fFlags = FOF_NOCONFIRMATION | FOF_SILENT;
	sh.hNameMappings = 0;
	sh.lpszProgressTitle = NULL;
	::SHFileOperationW(&sh);
	return	true;
}
bool				Recycle(PCWSTR path) {
	SHFILEOPSTRUCTW	info = {0};
	info.wFunc	= FO_DELETE;
	info.pFrom	= path;
	info.fFlags	= FOF_ALLOWUNDO | FOF_SILENT | FOF_NOERRORUI | FOF_NOCONFIRMATION;
	return	::SHFileOperationW(&info) == 0;
}

bool				FileCopySecurity(PCWSTR path, PCWSTR dest) {
	return	false;
}

AutoUTF				GetDrives() {
	WCHAR	Result[MAX_PATH] = {0};
	WCHAR	szTemp[::GetLogicalDriveStringsW(0, NULL)];
	if (::GetLogicalDriveStringsW(sizeofa(szTemp), szTemp)) {
		bool	bFound = false;
		WCHAR	*p = szTemp;
		do {
			Cat(Result, p, sizeofa(Result));
			Cat(Result, L";", sizeofa(Result));
			while (*p++);
		} while (!bFound && *p); // end of string
	}
	return	Result;
}

bool				FileRead(PCWSTR	path, CStrA &buf) {
	bool	Result = false;
	HANDLE	hFile = ::CreateFileW(path, GENERIC_READ, 0, NULL, OPEN_EXISTING,
								 FILE_FLAG_OPEN_REPARSE_POINT | FILE_FLAG_BACKUP_SEMANTICS, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD	size = (DWORD)FileSize(hFile);
		buf.reserve(size);
		Result = ::ReadFile(hFile, (PWSTR)buf.c_str(), buf.size(), &size, NULL) != 0;
		::CloseHandle(hFile);
	}
	return	Result;
}
bool				FileWrite(PCWSTR path, PCVOID buf, size_t size, bool rewrite) {
	bool	Result = false;
	DWORD	dwCreationDisposition = rewrite ? CREATE_ALWAYS : CREATE_NEW;
	HANDLE	hFile = ::CreateFileW(path, GENERIC_WRITE, 0, NULL, dwCreationDisposition, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE) {
		DWORD	cbWritten = 0;
		Result = ::WriteFile(hFile, buf, size, &cbWritten, NULL) != 0;
		::CloseHandle(hFile);
	}
	return	Result;
}

///========================================================================================= WinJunc
bool				WinJunc::IsJunc(const AutoUTF &path) {
	if (!::IsJunc(path.c_str()))
		return	false;

	HANDLE	hDir = WinPolicy::Handle(path.c_str());
	if (hDir == INVALID_HANDLE_VALUE)
		return	false;	// Failed to open directory

	BYTE	buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE];
	REPARSE_GUID_DATA_BUFFER *rgdb = (REPARSE_GUID_DATA_BUFFER*) & buf;

	DWORD	dwRet;
	BOOL	br = ::DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT, NULL, 0, rgdb,
								MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL);
	::CloseHandle(hDir);
	return	(br ? (rgdb->ReparseTag == IO_REPARSE_TAG_MOUNT_POINT) : false);
}
bool				WinJunc::Add(const AutoUTF &path, const AutoUTF &dest) {
	if (path.empty() || dest.empty() || !IsExist(dest.c_str())) {
		return	false;
	}
	if (!IsExist(path.c_str())) {
		if (!DirCreate(path.c_str()))
			return	false;
	} else if (!IsDirEmpty(path.c_str()))
		return	false;

	AutoUTF	targ;
	targ.Add(dest, L"\\??\\", false);
//	BYTE buf[MAXIMUM_REPARSE_DATA_BUFFER_SIZE + MAX_PATH * sizeof(WCHAR)] = {0};
	WinBuf<TMN_REPARSE_DATA_BUFFER> buf(MAXIMUM_REPARSE_DATA_BUFFER_SIZE + MAX_PATH_LENGTH * sizeof(WCHAR), true);
//	TMN_REPARSE_DATA_BUFFER &rdb = *(TMN_REPARSE_DATA_BUFFER*)buf;
//	TMN_REPARSE_DATA_BUFFER &rdb = buf.data();

	DWORD nDestMountPointBytes = (targ.size() * sizeof(WCHAR));
	buf->ReparseTag				= IO_REPARSE_TAG_MOUNT_POINT;
	buf->ReparseDataLength		= nDestMountPointBytes + 12;
	buf->SubstituteNameLength	= nDestMountPointBytes;
	buf->PrintNameOffset			= nDestMountPointBytes + 2;
	::wcscpy(buf->PathBuffer, targ.c_str());

	HANDLE hDir = WinPolicy::Handle(path, true);
	if (hDir == INVALID_HANDLE_VALUE)
		return	false;

	DWORD dwTMN_REPARSE_DATA_BUFFER_HEADER_SIZE = FIELD_OFFSET(TMN_REPARSE_DATA_BUFFER, SubstituteNameOffset);
	DWORD dwBytes;
	if (!::DeviceIoControl(hDir, FSCTL_SET_REPARSE_POINT, buf.data(),
						   buf->ReparseDataLength + dwTMN_REPARSE_DATA_BUFFER_HEADER_SIZE,
						   NULL, 0, &dwBytes, 0)) {
		::CloseHandle(hDir);
		::RemoveDirectoryW(path.c_str());
		return	(false);
	}
	::CloseHandle(hDir);
	return	true;
}
bool				WinJunc::Del(const AutoUTF &path) {
	bool Result = false;
	HANDLE hDir = WinPolicy::Handle(path, true);
	if (hDir == INVALID_HANDLE_VALUE) {
		::SetLastError(ERROR_PATH_NOT_FOUND);
		return	Result;
	}

	REPARSE_GUID_DATA_BUFFER rgdb = { 0 };
	rgdb.ReparseTag = IO_REPARSE_TAG_MOUNT_POINT;
	DWORD dwBytes;
	Result = ::DeviceIoControl(hDir, FSCTL_DELETE_REPARSE_POINT, &rgdb,
							   REPARSE_GUID_DATA_BUFFER_HEADER_SIZE, NULL, 0, &dwBytes, 0);
	::CloseHandle(hDir);
	return	Result;
}
AutoUTF				WinJunc::GetDest(const AutoUTF &path) {
	AutoUTF	Result;
	HANDLE	hDir = WinPolicy::Handle(path);
	if (hDir != INVALID_HANDLE_VALUE) {
		WinBuf<TMN_REPARSE_DATA_BUFFER>	buf(MAXIMUM_REPARSE_DATA_BUFFER_SIZE, true);

		DWORD dwRet;
		if (::DeviceIoControl(hDir, FSCTL_GET_REPARSE_POINT, NULL, 0, buf.data(),
							  MAXIMUM_REPARSE_DATA_BUFFER_SIZE, &dwRet, NULL)) {
			if (IsReparseTagValid(buf->ReparseTag)) {
				PCWSTR	pPath = buf->PathBuffer;
				if (::wcsncmp(pPath, L"\\??\\", 4) == 0)
					pPath += 4;
				Result = pPath;
			}
		}
		::CloseHandle(hDir);
	}
	return	Result;
}

///================================================================================================
bool				WinFile::Path(PWSTR path, size_t len) const {
	if (m_hndl && m_hndl != INVALID_HANDLE_VALUE) {
		/*
		// Get the file size.
		DWORD dwFileSizeHi = 0;
		DWORD dwFileSizeLo = GetFileSize(hFile, &dwFileSizeHi);
		if( dwFileSizeLo == 0 && dwFileSizeHi == 0 ) {
			printf("Cannot map a file with a length of zero.\n");
			return FALSE;
		}
		*/
		// Create a file mapping object.
		HANDLE	hFileMap = ::CreateFileMapping(m_hndl, NULL, PAGE_READONLY, 0, 1, NULL);
		if (hFileMap) {
			PVOID	pMem = ::MapViewOfFile(hFileMap, FILE_MAP_READ, 0, 0, 1);
			if (pMem) {
				if (::GetMappedFileNameW(::GetCurrentProcess(), pMem, path, len)) {
					// Translate path with device name to drive letters.
					WCHAR	szTemp[len];
					szTemp[0] = L'\0';
					if (::GetLogicalDriveStringsW(len - 1, szTemp)) {
						WCHAR	szName[MAX_PATH], *p = szTemp;
						WCHAR	szDrive[3] = L" :";
						bool	bFound = false;

						do {
							// Copy the drive letter to the template string
							*szDrive = *p;
							// Look up each device name
							if (::QueryDosDevice(szDrive, szName, sizeofa(szName))) {
								size_t uNameLen = Len(szName);

								if (uNameLen < sizeofa(szName)) {
									bFound = Find(path, szName) == path;
									if (bFound) {
										// Reconstruct pszFilename using szTempFile Replace device path with DOS path
										WCHAR	szTempFile[MAX_PATH];
										_snwprintf(szTempFile, sizeofa(szTempFile), TEXT("%s%s"), szDrive, path + uNameLen);
										Copy(path, szTempFile, len);
									}
								}
							}
							// Go to the next NULL character.
							while (*p++);
						} while (!bFound && *p); // end of string
					}
				}
				::UnmapViewOfFile(pMem);
				return	true;
			}
		}
	}
	return	false;
}

bool				WipeFile(PCWSTR path) {
	{
		DWORD	attr = Attributes(path);
		if (!Attributes(path, FILE_ATTRIBUTE_NORMAL))
			return	false;
		WinFile	WipeFile;
		if (!WipeFile.Open(path, GENERIC_WRITE, FILE_SHARE_DELETE | FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_FLAG_WRITE_THROUGH | FILE_FLAG_SEQUENTIAL_SCAN)) {
			Attributes(path, attr);
			return	false;
		}

		uint64_t	size = 0;
		if (!WipeFile.Size(size)) {
			Attributes(path, attr);
			return	false;
		}
		{
			const uint64_t BufSize = 65536;
			char	*buf[BufSize];
			WinMem::Fill(buf, BufSize, (char)'\0'); // используем символ заполнитель

			DWORD	Written;
			while (size > 0) {
				DWORD	WriteSize = Min(BufSize, size);
				WipeFile.Write(buf, WriteSize, Written);
				size -= WriteSize;
			}
			WipeFile.Write(buf, BufSize, Written);
		}
		WipeFile.Pointer(0, FILE_BEGIN);
		WipeFile.SetEnd();
	}
	AutoUTF	TmpName(TempFile(ExtractPath(path).c_str()));
	if (!FileMove(path, TmpName.c_str(), MOVEFILE_REPLACE_EXISTING))
		return	FileDel(path);
	return	FileDel(TmpName);
}
/*
#define BUFF_SIZE (64*1024)

BOOL WipeFileW(wchar_t *filename) {
	DWORD Error = 0, OldAttr, needed;
	void *SD = NULL;
	int correct_SD = FALSE;
	wchar_t dir[2*MAX_PATH], tmpname[MAX_PATH], *fileptr = wcsrchr(filename, L'\\');
	unsigned char *buffer = (unsigned char *)malloc(BUFF_SIZE);
	if (fileptr && buffer) {
		OldAttr = GetFileAttributesW(filename);
		SetFileAttributesW(filename, OldAttr&(~FILE_ATTRIBUTE_READONLY));
		if (!GetFileSecurityW(filename, DACL_SECURITY_INFORMATION, NULL, 0, &needed))
			if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) {
				SD = malloc(needed);
				if (SD)
					if (GetFileSecurityW(filename, DACL_SECURITY_INFORMATION, SD, needed, &needed)) correct_SD = TRUE;
			}
		wcsncpy(dir, filename, fileptr - filename + 1);
		dir[fileptr-filename+1] = 0;
		if (GetTempFileNameW(dir, L"bc", 0, tmpname)) {
			if (MoveFileExW(filename, tmpname, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
				HANDLE f = CreateFileW(tmpname, FILE_GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
				if (f != INVALID_HANDLE_VALUE) {
					BY_HANDLE_FILE_INFORMATION info;
					if (GetFileInformationByHandle(f, &info)) {
						unsigned long long size = (unsigned long long)info.nFileSizeLow + (unsigned long long)info.nFileSizeHigh * 4294967296ULL;
						unsigned long long processed_size = 0;
						while (size) {
							unsigned long outsize = (unsigned long)((size >= BUFF_SIZE) ? BUFF_SIZE : size), transferred;
							WriteFile(f, buffer, outsize, &transferred, NULL);
							size -= outsize;
							processed_size += outsize;
							if (UpdatePosInfo(0ULL, processed_size)) break;
						}
					}
					if ((SetFilePointer(f, 0, NULL, FILE_BEGIN) == INVALID_SET_FILE_POINTER) || (!SetEndOfFile(f))) Error = GetLastError();
					CloseHandle(f);
				}
				if (Error) MoveFileExW(tmpname, filename, MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH);
				else if (!DeleteFileW(tmpname)) Error = GetLastError();
			} else {
				Error = GetLastError();
				DeleteFileW(tmpname);
			}
		} else Error = GetLastError();
		if (Error) {
			SetFileAttributesW(filename, OldAttr);
			if (correct_SD) SetFileSecurityW(filename, DACL_SECURITY_INFORMATION, SD);
		}
	}
	free(SD);
	free(buffer);
	if (Error) {
		SetLastError(Error);
		return FALSE;
	}
	return TRUE;
}
*/
/*
int			WipeDirectory(const wchar_t *Name) {

	string strTempName, strSavePath(Opt.strTempPath);
	BOOL usePath = FALSE;

	if (FirstSlash(Name)) {
		Opt.strTempPath = Name;
		DeleteEndSlash(Opt.strTempPath);
		CutToSlash(Opt.strTempPath);
		usePath = TRUE;
	}

	FarMkTempEx(strTempName, nullptr, usePath);
	Opt.strTempPath = strSavePath;

	if (!apiMoveFile(Name, strTempName)) {
		SetLastError((_localLastError = GetLastError()));
		return FALSE;
	}

	return apiRemoveDirectory(strTempName);

}
*/
/*
int			DeleteFileWithFolder(const wchar_t *FileName) {
	string strFileOrFolderName;
	strFileOrFolderName = FileName;
	Unquote(strFileOrFolderName);
	BOOL Ret = apiSetFileAttributes(strFileOrFolderName, FILE_ATTRIBUTE_NORMAL);

	if (Ret) {
		if (apiDeleteFile(strFileOrFolderName)) { //BUGBUG
			CutToSlash(strFileOrFolderName, true);
			return apiRemoveDirectory(strFileOrFolderName);
		}
	}

	SetLastError((_localLastError = GetLastError()));
	return FALSE;
}
*/
/*
void		DeleteDirTree(PCWSTR Dir) {
	if (*Dir == 0 ||
			(IsSlash(Dir[0]) && Dir[1] == 0) ||
			(Dir[1] == L':' && IsSlash(Dir[2]) && Dir[3] == 0))
		return;

	wstring strFullName;
	FAR_FIND_DATA_EX FindData;
	ScanTree ScTree(TRUE, TRUE, FALSE);
	ScTree.SetFindPath(Dir, L"*", 0);

	while (ScTree.GetNextName(&FindData, strFullName)) {
		apiSetFileAttributes(strFullName, FILE_ATTRIBUTE_NORMAL);

		if (FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
			if (ScTree.IsDirSearchDone())
				apiRemoveDirectory(strFullName);
		} else
			apiDeleteFile(strFullName);
	}

	apiSetFileAttributes(Dir, FILE_ATTRIBUTE_NORMAL);
	apiRemoveDirectory(Dir);
}
*/
