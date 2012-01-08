﻿#include "farplugin.hpp"
#include "fileversion.hpp"
#include "lang.hpp"
#include "guid.hpp"

#include <libwin_def/memory.h>

winstd::shared_ptr<FarPlugin> plugin;

NamedValues<WORD> Machines[] = {
	{ L"UNKNOWN", IMAGE_FILE_MACHINE_UNKNOWN },
	{ L"I386", IMAGE_FILE_MACHINE_I386 },
	{ L"R4000", IMAGE_FILE_MACHINE_R4000 },
	{ L"WCEMIPSV2", IMAGE_FILE_MACHINE_WCEMIPSV2 },
	{ L"SH3", IMAGE_FILE_MACHINE_SH3 },
	{ L"SH3DSP", IMAGE_FILE_MACHINE_SH3DSP },
	{ L"SH4", IMAGE_FILE_MACHINE_SH4 },
	{ L"SH5", IMAGE_FILE_MACHINE_SH5 },
	{ L"ARM", IMAGE_FILE_MACHINE_ARM },
	{ L"THUMB", IMAGE_FILE_MACHINE_THUMB },
	{ L"AM33", IMAGE_FILE_MACHINE_AM33 },
	{ L"POWERPC", IMAGE_FILE_MACHINE_POWERPC },
	{ L"POWERPCFP", IMAGE_FILE_MACHINE_POWERPCFP },
	{ L"IA64", IMAGE_FILE_MACHINE_IA64 },
	{ L"MIPS16", IMAGE_FILE_MACHINE_MIPS16 },
	{ L"MIPSFPU", IMAGE_FILE_MACHINE_MIPSFPU },
	{ L"MIPSFPU16", IMAGE_FILE_MACHINE_MIPSFPU16 },
	{ L"EBC", IMAGE_FILE_MACHINE_EBC },
	{ L"AMD64", IMAGE_FILE_MACHINE_AMD64 },
	{ L"M32R", IMAGE_FILE_MACHINE_M32R },
};

ustring make_path(const ustring & path, const ustring & name) {
	return path + PATH_SEPARATOR + name;
}

#ifndef FAR2
GUID FarPlugin::get_guid() {
	return PluginGuid;
}
#endif

PCWSTR FarPlugin::get_prefix() const {
	return L"fver";
}

PCWSTR FarPlugin::get_name() {
	return L"filever";
}

PCWSTR FarPlugin::get_description() {
	return L"Displays version information from file resource";
}

PCWSTR FarPlugin::get_author() {
	return L"© 2012 Andrew Grechkin";
}

FarPlugin::FarPlugin(const PluginStartupInfo * psi) {
#ifndef FAR2
	Far::helper_t::inst().init(PluginGuid, psi);
#else
	Far::helper_t::inst().init(psi);
#endif
}

void FarPlugin::get_info(PluginInfo * pi) const {
	pi->StructSize = sizeof(PluginInfo);
	pi->Flags = 0;
	static PCWSTR PluginMenuStrings[] = {Far::get_msg(Far::MenuTitle)};
#ifndef FAR2
	pi->PluginMenu.Guids = &MenuGuid;
	pi->PluginMenu.Strings = PluginMenuStrings;
	pi->PluginMenu.Count = lengthof(PluginMenuStrings);
#else
	pi->PluginMenuStrings = PluginMenuStrings;
	pi->PluginMenuStringsNumber = lengthof(PluginMenuStrings);
#endif
	pi->CommandPrefix = get_prefix();
}

#ifndef FAR2
HANDLE FarPlugin::open(const OpenInfo * Info) {
	OPENFROM OpenFrom = Info->OpenFrom;
#else
HANDLE FarPlugin::open(int OpenFrom, INT_PTR Item) {
#endif
	if (!version_dll::inst().is_ok()) {
		Far::farebox(L"Can't load version.dll");
		return INVALID_HANDLE_VALUE;
	}

	auto_array<WCHAR> buf(MAX_PATH_LEN);
	if (OpenFrom == OPEN_PLUGINSMENU) {
		Far::Panel pi(PANEL_ACTIVE, FCTL_GETPANELINFO);
		if (pi.IsOK()) {
			PluginPanelItem ppi = pi[pi.CurrentItem()];
#ifndef FAR2
			PCWSTR fileName = ppi.FileName;
#else
			PCWSTR fileName = ppi.FindData.lpwszFileName;
#endif
			if (Find(fileName, PATH_SEPARATOR)) {
				Copy(buf, fileName, buf.size());
			} else {
				Copy(buf, pi.CurDir(), buf.size());
				if (!Empty(buf))
					Far::fsf().AddEndSlash(buf);
				Cat(buf, fileName, buf.size());
			}
		}
	} else if (OpenFrom == OPEN_COMMANDLINE) {
#ifndef FAR2
		Copy(buf, (PCWSTR)Info->Data, buf.size());
#else
		Copy(buf, (PCWSTR)Item, buf.size());
#endif
	}

	Far::fsf().Trim(buf);
	Far::fsf().Unquote(buf);

	FileVersion fv(buf);
	if (fv.IsOK()) {
		FVI fvi(fv);
		if (true) {
			size_t i = 0, x = 70, y = 2;
			Far::InitDialogItemF Items[] = {
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)MtxtFileFullVer},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fv.ver()},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)MtxtFileLang},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fv.lng()},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)MtxtMachine},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, NamedValues<WORD>::GetName(Machines, lengthof(Machines), fv.machine())},
				{DI_TEXT, 5, y++, 0, 0,         DIF_SEPARATOR, L""},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y, 26, 0,          0,            (PCWSTR)fvi[i].msgTxt},
				{DI_EDIT, 28, y++, x - 2, 0,    DIF_READONLY, fvi[i++].data},
				{DI_TEXT, 5, y++, 0, 0,         DIF_SEPARATOR, L""},
				{DI_BUTTON, 0, y++, 0, 0,       DIF_CENTERGROUP, (PCWSTR)Far::txtBtnOk},
				{DI_DOUBLEBOX, 3, 1, x, y,      0,               (PCWSTR)Far::DlgTitle},
			};

			size_t size = lengthof(Items);
			FarDialogItem FarItems[size];
			InitDialogItemsF(Items, FarItems, size);
#ifndef FAR2
			HANDLE hndl = Far::psi().DialogInit(Far::guid(), &DialogGuid, -1, -1, x + 4, y + 2, L"Contents", FarItems, size, 0, 0, nullptr, 0);
#else
			HANDLE hndl = Far::psi().DialogInit(Far::psi().ModuleNumber, -1, -1, x + 4, y + 2, L"Contents", FarItems, size, 0, 0, nullptr, 0);
#endif
			Far::psi().DialogRun(hndl);
			Far::psi().DialogFree(hndl);
		}
	}
	return INVALID_HANDLE_VALUE;
}
