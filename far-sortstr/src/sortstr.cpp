﻿/**
	sortstr: Sort strings in editor
	FAR2, FAR3 plugin

	© 2012 Andrew Grechkin

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

#include "farplugin.hpp"
#include "version.h"

///========================================================================================== Export
#ifndef FAR2
void WINAPI GetGlobalInfoW(GlobalInfo *Info)
{
	using namespace AutoVersion;
	Info->StructSize = sizeof(GlobalInfo);
	Info->MinFarVersion = FARMANAGERVERSION;
	Info->Version = MAKEFARVERSION(MAJOR,MINOR,REVISION,BUILD,VS_RELEASE);
	Info->Guid = FarPlugin::get_guid();
	Info->Title = FarPlugin::get_name();
	Info->Description = FarPlugin::get_description();
	Info->Author = FarPlugin::get_author();
}
#endif

void WINAPI SetStartupInfoW(const PluginStartupInfo * psi) {
	plugin.reset(new FarPlugin(psi));
}

void WINAPI GetPluginInfoW(PluginInfo * pi) {
	plugin->get_info(pi);
}

#ifndef FAR2
HANDLE WINAPI OpenW(const OpenInfo * Info) {
	return plugin->open(Info);
}
#else
HANDLE WINAPI OpenPluginW(int OpenFrom, INT_PTR Item) {
	return plugin->open(OpenFrom, Item);
}
#endif
