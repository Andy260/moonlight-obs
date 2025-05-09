#include "OBSSource.hpp"

// STL includes
#include <new>
#include <stdexcept>

// OBS Studio includes
#include <obs.h>
#include <obs-module.h>
#include <obs-source.h>
#include <util/base.h>

// Project includes
#include "plugin-support.h"
#include "Properties.hpp"

using namespace MoonlightOBS;

// Creates the OBS Studio source information object.
obs_source_info OBSSource::CreateSourceInfo()
{
    obs_source_info source_info = {};
    source_info.id              = "moonlight_source";
    source_info.type            = OBS_SOURCE_TYPE_INPUT;
    source_info.output_flags    = OBS_SOURCE_ASYNC_VIDEO | OBS_SOURCE_AUDIO | OBS_SOURCE_DO_NOT_DUPLICATE;
    source_info.icon_type       = OBS_ICON_TYPE_GAME_CAPTURE;
    source_info.get_name        = OnOBSSourceGetName;
    source_info.create          = OnOBSSourceCreate;
    source_info.destroy         = OnOBSSourceDestroy;
    source_info.get_properties  = OnOBSGetProperties;
    source_info.audio_render    = OnOBSAudioRender;

    return source_info;
}

// Construct a new OBSSource object
OBSSource::OBSSource(obs_source_t* source)
    : m_source(source)
{
    if (m_source == nullptr)
    {
        // Handle null source
        obs_log(LOG_ERROR, "Failed to initialize OBSSource: source is null");
        throw std::runtime_error("Source is null");
    }
}

// obs_source_info.get_name callback
const char* OBSSource::OnOBSSourceGetName(void* type_data)
{
    UNUSED_PARAMETER(type_data);
    return obs_module_text("MoonlightCapture");
}

// obs_source_info.create callback
void* OBSSource::OnOBSSourceCreate(obs_data_t* settings, obs_source_t* source)
{
    void* memory = bzalloc(sizeof(OBSSource));
    if (memory == nullptr)
    {
        // Handle memory allocation failure
        obs_log(LOG_ERROR, "Failed to allocate memory for source");
        return nullptr;
    }

    // Initialize the source instance
    // new (memory) OBSSource(); constructs the OBSSource object in the allocated memory, 
    // but requires the <new> STL header
    OBSSource* context = new (memory) OBSSource(source);

    UNUSED_PARAMETER(settings);
    UNUSED_PARAMETER(source);
    return context;
}

// obs_source_info.destroy callback
void OBSSource::OnOBSSourceDestroy(void* data)
{
    OBSSource* context = static_cast<OBSSource*>(data);
    if (context != nullptr) 
    {
        // Explicitly call the destructor as this won't be called
        // since the memory was allocated with bzalloc
        context->~OBSSource();

        // Clean up the memory
        bfree(context);
    }

    UNUSED_PARAMETER(data);
}

// obs_source_info.get_properties callback
obs_properties_t* OBSSource::OnOBSGetProperties(void* data)
{
    // Create the source properties
    MoonlightOBS::Properties properties;

    // TODO: Set up the properties here

    UNUSED_PARAMETER(data);
    return properties.GetHandle();
}

// obs_source_info.audio_render callback
bool OBSSource::OnOBSAudioRender(void* data, uint64_t* ts_out, 
    struct obs_source_audio_mix* audio_output, 
    uint32_t mixers, size_t channels, size_t sample_rate)
{
    // Implement audio rendering logic here if needed

    UNUSED_PARAMETER(data);
    UNUSED_PARAMETER(ts_out);
    UNUSED_PARAMETER(audio_output);
    UNUSED_PARAMETER(mixers);
    UNUSED_PARAMETER(channels);
    UNUSED_PARAMETER(sample_rate);
    return true; // Return true to indicate successful audio rendering
}
