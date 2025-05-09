#pragma once

// OBS Studio includes
// (forward declarations can't be used since OBS Studio's type are typedefs)
#include <obs-properties.h>
#include <obs-data.h>

namespace MoonlightOBS
{
    /**
     * @brief Handles the properties of the moonlight source.
     * 
     */
    class Properties
    {
    public:
        /**
         * @brief Construct a new Properties object
         * 
         */
        Properties();
        /**
         * @brief Destroy the Properties object
         * 
         */
        ~Properties() = default;

        /**
         * @brief Get the OBS properties handle.
         * 
         * @return obs_properties_t* Returns the handle to the properties.
         */
        inline obs_properties_t* GetHandle() const
        {
            return m_handle;
        }
        
    private:
        // Pointer to the OBS properties handle
        obs_properties_t* m_handle;

        // "Device" combo box
        obs_property_t* m_hostList;
        static obs_property_t* CreateHostListProperty(obs_properties_t* props);

        // Connection status text property
        obs_property_t* m_connectionStatus;
        static obs_property_t* CreateConnectionStatusProperty(obs_properties_t* props);

        // "Connect" button
        obs_property_t* m_connectButton;
        static obs_property_t* CreateConnectButton(obs_properties_t* props);
        static bool OnConnectButtonPressed(obs_properties_t *props, obs_property_t *property, void *data);

        // "Pair New Device" button
        obs_property_t* m_pairButton;
        static obs_property_t* CreatePairButton(obs_properties_t* props);
        static bool OnPairButtonPressed(obs_properties_t *props, obs_property_t *property, void *data);

        // "Unpair Device" button
        obs_property_t* m_removeButton;
        static obs_property_t* CreateRemoveButton(obs_properties_t* props);
        static bool OnRemoveButtonPressed(obs_properties_t *props, obs_property_t *property, void *data);

        // "Automatically reconnect" checkbox
        obs_property_t* m_reconnectCheckbox;
        static obs_property_t* CreateReconnectCheckbox(obs_properties_t* props);

        // "BitRate (Mbps)" slider
        obs_property_t* m_bitRateSlider;
        static obs_property_t* CreateBitRateSlider(obs_properties_t* props);

        // "Resolution/FPS Type" combo box
        obs_property_t* m_displayTypeList;
        static obs_property_t* CreateDisplayTypeList(obs_properties_t* props);
        static bool OnDisplayTypeChanged(obs_properties_t *props, obs_property_t* property, obs_data_t* settings);

        // "Resolution" combo box
        obs_property_t* m_resolutionList;
        static obs_property_t* CreateResolutionList(obs_properties_t* props);

        // "FPS" combo box
        obs_property_t* m_fpsList;
        static obs_property_t* CreateFPSList(obs_properties_t* props);

        // "Use hardware decoding when available" checkbox
        obs_property_t* m_hardwareDecodingCheckbox;
        static obs_property_t* CreateHardwareDecodingCheckbox(obs_properties_t* props);

        // "Audio Output Mode" combo box
        obs_property_t* m_audioModeList;
        static obs_property_t* CreateAudioModeList(obs_properties_t* props);
    };
} // namespace MoonlightOBS
