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

#include "moonlight-source.hpp"

// STL includes
#include <stdlib.h>

// OBS Studio includes
#include <util/threading.h>
#include <util/platform.h>
#include <obs.hpp>

// Moonlight Common includes
#include <Limelight.h>

// Project includes
#include "plugin-support.h"

struct MoonlightSource
{
    obs_source_t* source;
};

static const char* MoonlightGetName(void* type_data)
{
    UNUSED_PARAMETER(type_data);
    return obs_module_text("MoonlightCapture");
}

static void* MoonlightCreateSource(obs_data_t* settings, obs_source_t* source)
{
    struct MoonlightSource* context = static_cast<MoonlightSource*>(bzalloc(sizeof(struct MoonlightSource)));
    context->source = source;

    return context;
}

static obs_properties_t* MoonlightGetProperties(void* data)
{
    obs_properties_t* props = obs_properties_create();

    // Add a combo box (drop-down list) for selecting the host
    obs_property_t* connectionCombo = obs_properties_add_list(
        props,
        "host",                 // Internal name of the property
        "Paired Device",        // Label displayed in the UI
        OBS_COMBO_TYPE_LIST,    // Combo box type
        OBS_COMBO_FORMAT_STRING // Format as strings
    );
    // Add options to the connection combo box
    // TODO: Retrieve the list of paired devices and add them to the combo box

    // Add a button to connect to the selected host
    obs_properties_add_button(
        props,          
        "connect_button",   // Internal name of the button
        "Connect",          // Label displayed on the button
        nullptr             // Callback function for button click
    );

    UNUSED_PARAMETER(data);
    return props;
}

static bool MoonlightAudioRender(void* data, uint64_t* ts_out, 
    struct obs_source_audio_mix* audio_output, 
    uint32_t mixers, size_t channels, size_t sample_rate)
{
    // Implement audio rendering logic here if needed
    return true; // Return true to indicate successful audio rendering
}

static void MoonlightDestroySource(void* data)
{
    
}

static obs_source_info CreateSourceInfo()
{
    obs_source_info source_info = {};
    source_info.id              = "moonlight_source";
    source_info.type            = OBS_SOURCE_TYPE_INPUT;
    source_info.output_flags    = OBS_SOURCE_ASYNC_VIDEO | OBS_SOURCE_AUDIO | OBS_SOURCE_DO_NOT_DUPLICATE;
    source_info.icon_type       = OBS_ICON_TYPE_GAME_CAPTURE;
    source_info.get_name        = MoonlightGetName;
    source_info.create          = MoonlightCreateSource;
    source_info.destroy         = MoonlightDestroySource;
    source_info.get_properties  = MoonlightGetProperties;
    source_info.audio_render    = MoonlightAudioRender;

    return source_info;
}
struct obs_source_info moonlight_source_info = CreateSourceInfo();
