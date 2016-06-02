/**
 * @file abstractcrt591.hpp
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

#ifndef DEVICES_DISPENSER_ABSTRACTCRT591_HPP
#define DEVICES_DISPENSER_ABSTRACTCRT591_HPP

#include <utils/serialport.hpp>

// Override!
namespace utils {
    class SerialPort;
}

/**
 * Options for execute commands initialize CRT-591.
 *
 * \enum eAbtractCrt591Initialize
 */
enum eAbtractCrt591Initialize {
    ABSTRACT_CRT591_INIT_MOVE_CARD_TO_HOLDING_POSITION,     //! Move card to holding position.
    ABSTRACT_CRT591_INIT_CAPTURE_CARD,                      //! Capturing card.
    ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION,            //! Not move card.
    ABSTRACT_CRT591_INIT_MOVE_CARD_TO_HOLDING_POSITION_RCW, //! Move card position RCW.
    ABSTRACT_CRT591_INIT_CAPTURE_CARD_RCW,                  //! Capture card RCW.
    ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION_RCW         //! Move card position RCW.
};

/**
 * Options for execute commands card move CRT-591.
 *
 * \enum eAbtractCrt591CardMove
 */
enum eAbtractCrt591CardMove {
    ABSTRACT_CRT591_MOVE_CARD_TO_HOLDING_POSITION, //! Move card to holding position.
    ABSTRACT_CRT591_MOVE_CARD_TO_IC_POSITION,      //!  Move card to IC position.
    ABSTRACT_CRT591_MOVE_CARD_TO_RF_POSITION,      //! Move card to RF position.
    ABSTRACT_CRT591_MOVE_CARD_TO_ERROR_BIN,        //! Move card to bin rejects.
    ABSTRACT_CRT591_MOVE_CARD_TO_GATE_EJECT        //! Move card to gate eject.
};


/**
 * Options for get data the commands in Bill Validator.
 *
 * \enum eAbtractCrt591GetData
 */
enum eAbtractCrt591GetData {
    ABSTRACT_CRT591_GET_STATUS_OPEN,               //! Get status open dispenser.
    ABSTRACT_CRT591_GET_STATUS_INIT,               //! Get status init dispenser.
    ABSTRACT_CRT591_GET_STATUS_POSITION,           //! Get status position card value.
    ABSTRACT_CRT591_GET_MIFARE_SERIAL_CARD_NUMBER, //! Get mifare serial card number.
    ABSTRACT_CRT591_GET_STATUS_SAM_CARD_ACTIVATED, //! Get status SAM activated.
    ABSTRACT_CRT591_GET_STATUS_SAM_CARD,           //! Get status SAM card.
    ABSTRACT_CRT591_GET_STATUS_CARD_LEVEL,         //! Get status level card.
    ABSTRACT_CRT591_GET_STATUS_REJECT_BOX,         //! Get status the reject box.
    ABSTRACT_CRT591_GET_VERSION,                   //! Get version firmware device.
    ABSTRACT_CRT591_GET_STATUS_SAM_APDU,           //! Get status SAM APDU.
    ABSTRACT_CRT591_GET_STATUS_LOAD_KEY,           //! Get status load key.
    ABSTRACT_CRT591_GET_STATUS_READ_BLOCK,         //! Get status/data read block.
    ABSTRACT_CRT591_GET_STATUS_WRITE_BLOCK         //! Get status/data write block.
};

/**
 * @brief Namespace for the access to native devices.
 */
namespace devices {
  /**
   * @brief Namespace for the access to dispenser.
   */
  namespace dispenser {
    /**
     * @class AbstractCrt591
     *
     * @brief Card Dispenser Control Model: <b>Creator CRT-591</b>.
     *
     * @details This class is used to control the CRT-591 dispenser is used
     * the command serial, in addition to controlling the movement of the card
     * is also used to authenticate the antenna read serial number cards.
     * Under the model image of the dispenser used:
     *
     * @image html dispenser-CRT591.png 
     */
    class AbstractCrt591  {
    public:
      /**
       * Constructor default.
       */
      explicit AbstractCrt591();

      /**
       * Singleton for class AbstractCrt591.
       *
       * @return The static class.
       */
      static AbstractCrt591 *getInstance();

      /**
       * Destructor.
       */
      ~AbstractCrt591();

      /**
       * Open connection with Dispenser.
       *
       * @param comport - The name of the serial port.
       * @param baudrate - The data baud rate for the desired direction.
       * @param databit - Set the number of data bits used.
       * @param tparity - This property holds the parity checking mode.
       * @param stopbit - This property holds the number of stop bits in a frame.
       * @param flowcontrol - Set the flow control used.
       * @return Return true if command successful, others false.
       */
      virtual bool open(std::string comport = "COM4", unsigned int baudrate = 38400,
                        unsigned int databit = 8, unsigned int tparity = 0,
                        unsigned int stopbit = 3, unsigned int flowcontrol = 0);

