﻿#include "win_def.h"

astring& Trim_l(astring &str, const astring &chrs) {
	astring::size_type pos = str.find_first_not_of(chrs);
	if (pos && pos != astring::npos) {
		str.erase(0, pos);
	}
	return str;
}

astring& Trim_r(astring &str, const astring &chrs) {
	astring::size_type pos = str.find_last_not_of(chrs);
	if (pos != astring::npos && (++pos < str.size())) {
		str.erase(pos);
	}
	return str;
}

astring& Trim(astring &str, const astring &chrs) {
	Trim_r(str, chrs);
	Trim_l(str, chrs);
	return str;
}

astring TrimOut(const astring &str, const astring &chrs) {
	astring tmp(str);
	return Trim(tmp, chrs);
}

AutoUTF& Trim_l(AutoUTF &str, const AutoUTF &chrs) {
	AutoUTF::size_type pos = str.find_first_not_of(chrs);
	if (pos && pos != AutoUTF::npos) {
		str.erase(0, pos);
	}
	return str;
}

AutoUTF& Trim_r(AutoUTF &str, const AutoUTF &chrs) {
	AutoUTF::size_type pos = str.find_last_not_of(chrs);
	if (pos != AutoUTF::npos && (++pos < str.size())) {
		str.erase(pos);
	}
	return str;
}

AutoUTF& Trim(AutoUTF &str, const AutoUTF &chrs) {
	Trim_r(str, chrs);
	Trim_l(str, chrs);
	return str;
}

AutoUTF TrimOut(const AutoUTF &str, const AutoUTF &chrs) {
	AutoUTF tmp(str);
	return Trim(tmp, chrs);
}

AutoUTF GetWord(const AutoUTF &str, WCHAR d) {
	AutoUTF::size_type pos = str.find(d);
	if (pos != AutoUTF::npos)
		return str.substr(0, pos);
	return str;
}

astring& AddWord(astring &inout, const astring &add, const astring &delim) {
	astring::size_type pos = inout.size() - delim.size();
	if (!(delim.empty() || inout.empty() || (inout.rfind(delim) == pos) || (add.find(delim) == 0)))
		inout += delim;
	if (!add.empty())
		inout += add;
	return inout;
}

AutoUTF& AddWord(AutoUTF &inout, const AutoUTF &add, const AutoUTF &delim) {
	// добаваляет строку через разделитель кроме случаев
	// 1) исходная строка пуста
	// 2) если разделитель есть в конце исходной строки
	// 3) если разделитель есть в начале добавляемой
	AutoUTF::size_type pos = inout.size() - delim.size();
	if (!(delim.empty() || inout.empty() || (inout.rfind(delim) == pos) || (add.find(delim) == 0)))
		inout += delim;
	if (!add.empty())
		inout += add;
	return inout;
}

astring& AddWordEx(astring &inout, const astring &add, const astring &delim) {
	astring::size_type pos = inout.size() - delim.size();
	if (!(add.empty() || delim.empty() || inout.empty() || (inout.rfind(delim) == pos)
	    || (add.find(delim) == 0)))
		inout += delim;
	if (!add.empty())
		inout += add;
	return inout;
}

AutoUTF& AddWordEx(AutoUTF &inout, const AutoUTF &add, const AutoUTF &delim) {
	// добаваляет строку через разделитель кроме случаев
	// 1) исходная строка пуста
	// 2) если добавляемая строка пуста
	// 3) если разделитель есть в конце исходной строки
	// 4) если разделитель есть в начале добавляемой
	AutoUTF::size_type pos = inout.size() - delim.size();
	if (!(add.empty() || delim.empty() || inout.empty() || (inout.rfind(delim) == pos)
	    || (add.find(delim) == 0)))
		inout += delim;
	if (!add.empty())
		inout += add;
	return inout;
}

astring CutWord(astring &inout, const astring &delim, bool delDelim) {
	astring::size_type pos = inout.find_first_of(delim);
	astring Result(inout.substr(0, pos));
	if (delDelim && pos != astring::npos)
	//	pos = inout.find_first_not_of(delim, pos);
		++pos;
	inout.erase(0, pos);
	Trim_l(inout);
	return Trim(Result);
}

AutoUTF CutWord(AutoUTF &inout, const AutoUTF &delim, bool delDelim) {
	AutoUTF::size_type pos = inout.find_first_of(delim);
	AutoUTF Result(inout.substr(0, pos));
	if (delDelim && pos != AutoUTF::npos) {
		//		pos = inout.find_first_not_of(delim, pos);
		++pos;
	}
	inout.erase(0, pos);
	return Trim(Result);
}

astring CutWordEx(astring &inout, const astring &delim, bool delDelim) {
	astring::size_type pos = inout.find(delim);
	astring Result = inout.substr(0, pos);
	if (delDelim && pos != astring::npos)
		pos += delim.size();
	inout.erase(0, pos);
	return Trim(Result);
}

