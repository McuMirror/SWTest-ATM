/**
 * @file serialloopbackfit.hpp
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

#ifndef SERIALLOOPBACKFIT_HPP
#define SERIALLOOPBACKFIT_HPP

#include <utils/serialport.hpp>

#include <QWidget>
#include <QCloseEvent>

// Override!
namespace Ui {
    class SerialLoopBackFit;
}

// Override!
class SettingsSerial;

/**
 * @class SerialLoopBackFit
 *
 * @brief Widget UI that make test serial loopback.
 *
 * @details This class make tests loopback in serial port.
 */
class SerialLoopBackFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     * @param type - Type instance.
     */
    explicit SerialLoopBackFit(QWidget *parent = 0, bool type = false);

    /**
     * Destructor.
     */
    ~SerialLoopBackFit();

    /**
     * Set serial port.
     *
     * @param comPort - The name of the serial port.
     * @param baudRate - The data baud rate for the desired direction.
     * @param dataBits - Set the number of data bits used.
     * @param parity - This property holds the parity checking mode.
     * @param stopBits - This property holds the number of stop bits in a frame.
     * @param flowControl - Set the flow control used.
     * @return Return true if command successful, others false.
     */
    void setSerialPort(const QString comPort = QString(), const unsigned int baudRate = 0,
                       const unsigned int dataBits = 0, const unsigned int parity = 0,
                       const unsigned int stopBits = 0, const unsigned int flowControl = 0);

Q_SIGNALS:
    /**
     * Trigged wheen finish serial commands.
     */
    void finishSerial();

    /**
     * Trigged wheen ocorr error in serial serial connection.
     */
    void errorConnection();

public slots:
    /**
     * Open serial connection.
     */
    void serialConnect();

    /**
     * Close serial connection.
     */
    void serialDisconnect();

private slots:
    /**
     * Run loopback tests.
     */
    void runLoopback();

    /**
     * Get serial answer data.
     *
     * @param cmd - Number command.
     */
    void getData(int cmd = 0);

    /**
     * Count the time connection elapsed.
     */
    void countUp();

    /**
     * Generete log/report in display.
     */
    void serialReport();

    /**
     * Close form and disconnect a serial port used.
     */
    void closeForm();

    /**
     * Called by combo box, change the configuration serial port.
     *
     * @param text - Serial port to set configuration.
     */
    void handleSelectionSerialChanged(const QString &text);

    /**
     * Make serial settings.
     */
    void handleSerialSettings();

private:
    /**
     * Get serial ports avaliable in system.
     */
    void getSerialPorts();

    /// Store instance (ui interface).
    Ui::SerialLoopBackFit *ui;

    /// The timer for count.
    QTimer *m_timer;

    /// Store the portname selected.
    QStringList m_portName;

    /// Store number serial ports found in system.
    int m_countSerialPorts;

    /// Store the count time.
    int m_counttime;

    /// Store serial control.
    utils::SerialPort *m_serial;

    /// Store answer ascii.
    QString m_answerAscii;

    /// Store answer hex.
    QString m_answerHex;

    /**
     * Structure used for test (loopback).
     */
    struct cmdSerial {
        size_t requestAlphabet;     //< Store serial size send package.
        size_t answerAlphabet;      //< Store serial size  anwser alphabet.
        size_t requestNumbers;      //< Store serial size send package.
        size_t answerNumbers;       //< Store serial size anwser alphabet.
        size_t requestProbeDroidP1; //< Store serial size send package.
        size_t answerProbeDroidP1;  //< Store serial size anwser alphabet.
        size_t requestProbeDroidP2; //< Store serial size send package.
        size_t answerProbeDroidP2;  //< Store serial size anwser alphabet.
    };

    /// Instance the struct cmd serial.
    cmdSerial m_cmdSerial;

    /**
     * Structure used for store parameters serial.
     */
    struct cmdParamSerial {
        QString comPort;          //< Store com name serial port.
        unsigned int baudRate;    //< Store baudrate serial port.
        unsigned int dataBits;    //< Store databit serial port.
        unsigned int parity;      //< Store parity serial port.
        unsigned int stopBits;    //< Store stopbits serial port.
        unsigned int flowControl; //< Store flowcontrol seril port.
    };


    /// Instance the struct cmd param serial.
    cmdParamSerial m_cmdParamSerial;

    /// Instance for class Settings Serial.
    SettingsSerial *m_settingsserial;

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

#endif // SERIALLOOPBACKFIT_HPP
