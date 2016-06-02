/**
 * @file abstractmei9520e.hpp
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
 
#ifndef DEVICES_COINVALIDATOR_ABSTRACTMEI9520E_HPP
#define DEVICES_COINVALIDATOR_ABSTRACTMEI9520E_HPP

#include <utils/serialport.hpp>

#include <string>
#include <typeinfo>

// Override!
namespace utils {
    class SerialPort;
}

/**
 * Options for get data the commands in Coin Validator.
 *
 * \enum eAbstractMei9520eGetData
 */
enum eAbstractMei9520eGetData {
    ABSTRACT_MEI9520E_GET_MONEY,       //! Get money value.
    ABSTRACT_MEI9520E_GET_VERSION,     //! Get serial answer version value.
    ABSTRACT_MEI9520E_GET_PRODUCTNAME, //! Get serial answer product name.
    ABSTRACT_MEI9520E_GET_NAMEDEVICE   //! Get serial anwser name device.
};

namespace devices {
  /**
   * @brief Namespace for the access to Bill Validator.
   */
  namespace coinvalidator {
    /**
     * @class AbstractMei9520e
     *
     * @brief MEI Coin Validator.
     *
     * @details This class control Coin Validator MEI. Under the
     * model image of the Coin Validator used:
     *
     * @image html coinvalidator-MEI9520e.png
     */
    class AbstractMei9520e {
    public:
      /**
       * Constructor default.
       */
      explicit AbstractMei9520e();

      /**
       * Destructor.
       */
      ~AbstractMei9520e();

      /**
       * Singleton for this class.
       *
       * @return The static class.
       */
      static AbstractMei9520e* getInstance();

      /**
       * Open connection with MEI-i9520e.
       *
       * @param comport - The name of the serial port.
       * @param baudrate - The data baud rate for the desired direction.
       * @param databit - Set the number of data bits used.
       * @param tparity - This property holds the parity checking mode.
       * @param stopbit - This property holds the number of stop bits in a frame.
       * @param flowcontrol - Set the flow control used.
       * @return Return true if command successful, others false.
       */
      virtual bool open(std::string comport = "COM2", unsigned int baudrate = 9600,
                        unsigned int databit = 8, unsigned int tparity = 0,
                        unsigned int stopbit = 3, unsigned int flowcontrol = 0);

      /**
       * Close all connections with Coin Validator.
       *
       * @return Return true if the cashcode is closed with success.
       */
      bool close();

      /**
       * Check the serial COM port is open or close.
       *
       * @return Return true if com is open, others false.
       */
      bool comIsOpen();

      /**
       * Send command for reset Coin Validator.
       *
       * @return Return true if command successful, others false.
       */
      bool reset();

      /**
       * Send commmand to poll, case the holdnote is enable store
       * coin in escrow.
       *
       * @see Check too hold command.
       * @return Return true if command successful, others false.
       */
      bool poll();

      /**
       * Print the value coin loader.
       *
       * Number 0 = The coin value not found;\n
       * Number 1 = The coin value found (check comand coin table);\n
       * Number 2 = The coin value found (check comand coin table);\n
       * Number 3 = The coin value found (check comand coin table);\n
       * Number 4 = The coin value found (check comand coin table);\n
       * Number 5 = The coin value found (check comand coin table);\n
       * Number 6 = The coin value found (check comand coin table);\n
       *
       * @return Return the number the note loader.
       */
      unsigned int printCoinLoadedValue();

      /**
       * Clear value note loaded.
       */
      void clearCoinLoadedValue();

      /**
       * Send command to get version the Coin Validator.
       *
       * @param tVersion - Type get version.
       * @return Return true if command successful, others false.
       */
      bool version(int tVersion = 0);

      /**
       * Print device version.
       *
       * @return Return string device version.
       */
      const std::string printVersion();

    private:
      /**
       * Get data in Coin Validator MEI.
       *
       * @param cmd - The command for get data.
       */
      void getData(eAbstractMei9520eGetData cmd = ABSTRACT_MEI9520E_GET_MONEY);

      /**
       * Check if data command is ok.
       *
       * @return Return true if command ok; otherwise false.
       */
      bool getDataIsOk();

    protected:
      /// Pointer for class using singleton.
      static AbstractMei9520e *m_instance;

      /// Check if serial COM port is open or close.
      static bool m_ComIsOpen;

      /// Store number value note.
      unsigned int m_statusStoreCoinValue;

      /// Check data is ok.
      bool m_getDataIsOk;

      /// Store version the cachcode.
      std::string m_version;

      /// Store serial control.
      utils::SerialPort *m_serial;
    };

  }
}

#endif // DEVICES_COINVALIDATOR_ABSTRACTMEI9520E_HPP