AutoUTF CutWordEx(AutoUTF &inout, const AutoUTF &delim, bool delDelim) {
	AutoUTF::size_type pos = inout.find(delim);
	AutoUTF Result = inout.substr(0, pos);
	if (delDelim && pos != AutoUTF::npos)
		pos += delim.size();
	inout.erase(0, pos);
	return Trim(Result);
}

AutoUTF& ReplaceAll(AutoUTF& str, const AutoUTF &from, const AutoUTF &to) {
	AutoUTF::size_type pos;
	while ((pos = str.find(from)) != AutoUTF::npos) {
		str.replace(pos, from.size(), to);
	}
	return str;
}

AutoUTF ReplaceAllOut(const AutoUTF& str, const AutoUTF &from, const AutoUTF &to) {
	AutoUTF Result(str);
	return ReplaceAll(Result, from, to);
}

UINT				CheckUnicode(const PVOID buf, size_t size) {
	int test = 0xFFFF;
	if (::IsTextUnicode(buf, (int)size, &test)) {
		if (test & IS_TEXT_UNICODE_UNICODE_MASK) {
			return	CP_UTF16le;
		} else if (test & IS_TEXT_UNICODE_REVERSE_MASK) {
			return	CP_UTF16be;
		}
		/*
						if (!(test&IS_TEXT_UNICODE_ODD_LENGTH) && !(test&IS_TEXT_UNICODE_ILLEGAL_CHARS)) {
							if ((test&IS_TEXT_UNICODE_NULL_BYTES) ||
									(test&IS_TEXT_UNICODE_CONTROLS) ||
									(test&IS_TEXT_UNICODE_REVERSE_CONTROLS)) {
								if ((test&IS_TEXT_UNICODE_CONTROLS) || (test&IS_TEXT_UNICODE_STATISTICS)) {
									nCodePage = CP_UTF16le;
									bDetect = true;
								} else if ((test&IS_TEXT_UNICODE_REVERSE_CONTROLS) || (test&IS_TEXT_UNICODE_REVERSE_STATISTICS)) {
									nCodePage = CP_UTF16be;
									bDetect = true;
								}
							}
						}
		*/
	}
	return	0;
}
UINT				IsUTF8(const PVOID buf, size_t size) {
	bool	Ascii = true;
	UINT	Octets = 0;
	for (size_t i = 0; i < size; ++i) {
		BYTE c = ((PBYTE)buf)[i];
		if (c & 0x80)
			Ascii = 0;
		if (Octets) {
			if ((c & 0xC0) != 0x80)
				return	0;
			Octets--;
		} else {
			if (c & 0x80) {
				while (c & 0x80) {
					c <<= 1;
					Octets++;
				}
				Octets--;
				if (!Octets)
					return	0;
			}
		}
	}
	return	(Octets > 0 || Ascii) ? 0 : CP_UTF8;
}
bool				GetCP(HANDLE hFile, UINT &cp, bool bUseHeuristics) {
	DWORD	dwTemp = 0;
	DWORD	size = 0;

	::SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
	if (::ReadFile(hFile, &dwTemp, sizeof(dwTemp), &size, nullptr)) {
		if (LOWORD(dwTemp) == BOM_UTF16le) {
			::SetFilePointer(hFile, 2, nullptr, FILE_BEGIN);
			cp = CP_UTF16le;
			return	true;
		} else if (LOWORD(dwTemp) == BOM_UTF16be) {
			::SetFilePointer(hFile, 2, nullptr, FILE_BEGIN);
			cp = CP_UTF16be;
			return	true;
		} else if ((dwTemp & 0x00FFFFFF) == BOM_UTF8) {
			::SetFilePointer(hFile, 3, nullptr, FILE_BEGIN);
			cp = CP_UTF8;
			return	true;
		} else if (dwTemp == BOM_UTF32le) {
			cp = CP_UTF32le;
			return	true;
		} else if (dwTemp == BOM_UTF32be) {
			cp = CP_UTF32be;
			return	true;
		} else {
			::SetFilePointer(hFile, 0, nullptr, FILE_BEGIN);
		}
	}

	UINT	nCodePage = 0;
	if (bUseHeuristics) {
		size = 0x8000; // BUGBUG. TODO: configurable
		PBYTE 	buf;
		if (WinMem::Alloc(buf, size) && read_file(hFile, buf, size)) {
			UINT	cp = CheckUnicode(buf, size);
			if (cp) {
				nCodePage = cp;
			} else if (IsUTF8(buf, size)) {
				nCodePage = CP_UTF8;
			} else {
				/*
				nsUniversalDetectorEx *ns = new nsUniversalDetectorEx();

				ns->HandleData((const char*)Buffer, (PRUint32)sz);
				ns->DataEnd();

				int cp = ns->getCodePage();

				if (cp != -1) {
					nCodePage = cp;
					bDetect = true;
				}

				delete ns;
				*/
			}
			WinMem::Free(buf);
		}
	}
	return	nCodePage;
}

