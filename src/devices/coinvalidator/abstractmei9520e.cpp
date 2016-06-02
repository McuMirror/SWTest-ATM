/**
 * @file abstractmei9520e.cpp
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

#include <devices/coinvalidator/abstractmei9520e.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QThread>

// Start static variable and objects.
devices::coinvalidator::AbstractMei9520e* devices::coinvalidator::AbstractMei9520e::m_instance = 0;
bool devices::coinvalidator::AbstractMei9520e::m_ComIsOpen = false;

namespace devices {
  namespace coinvalidator {

    AbstractMei9520e* AbstractMei9520e::getInstance() {
        if (m_instance == 0) {
            m_instance = new AbstractMei9520e();
        }
        return m_instance;
    }

    AbstractMei9520e::AbstractMei9520e() {       
        QLOG_INFO() << "Constructor class:" << "AbstractMei9520e";

        // Instance class.
        this->m_serial = new utils::SerialPort;
    }

    AbstractMei9520e::~AbstractMei9520e() {
        QLOG_INFO() << "Destructor class:" << "AbstractMei9520e";
        if (this->comIsOpen()) {
           this->close();
        }

        AbstractMei9520e::m_instance = 0;
    }

    bool AbstractMei9520e::open(std::string comport, unsigned int baudrate,
                                unsigned int databit, unsigned int tparity,
                                unsigned int stopbit, unsigned int flowcontrol) {
        bool ok = this->m_serial->open(comport, baudrate, databit,
                                       tparity, stopbit, flowcontrol);

        if (ok) {
            QLOG_INFO() << "AbstractMei9520e: open connection successful.";
            AbstractMei9520e::m_ComIsOpen = true;
            return true;
        } else {
            QLOG_ERROR() << "AbstractMei9520e: Error open connection.";
            AbstractMei9520e::m_ComIsOpen = false;
            return false;
        }
    }

    bool AbstractMei9520e::close() {
        bool ok = this->m_serial->close();

        if (ok) {
            QLOG_INFO() << "AbstractMei9520e: close connection successful.";
            return true;
        } else {
            QLOG_ERROR() << "AbstractMei9520e: Error close connection.";
            return false;
        }
    }

    bool AbstractMei9520e::comIsOpen() {
        return AbstractMei9520e::m_ComIsOpen;
    }

    bool AbstractMei9520e::reset() {
        if (!AbstractMei9520e::comIsOpen()) {
            QLOG_ERROR() << "AbstractMei9520e: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractMei9520e: Calling command reset...";

        QByteArray cmd;

        // Simple poll.
        cmd.resize(5);
        cmd[0] = 0x02;
        cmd[1] = 0x00;
        cmd[2] = 0x01;
        cmd[3] = 0xFE;
        cmd[4] = 0xFF;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);
        cmd.clear();

        cmd.resize(5);
        cmd[0] = 0x02;
        cmd[1] = 0x00;
        cmd[2] = 0x01;
        cmd[3] = 0x01;
        cmd[4] = 0xFC;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);
        cmd.clear();

        // Modify inhibit status.
        unsigned int enCoin = 28356628;
        unsigned char enCoinMSB = enCoin >> 8;
        unsigned char enCoinLSB = enCoin & 0xFF;

        cmd.resize(8);
        cmd[0] = 0x02;
        cmd[1] = 0x02;
        cmd[2] = 0x01;
        cmd[3] = 0xE7;
        cmd[4] = enCoinLSB;
        cmd[5] = enCoinMSB;
        cmd[6] = 0x50;
        cmd[7] = (0x214 - enCoinLSB - enCoinMSB) & 0xFF;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);
        cmd.clear();

        cmd.resize(12);
        cmd[0]  = 0x02;
        cmd[1]  = 0x02;
        cmd[2]  = 0x01;
        cmd[3]  = 0xE7;
        cmd[4]  = 0x14;
        cmd[5]  = 0xB0;
        cmd[6]  = 0x50;
        cmd[7]  = 0x01;
        cmd[8]  = 0x00;
        cmd[9]  = 0x02;
        cmd[10] = 0x00;
        cmd[11] = 0xFD;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);
        cmd.clear();

        // Perform self-check.
        cmd.resize(5);
        cmd[0] = 0x02;
        cmd[1] = 0x00;
        cmd[2] = 0x01;
        cmd[3] = 0xE8;
        cmd[4] = 0x15;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);
        cmd.clear();

        // Modify master inhibit status - normal operation.
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x01;
        cmd[2] = 0x01;
        cmd[3] = 0xE4;
        cmd[4] = 0x01;
        cmd[5] = 0x17;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);
        cmd.clear();
        return true;
    }

    void AbstractMei9520e::getData(eAbstractMei9520eGetData cmd) {
        const QByteArray data = m_serial->getData();

        if (data.isEmpty()) {
            QLOG_WARN() << "AbstractMei9520e: Get data package is invalid.";
            m_getDataIsOk = false;
            return; // Avoid segmentation fault.
        }

        std::string pVersion = "";
        switch (cmd) {
        case ABSTRACT_MEI9520E_GET_MONEY:
            if (data.at(10) == QChar(0x03)) {
                m_statusStoreCoinValue = 1;
            } else if (data.at(10) == QChar(0x04)) {
                m_statusStoreCoinValue = 2;
            } else if (data.at(10) == QChar(0x05)) {
                m_statusStoreCoinValue = 3;
            } else if (data.at(10) == QChar(0x06)) {
                m_statusStoreCoinValue = 4;
            } else if (data.at(10) == QChar(0x07)) {
                m_statusStoreCoinValue = 5;
            } else if (data.at(10) == QChar(0x08)) {
                m_statusStoreCoinValue = 6;
            } else {
                QLOG_WARN() << "AbstractMei9520e: Get data package is invalid.";
            }
            break;
        case ABSTRACT_MEI9520E_GET_VERSION:
            if (!data.isEmpty() && data.size() >= 14) {
                pVersion.clear();

                pVersion += data.toHex().toUpper().toStdString().at(18);
                pVersion += data.toHex().toUpper().toStdString().at(19);

                pVersion += data.toHex().toUpper().toStdString().at(20);
                pVersion += data.toHex().toUpper().toStdString().at(21);

                pVersion += data.toHex().toUpper().toStdString().at(22);
                pVersion += data.toHex().toUpper().toStdString().at(23);

                pVersion += data.toHex().toUpper().toStdString().at(24);
                pVersion += data.toHex().toUpper().toStdString().at(25);

                pVersion += data.toHex().toUpper().toStdString().at(26);
                pVersion += data.toHex().toUpper().toStdString().at(27);

                // Store version.
                this->m_version = utils::Tools::hexToAscii(pVersion);
            }
            break;
        case ABSTRACT_MEI9520E_GET_NAMEDEVICE:
            qDebug() << "Name device " << data.toHex();
            break;
        case ABSTRACT_MEI9520E_GET_PRODUCTNAME:
            qDebug() << "Product name " << data.toHex();
            break;
        default:
            break;
        }
    }

    bool AbstractMei9520e::getDataIsOk() {
        return m_getDataIsOk;
    }

    bool AbstractMei9520e::poll() {
        QLOG_INFO() << "AbstractMei9520e: Calling command poll...";

        QByteArray cmd;
        cmd.resize(5);
        cmd[0] = 0x02;
        cmd[1] = 0x00;
        cmd[2] = 0x01;
        cmd[3] = 0xE5;
        cmd[4] = 0x18;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command.
        this->getData(ABSTRACT_MEI9520E_GET_MONEY);
        return true;
    }

    void AbstractMei9520e::clearCoinLoadedValue() {
        this->m_statusStoreCoinValue = 0;
    }

    unsigned int AbstractMei9520e::printCoinLoadedValue() {
        return this->m_statusStoreCoinValue;
    }

    bool AbstractMei9520e::version(int tVersion) {
        if (!AbstractMei9520e::comIsOpen()) {
            QLOG_ERROR() << "AbstractMei9520e: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractMei9520e: Calling command version...";

        QByteArray cmd;

        if (tVersion == 0) {
            // Software version.
            cmd.resize(5);
            cmd[0] = 0x02;
            cmd[1] = 0x00;
            cmd[2] = 0x01;
            cmd[3] = 0xF1;
            cmd[4] = 0x0C;

            this->m_serial->writeData(cmd, cmd.size(), 100);
            this->getData(ABSTRACT_MEI9520E_GET_VERSION);
            cmd.clear();
        } else if (tVersion == 1) {
            // Product name.
            cmd.resize(5);
            cmd[0] = 0x02;
            cmd[1] = 0x00;
            cmd[2] = 0x01;
            cmd[3] = 0xF4;
            cmd[4] = 0x09;

            this->m_serial->writeData(cmd, cmd.size(), 100);
            this->getData(ABSTRACT_MEI9520E_GET_PRODUCTNAME);
            cmd.clear();
        } else if (tVersion == 2) {
            // Name device.
            cmd.resize(5);
            cmd[0] = 0x02;
            cmd[1] = 0x00;
            cmd[2] = 0x01;
            cmd[3] = 0xF5;
            cmd[4] = 0x08;

            this->m_serial->writeData(cmd, cmd.size(), 100);
            this->getData(ABSTRACT_MEI9520E_GET_NAMEDEVICE);
            cmd.clear();
        }
        return true;
    }

    const std::string AbstractMei9520e::printVersion() {
        return this->m_version;
    }

  }
}
