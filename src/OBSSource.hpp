#pragma once

// OBS Studio includes
#include <obs.h>

namespace MoonlightOBS
{
    /**
     * @brief Handles the OBS studio source.
     * 
     */
    class OBSSource
    {
    public:
        /**
         * @brief Creates the OBS Studio source information object.
         * 
         * @return obs_source_info Returns the OBS source information object.
         */
        static obs_source_info CreateSourceInfo();

        /**
         * @brief Get the OBS Source object.
         * 
         * @return obs_source_t* Returns the OBS source object.
         */
        inline obs_source_t* GetSource() const
        {
            return m_source;
        }

    private:
        /**
         * @brief Construct a new OBSSource object
         * 
         */
        OBSSource(obs_source_t*);
        /**
         * @brief Destroy the OBSSource object
         * 
         */
        ~OBSSource() = default;
        OBSSource(const OBSSource&) = delete; // Disable copy constructor

        // OBS Studio source instance
        obs_source_t* m_source;

        // obs_source_info.get_name callback
        static const char* OnOBSSourceGetName(void* type_data);
        // obs_source_info.create callback
        static void* OnOBSSourceCreate(obs_data_t* settings, obs_source_t* source);
        // obs_source_info.destroy callback
        static void OnOBSSourceDestroy(void* data);
        // obs_source_info.get_properties callback
        static obs_properties_t* OnOBSGetProperties(void* data);
        // obs_source_info.audio_render callback
        static bool OnOBSAudioRender(void* data, uint64_t* ts_out, 
            struct obs_source_audio_mix* audio_output, 
            uint32_t mixers, size_t channels, size_t sample_rate);
        // obs_source_info.get_defaults callback
        static void OnOBSGetDefaults(obs_data_t* settings);
    };
}
