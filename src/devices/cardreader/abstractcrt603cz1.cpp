/**
 * @file abstractcrt603cz1.cpp
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

#include <devices/cardreader/abstractcrt603cz1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

// Store out log error.
static char* m_out;

#ifdef Q_OS_WIN
static char *pcsc_stringify_error(LONG rv) {
    static char out[20];
    std::snprintf(out, sizeof(out), "0x%08X", (unsigned int)rv);
    m_out = out;
    return out;
}
#endif

/// Check command is success.
#define SCARD_CHECK_COMMAND(f, rv) \
    pcsc_stringify_error(rv);\
    if (SCARD_S_SUCCESS != rv) { \
        QLOG_ERROR() << "AbstractCrt603Cz1: Command failed" << f << "code =" << m_out;\
    } else {\
        QLOG_INFO() << "AbstractCrt603Cz1: Command successful" << f << "code = 9090";\
    }\

// Start static variable and objects.
devices::cardreader::AbstractCrt603Cz1* devices::cardreader::AbstractCrt603Cz1::m_instance = 0;
devices::cardreader::AbstractCrt603Cz1Sam* devices::cardreader::AbstractCrt603Cz1Sam::m_instance = 0;

namespace devices {
  namespace cardreader {

    AbstractCrt603Cz1* AbstractCrt603Cz1::getInstance() {
        if (m_instance == 0) {
            m_instance = new AbstractCrt603Cz1();
        }
        return m_instance;
    }

    AbstractCrt603Cz1::AbstractCrt603Cz1() : m_serialNumber(""), m_readDataBlock("") {
        QLOG_INFO() << "Constructor class:" << "AbstractCrt603Cz1";
    }

    AbstractCrt603Cz1::~AbstractCrt603Cz1() {
        QLOG_INFO() << "Destructor class:" << "AbstractCrt603Cz1";
        AbstractCrt603Cz1::m_instance = 0;
    }

    bool AbstractCrt603Cz1::establishContext() {
        LONG rc = SCardEstablishContext(SCARD_SCOPE_USER,
                                        NULL,
                                        NULL,
                                        &this->m_context);
        SCARD_CHECK_COMMAND("SCardEstablishContext", rc);
        return (rc != 0) ?  false : true;
    }

    bool AbstractCrt603Cz1::getReaders() {
        if (!this->establishContext()) {
            return false;
        }

        LPTSTR pReader = 0;
        LONG   rc = 0;
        DWORD  cch = SCARD_AUTOALLOCATE;

        rc = SCardListReaders(this->m_context,
                              NULL,
                              (LPTSTR)&this->m_mszReaders,
                              &cch);

        SCARD_CHECK_COMMAND("SCardListReaders", rc);

        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error get scard list reader.";
            return false;
        }

        pReader = this->m_mszReaders;
        while ('\0' != *pReader) {
            // Store readers values.
            this->m_listReaders.push_back(pReader);
            // Advance to the next value.
            pReader = pReader + wcslen((wchar_t *)pReader) + 1;
        }

        return true;
    }

    std::vector<LPTSTR> AbstractCrt603Cz1::printReaders() {
        return this->m_listReaders;
    }

    bool AbstractCrt603Cz1::open() {
        if (!this->getReaders()) {
            return false;
        }

        DWORD dwActiveProtocol = 0;
        LONG rc = SCardConnect(this->m_context,
                               (LPTSTR)this->printReaders()[0],
                               SCARD_SHARE_SHARED,
                               SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
                               &this->m_cardHandle, &dwActiveProtocol);
        SCARD_CHECK_COMMAND("SCardConnect", rc);
        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error open connection.";
            return false;
        }

        switch (dwActiveProtocol) {
        case SCARD_PROTOCOL_T0:
            this->m_pioSendPci = *SCARD_PCI_T0;
            break;
        case SCARD_PROTOCOL_T1:
            this->m_pioSendPci = *SCARD_PCI_T1;
            break;
        default:
            this->m_pioSendPci = *SCARD_PCI_T1;
            break;
        }

        QLOG_INFO() << "AbstractCrt603Cz1: open connection successful.";
        return true;
    }

    bool AbstractCrt603Cz1::close() {
        long rc = SCardDisconnect(this->m_cardHandle, SCARD_LEAVE_CARD);
        SCARD_CHECK_COMMAND("SCardDisconnect", rc);
        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error close connection.";
            return false;
        }

#ifdef SCARD_AUTOALLOCATE
        rc = SCardFreeMemory(this->m_context, this->m_mszReaders);
        SCARD_CHECK_COMMAND("SCardFreeMemory", rc);
        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error close connection free memory.";
            return false;
        }
#else
        std::free(this->m_mszReaders);
#endif
        rc = SCardReleaseContext(this->m_context);
        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error close connection release context.";
            return false;
        }
        SCARD_CHECK_COMMAND("SCardReleaseContext", rc);
        return true;
    }

    bool AbstractCrt603Cz1::cardStatus() {
         WCHAR szReader[200];
         DWORD cch = 200;
         BYTE  bAttr[32];
         DWORD cByte = 32;
         DWORD dwState, dwProtocol;

         LONG rc = SCardStatus(this->m_cardHandle,
                               szReader,
                               &cch,
                               &dwState,
                               &dwProtocol,
                               (LPBYTE)&bAttr,
                               &cByte);

        SCARD_CHECK_COMMAND("SCardStatus", rc);
        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command status card.";
            return false;
        }

        //printf("%S\n", szReader);
        switch (dwState) {
        case SCARD_ABSENT:
            QLOG_INFO() << "AbstractCrt603Cz1: Card absent.";
            break;
        case SCARD_PRESENT:
            QLOG_INFO() << "AbstractCrt603Cz1: Card present.";
            break;
        case SCARD_SWALLOWED:
            QLOG_INFO() << "AbstractCrt603Cz1: Card swallowed.";
            break;
        case SCARD_POWERED:
            QLOG_INFO() << "AbstractCrt603Cz1: Card has power.";
            break;
        case SCARD_NEGOTIABLE:
            QLOG_INFO() << "AbstractCrt603Cz1: Card reset and waiting PTS negotiation.";
            break;
        case SCARD_SPECIFIC:
            QLOG_INFO() << "AbstractCrt603Cz1: Card has specific communication protocols set";
            break;
        default:
            QLOG_INFO() << "AbstractCrt603Cz1: Unknown or unexpected card state.";
            break;
        }

        QLOG_INFO() << "AbstractCrt603Cz1: Card status command successful.";
        return true;
    }

    bool AbstractCrt603Cz1::getCardMifareSerialNumber() {
        DWORD dwSendLength, dwRecvLength;
        BYTE pbRecvBuffer[16];
        BYTE pbSendBuffer[] = {(BYTE)0xFF, (BYTE)0xCA,(BYTE)0x00, (BYTE)0x00, (BYTE)0x00};

        dwSendLength = sizeof(pbSendBuffer);
        dwRecvLength = sizeof(pbRecvBuffer);

        SCARD_IO_REQUEST pioSendPci;
        pioSendPci.dwProtocol  = SCARD_PROTOCOL_T1;
        pioSendPci.cbPciLength = sizeof(pioSendPci);

        LONG rc = SCardTransmit(m_cardHandle,
                                &pioSendPci,
                                pbSendBuffer,
                                dwSendLength,
                                NULL,
                                pbRecvBuffer,
                                &dwRecvLength);

        SCARD_CHECK_COMMAND("SCardTransmit", rc);

        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command transmit in get serial number.";
            return false;
        }

        const std::string data = utils::Tools::hexToStr(pbRecvBuffer, 10);

        if (!data.empty()) {
            QLOG_INFO() << "AbstractCrt603Cz1: Get serial number command successful.";
            m_serialNumber.clear();

            m_serialNumber += std::toupper(data.at(0));
            m_serialNumber += std::toupper(data.at(1));
            m_serialNumber += " ";

            m_serialNumber += std::toupper(data.at(2));
            m_serialNumber += std::toupper(data.at(3));
            m_serialNumber += " ";

            m_serialNumber += std::toupper(data.at(4));
            m_serialNumber += std::toupper(data.at(5));
            m_serialNumber += " ";

            m_serialNumber += std::toupper(data.at(6));
            m_serialNumber += std::toupper(data.at(7));
            return true;
        }
        m_serialNumber.clear();
        return false;
    }

    std::string AbstractCrt603Cz1::printCardMifareSerialNumber() const {
        return this->m_serialNumber;
    }

    bool AbstractCrt603Cz1::loadKey(std::vector<unsigned char> key) {
        DWORD dwSendLength, dwRecvLength;
        BYTE pbRecvBuffer[16];

        if (key.empty()) {}

        BYTE pbSendBuffer[] = {(BYTE)0xFF, (BYTE)0x82, (BYTE)0x20, (BYTE)0x02, (BYTE)0x06,
                               (BYTE)0xFF, (BYTE)0xFF, (BYTE)0xFF, (BYTE)0xFF, (BYTE)0xFF, (BYTE)0xFF};

        dwSendLength = sizeof(pbSendBuffer);
        dwRecvLength = sizeof(pbRecvBuffer);

        SCARD_IO_REQUEST pioSendPci;
        pioSendPci.dwProtocol  = SCARD_PROTOCOL_T1;
        pioSendPci.cbPciLength = sizeof(pioSendPci);

        LONG rc = SCardTransmit(m_cardHandle,
                                &pioSendPci,
                                pbSendBuffer,
                                dwSendLength,
                                NULL,
                                pbRecvBuffer,
                                &dwRecvLength);

        SCARD_CHECK_COMMAND("SCardTransmit", rc);

        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command transmit load key.";
            return false;
        }

        QLOG_INFO() << "AbstractCrt603Cz1: Load key command successful.";
        return true;
    }

    bool AbstractCrt603Cz1::authSector() {
        DWORD dwSendLength, dwRecvLength;
        BYTE pbRecvBuffer[16];

        BYTE pbSendBuffer[] = {(BYTE)0xFF, (BYTE)0x86, (BYTE)0x00, (BYTE)0x00, (BYTE)0x05,
                               (BYTE)0x01, (BYTE)0x00, (BYTE)0x01, (BYTE)0x60, (BYTE)0x00};

        dwSendLength = sizeof(pbSendBuffer);
        dwRecvLength = sizeof(pbRecvBuffer);

        SCARD_IO_REQUEST pioSendPci;
        pioSendPci.dwProtocol  = SCARD_PROTOCOL_T1;
        pioSendPci.cbPciLength = sizeof(pioSendPci);

        LONG rc = SCardTransmit(m_cardHandle,
                                &pioSendPci,
                                pbSendBuffer,
                                dwSendLength,
                                NULL,
                                pbRecvBuffer,
                                &dwRecvLength);

        SCARD_CHECK_COMMAND("SCardTransmit", rc);

        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command auth sector.";
            return false;
        }

        QLOG_INFO() << "AbstractCrt603Cz1: Auth sector command successful.";
        return true;
    }

    bool  AbstractCrt603Cz1::mifareReadDataBlock(unsigned char keyIndex, std::vector<unsigned char> blockSN) {
        this->authSector();

        DWORD dwSendLength, dwRecvLength;
        BYTE pbRecvBuffer[1024];

        if (blockSN.empty()) {}
        if (keyIndex) {}

        BYTE pbSendBuffer[] = {(BYTE)0xFF, (BYTE)0xB0, (BYTE)0x00, (BYTE)0x01, (BYTE)0x10};

        dwSendLength = sizeof(pbSendBuffer);
        dwRecvLength = sizeof(pbRecvBuffer);

        SCARD_IO_REQUEST pioSendPci;
        pioSendPci.dwProtocol  = SCARD_PROTOCOL_T1;
        pioSendPci.cbPciLength = sizeof(pioSendPci);

        LONG rc = SCardTransmit(m_cardHandle,
                                &pioSendPci,
                                pbSendBuffer,
                                dwSendLength,
                                NULL,
                                pbRecvBuffer,
                                &dwRecvLength);

        SCARD_CHECK_COMMAND("SCardTransmit", rc);

        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command mifare read data block.";
            return false;
        }

        const std::string data = utils::Tools::hexToStr(pbRecvBuffer, 16);

        if (!data.empty()) {
            QLOG_INFO() << "AbstractCrt603Cz1: Mifare read block command successful.";
            m_readDataBlock.clear();

            m_readDataBlock += std::toupper(data.at(0));
            m_readDataBlock += std::toupper(data.at(1));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(2));
            m_readDataBlock += std::toupper(data.at(3));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(4));
            m_readDataBlock += std::toupper(data.at(5));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(6));
            m_readDataBlock += std::toupper(data.at(7));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(8));
            m_readDataBlock += std::toupper(data.at(9));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(10));
            m_readDataBlock += std::toupper(data.at(11));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(12));
            m_readDataBlock += std::toupper(data.at(13));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(14));
            m_readDataBlock += std::toupper(data.at(15));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(16));
            m_readDataBlock += std::toupper(data.at(17));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(18));
            m_readDataBlock += std::toupper(data.at(19));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(20));
            m_readDataBlock += std::toupper(data.at(21));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(22));
            m_readDataBlock += std::toupper(data.at(23));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(24));
            m_readDataBlock += std::toupper(data.at(25));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(26));
            m_readDataBlock += std::toupper(data.at(27));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(28));
            m_readDataBlock += std::toupper(data.at(29));
            m_readDataBlock += " ";

            m_readDataBlock += std::toupper(data.at(30));
            m_readDataBlock += std::toupper(data.at(31));
            return true;
        }

        m_readDataBlock.clear();
        return true;
    }

    std::string AbstractCrt603Cz1::printCardMifareReadDataBlock() const {
        return this->m_readDataBlock;
    }
    
    bool AbstractCrt603Cz1::mifareWriteDataBlock(unsigned char keyIndex, std::vector<unsigned char> blockSN) {
        this->authSector();

        DWORD dwSendLength, dwRecvLength;
        BYTE pbRecvBuffer[1024];

        if (keyIndex) {}

        BYTE pbSendBuffer[] = {(BYTE)0xFF, (BYTE)0xD6, (BYTE)0x00, (BYTE)0x01, (BYTE)0x10,
                               (BYTE)blockSN[0], (BYTE)blockSN[1], (BYTE)blockSN[2], (BYTE)blockSN[3], (BYTE)blockSN[4],
                               (BYTE)blockSN[5], (BYTE)blockSN[6], (BYTE)blockSN[7], (BYTE)blockSN[8], (BYTE)blockSN[9],
                               (BYTE)blockSN[10], (BYTE)blockSN[11], (BYTE)blockSN[12], (BYTE)blockSN[13], (BYTE)blockSN[14],
                               (BYTE)blockSN[15]};

        dwSendLength = sizeof(pbSendBuffer);
        dwRecvLength = sizeof(pbRecvBuffer);

        SCARD_IO_REQUEST pioSendPci;
        pioSendPci.dwProtocol  = SCARD_PROTOCOL_T1;
        pioSendPci.cbPciLength = sizeof(pioSendPci);

        LONG rc = SCardTransmit(m_cardHandle,
                                &pioSendPci,
                                pbSendBuffer,
                                dwSendLength,
                                NULL,
                                pbRecvBuffer,
                                &dwRecvLength);

        SCARD_CHECK_COMMAND("SCardTransmit", rc);

        if (rc != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command mifare write data block.";
            return false;
        }

        return true;
    }

    AbstractCrt603Cz1Sam* AbstractCrt603Cz1Sam::getInstance() {
        if (m_instance == 0) {
            m_instance = new AbstractCrt603Cz1Sam();
        }
        return m_instance;
    }

    AbstractCrt603Cz1Sam::AbstractCrt603Cz1Sam() {
        QLOG_INFO() << "Constructor class:" << "AbstractCrt603Cz1Sam";
    }

    AbstractCrt603Cz1Sam::~AbstractCrt603Cz1Sam() {
        QLOG_INFO() << "Destructor class:" << "AbstractCrt603Cz1";
        AbstractCrt603Cz1::m_instance = 0;
    }

    bool AbstractCrt603Cz1Sam::connectSamSlot(unsigned int slot) {
        BYTE slotN = 0x01;

        if (slot == 1) {
            slotN = 0x01;
        } else if (slot == 2) {
            slotN = 0x02;
        }

        SCARDCONTEXT context;
        SCARDHANDLE  cardHandle;
        SCARD_IO_REQUEST pioSendPci;

        SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &context);

        LPTSTR pmszReaders = NULL;
        LPTSTR pReader = NULL;
        DWORD  cch = SCARD_AUTOALLOCATE;

        LONG result = SCardListReaders(context,
                                       NULL,
                                       (LPTSTR)&pmszReaders,
                                       &cch);
        std::vector<LPTSTR> localReader;
        pReader = pmszReaders;
        while ('\0' != *pReader) {
            localReader.push_back(pReader);
            // Advance to the next value.
            pReader = pReader + wcslen((wchar_t *)pReader) + 1;
        }

        DWORD dwAP;
        result = SCardConnect(context,
                              (LPCTSTR)localReader[1],
                              SCARD_SHARE_DIRECT,
                              0,
                              &cardHandle,
                              &dwAP);

        SCARD_CHECK_COMMAND("SCardConnect", result);
        if (result != 0) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command ScardConnect.";
            return false;
        }

        switch(dwAP) {
        case SCARD_PROTOCOL_T0:
            pioSendPci = *SCARD_PCI_T0;
            break;
        case SCARD_PROTOCOL_T1:
            pioSendPci = *SCARD_PCI_T1;
            break;
        default:
            pioSendPci = *SCARD_PCI_T1;
            break;
        }

        DWORD dwRecvLength;

        BYTE pbrecv [10];
        struct {
            BYTE Class;
            BYTE INS;
            BYTE P1;
            BYTE P2;
            BYTE Le;
            BYTE Data1;
            BYTE Data2;
            BYTE Data3;
        } CmdChangeSlot;
        CmdChangeSlot.Class = 0x68;
        CmdChangeSlot.INS = 0x92;
        CmdChangeSlot.P1 = 0x01;
        CmdChangeSlot.P2 = 0x00;
        CmdChangeSlot.Le = 0x03;
        CmdChangeSlot.Data1 = slotN;
        CmdChangeSlot.Data2 = 0x00;
        CmdChangeSlot.Data2 = 0x00;
        LONG control = SCardControl(cardHandle,
                                    (DWORD)3500,
                                    (BYTE*)&CmdChangeSlot,
                                    sizeof(CmdChangeSlot),
                                    pbrecv,
                                    sizeof (pbrecv),
                                    &dwRecvLength);
        SCARD_CHECK_COMMAND("SCardControl", control);

        if (SCARD_S_SUCCESS != control) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command SCARDCONTROL.";
            return control;
        }

        result = SCardDisconnect(cardHandle, SCARD_LEAVE_CARD);
        SCARD_CHECK_COMMAND("SCardDisconnect", result);

        if (SCARD_S_SUCCESS != result) {
            QLOG_ERROR() << "AbstractCrt603Cz1: Error execute command disconnect.";
            return result;
        }

        SCARD_READERSTATE readerState;
        readerState.szReader       = (LPCTSTR)localReader[1];
        readerState.dwCurrentState = SCARD_STATE_UNAWARE;
        readerState.dwEventState   = SCARD_STATE_UNKNOWN;

        result = SCardGetStatusChange(context, INFINITE, &readerState, 1);
        SCARD_CHECK_COMMAND("SCardGetStatusChange", result);

        result = SCardConnect(context,
                                   (LPCTSTR)localReader[1],
                                   SCARD_SHARE_DIRECT,
                                   0,
                                   &cardHandle,
                                   &dwAP);

        SCARD_CHECK_COMMAND("SCardConnect", result);
        if (result != 0) {
            return false;
        }

        return true;
    }

  }
}