      /**
       * Close all connections with dispenser.
       *
       * @return Return true if the dispenser is closed with success, others false.
       */
      bool close();

      /**
       * Check COM is open or close.
       *
       * @return Return true if com is open, others false.
       */
      bool comIsOpen();

      /**
       * Initialize dispenser, this is requerired after open connection
       * with dispenser.
       *
       * @param initialize - Initialize the dispenser moveing the card.
       * @return Return true if command successful, others false.
       */
      bool init(eAbtractCrt591Initialize
                initialize = ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION);

      /**
       * Check status card in dispenser.
       *
       * @param checkRejectBox - If true cehck status the reject box.
       * @return Return true if command successful, others false.
       */
      bool cardStatus(bool checkRejectBox = false);

      /**
       * Print the number status card level in ST-1. Is necessary first
       * call the method cardStatus().
       *
       * Number status 0 = No card in stacker;\n
       * Number status 1 = Low card in stacker (level);\n
       * Number status 2 = Hight card in stacker (level);\n
       * Number status -1 = Error in card or execute command.\n
       *
       * @return Number status card ST-1.
       */
      int printStatusCardLevel() const;

      /**
       * Check if exist card in reject box.
       *
       * @return Return true if exist card in reject box; otherwize false.
       */
      bool isCardRejectBox() const;

      /**
       * Check sensors status in dispenser.
       *
       * @return Return true if command successful, others false.
       */
      bool sensorStatus();

      /**
       * Command reset dispenser.
       *
       * @return Return true if command successful, others false.
       */
      bool reset();

      /**
       * Card move position in dispenser.
       *
       * @param cardMove - Set mode option card move.
       * @return Return true if card status request is ok, others false.
       */
      bool cardMovePosition(eAbtractCrt591CardMove
                            cardMove = ABSTRACT_CRT591_MOVE_CARD_TO_HOLDING_POSITION);

      /**
       * Check version this device.
       *
       * @return Return true if version command is ok; otherwise false.
       */
      bool version();

      /**
       * Print device version.
       *
       * @return Return string device version.
       */
      const std::string printVersion();

      /**
       * Print number serial the Mifare Card.
       *
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

      /**
       * Print number result command SAM Card APDU.
       *
       * @return APDU result commmand.
       */
      std::string printCardSamAPDU() const;

      /**
       * Active Mifare card.
       *
       * @return Return true if card command is ok; otherwize false.
       */
      bool cardMifareActivate();

      /**
       * Desactive Mifare card.
       *
       * @return Return true if card command is ok; otherwize false.
       */
      bool cardMifareDesactivate();

      /**
       * Active SAM. The protocol is 30H=T0 or 31H=T=1.
       *
       * @return Return true if activate command is ok; otherwize false.
       */
       bool samActivate();

       /**
        * Send command could reset sam.
        *
        * @return Return true if activate command is ok; otherwize false.
        */
       bool samCouldReset();

       /**
        * Enable command send APDU.
        *
        * @return Return true if command is ok; otherwize false.
        */
       bool samSendAPDU();

       /**
        * Check SAM status.
        *
        * @return Return true if the sam is the slot; otherwise false.
        */
       bool samStatus();

       /**
        * Verify is SAM present or not.
        *
        * @return Return true if the sam is present; otherwise false.
        */
       bool samIsPresent();

    private:
      /**
       * Get data in CRT-591.
       *
       * @param cmd - The command for get data.
       */
      void getData(eAbtractCrt591GetData cmd = ABSTRACT_CRT591_GET_STATUS_POSITION);

      /**
       * Check if data command is ok.
       *
       * @return Return true if command ok; otherwise false.
       */
      bool getDataIsOk();

    protected:
      /// Pointer for class using singleton.
      static AbstractCrt591 *m_instance;

      /// Check if COM port is open or close.
      static bool m_ComIsOpen;

      /// Store serial control.
      utils::SerialPort *m_serial;

      /// Store serial number, in active card Mifare.
      std::string m_serialNumber;

      /// Store read block, in active card Mifare.
      std::string m_readDataBlock;

      /// Store value result SAM APDU.
      std::string m_samAPDU;

      /// Store version device.
      std::string m_version;

      /// Check data is ok.
      bool m_getDataIsOk;

      /// Store values for card status ST-1 (level).
      int m_cardStatusLevel;

      /// Store value if card sam present.
      bool m_samCardPresent;

      /// Store reject card box.
      bool m_rejectBox;
    };

  }
}

#endif // DEVICES_DISPENSER_ABSTRACTCRT591_HPP
