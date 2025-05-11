#pragma once

// Qt includes
#include <QDialog>
#include <QString>

// Forward declarations
class QLineEdit;

namespace MoonlightOBS
{
    /**
     * @brief Dialog for pairing with a device.
     */
    class PairDialog : public QDialog
    {
        Q_OBJECT

    public:
        /**
         * @brief Construct a new Pair Dialog object
         * 
         * @param parent The parent widget for this dialog.    
         */
        explicit PairDialog(QWidget* parent = nullptr);

        /**
         * @brief Get the entered address from the dialog.
         * 
         * @return QString The address entered in the dialog.
         */
        inline QString GetAddress() const
        {
            return m_address;
        }

    private:
        QLineEdit* m_addressLineEdit;
        QString m_address;
    };
} // namespace MoonlightOBS

