#include "PairDialog.hpp"

// OBS Studio includes
#include <obs-module.h>

// Qt includes
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

using namespace MoonlightOBS;

PairDialog::PairDialog(QWidget* parent) 
    : QDialog(parent)
{
    // Set the dialog title
    setWindowTitle(obs_module_text("PairingDialog.Title"));

    // Address Label and Text Box
    QLabel *addressLabel = new QLabel(obs_module_text("PairingDialog.Address"), this);
    m_addressLineEdit = new QLineEdit(this);

    // Buttons
    QPushButton* connectButton  = new QPushButton(obs_module_text("PairingDialog.Connect"), this);
    QPushButton* cancelButton   = new QPushButton(obs_module_text("PairingDialog.Cancel"), this);

    // Set the "Connect" button's OnClick event
    // to save the address entered from the text box
    connect(connectButton, &QPushButton::clicked, [this]() 
    {
        // Save the address entered from the text box
        m_address = m_addressLineEdit->text();

        // Closes the dialog and sets result to QDialog::Accepted
        accept();
    });
    // Set the "Cancel" buttons OnClick event
    // to close the dialog and set result to QDialog::Rejected
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    // Layout for Address Label and LineEdit
    QHBoxLayout* addressLayout = new QHBoxLayout();
    addressLayout->addWidget(addressLabel);
    addressLayout->addWidget(m_addressLineEdit);

    // Layout for Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(connectButton);
    buttonLayout->addWidget(cancelButton);

    // Main Layout
    QVBoxLayout* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(addressLayout);
    mainLayout->addSpacing(10);
    mainLayout->addLayout(buttonLayout);

    setLayout(mainLayout);
    setMinimumWidth(350);
}
