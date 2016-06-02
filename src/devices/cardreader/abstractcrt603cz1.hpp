/**
 * @file abstractcashcode609.hpp
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

#ifndef DEVICES_CARDREADER_ABSTRACTCRT603CZ1_HPP
#define DEVICES_CARDREADER_ABSTRACTCRT603CZ1_HPP

#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>

#ifdef Q_OS_WIN
#undef UNICODE
extern "C" {
#include <windows.h>
}
#endif


#if defined(Q_OS_LINUX) || defined(Q_OS_MAC)
#include <PCSC/winscard.h>
#include <PCSC/wintypes.h>
#include <PCSC/reader.h>
#include <wchar.h>
#else
extern "C" {
#include <winscard.h>
}
#endif

/// IO CTL Scape for CRT-603 CZ1 SAM Card.
#define ABSTRACTCRT603CZ1_IOCTL_CCID_ESCAPE SCARD_CTL_CODE(3500)

namespace devices {
  /**
   * @brief Namespace for the access to Card Reader.
   */
  namespace cardreader {
    /**
     * @class AbstractCrt603Cz1
     *
     * @brief SmartCard Creator CRT 603 Control.
     *
     * @details This class control smartcard reader.
     * Under the model image of the Smardcard CRT-603 used:
     *
     * @image html smartcard-reader-603.png
     */
    class AbstractCrt603Cz1 {
    public:
      /**
       * Constructor default.
       */
      explicit AbstractCrt603Cz1();

      /**
       * Destructor.
       */
      ~AbstractCrt603Cz1();

      /**
       * Singleton for this class.
       *
       * @return The static class.
       */
      static AbstractCrt603Cz1* getInstance();

      /**
       * Open connection with SmartCard.
       *
       * @return Return true if open connection is ok; otherwise returns false.
       */
      bool open();

      /**
       * Close all connections with SmartCard.
       *
       * @return Return true if the Crt-603 is closed with success; otherwise return false.
       */
      bool close();

      /**
       * Check status card in dispenser.
       *
       * @return Return true if command successful, otherwise return false.
       */
      bool cardStatus();

      /**
       * Get readers (usb/devices) in system.
       *
       * @return Return true if command successful, otherwise return false.
       */
      bool getReaders();

      /**
       * Printer readers found in system.
       *
       * @see Firt call getReaders().
       * @return Return readers found in system.
       */
      std::vector<LPTSTR> printReaders();

      /**
       * Get card mifare serial number.
       *
       * @return Return true if command successful, otherwise return false.
       */
      bool getCardMifareSerialNumber();

      /**
       * Print serial number card, card type (Mifare S50/S70).
       *
       * @see Firt call getReaders().
       * @return Number serial card.
       */
      std::string printCardMifareSerialNumber() const;

      /**
       * Load the key in the reader, the generic key used to read and write in
       * the sector 0/1/2/3.
       *
       * @param key - The key.
       * @return Return true if command successful, otherwise return false.
       */
      bool loadKey(std::vector<unsigned char> key);

      /**
       * Read in particular block in card.
       *
       * @param keyIndex - The key.
       * @param blockSN - The block to read in the card.
       * @return Return true if command successful, otherwise return false.
       */
      bool mifareReadDataBlock(unsigned char keyIndex, std::vector<unsigned char> blockSN);

      /**
       * Print the data read.
       *
       * @see First call mifareReadDataBlock().
       * @return Return true if command successful, otherwise return false.
       */
      std::string printCardMifareReadDataBlock() const;

      /**
       * Write in particular block in card.
       *
       * @param keyIndex - The key.
       * @param blockSN - The block to write in the card.
       * @return Return true if command successful, otherwise return false.
       */
      bool mifareWriteDataBlock(unsigned char keyIndex, std::vector<unsigned char> blockSN);

    private:
      /**
       * Establishes the resource manager context within which
       * database operations are performed.
       *
       * @return Returns true if establishes is ok; otherwise returns false.
       */
      bool establishContext();

      /**
       * Auth Sector for read and write in mifare card.
       *
       * @return Returns true if auth is ok; otherwise returns false.
       */
      bool authSector();

    protected:
      /// Pointer for class using singleton.
      static AbstractCrt603Cz1 *m_instance;

      /// Store the context.
      SCARDCONTEXT m_context;

      /// Store the card handle.
      SCARDHANDLE m_cardHandle;

      /// Store the io request.
      SCARD_IO_REQUEST m_pioSendPci;

      /// Store usb readers.
      LPTSTR m_mszReaders;

      /// Store all usb readers.
      std::vector<LPTSTR> m_listReaders;

      /// Store serial number, card mifare.
      std::string m_serialNumber;

      /// Store read block, in active card Mifare.
      std::string m_readDataBlock;
    };

    /**
     * @class AbstractCrt603Cz1Sam
     *
     * @brief SmartCard Creator CRT 603 (SAM) Control.
     *
     * @details This class is responsible for control of the SAM, on and
     * off the slots. Under the model image of the SAM/Card used:
     *
     * @image html smartcard-reader-603.png
     */
    class AbstractCrt603Cz1Sam : public AbstractCrt603Cz1 {
    public:
        /**
         * Constructor default.
         */
        explicit AbstractCrt603Cz1Sam();

        /**
         * Destructor.
         */
        ~AbstractCrt603Cz1Sam();

        /**
         * Singleton for this class.
         *
         * @return The static class.
         */
        static AbstractCrt603Cz1Sam* getInstance();

        /**
         * Connect in slot sam the SmartCard Reader CRT-603.
         *
         * @param slot - The slot to connect, 1 or 2.
         * @return Returns true if auth is ok; otherwise returns false.
         */
        bool connectSamSlot(unsigned int slot = 1);

    private:
        /// Pointer for class using singleton.
        static AbstractCrt603Cz1Sam *m_instance;
    };

  }
}

#endif // DEVICES_CARDREADER_ABSTRACTCRT603CZ1_HPP
