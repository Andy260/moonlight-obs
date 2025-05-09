/*
Moonlight OBS
Copyright (C) 2025 Andrew Barbour andrew.barbour1@gmail.com

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program. If not, see <https://www.gnu.org/licenses/>
*/

// OBS Studio headers
#include <obs-module.h>

// Plugin headers
#include <plugin-support.h>
//#include "moonlight-source.hpp"
#include "OBSSource.hpp"

using namespace MoonlightOBS;

OBS_DECLARE_MODULE()
OBS_MODULE_USE_DEFAULT_LOCALE(PLUGIN_NAME, "en-US")

// Since OBS Studio is C-based and requires a pointer to the source information object
// we'll have to declare it within the extern "C" block outside of the class
extern "C"
{
    // Source information for the Moonlight source.
    extern struct obs_source_info moonlight_source_info = OBSSource::CreateSourceInfo();
}

bool obs_module_load(void)
{
	// Register the source
	obs_register_source(&moonlight_source_info);

	obs_log(LOG_INFO, "plugin loaded successfully (version %s)", PLUGIN_VERSION);
	return true;
}

void obs_module_unload(void)
{
	// TODO: Disconnect from any connected paired devices
	obs_log(LOG_INFO, "plugin unloaded");
}

const char* obs_module_name(void)
{
	return PLUGIN_NAME;
}

const char* obs_module_description(void)
{
	return "Unofficial OBS Studio implementation of Moonlight";
}
