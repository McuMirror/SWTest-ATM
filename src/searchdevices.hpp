/**
 * @file searchdevices.hpp
 *
 * Definitions for a SWTest-ATM.
 *
 * ATM for banks Project.
 *
 * Copyright (c) 2015-2016 BlueSalute Company.
 *
 * @author Ederson Moura - <ederson.moura@bluesalute.com>
 *
 * $Id: Exp$
 */

#ifndef SEARCHDEVICES_HPP
#define SEARCHDEVICES_HPP

#include <utils/serialport.hpp>
#include <utils/contrib/qcgaugewidget.hpp>

#include <QWidget>
#include <QCloseEvent>

// Override!
namespace Ui {
    class SearchDevices;
}

/**
 * Options for get data for devices.
 *
 * \enum eSearchDevicesGetData
 */
enum eSearchDevicesGetData {
    SEARCHDEVICES_GETDATA_BILLVALIDATOR, //! Get serial answer Bill Validator value.
    SEARCHDEVICES_GETDATA_COINVALIDATOR, //! Get serial answer Coin Validator value.
    SEARCHDEVICES_GETDATA_DISPENSER,     //! Get serial answer Dispenser value.
    SEARCHDEVICES_GETDATA_MULTIIO        //! Get serial answer Multi/IO value.
};

/**
 * @class SearchDevices
 *
 * @brief Widget UI that find devices in ATM.
 *
 * @details This class makes the search for devices connected to
 * the serial ports of the ATM.
 */
class SearchDevices : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit SearchDevices(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~SearchDevices();

    /**
     * Get data for devices.
     *
     * @param cmd - The command for get data.
     */
    void getData(eSearchDevicesGetData cmd = SEARCHDEVICES_GETDATA_BILLVALIDATOR);

    /**
     * Check if data command is ok.
     *
     * @return Return true if command ok; otherwise false.
     */
    bool getDataIsOk();

    /**
     * Print the serial port found for this device.
     *
     * @return Port found.
     */
    QString printFoundPortDispenser() {
        Q_ASSERT(m_serialFound.dispenser.isEmpty());
        return m_serialFound.dispenser;
    }

    /**
     * Print the serial port found for this device.
     *
     * @return Port found.
     */
    QString printFoundPortBillValidator() {
        Q_ASSERT(m_serialFound.billvalidator.isEmpty());
        return m_serialFound.billvalidator;
    }

    /**
     * Print the serial port found for this device.
     *
     * @return Port found.
     */
    QString printFoundPortCoinValidator() {
        Q_ASSERT(m_serialFound.coinvalidator.isEmpty());
        return m_serialFound.coinvalidator;
    }

    /**
     * Print the serial port found for this device.
     *
     * @return Port found.
     */
    QString printFoundPortMultiIo() {
        Q_ASSERT(m_serialFound.multiIo.isEmpty());
        return m_serialFound.multiIo;
    }

Q_SIGNALS:
    /**
     * Trigged wheen is necessary make the update serial ports.
     */
    void signalUpdateSerialPorts();
    
private slots:
    /**
     * Makes the communication test to find which serial port is the device.
     */
    void tryTestDeviceCommunication();

    /**
     * Close widget form.
     */
    void closeForm();

private:
    /// Store instance (ui interface).
    Ui::SearchDevices *ui;

    /// Store serial control.
    utils::SerialPort *m_serial;

    /// Check data is ok.
    bool m_getDataIsOk;

    /// Check if any device found.
    bool m_foundDevice;

    /// Store widge gauge.
    QcGaugeWidget *m_speedGauge;

    /// Store widget intem gauge.
    QcNeedleItem *m_speedNeedle;

    /**
     * Stores the serial ports of the devices.
     */
    struct serialFound {
        QString dispenser;     //< Stores serial ports of the Dispenser.
        QString billvalidator; //< Stores serial ports of the Bill Validator.
        QString coinvalidator; //< Stores serial ports of the Coin Validator.
        QString multiIo;       //< Stores serial ports of the Multi/IO.
    };

    /// Instance the struct serialFound.
    serialFound m_serialFound;

    /**
     * Put text data for show in console.
     *
     * @param data - Text data string.
     */
    void putData(const QString &data);

    /**
     * Is trying to communicate with the device: Coin Validator.
     *
     * @param comport - The name of the serial port
     * @param baudrate - The data baud rate for the desired direction.
     * @param databit - Set the number of data bits used.
     * @param tparity - This property holds the parity checking mode.
     * @param stopbit - This property holds the number of stop bits in a frame.
     * @param flowcontrol - Set the flow control used.
     * @return Return true if command successful, others false.
     */
    bool tryCommunicationCoinValidator(QString comport, unsigned int baudrate, unsigned int databit,
                                       unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol);

    /**
     * Is trying to communicate with the device: Bill Validator.
     *
     * @param comport - The name of the serial port
     * @param baudrate - The data baud rate for the desired direction.
     * @param databit - Set the number of data bits used.
     * @param tparity - This property holds the parity checking mode.
     * @param stopbit - This property holds the number of stop bits in a frame.
     * @param flowcontrol - Set the flow control used.
     * @return Return true if command successful, others false.
     */
    bool tryCommunicationBillValidator(QString comport, unsigned int baudrate, unsigned int databit,
                                       unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol);

    /**
     * Is trying to communicate with the device: Dispenser.
     *
     * @param comport - The name of the serial port
     * @param baudrate - The data baud rate for the desired direction.
     * @param databit - Set the number of data bits used.
     * @param tparity - This property holds the parity checking mode.
     * @param stopbit - This property holds the number of stop bits in a frame.
     * @param flowcontrol - Set the flow control used.
     * @return Return true if command successful, others false.
     */
    bool tryCommunicationDispenser(QString comport, unsigned int baudrate, unsigned int databit,
                                   unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol);

    /**
     * Is trying to communicate with the device: Multi/IO.
     *
     * @param comport - The name of the serial port
     * @param baudrate - The data baud rate for the desired direction.
     * @param databit - Set the number of data bits used.
     * @param tparity - This property holds the parity checking mode.
     * @param stopbit - This property holds the number of stop bits in a frame.
     * @param flowcontrol - Set the flow control used.
     * @return Return true if command successful, others false.
     */
    bool tryCommunicationMultiIo(QString comport, unsigned int baudrate, unsigned int databit,
                                 unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol);

protected:
    /**
     * Reimplemented method, triggered automatically when the widget
     * is closed.
     *
     * @param event - Some event.
     */
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    /**
     * Reimplemented method, triggered automatically when the widget
     * is changed.
     *
     * @param event - Some event.
     */
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;
};

#endif // SEARCHDEVICES_HPP