///====================================================================== Функции работы со строками
/*
PWSTR				CharFirstOf(PCWSTR in, PCWSTR mask) {
	size_t	lin = Len(in);
	size_t	len = Len(mask);
	for (size_t i = 0; i < lin; ++i) {
		for (size_t j = 0; j < len; ++j) {
			if (in[i] == mask[j])
				return	(PWSTR)&in[i];
		}
	}
	return	nullptr;
}
PWSTR				CharFirstNotOf(PCWSTR in, PCWSTR mask) {
	size_t	lin = Len(in);
	size_t	len = Len(mask);
	for (size_t i = 0; i < lin; ++i) {
		for (size_t j = 0; j < len; ++j) {
			if (in[i] == mask[j])
				break;
			if (j == len - 1)
				return	(PWSTR)&in[i];
		}
	}
	return	nullptr;
}
*/

astring				Hash2Str(const PBYTE hash, size_t size) {
	CHAR	buf[(size + 1) * 2];
	PSTR	tmp = buf;
	for (size_t i = 0; i < size; ++i) {
		snprintf(tmp, sizeofa(buf) - i * 2, "%02x", hash[i]);
		tmp += 2;
	}
	return	buf;
}

astring				Hash2StrNum(const PBYTE hash, size_t size) {
	CHAR	buf[(size + 1) * 4];
	PSTR	tmp = buf;
	for (size_t i = 0; i < size; ++i) {
		tmp += snprintf(tmp, sizeofa(buf) - i * 2, "%02i ", hash[i]);
	}
	return	buf;
}

bool				Str2Hash(const astring &str, PVOID &hash, ULONG &size) {
	size_t strsize = str.size();
	if (strsize % 2 == 0) {
		size = strsize / 2;
		if (WinMem::Alloc(hash, size)) {
			for (size_t i = 0; i < size; ++i) {
				astring	tmp = str.substr(i * 2, 2);
				((PBYTE)hash)[i] = (BYTE)AsInt(tmp.c_str(), 16);
			}
			return	true;
		}
	}
	return	false;
}

AutoUTF		AsStr(const SYSTEMTIME &in, bool tolocal) {
	SYSTEMTIME	stTime;
	if (tolocal) {
		::SystemTimeToTzSpecificLocalTime(nullptr, (SYSTEMTIME*)&in, &stTime);
	} else {
		stTime = in;
	}
	WCHAR	buf[MAX_PATH];
	_snwprintf(buf, sizeofa(buf), L"%04d-%02d-%02d %02d:%02d:%02d",
			   stTime.wYear, stTime.wMonth, stTime.wDay,
			   stTime.wHour, stTime.wMinute, stTime.wSecond);
	return	buf;
}

AutoUTF		AsStr(const FILETIME &in) {
	SYSTEMTIME	stUTC;
	::FileTimeToSystemTime(&in, &stUTC);
	return	AsStr(stUTC);
}

AutoUTF		CopyAfterLast(const AutoUTF &in, const AutoUTF &delim) {
	AutoUTF	Result;
	AutoUTF::size_type pos = in.find_last_of(delim);
	if (pos != AutoUTF::npos) {
		Result = in.substr(pos + 1);
	}
	return	Result;
}

AutoUTF&	Cut(AutoUTF &inout, const AutoUTF &in) {
	AutoUTF::size_type pos = inout.find(in);
	if (pos != AutoUTF::npos) {
		inout.erase(pos, in.size());
	}
	return	inout;
}

bool		Cut(AutoUTF &inout, intmax_t &num, int base) {
	return	inout.Cut(num, base);
}

AutoUTF&	CutAfter(AutoUTF &inout, const AutoUTF &delim) {
	AutoUTF::size_type pos = inout.find_first_of(delim);
	if (pos != AutoUTF::npos) {
		inout.erase(pos);
	}
	return	inout;
}

AutoUTF&	CutBefore(AutoUTF &inout, const AutoUTF &delim) {
	size_t	pos = inout.find_first_of(delim);
	if (pos != 0) {
		inout.erase(0, pos);
	}
	return	inout;
}

AutoUTF&	ToLower(AutoUTF &inout) {
	if (!inout.empty())
		::CharLowerW((WCHAR*)inout.c_str());
	return	inout;
}

AutoUTF		ToLowerOut(const AutoUTF &in) {
	AutoUTF	tmp(in);
	return	ToLower(tmp);
}

AutoUTF&	ToUpper(AutoUTF &inout) {
	if (!inout.empty())
		::CharUpperW((WCHAR*)inout.c_str());
	return	inout;
}

AutoUTF		ToUpperOut(const AutoUTF &in) {
	AutoUTF	tmp(in);
	return	ToUpper(tmp);
}
