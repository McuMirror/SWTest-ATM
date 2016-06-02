/**
 * @file serialport.hpp
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

#ifndef UTILS_SERIALPORT_HPP
#define UTILS_SERIALPORT_HPP

#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QTextStream>
#include <QStringList>
#include <QCloseEvent>
#include <QByteArray>

namespace utils {
    /**
     * @class SerialPort
     *
     * @brief Class making the serial communication with devices.
     *
     * @details This class is all part of serial communication with the devices.
     */
    class SerialPort : public QObject {

    Q_OBJECT

    public:
        /**
         * Constructor default.
         */
        explicit SerialPort(QObject *parent = 0);

        /**
         * Destructor.
         */
        ~SerialPort();

        /**
         * Singleton for this class.
         *
         * @return The static class.
         */
        static SerialPort* getInstance();

        /**
         * Print all serial ports avaliable in system.
         *
         * @return The static class.
         */
        static QList<QSerialPortInfo> printAvaliablePorts();

        /**
         * Open connection with serial port.
         *
         * @param comport - The name of the serial port.
         * @param baudrate - The data baud rate for the desired direction.
         * @param databit - Set the number of data bits used.
         * @param tparity - This property holds the parity checking mode.
         * @param stopbit - This property holds the number of stop bits in a frame.
         * @param flowcontrol - Set the flow control used.
         * @return Return true if command successful, others false.
         */
        bool open(std::string comport = "COM1", unsigned int baudrate = 9600,
                  unsigned int databit = 8, unsigned int tparity = 0,
                  unsigned int stopbit = 3, unsigned int flowcontrol = 0);

        /**
         * Close connection with serial port.
         *
         * @return Return true if command successful, others false.
         */
        bool close();

        /**
         * This command print last command (hex) send to serial.
         *
         * @return Last command send.
         */
        static QString printCommandSend();

        /**
         * This command print last command (hex) response to serial.
         *
         * @return Last command response.
         */
        static QString printCommandResponse();

        /**
         * Get the return (answer) command.
         *
         * @return Commands serial answer.
         */
        QByteArray getData();

        /**
         * Clear serial data.
         */
        void clear();

    public slots:
        /**
         * Write data in serial port.
         *
         * @param data - Command data to write.
         * @param size - Command size.
         * @param waitread - Time to wait answer.
         */
        void writeData(const QByteArray &data, unsigned int size, unsigned int waitread);

    private slots:
        /**
         * Get errors in serial port.
         *
         * @param error - Some error...
         */
        void handleError(QSerialPort::SerialPortError error);

    protected:
        /**
         * Close events are sent to widgets that the user wants to close.
         *
         * @param event - Some event received.
         */
        void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

        /// Store serial port instance.
        static SerialPort *m_instance;

        /// Store serial write data.
        QSerialPort *m_serial;

        /// Store serial response data.
        QByteArray m_responseData;

        /// Store serial command send data.
        static QByteArray m_commandSend;

        /// Store serial command response data.
        static QByteArray m_commandResponse;
    };

};

#endif // UTILS_SERIALPORT_HPP
