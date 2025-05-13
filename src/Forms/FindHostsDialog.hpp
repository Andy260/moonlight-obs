#pragma once

// STL includes
#include <map>

// Qt includes
#include <QDialog>

// Project includes
#include "../Connections/Host.hpp"

// Forward declarations
class QListWidget;
class QPushButton;
class QListWidgetItem;

namespace MoonlightOBS
{
    /**
     * @brief Dialog for displaying found GameStream hosts on 
     *        the local network to pair with.
     * 
     */
    class FindHostsDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * @brief Construct the FindHostsDialog dialog for pairing with a 
         *        GameStream host found on the local network.
         * 
         * @param parent The parent widget for this dialog.
         */
        explicit FindHostsDialog(QWidget* parent = nullptr);

        /**
         * @brief Destroy the FindHostsDialog dialog.
         * 
         */
        ~FindHostsDialog() override;

        /**
         * @brief Get the Selected Host.
         * 
         * @return Host The selected host.
         */
        inline Host GetSelectedHost() const
        {
            return m_selectedHost;
        }

    private slots:
        void OnHostSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous);
        void OnManuallyConnectClicked();

    private:
        // List of found hosts
        QListWidget* m_hostListWidget;
        // Button for pairing with the selected host
        QPushButton* m_pairButton;
        // Button for manually adding a host
        QPushButton* m_manuallyConnectButton;
        // Button for canceling the dialog
        QPushButton* m_cancelButton;

        // Selected host
        Host m_selectedHost;
        // Map of found hosts
        // Key: Host name, Value: Host object
        std::map<std::string, Host> m_foundHosts;
    };
} // namespace MoonlightOBS
