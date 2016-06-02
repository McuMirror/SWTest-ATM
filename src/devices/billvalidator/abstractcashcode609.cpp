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

#include <devices/billvalidator/abstractcashcode609.hpp>
#include <utils/log.hpp>

// CashCode commands.
static const unsigned char ESCROW_POSITION = 0x80;
static const unsigned char BILL_STACKED = 0x81;
static const unsigned char BILL_RETURNED = 0x82;

// Start static variable and objects.
devices::billvalidator::AbstractCashCode609* devices::billvalidator::AbstractCashCode609::m_instance = 0;
bool devices::billvalidator::AbstractCashCode609::m_ComIsOpen = false;

namespace devices {
  namespace billvalidator {

    AbstractCashCode609* AbstractCashCode609::getInstance() {
        if (m_instance == 0) {
            m_instance = new AbstractCashCode609();
        }
        return m_instance;
    }

    AbstractCashCode609::AbstractCashCode609()
        : m_statusStoreNoteValue(0), m_billReturning(false), m_version("") {       
        QLOG_INFO() << "Constructor class:" << "AbstractCashCode609";

        // Instance class.
        this->m_serial = new utils::SerialPort;
    }

    AbstractCashCode609::~AbstractCashCode609() {
        QLOG_INFO() << "Destructor class:" << "AbstractCashCode609";
        if (this->comIsOpen()) {
           this->close();
        }
        AbstractCashCode609::m_instance = 0;
    }

    bool AbstractCashCode609::open(std::string comport, unsigned int baudrate,
                                   unsigned int databit, unsigned int tparity,
                                   unsigned int stopbit, unsigned int flowcontrol) {
        bool ok = this->m_serial->open(comport, baudrate, databit,
                                       tparity, stopbit, flowcontrol);

        if (ok) {
            QLOG_INFO() << "AbstractCashCode609: open connection successful.";
            AbstractCashCode609::m_ComIsOpen = true;
            return true;
        } else {
            QLOG_ERROR() << "AbstractCashCode609: Error open connection.";
            AbstractCashCode609::m_ComIsOpen = false;
            return false;
        }
    }

    bool AbstractCashCode609::close() {
        bool ok = this->m_serial->close();

        if (ok) {
            QLOG_INFO() << "AbstractCashCode609: close connection successful.";
            return true;
        } else {
            QLOG_ERROR() << "AbstractCashCode609: Error close connection.";
            return false;
        }
    }

    bool AbstractCashCode609::comIsOpen() {
        return AbstractCashCode609::m_ComIsOpen;
    }

    unsigned int AbstractCashCode609::printNoteLoadedValue() {
        return this->m_statusStoreNoteValue;
    }

    void AbstractCashCode609::clearNoteLoadedValue() {
        this->m_statusStoreNoteValue = 0;
    }

    void AbstractCashCode609::getData(eAbstractCashCode609GetData cmd) {
        const QByteArray data = m_serial->getData();

        if (data.isEmpty()) {
            QLOG_WARN() << "AbstractCashCode609: Get data package is invalid.";
            m_getDataIsOk = false;
            return; // Avoid segmentation fault.
        }

        switch (cmd) {
        case ABSTRACT_CASHCODE_GET_MONEY:
            if (data.contains(ESCROW_POSITION)) {
                if (data.at(4) == QChar(0x01)) {
                    this->m_statusStoreNoteValue = 1;
                } else if (data.at(4) == QChar(0x02)) {
                    this->m_statusStoreNoteValue = 2;
                } else if (data.at(4) == QChar(0x03)) {
                    this->m_statusStoreNoteValue = 3;
                } else if (data.at(4) == QChar(0x04)) {
                    this->m_statusStoreNoteValue = 4;
                } else if (data.at(4) == QChar(0x05)) {
                    this->m_statusStoreNoteValue = 5;
                } else if (data.at(4) == QChar(0x06)) {
                    this->m_statusStoreNoteValue = 6;
                } else {
                    this->m_statusStoreNoteValue = 0;
                }
            }
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_MONEY:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_BILLTABLE:
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_BILLTABLE:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_VERSION:
            this->m_version = "CashCode SM-BR1615-21KE36023621";
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_VERSION:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_STATUS:
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_STATUS:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_BILLTYPES:
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_BILLTYPES:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_STACKBILL:
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_STACKBILL:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_RETURNBILL:
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_RETURNBILL:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_RESET:
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_RESET:" << data.toHex();
            break;
        case ABSTRACT_CASHCODE_GET_HOLD:
            QLOG_INFO() << "ABSTRACT_CASHCODE_GET_HOLD:" << data.toHex();
            break;
        }
    }

    bool AbstractCashCode609::getDataIsOk() {
        return m_getDataIsOk;
    }

    bool AbstractCashCode609::reset() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command reset...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x30;
        cmd[4] = 0x41;
        cmd[5] = 0xB3;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_RESET);
        return true;
    }

    bool AbstractCashCode609::poll() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command poll...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x33;
        cmd[4] = 0xDA;
        cmd[5] = 0x81;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_MONEY);
        return true;
    }

    bool AbstractCashCode609::enableBillTypes() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command enableBillTypes...";

        QByteArray cmd;
        cmd.resize(12);
        cmd[0]  = 0x02;
        cmd[1]  = 0x03;
        cmd[2]  = 0x0C;
        cmd[3]  = 0x34;
        cmd[4]  = 0xFF;
        cmd[5]  = 0xFF;
        cmd[6]  = 0xFF;
        cmd[7]  = 0xFF;
        cmd[8]  = 0xFF;
        cmd[9]  = 0xFF;
        cmd[10] = 0xFE;
        cmd[11] = 0xF7;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_BILLTYPES);
        return true;
    }

    bool AbstractCashCode609::hold() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command hold...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x38;
        cmd[4] = 0x09;
        cmd[5] = 0x3F;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_HOLD);
        return true;
    }

    bool AbstractCashCode609::returnBill() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command returnBill...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x30;
        cmd[4] = 0x41;
        cmd[5] = 0xB3;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_RETURNBILL);
        return true;
    }

    bool AbstractCashCode609::billReturning() {
        return this->m_billReturning;
    }

    bool AbstractCashCode609::stackBill() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command stackBill...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x35;
        cmd[4] = 0xEC;
        cmd[5] = 0xE4;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_STACKBILL);
        return true;
    }

    bool AbstractCashCode609::version() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command version...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x37;
        cmd[4] = 0xFE;
        cmd[5] = 0xC7;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_VERSION);
        return true;
    }

    const std::string AbstractCashCode609::printVersion() {
        this->version();
        this->m_version = "SM-BR-1615";
        return this->m_version;
    }

    bool AbstractCashCode609::getBillTable() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command getBillTable...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x41;
        cmd[4] = 0x4F;
        cmd[5] = 0xD1;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_BILLTABLE);
        return true;
    }

    bool AbstractCashCode609::statusRequest() {
        if (!AbstractCashCode609::comIsOpen()) {
            QLOG_ERROR() << "AbstractCashCode609: Error connection.";
            return false;
        }

        QLOG_INFO() << "AbstractCashCode609: Calling command statusRequest...";

        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x31;
        cmd[4] = 0xC8;
        cmd[5] = 0xA2;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData(ABSTRACT_CASHCODE_GET_STATUS);
        return true;
    }

  }
}
