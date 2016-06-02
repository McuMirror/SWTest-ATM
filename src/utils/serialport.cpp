/**
 * @file serialport.cpp
 *
 * Implementations for a SWTest-ATM.
 *
 * ATM for banks Project.
 *
 * Copyright (c) 2015-2016 BlueSalute Company.
 *
 * @author Ederson Moura - <ederson.moura@bluesalute.com>
 *
 * $Id: Exp$
 */

#include <utils/serialport.hpp>
#include <utils/log.hpp>

// Start static variable and objects.
utils::SerialPort* utils::SerialPort::m_instance = 0;
QByteArray utils::SerialPort::m_commandSend = QByteArray();
QByteArray utils::SerialPort::m_commandResponse = QByteArray();

namespace utils {

    SerialPort* SerialPort::getInstance() {
        if (m_instance == 0) {
            m_instance = new SerialPort;
        }
        return m_instance;
    }

    SerialPort::SerialPort(QObject *parent) :
        QObject(parent) {

        // First log class.
        QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

        // Instance class.
        this->m_serial = new QSerialPort(this);

        // Signals and slots.
        QObject::connect(m_serial, SIGNAL(error(QSerialPort::SerialPortError)), this,
                         SLOT(handleError(QSerialPort::SerialPortError)));
    }

    SerialPort::~SerialPort() {
        delete this;
    }

    void SerialPort::closeEvent(QCloseEvent *event) {
        QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
        event->accept();
    }

    QList<QSerialPortInfo> SerialPort::printAvaliablePorts() {
        QList<QSerialPortInfo> serialPortInfoList = QSerialPortInfo::availablePorts();
        return serialPortInfoList;
    }

    bool SerialPort::open(std::string comport, unsigned int baudrate,
                          unsigned int databit, unsigned int tparity,
                          unsigned int stopbit, unsigned int flowcontrol) {
        m_serial->setPortName(QString(comport.c_str()));

        // Check baudrate.
        switch (baudrate) {
        case 1200:
            m_serial->setBaudRate(QSerialPort::Baud1200);
            break;
        case 2400:
            m_serial->setBaudRate(QSerialPort::Baud2400);
            break;
        case 4800:
            m_serial->setBaudRate(QSerialPort::Baud4800);
            break;
        case 9600:
            m_serial->setBaudRate(QSerialPort::Baud9600);
            break;
        case 19200:
            m_serial->setBaudRate(QSerialPort::Baud19200);
            break;
        case 38400:
            m_serial->setBaudRate(QSerialPort::Baud38400);
            break;
        case 57600:
            m_serial->setBaudRate(QSerialPort::Baud57600);
            break;
        case 115200:
            m_serial->setBaudRate(QSerialPort::Baud115200);
            break;
        default:
            // Custom...
            break;
        }

        // Check databit.
        switch (databit) {
        case 5:
            m_serial->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            m_serial->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            m_serial->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            m_serial->setDataBits(QSerialPort::Data8);
            break;
        default:
            // Unknow data bit...
            break;
        }

        // Check parity.
        switch (tparity) {
        case 0:
            m_serial->setParity(QSerialPort::NoParity);
            break;
        case 2:
            m_serial->setParity(QSerialPort::EvenParity);
            break;
        case 3:
            m_serial->setParity(QSerialPort::OddParity);
            break;
        case 4:
            m_serial->setParity(QSerialPort::SpaceParity);
            break;
        case 5:
            m_serial->setParity(QSerialPort::MarkParity);
            break;
        default:
            m_serial->setParity(QSerialPort::UnknownParity);
        }

        // Check stop bit.
        switch (stopbit) {
        case 1:
            m_serial->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            m_serial->setStopBits(QSerialPort::TwoStop);
            break;
        case 3:
#ifdef Q_OS_WIN
            m_serial->setStopBits(QSerialPort::OneAndHalfStop);
#endif
            break;
        default:
            // Unknow stop bit...
            break;
        }

        // Check flow control.
        switch (flowcontrol) {
        case 0:
            m_serial->setFlowControl(QSerialPort::NoFlowControl);
            break;
        case 1:
            m_serial->setFlowControl(QSerialPort::HardwareControl);
            break;
        case 2:
            m_serial->setFlowControl(QSerialPort::SoftwareControl);
            break;
        default:
            m_serial->setFlowControl(QSerialPort::UnknownFlowControl);
            break;
        }

        if (m_serial->open(QIODevice::ReadWrite)) {
            QLOG_INFO() << "SerialPort: open connection successful.";
            return true;
        } else {
            QLOG_ERROR() << "SerialPort: Error open connection.";
            return false;
        }
    }

    bool SerialPort::close() {
        if (m_serial->isOpen()) {
            QLOG_INFO() << "SerialPort: close connection successful.";
            m_serial->clear();
            m_serial->close();
            return true;
        } else {
            QLOG_ERROR() << "SerialPort: Error close connection.";
            return false;
        }
    }

    void SerialPort::clear() {
        utils::SerialPort::m_commandResponse.clear();
        utils::SerialPort::m_commandSend.clear();
        this->m_serial->flush();
        this->m_serial->clear();
    }

    void SerialPort::writeData(const QByteArray &data, unsigned int size, unsigned int waitread) {
        QLOG_INFO() << "SerialPort: Write serial package data:" << data.toHex().toUpper();
        QLOG_INFO() << "SerialPort: Size command send package:" << data.size();

        // Clear serial.
        this->clear();

        if (waitread == 0) {
            waitread = 10; // Default in milisecounds.
        }

        // Write serial request.
        m_serial->write(data, size);

        // Store command write.
        utils::SerialPort::m_commandSend = data;

        if (m_serial->waitForBytesWritten(100)) {
            // Read serial response.
            if (m_serial->waitForReadyRead(100)) {

                this->m_responseData = m_serial->readAll();
                utils::SerialPort::m_commandResponse = this->m_responseData;

                while (m_serial->waitForReadyRead(waitread)) {
                    this->m_responseData += m_serial->readAll();
                    utils::SerialPort::m_commandResponse = this->m_responseData;
                    QLOG_INFO() << "SerialPort: Reading bytes:" << m_responseData.toHex().toUpper();
                    this->m_serial->flush();
                }
            }
        }
        QLOG_INFO() << "SerialPort: Read serial package data:" << this->m_responseData.toHex().toUpper();
        QLOG_INFO() << "SerialPort: Size command response package:" << this->m_responseData.size();
    }

    QByteArray SerialPort::getData() {
        QLOG_INFO() << "SerialPort: Get data package:" << utils::SerialPort::m_commandResponse.toHex().toUpper();
        if (utils::SerialPort::m_commandResponse.isEmpty()) {
            QLOG_WARN() << "SerialPort: Get data package is invalid.";
        }
        const QByteArray data = utils::SerialPort::m_commandResponse;
        return data;
    }

    void SerialPort::handleError(QSerialPort::SerialPortError error) {
        if (error == QSerialPort::ResourceError) {
            QLOG_ERROR() << "SerialPort: Critical error" <<  m_serial->errorString();
            this->m_serial->close();
        }
    }

    QString SerialPort::printCommandSend() {
        return utils::SerialPort::m_commandSend.toHex().toUpper();
    }

    QString SerialPort::printCommandResponse() {
        return utils::SerialPort::m_commandResponse.toHex().toUpper();
    }

}
