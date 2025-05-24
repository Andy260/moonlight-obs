#include "FindHostsDialog.hpp"

// Qt includes
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>

// OBS Studio includes
#include <util/base.h>
#include <obs-module.h>

// Project includes
#include "../plugin-support.h"
#include "../Connections/GameStreamHost.hpp"
#include "../Discovery/LANSearcher.hpp"
#include "ManualPairingDialog.hpp"

using namespace MoonlightOBS;

FindHostsDialog::FindHostsDialog(QWidget* parent)
    : QDialog(parent), m_selectedHost(GameStreamHost::GetEmpty())
{
    setWindowTitle(obs_module_text("FindHostsDialog.Title"));

    // Label for the host list
    QLabel* hostsLabel = new QLabel(obs_module_text("FindHostsDialog.AvailableHosts"), this);
    // Hosts list widget
    m_hostListWidget = new QListWidget(this);
    m_hostListWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    // Pair button
    m_pairButton = new QPushButton(obs_module_text("FindHostsDialog.Pair"), this);
    // Manually Connect button
    m_manuallyConnectButton = new QPushButton(obs_module_text("FindHostsDialog.ManuallyConnect"), this);
    // Cancel button
    m_cancelButton = new QPushButton(obs_module_text("FindHostsDialog.Cancel"), this);

    // Disable the pair button by default
    m_pairButton->setEnabled(false);

    // Main layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addWidget(hostsLabel);
    mainLayout->addWidget(m_hostListWidget);
    // Button layout
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_pairButton);
    buttonLayout->addWidget(m_manuallyConnectButton);
    buttonLayout->addWidget(m_cancelButton);

    // Add the button layout to the main layout
    // and set the main layout
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);
    setMinimumWidth(400);

    // Connect signals and slots
    connect(m_hostListWidget, &QListWidget::currentItemChanged, this, &FindHostsDialog::OnHostSelectionChanged);
    connect(m_pairButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(m_manuallyConnectButton, &QPushButton::clicked, this, &FindHostsDialog::OnManuallyConnectClicked);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Start searching for hosts
    LANSearcher::Start([this](const GameStreamHost& foundHost)
    {
        // Keep track of the found hosts
        m_foundHosts.emplace(foundHost.GetHostname(), foundHost);
        // Add the found host to the list widget
        m_hostListWidget->addItem(QString::fromStdString(foundHost.GetHostname()));
    });
}

FindHostsDialog::~FindHostsDialog()
{
    // Stop searching for hosts if the search is running
    if (LANSearcher::IsSearching())
    {
        LANSearcher::Stop();
    }
}

void FindHostsDialog::OnHostSelectionChanged(QListWidgetItem* current, QListWidgetItem* previous)
{
    UNUSED_PARAMETER(previous);

    if (current != nullptr)
    {
        // Enable the pair button if a host is selected
        m_pairButton->setEnabled(true);

        std::string hostName = current->text().toStdString();

        auto iteratorToHost = m_foundHosts.find(hostName);
        if (iteratorToHost != m_foundHosts.end())
        {
            // Set the selected host
            m_selectedHost = iteratorToHost->second;
        }
        else
        {
            // This shouldn't happen
            // Set the selected host to an empty host
            m_selectedHost = GameStreamHost::GetEmpty();
            
            // Log this event
            obs_log(LOG_WARNING, "Unable to select %s, not in map.", hostName.c_str());
        }
    }
    else
    {
        // Disable the pair button if no host is selected
        m_pairButton->setEnabled(false);
    }
}

void FindHostsDialog::OnManuallyConnectClicked()
{
    ManualPairingDialog dialog(this);

    // Display the manual pairing dialog
    if (dialog.exec() == QDialog::Accepted) 
    {
        // Get the address entered in the dialog
        QString address = dialog.GetAddress();
        
        // Close the main dialog
        // as the user has selected the device they wish to pair with
        accept();
    }
}
