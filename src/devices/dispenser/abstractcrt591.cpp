/**
 * @file abstractcrt591.cpp
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

#include <devices/dispenser/abstractcrt591.hpp>
#include <utils/log.hpp>

// Start static variable and objects.
devices::dispenser::AbstractCrt591* devices::dispenser::AbstractCrt591::m_instance = 0;
bool devices::dispenser::AbstractCrt591::m_ComIsOpen = false;

namespace devices {
    namespace dispenser {

        AbstractCrt591* AbstractCrt591::getInstance() {
            if (m_instance == 0) {
               m_instance = new AbstractCrt591();
            }
            return m_instance;
        }

        AbstractCrt591::AbstractCrt591() : m_serialNumber(""), m_readDataBlock(""),
            m_samAPDU(""), m_version(""), m_getDataIsOk(false), m_cardStatusLevel(0),
            m_samCardPresent(false), m_rejectBox(false) {
            QLOG_INFO() << "Constructor class:" << "AbstractCrt591";

            // Instance class.
            this->m_serial = new utils::SerialPort;
        }

        AbstractCrt591::~AbstractCrt591() {
            QLOG_INFO() << "Destructor class:" << "AbstractCrt591";
            if (this->comIsOpen()) {
                this->close();
            }

            AbstractCrt591::m_instance = 0;
            AbstractCrt591::m_ComIsOpen = false;
        }

        bool AbstractCrt591::open(std::string comport, unsigned int baudrate,
                                  unsigned int databit, unsigned int tparity,
                                  unsigned int stopbit, unsigned int flowcontrol) {
            bool ok = this->m_serial->open(comport, baudrate, databit,
                                           tparity, stopbit, flowcontrol);

            if (ok) {
                QLOG_INFO() << "AbstractCrt591: open connection successful.";
                AbstractCrt591::m_ComIsOpen = true;

                QLOG_INFO() << "AbstractCrt591: Calling command open...";
                QByteArray cmd;
                cmd.resize(9);
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0x31;
                cmd[6] = 0x30;
                cmd[7] = 0x03;
                cmd[8] = 0xB0;

                // Send commands.
                this->m_serial->writeData(cmd, cmd.size(), 100);

                // Get response command.
                this->getData(ABSTRACT_CRT591_GET_STATUS_OPEN);

                if (!getDataIsOk()) {
                    QLOG_ERROR() << "AbstractCrt591: Error execute command open in dispenser.";
                }

                bool init = this->init(ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION);

                if (!init) {
                    QLOG_ERROR() << "AbstractCrt591: Error execute command init in dispenser.";
                    this->close();
                    return false;
                }

                cmd.clear();
                cmd.resize(9);
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0xDC;
                cmd[6] = 0x31;
                cmd[7] = 0x03;
                cmd[8] = 0x5C;

                // Send commands.
                this->m_serial->writeData(cmd, cmd.size(), 100);

                qDebug() << "OK! INIT Dispenser.....";
                return true;
            } else {
                QLOG_ERROR() << "AbstractCrt591: Error open connection.";
                AbstractCrt591::m_ComIsOpen = false;
                return false;
            }
        }

        bool AbstractCrt591::close() {
            bool ok = this->m_serial->close();

            if (ok) {
                QLOG_INFO() << "AbstractCrt591: close connection successful.";
                return true;
            } else {
                QLOG_ERROR() << "AbstractCrt591: Error close connection.";
                return false;
            }
        }

        bool AbstractCrt591::comIsOpen() {
            return AbstractCrt591::m_ComIsOpen;
        }

        bool AbstractCrt591::init(eAbtractCrt591Initialize initialize) {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QByteArray cmd;
            cmd.resize(9);

            switch (initialize) {
            case ABSTRACT_CRT591_INIT_MOVE_CARD_TO_HOLDING_POSITION:
                QLOG_WARN() << "AbstractCrt591: Method not implemented.";
                break;
            case ABSTRACT_CRT591_INIT_CAPTURE_CARD:
                QLOG_WARN() << "AbstractCrt591: Method not implemented.";
                break;
            case ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION:
                QLOG_INFO() << "AbstractCrt591: Calling command init not move card...";
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0x30;
                cmd[6] = 0x33;
                cmd[7] = 0x03;
                cmd[8] = 0xB2;
                break;
            case ABSTRACT_CRT591_INIT_MOVE_CARD_TO_HOLDING_POSITION_RCW:
                QLOG_WARN() << "AbstractCrt591: Method not implemented.";
                break;
            case ABSTRACT_CRT591_INIT_CAPTURE_CARD_RCW:
                QLOG_WARN() << "AbstractCrt591: Method not implemented.";
                break;
            case ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION_RCW:
                QLOG_WARN() << "AbstractCrt591: Method not implemented.";
                break;
            default:
                break;
            }

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 100);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_STATUS_INIT);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command init status.";
                return false;
            }
            return true;
        }

        void AbstractCrt591::getData(eAbtractCrt591GetData cmd) {
            const QByteArray data = m_serial->getData();

            if (data.isEmpty()) {
                QLOG_WARN() << "AbstractCrt591: Get data package is invalid.";
                m_getDataIsOk = false;
                return;
            }

            // Check command is ok.
            if (data.at(5) == QChar(0x50)) {
                QLOG_INFO() << "AbstractCrt591: Get data 0x50 command ok.";
                m_getDataIsOk = true;
            } else if(data.at(5) == QChar(0x4E)) {
                QLOG_ERROR() << "AbstractCrt591: Get data 0x4E command error.";
                m_getDataIsOk = false;
                return;
            }

            switch (cmd) {
            case ABSTRACT_CRT591_GET_STATUS_OPEN:
                QLOG_INFO() << "AbstractCrt591: Get status open.";
                break;
            case ABSTRACT_CRT591_GET_STATUS_INIT:
                QLOG_INFO() << "AbstractCrt591: Get status init.";
                break;
            case ABSTRACT_CRT591_GET_STATUS_POSITION:
                QLOG_INFO() << "AbstractCrt591: Get status position.";
                break;
            case ABSTRACT_CRT591_GET_MIFARE_SERIAL_CARD_NUMBER:
                QLOG_INFO() << "AbstractCrt591: Get status mifare serial card number.";

                if (!data.isEmpty()) {
                    m_serialNumber.clear();

                    m_serialNumber += data.toHex().toUpper().toStdString().at(30);
                    m_serialNumber += data.toHex().toUpper().toStdString().at(31);
                    m_serialNumber += " ";

                    m_serialNumber += data.toHex().toUpper().toStdString().at(32);
                    m_serialNumber += data.toHex().toUpper().toStdString().at(33);
                    m_serialNumber += " ";

                    m_serialNumber += data.toHex().toUpper().toStdString().at(34);
                    m_serialNumber += data.toHex().toUpper().toStdString().at(35);
                    m_serialNumber += " ";

                    m_serialNumber += data.toHex().toUpper().toStdString().at(36);
                    m_serialNumber += data.toHex().toUpper().toStdString().at(37);
                }
                break;
            case ABSTRACT_CRT591_GET_STATUS_SAM_CARD_ACTIVATED:
                QLOG_INFO() << "AbstractCrt591: Get status SAM activated.";
                break;
            case ABSTRACT_CRT591_GET_STATUS_SAM_CARD:
                QLOG_INFO() << "AbstractCrt591: Get status SAM card.";

                if (data.contains(0xC5) || data.contains(0xC6)) {
                    QLOG_INFO() << "AbstractCrt591: Card SAM is present.";
                    this->m_samCardPresent = true;
                } else if(data.contains(0xC4)) {
                    QLOG_INFO() << "AbstractCrt591: Card SAM is not present.";
                    this->m_samCardPresent = false;
                } else {
                    QLOG_INFO() << "AbstractCrt591: Card SAM is not present.";
                    this->m_samCardPresent = false;
                }
                break;
            case ABSTRACT_CRT591_GET_STATUS_CARD_LEVEL:
                QLOG_INFO() << "AbstractCrt591: Get status card tower level.";

                if (data.at(9) == QChar(0x32)) {
                    QLOG_INFO() << "AbstractCrt591: Hight level card in the stacker.";
                    this->m_cardStatusLevel = 2;
                } else if (data.at(9) == QChar(0x30)) {
                    QLOG_INFO() << "AbstractCrt591: No card in stacker.";
                    this->m_cardStatusLevel = 0;
                } else if (data.at(9) == QChar(0x31)) {
                    QLOG_INFO() << "AbstractCrt591: Low level card in the stacker.";
                    this->m_cardStatusLevel = 1;
                }
                break;
            case ABSTRACT_CRT591_GET_STATUS_REJECT_BOX:
                QLOG_INFO() << "AbstractCrt591: Get status reject box.";

                if (data.at(10) == QChar(0x31)) {
                    QLOG_INFO() << "AbstractCrt591: No card in reject box.";
                    this->m_rejectBox = false;
                } else if (data.at(10) == QChar(0x30)) {
                    QLOG_INFO() << "AbstractCrt591: Exist cards in reject box.";
                    this->m_rejectBox = true;
                }
                break;
            case ABSTRACT_CRT591_GET_VERSION:
                QLOG_INFO() << "AbstractCrt591: Get status version.";
                this->m_version = "CRT-591";
                break;
            case ABSTRACT_CRT591_GET_STATUS_SAM_APDU:
                QLOG_INFO() << "AbstractCrt591: Get status SAM APDU.";
                m_samAPDU.clear();

                m_samAPDU += data.toHex().toUpper().toStdString().at(22);
                m_samAPDU += data.toHex().toUpper().toStdString().at(23);
                m_samAPDU += " ";

                m_samAPDU += data.toHex().toUpper().toStdString().at(24);
                m_samAPDU += data.toHex().toUpper().toStdString().at(25);
                break;
            case ABSTRACT_CRT591_GET_STATUS_LOAD_KEY:
                QLOG_INFO() << "AbstractCrt591: Get status load key.";
                if (data.contains(0x90)) {
                    QLOG_INFO() << "AbstractCrt591: Execute load key successful.";
                    m_getDataIsOk = true;

                } else {
                    QLOG_ERROR() << "AbstractCrt591: Error execute load key.";
                    m_getDataIsOk = false;
                }
                break;
            case ABSTRACT_CRT591_GET_STATUS_READ_BLOCK:
                QLOG_INFO() << "AbstractCrt591: Get status read block.";
                m_readDataBlock.clear();

                if (data.contains(0x90)) {
                    QLOG_INFO() << "AbstractCrt591: Execute read block data successful.";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(22);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(23);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(24);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(25);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(26);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(27);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(28);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(29);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(30);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(31);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(32);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(33);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(34);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(35);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(36);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(37);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(38);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(39);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(40);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(41);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(42);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(43);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(44);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(45);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(46);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(47);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(48);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(49);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(50);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(51);
                    m_readDataBlock += " ";

                    m_readDataBlock += data.toHex().toUpper().toStdString().at(52);
                    m_readDataBlock += data.toHex().toUpper().toStdString().at(53);
                } else {
                    QLOG_ERROR() << "AbstractCrt591: Error execute read block data.";
                    m_getDataIsOk = false;
                }
                break;
            default:
                break;
            }
        }

        bool AbstractCrt591::getDataIsOk() {
            return this->m_getDataIsOk;
        }

        bool AbstractCrt591::cardStatus(bool checkRejectBox) {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QLOG_INFO() << "AbstractCrt591: Calling command cardStatus...";

            QByteArray cmd;
            cmd.resize(9);
            cmd[0] = 0xF2;
            cmd[1] = 0x00;
            cmd[2] = 0x00;
            cmd[3] = 0x03;
            cmd[4] = 0x43;
            cmd[5] = 0x31;
            cmd[6] = 0x30;
            cmd[7] = 0x03;
            cmd[8] = 0xB0;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 300);

            if (!checkRejectBox) {
                // Get response command.
                this->getData(ABSTRACT_CRT591_GET_STATUS_CARD_LEVEL);
            } else {
                // Get response command.
                this->getData(ABSTRACT_CRT591_GET_STATUS_REJECT_BOX);
            }

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command card status (level).";
                return false;
            }

            cmd.clear();
            return true;
        }

        bool AbstractCrt591::sensorStatus() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QLOG_INFO() << "AbstractCrt591: Calling command sensorStatus...";

            QByteArray cmd;
            cmd.resize(9);
            cmd[0] = 0xF2;
            cmd[1] = 0x00;
            cmd[2] = 0x00;
            cmd[3] = 0x03;
            cmd[4] = 0x43;
            cmd[5] = 0x31;
            cmd[6] = 0x31;
            cmd[7] = 0x03;
            cmd[8] = 0xB1;

            this->m_serial->writeData(cmd, cmd.size(), 10);
            return true;
        }

        bool AbstractCrt591::reset() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QLOG_INFO() << "AbstractCrt591: Calling command reset...";
            this->init(ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION);

            return true;
        }

        bool AbstractCrt591::cardMovePosition(eAbtractCrt591CardMove cardMove) {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            unsigned int wait = 100;

            QByteArray cmd;
            cmd.resize(9);

            switch (cardMove) {
            case ABSTRACT_CRT591_MOVE_CARD_TO_HOLDING_POSITION:
                QLOG_INFO() << "AbstractCrt591: Calling command move card to holding position...";
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0x32;
                cmd[6] = 0x39;
                cmd[7] = 0x03;
                cmd[8] = 0xBA;
                wait = 2000;
                break;
            case ABSTRACT_CRT591_MOVE_CARD_TO_IC_POSITION:
                QLOG_INFO() << "AbstractCrt591: Calling command move card to IC position...";
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0x32;
                cmd[6] = 0x31;
                cmd[7] = 0x03;
                cmd[8] = 0xB2;
                wait = 2000;
                break;
            case ABSTRACT_CRT591_MOVE_CARD_TO_RF_POSITION:
                QLOG_INFO() << "AbstractCrt591: Calling command move card RF holding position...";
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0x32;
                cmd[6] = 0x32;
                cmd[7] = 0x03;
                cmd[8] = 0xB1;
                wait = 2000;
                break;
            case ABSTRACT_CRT591_MOVE_CARD_TO_ERROR_BIN:
                QLOG_INFO() << "AbstractCrt591: Calling command move card to error bin position...";
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0x32;
                cmd[6] = 0x33;
                cmd[7] = 0x03;
                cmd[8] = 0xB0;
                wait = 2100;
                break;
            case ABSTRACT_CRT591_MOVE_CARD_TO_GATE_EJECT:
                QLOG_INFO() << "AbstractCrt591: Calling command move card to gate eject position...";
                cmd[0] = 0xF2;
                cmd[1] = 0x00;
                cmd[2] = 0x00;
                cmd[3] = 0x03;
                cmd[4] = 0x43;
                cmd[5] = 0x32;
                cmd[6] = 0x30;
                cmd[7] = 0x03;
                cmd[8] = 0xB3;
                wait = 2000;
                break;
            default:
                break;
            }

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), wait);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_STATUS_POSITION);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command card move position.";
                return false;
            }

            return true;
        }

        int AbstractCrt591::printStatusCardLevel() const {
            // 0  = No card in stacker;
            // 1  = Low card in stacker (level);
            // 2  = Hight card in stacker (level);
            // -1 = Error in card or execute command.
            return AbstractCrt591::m_cardStatusLevel;
        }

        bool AbstractCrt591::isCardRejectBox() const {
            // true  = Case true exsit card in reject box;
            // false = No exist card in reject box;
            return this->m_rejectBox;
        }

        bool AbstractCrt591::cardMifareActivate() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QByteArray cmd;
            cmd.resize(11);
            cmd[0]  = 0xF2;
            cmd[1]  = 0x00;
            cmd[2]  = 0x00;
            cmd[3]  = 0x05;
            cmd[4]  = 0x43;
            cmd[5]  = 0x60;
            cmd[6]  = 0x30;
            cmd[7]  = 0x41;
            cmd[8]  = 0x42;
            cmd[9]  = 0x03;
            cmd[10] = 0xE4;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 800);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_MIFARE_SERIAL_CARD_NUMBER);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command card mifare activate.";
                return false;
            }

            return true;
        }

        std::string AbstractCrt591::printCardMifareSerialNumber() const {
            return this->m_serialNumber;
        }

        std::string AbstractCrt591::printCardMifareReadDataBlock() const {
            return this->m_readDataBlock;
        }

        bool AbstractCrt591::cardMifareDesactivate() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QByteArray cmd;
            cmd.resize(9);
            cmd[0]  = 0xF2;
            cmd[1]  = 0x00;
            cmd[2]  = 0x00;
            cmd[3]  = 0x03;
            cmd[4]  = 0x43;
            cmd[5]  = 0x60;
            cmd[6]  = 0x31;
            cmd[7]  = 0x03;
            cmd[8]  = 0xE0;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 100);

            return true;
        }

        bool AbstractCrt591::loadKey(std::vector<unsigned char> key) {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            // F2 00 00 0E 43 60 33 00 20 00 00 06 FF FF FF FF FF FF 03 C9
            QByteArray cmd;
            cmd.resize(20);
            cmd[0]  = 0xF2;
            cmd[1]  = 0x00;
            cmd[2]  = 0x00;
            cmd[3]  = 0x0E;
            cmd[4]  = 0x43;
            cmd[5]  = 0x60;
            cmd[6]  = 0x33;
            cmd[7]  = 0x00;
            cmd[8]  = 0x20;
            cmd[9]  = 0x00;
            cmd[10] = 0x00;
            cmd[11] = 0x06;

            // Key.
            cmd[12] = key[0];
            cmd[13] = key[1];
            cmd[14] = key[2];
            cmd[15] = key[3];
            cmd[16] = key[4];
            cmd[17] = key[5];

            // Part cmd.
            cmd[18] = 0x03;
            cmd[19] = 0xC9;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 400);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_STATUS_LOAD_KEY);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command load key.";
                return false;
            }

            return true;
        }

        bool AbstractCrt591::mifareReadDataBlock(unsigned char keyIndex, std::vector<unsigned char> blockSN) {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            if (blockSN.empty()) {}
            if (keyIndex) {}

            QByteArray cmd;
            cmd.resize(14);
            cmd[0]  = 0xF2;
            cmd[1]  = 0x00;
            cmd[2]  = 0x00;
            cmd[3]  = 0x08;
            cmd[4]  = 0x43;
            cmd[5]  = 0x60;
            cmd[6]  = 0x33;
            cmd[7]  = 0x00;
            cmd[8]  = 0xB0;
            cmd[9]  = 0x00;
            cmd[10] = 0x01;
            cmd[11] = 0x01;
            cmd[12] = 0x03;
            cmd[13] = 0x59;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 300);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_STATUS_READ_BLOCK);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute reader block.";
                return false;
            }

            return true;
        }

        bool AbstractCrt591::mifareWriteDataBlock(unsigned char keyIndex, std::vector<unsigned char> blockSN) {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            if (keyIndex) {}

            QByteArray cmd;
            cmd.resize(30);
            cmd[0]  = 0xF2;
            cmd[1]  = 0x00;
            cmd[2]  = 0x00;
            cmd[3]  = 0x18;
            cmd[4]  = 0x43;
            cmd[5]  = 0x60;
            cmd[6]  = 0x33;
            cmd[7]  = 0x00;
            cmd[8]  = 0xD1;
            cmd[9]  = 0x00;
            cmd[10] = 0x01;
            cmd[11] = 0x01;

            // Data write.
            cmd[12] = blockSN[0];
            cmd[13] = blockSN[1];
            cmd[14] = blockSN[2];
            cmd[15] = blockSN[3];
            cmd[16] = blockSN[4];
            cmd[17] = blockSN[5];
            cmd[18] = blockSN[6];
            cmd[19] = blockSN[7];
            cmd[20] = blockSN[8];
            cmd[21] = blockSN[9];
            cmd[22] = blockSN[10];
            cmd[23] = blockSN[11];
            cmd[24] = blockSN[12];
            cmd[25] = blockSN[13];
            cmd[26] = blockSN[14];
            cmd[27] = blockSN[15];

            // Part cmd.
            cmd[28] = 0x03;
            cmd[29] = 0x28;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 800);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute reader block.";
                return false;
            }

            return true;
        }

        bool AbstractCrt591::samActivate() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QByteArray cmd;
            cmd.resize(10);
            cmd[0] = 0xF2;
            cmd[1] = 0x00;
            cmd[2] = 0x00;
            cmd[3] = 0x04;
            cmd[4] = 0x43;
            cmd[5] = 0x52;
            cmd[6] = 0x40;
            cmd[7] = 0x30;
            cmd[8] = 0x03;
            cmd[9] = 0x94;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 100);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_STATUS_SAM_CARD_ACTIVATED);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command sam card activate.";
                return false;
            }

            return true;
        }

        bool AbstractCrt591::samIsPresent() {
            return this->m_samCardPresent;
        }

        bool AbstractCrt591::samCouldReset() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QByteArray cmd;
            cmd.resize(10);
            cmd[0] = 0xF2;
            cmd[1] = 0x00;
            cmd[2] = 0x00;
            cmd[3] = 0x04;
            cmd[4] = 0x43;
            cmd[5] = 0x52;
            cmd[6] = 0x30;
            cmd[7] = 0x33;
            cmd[8] = 0x03;
            cmd[9] = 0xE7;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 100);
            return true;
        }

        bool AbstractCrt591::samStatus() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QByteArray cmd;
            cmd.resize(9);
            cmd[0] = 0xF2;
            cmd[1] = 0x00;
            cmd[2] = 0x00;
            cmd[3] = 0x03;
            cmd[4] = 0x43;
            cmd[5] = 0x52;
            cmd[6] = 0x32;
            cmd[7] = 0x03;
            cmd[8] = 0xD1;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 100);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_STATUS_SAM_CARD);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command sam card status.";
                return false;
            }
            return true;
        }

        bool AbstractCrt591::samSendAPDU() {
            if (!samActivate()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute SAM active.";
            }

            QByteArray cmd;
            cmd.resize(16);
            cmd[0]  = 0xF2;
            cmd[1]  = 0x00;
            cmd[2]  = 0x00;
            cmd[3]  = 0x0A;
            cmd[4]  = 0x43;
            cmd[5]  = 0x52;
            cmd[6]  = 0x39;
            cmd[7]  = 0x00;
            cmd[8]  = 0xA4;
            cmd[9]  = 0x00;
            cmd[10] = 0x00;
            cmd[11] = 0x02;
            cmd[12] = 0x3F;
            cmd[13] = 0x00;
            cmd[14] = 0x03;
            cmd[15] = 0x4A;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 100);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_STATUS_SAM_APDU);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command sam send APDU.";
                return false;
            }
            return true;
        }

        std::string AbstractCrt591::printCardSamAPDU() const {
            return this->m_samAPDU;
        }

        bool AbstractCrt591::version() {
            if (!AbstractCrt591::comIsOpen()) {
                QLOG_ERROR() << "AbstractCrt591: Error connection.";
                return false;
            }

            QByteArray cmd;
            cmd.resize(9);
            cmd[0] = 0xF2;
            cmd[1] = 0x00;
            cmd[2] = 0x00;
            cmd[3] = 0x03;
            cmd[4] = 0x43;
            cmd[5] = 0x30;
            cmd[6] = 0x33;
            cmd[7] = 0x03;
            cmd[8] = 0xB2;

            // Send commands.
            this->m_serial->writeData(cmd, cmd.size(), 100);

            // Get response command.
            this->getData(ABSTRACT_CRT591_GET_VERSION);

            if (!getDataIsOk()) {
                QLOG_ERROR() << "AbstractCrt591: Error execute command version.";
                return false;
            }

            return true;
        }

        const std::string AbstractCrt591::printVersion() {
            this->version();
            this->m_version = "CRT-591";
            return this->m_version;
        }

    }
}
