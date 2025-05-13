#include "Properties.hpp"

// STL includes
#include <cassert>
#include <string>

// OBS Studio includes
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <obs-properties.h>

// Qt includes
#include <QtWidgets>

// Project includes
#include "Forms/FindHostsDialog.hpp"
#include "OBSSource.hpp"

using namespace MoonlightOBS;

Properties::Properties()
{
    // Create the properties handle
    m_handle = obs_properties_create();

    // Create the properties
    m_hostList                  = CreateHostListProperty(m_handle);
    m_connectionStatus          = CreateConnectionStatusProperty(m_handle);
    m_connectButton             = CreateConnectButton(m_handle);
    m_pairButton                = CreatePairButton(m_handle);
    m_removeButton              = CreateRemoveButton(m_handle);
    m_reconnectCheckbox         = CreateReconnectCheckbox(m_handle);
    m_bitRateSlider             = CreateBitRateSlider(m_handle);
    m_displayTypeList           = CreateDisplayTypeList(m_handle);
    m_resolutionList            = CreateResolutionList(m_handle);
    m_fpsList                   = CreateFPSList(m_handle);
    m_hardwareDecodingCheckbox  = CreateHardwareDecodingCheckbox(m_handle);
    m_audioModeList             = CreateAudioModeList(m_handle);

    // Initially disable the resolution and FPS properties
    // HACK: This is a workaround for the fact that the internal logic of the plugin isn't implemented yet
    obs_property_set_enabled(m_resolutionList, false);
    obs_property_set_enabled(m_fpsList, false);
}

obs_property_t* Properties::CreateHostListProperty(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the combo box for selecting the host
    obs_property_t* property = obs_properties_add_list(
        props,                      // Internal name of the property
        "host",                     // Label displayed in the UI 
        obs_module_text("Device"),  // Label displayed in the UI
        OBS_COMBO_TYPE_LIST,        // Combo box type
        OBS_COMBO_FORMAT_STRING     // Format as strings
    );

    // TODO: Retrieve the list of paired devices and add them to the combo box

    return property;
}

obs_property_t* Properties::CreateConnectionStatusProperty(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create connection status text
    obs_property_t* property = obs_properties_add_text(
        props,                                              // Internal name of the property
        "connectionText",                                   // Label displayed in the UI
        obs_module_text("ConnectionStatus.Disconnected"),   // Label displayed in the UI
        OBS_TEXT_INFO                                       // Text type
    );

    // Set the text type to error
    obs_property_text_set_info_type(property, OBS_TEXT_INFO_ERROR);

    // TODO: Retrieve the connection status and set the text accordingly

    return property;
}

obs_property_t* Properties::CreateConnectButton(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "Connect" button
    obs_property_t* button = obs_properties_add_button(
        props,                              // Internal name of the property
        "connect_device",                   // Label displayed in the UI
        obs_module_text("Device.Connect"),  // Label displayed in the UI
        [](obs_properties_t* props, obs_property_t* property, void* data) -> bool
        {
            return Properties::OnConnectButtonPressed(props, property, static_cast<OBSSource*>(data));
        } // Callback function for button click
    );

    return button;
}

bool Properties::OnConnectButtonPressed(obs_properties_t* props, obs_property_t* property, OBSSource* source)
{
    // Get the current settings of the source instance
    obs_data_t* settings = obs_source_get_settings(source->GetSource());

    // Get the currently selected value from the "host" list property
    std::string selectedHost(obs_data_get_string(settings, "host"));

    if (!selectedHost.empty())
    {
        // TODO: Implement the connection logic using the selected host
    }
    else
    {
        // No host selected, show an error message
        DisplayMessageBox("Error", obs_module_text("Device.NoDevice"));
    }

    // TODO: Implement the "Connect" button

    UNUSED_PARAMETER(props);
    UNUSED_PARAMETER(property);
    UNUSED_PARAMETER(source);
    
    // Don't repaint the UI
    return false;
}

