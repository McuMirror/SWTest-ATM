/**
 * @file abstractcashcode609.cpp
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


#include <devices/multiio/abstractled10v2.hpp>
#include <utils/log.hpp>

// Start static variable and objects.
devices::multiio::AbstractLed10v2* devices::multiio::AbstractLed10v2::m_instance = 0;
bool devices::multiio::AbstractLed10v2::m_ComIsOpen = false;

namespace devices {
  namespace multiio {

    AbstractLed10v2* AbstractLed10v2::getInstance() {
        if (m_instance == 0) {
            m_instance = new AbstractLed10v2();
        }
        return m_instance;
    }

    AbstractLed10v2::AbstractLed10v2() : m_getDataIsOk(false), m_statusSensor(false) {
        QLOG_INFO() << "Constructor class:" << "AbstractLed10v2";

        // Instance class.
        this->m_serial = new utils::SerialPort;
    }

    AbstractLed10v2::~AbstractLed10v2() {
        QLOG_INFO() << "Destructor class:" << "AbstractLed10v2";
        if (this->comIsOpen()) {
           this->close();
        }

        this->m_getDataIsOk = false;
        AbstractLed10v2::m_instance = 0;
    }

    bool AbstractLed10v2::open(std::string comport, unsigned int baudrate,
                              unsigned int databit, unsigned int tparity,
                              unsigned int stopbit, unsigned int flowcontrol) {
        bool ok = this->m_serial->open(comport, baudrate, databit,
                                       tparity, stopbit, flowcontrol);

        if (ok) {
            QLOG_INFO() << "AbstractLed10v2: open connection successful.";
            AbstractLed10v2::m_ComIsOpen = true;
            return true;
        } else {
            QLOG_ERROR() << "AbstractLed10v2: Error open connection.";
            AbstractLed10v2::m_ComIsOpen = false;
            return false;
        }
    }

    bool AbstractLed10v2::close() {
        bool ok = this->m_serial->close();

        if (ok) {
            QLOG_INFO() << "AbstractLed10v2: close connection successful.";
            return true;
        } else {
            QLOG_ERROR() << "AbstractLed10v2: Error close connection.";
            return false;
        }
    }

    bool AbstractLed10v2::comIsOpen() {
        return AbstractLed10v2::m_ComIsOpen;
    }

    void AbstractLed10v2::getData(eAbstractLed10v2GetData cmd) {
        const QByteArray data = m_serial->getData();

        if (data.isEmpty()) {
            QLOG_WARN() << "AbstractLed10v2: Get data package is invalid.";
            m_getDataIsOk = false;
            return; // Avoid segmentation fault.
        } else {
            m_getDataIsOk = true;
        }

        switch (cmd) {
        case ABSTRACT_LED10V2_GET_STATUS_SENSOR:
            QLOG_INFO() << "ABSTRACT_LED10V2_GET_STATUS_SENSOR:" << data.toHex().toUpper();
            if (data.contains(0x49) && data.contains(0x31)) {
                // Pin closed.
                this->m_statusSensor = true;
            } else {
                // Pin opened.
                this->m_statusSensor = false;
            }
            break;
        case ABSTRACT_LED10V2_GET_STATUS_SENSOR_VBR:
            QLOG_INFO() << "ABSTRACT_LED10V2_GET_STATUS_SENSOR_VBR:" << data.toHex().toUpper();
            if (data.contains(0x49) && data.contains(0x33)) {
                // Pin closed.
                this->m_statusSensor = true;
            } else {
                // Pin opened.
                this->m_statusSensor = false;
            }
            break;

        case ABSTRACT_LED10V2_GET_STATUS_ONOFF:
            QLOG_INFO() << "ABSTRACT_LED10V2_GET_STATUS_ONOFF:" << data.toHex().toUpper();
            break;
        default:
            break;
        }
    }

    bool AbstractLed10v2::getDataIsOk() {
        return m_getDataIsOk;
    }

    bool AbstractLed10v2::sensorStatus(unsigned int pin) {
        if (!AbstractLed10v2::comIsOpen()) {
            QLOG_ERROR() << "AbstractLed10v2: Error connection.";
            return false;
        }

        bool doorOrVibration = false;

        QLOG_INFO() << "AbstractLed10v2: Calling command status sensor pin..." << pin;

        QByteArray cmd;
        cmd.resize(11);

        switch (pin) {
        case 12:
            // STATUS: Sensor inferior door.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x31;
            cmd[6]  = 0x32;
            cmd[7]  = 0x2C;
            cmd[8]  = 0xFF;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            doorOrVibration = false;
            break;
        case 13:
            // STATUS: Sensor extern door.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x31;
            cmd[6]  = 0x33;
            cmd[7]  = 0x2C;
            cmd[8]  = 0xFF;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            doorOrVibration = false;
            break;
        case 14:
            // STATUS: Sensor internal vibration.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x31;
            cmd[6]  = 0x34;
            cmd[7]  = 0x2C;
            cmd[8]  = 0xFF;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            doorOrVibration = true;
            break;
        default:
            break;
        }

        this->m_serial->writeData(cmd, cmd.size(), 300);

        // Get response command.
        if (!doorOrVibration) {
            this->getData(ABSTRACT_LED10V2_GET_STATUS_SENSOR);
        } else {
            this->getData(ABSTRACT_LED10V2_GET_STATUS_SENSOR_VBR);
        }

        if (!getDataIsOk()) {
            QLOG_ERROR() << "AbstractLed10v2: Error execute command get status.";
            return false;
        }

        return true;
    }

    bool AbstractLed10v2::checkSensorStatus() {
        return this->m_statusSensor;
    }

    bool AbstractLed10v2::powerOn(unsigned int pin) {
        if (!AbstractLed10v2::comIsOpen()) {
            QLOG_ERROR() << "AbstractLed10v2: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractLed10v2: Calling command power on pin..." << pin;

        QByteArray cmd;
        cmd.resize(11);

        switch (pin) {
        case 1:
            // ON: Printer 24V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x31;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x30;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        case 3:
            // ON: Coin Validator 12V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x33;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x30;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        case 7:
            // ON: Dispenser 24V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x37;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x30;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        case 9:
            // ON: Bill Validator 12V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x39;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x30;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        default:
            break;
        }

        this->m_serial->writeData(cmd, cmd.size(), 100);
        return true;
    }

    bool AbstractLed10v2::powerOff(unsigned int pin) {
        if (!AbstractLed10v2::comIsOpen()) {
            QLOG_ERROR() << "AbstractLed10v2: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractLed10v2: Calling command power off pin..." << pin;

        QByteArray cmd;
        cmd.resize(11);

        switch (pin) {
        case 1:
            // OFF: Printer 24V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x31;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x31;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        case 3:
            // OFF: Coin Validator 12V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x33;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x31;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        case 7:
            // OFF: Dispenser 24V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x37;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x31;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        case 9:
            // OFF: Bill Validator 24V.
            cmd[0]  = 0x4F;
            cmd[1]  = 0x28;
            cmd[2]  = 0x30;
            cmd[3]  = 0x30;
            cmd[4]  = 0x2C;
            cmd[5]  = 0x30;
            cmd[6]  = 0x39;
            cmd[7]  = 0x2C;
            cmd[8]  = 0x31;
            cmd[9]  = 0x29;
            cmd[10] = 0x45;
            break;
        default:
            break;
        }

        this->m_serial->writeData(cmd, cmd.size(), 100);
        return true;
    }

    const std::string AbstractLed10v2::printVersion() {
        std::string v = "LED-10v2";
        return v;
    }

  }
}