obs_property_t* Properties::CreatePairButton(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "Pair New Device" button
    obs_property_t* button = obs_properties_add_button(
        props,          
        "add_device",                       // Internal name of the button
        obs_module_text("Device.Pair"),  // Label displayed on the button
        [](obs_properties_t* props, obs_property_t* property, void* data) -> bool
        {
            return Properties::OnPairButtonPressed(props, property, static_cast<OBSSource*>(data));
        } // Callback function for button click
    );

    return button;
}

bool Properties::OnPairButtonPressed(obs_properties_t* props, obs_property_t* property, OBSSource* source)
{
    // Display the Pairing dialog
    QWidget* mainWindow = static_cast<QWidget*>(obs_frontend_get_main_window());
    FindHostsDialog dialog(mainWindow);
    if (dialog.exec() == QDialog::Accepted)
    {
        // TODO: Implement the pairing logic using the selected host
    }

    UNUSED_PARAMETER(props);
    UNUSED_PARAMETER(property);
    UNUSED_PARAMETER(source);

    // Don't repaint the UI
    return false;
}

obs_property_t* Properties::CreateRemoveButton(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "Unpair Device" button
    obs_property_t* button = obs_properties_add_button(
        props,          
        "remove_device",                    // Internal name of the button
        obs_module_text("Device.Unpair"),   // Label displayed on the button
        [](obs_properties_t* props, obs_property_t* property, void* data) -> bool
        {
            return Properties::OnRemoveButtonPressed(props, property, static_cast<OBSSource*>(data));
        } // Callback function for button click
    );

    return button;
}

bool Properties::OnRemoveButtonPressed(obs_properties_t* props, obs_property_t* property, OBSSource* source)
{
    // Get the current settings of the source instance
    obs_data_t* settings = obs_source_get_settings(source->GetSource());

    // Get the currently selected value from the "host" list property
    std::string selectedHost(obs_data_get_string(settings, "host"));

    if (!selectedHost.empty())
    {
        // TODO: Implement the unpairing logic
    }
    else
    {
        // No host selected, show an error message
        DisplayMessageBox("Error", obs_module_text("Device.NoDevice"));
    }

    UNUSED_PARAMETER(props);
    UNUSED_PARAMETER(property);

    // Don't repaint the UI
    return false;
}

obs_property_t* Properties::CreateReconnectCheckbox(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "Automatically reconnect" checkbox
    obs_property_t* checkbox = obs_properties_add_bool(
        props,
        "reconnect",                                // Internal name of the property    
        obs_module_text("AutomaticallyReconnect")   // Label displayed in the UI
    );

    return checkbox;
}

obs_property_t* Properties::CreateBitRateSlider(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "BitRate (Mbps)" slider
    obs_property_t* slider = obs_properties_add_float_slider(
        props,
        "bitrate",                      // Internal name of the property
        obs_module_text("Bitrate"),     // Label displayed in the UI
        0.5,                            // Minimum value
        150,                            // Maximum value
        0.5                             // Step size
    );

    return slider;
}

obs_property_t* Properties::CreateDisplayTypeList(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "Resolution/FPS Type" combo box
    obs_property_t* comboBox = obs_properties_add_list(
        props,
        "display_type",                 // Internal name of the property
        obs_module_text("ResFPSType"),  // Label displayed in the UI
        OBS_COMBO_TYPE_LIST,            // Combo box type
        OBS_COMBO_FORMAT_STRING         // Format as strings
    );

    // Add options to the combo box
    obs_property_list_add_string(comboBox, obs_module_text("ResFPSType.DevPreferred"), "default");
    obs_property_list_add_string(comboBox, obs_module_text("ResFPSType.Custom"), "custom");
    // Add callback for when the combo box value changes
    obs_property_set_modified_callback(comboBox, OnDisplayTypeChanged);

    return comboBox;
}

bool Properties::OnDisplayTypeChanged(obs_properties_t *props, obs_property_t* property, obs_data_t* settings)
{
    // Get the selected display type
    std::string displayType(obs_data_get_string(settings, "display_type"));

    // Get the "fps" and "resolution" properties
    obs_property_t* resolution  = obs_properties_get(props, "resolution");
    obs_property_t* fps         = obs_properties_get(props, "fps");

    // Enable or disable based on the "displayType" value
    bool enable = displayType == "custom";
    obs_property_set_enabled(resolution, enable);
    obs_property_set_enabled(fps, enable);

    UNUSED_PARAMETER(property);

    // Repaint the UI
    return true;
}

obs_property_t* Properties::CreateResolutionList(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "Resolution" combo box
    obs_property_t* comboBox = obs_properties_add_list(
        props,
        "resolution",                   // Internal name of the property
        obs_module_text("Resolution"),  // Label displayed in the UI
        OBS_COMBO_TYPE_EDITABLE,        // Combo box type
        OBS_COMBO_FORMAT_STRING         // Format as strings
    );

    // Add options to the combo box
    obs_property_list_add_string(comboBox, "1920x1080", "1080p");

    return comboBox;
}

obs_property_t* Properties::CreateFPSList(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "FPS" combo box
    obs_property_t* comboBox = obs_properties_add_list(
        props,
        "fps",                  // Internal name of the property
        "FPS",                  // Label displayed in the UI
        OBS_COMBO_TYPE_LIST,    // Combo box type
        OBS_COMBO_FORMAT_FLOAT  // Format as strings
    );

    // Add options to the combo box
    obs_property_list_add_float(comboBox, obs_module_text("FPS.Matching"), 0.0f);
    obs_property_list_add_float(comboBox, obs_module_text("FPS.Highest"), -1.0f);
    obs_property_list_add_float(comboBox, "60", 60.0f);
    obs_property_list_add_float(comboBox, "59.94 NTSC", 59.94f);
    obs_property_list_add_float(comboBox, "50", 50);
    obs_property_list_add_float(comboBox, "48 film", 48.0f);
    obs_property_list_add_float(comboBox, "40", 40.0f);
    obs_property_list_add_float(comboBox, "30", 30.0f);
    obs_property_list_add_float(comboBox, "29.97 NTSC", 29.97f);
    obs_property_list_add_float(comboBox, "25", 25.0f);

    return comboBox;
}

obs_property_t* Properties::CreateHardwareDecodingCheckbox(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Create the "Use hardware decoding when available" checkbox
    obs_property_t* checkbox = obs_properties_add_bool(
        props,
        "hardware_decoding",                    // Internal name of the property
        obs_module_text("HardwareDecode")       // Label displayed in the UI
    );

    return checkbox;
}

obs_property_t* Properties::CreateAudioModeList(obs_properties_t* props)
{
    // Ensure the properties handle is valid
    assert(props != nullptr);

    // Add combo box for selecting the audio output mode
    obs_property_t* audioMode = obs_properties_add_list(
        props,
        "audio_mode",                           // Internal name of the property
        obs_module_text("AudioOutputMode"),     // Label displayed in the UI
        OBS_COMBO_TYPE_LIST,                    // Combo box type
        OBS_COMBO_FORMAT_STRING                 // Format as strings
    );

    // Add options to the combo box
    obs_property_list_add_string(audioMode, obs_module_text("AudioOutputMode.Capture"), "capture");
    obs_property_list_add_string(audioMode, obs_module_text("AudioOutputMode.DirectSound"), "directsound");
    obs_property_list_add_string(audioMode, obs_module_text("AudioOutputMode.WaveOut"), "waveout");

    return audioMode;
}

void Properties::DisplayMessageBox(std::string title, std::string message)
{
    // Ensure arguments are valid
    if (title.empty() || message.empty())
    {
        throw std::invalid_argument("Title and message cannot be empty");
    }

    // Get the main window of the OBS Studio application
    QWidget* mainWindow = static_cast<QWidget*>(obs_frontend_get_main_window());
    
    // Display the message box
    QMessageBox::warning(mainWindow, 
        QString::fromStdString(title), 
        QString::fromStdString(message), 
        QMessageBox::Ok
    );
}
