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

#ifndef DEVICES_BILLVALIDATOR_ABSTRACTCRT609_HPP
#define DEVICES_BILLVALIDATOR_ABSTRACTCRT609_HPP

#include <utils/serialport.hpp>

#include <string>
#include <typeinfo>

// Override!
namespace utils {
    class SerialPort;
}

/**
 * Options for get data the commands in Bill Validator.
 *
 * \enum eAbstractCashCode609GetData
 */
enum eAbstractCashCode609GetData {
    ABSTRACT_CASHCODE_GET_MONEY,      //! Get money value.
    ABSTRACT_CASHCODE_GET_BILLTABLE,  //! Get serial answer bill table value.
    ABSTRACT_CASHCODE_GET_VERSION,    //! Get serial answer version value.
    ABSTRACT_CASHCODE_GET_STATUS,     //! Get serial answer status value.
    ABSTRACT_CASHCODE_GET_BILLTYPES,  //! Get serial answer bill type value.
    ABSTRACT_CASHCODE_GET_STACKBILL,  //! Get serial answer stack bill value.
    ABSTRACT_CASHCODE_GET_RETURNBILL, //! Get serial answer return bill.
    ABSTRACT_CASHCODE_GET_RESET,      //! Get serial answer reset cashcode.
    ABSTRACT_CASHCODE_GET_HOLD        //! Get serial answer hold cashcode.
};

namespace devices {
  /**
   * @brief Namespace for the access to Bill Validator.
   */
  namespace billvalidator {
    /**
     * @class AbstractCashCode609
     *
     * @brief CashCode Bill Validator.
     *
     * @details This class control CashCode Bill Validator. Under the
     * model image of the Bill Validator used:
     *
     * @image html billvalidator-CashCode609.png
     */
    class AbstractCashCode609 {
    public:
      /**
       * Constructor default.
       */
      explicit AbstractCashCode609();

      /**
       * Destructor.
       */
      ~AbstractCashCode609();

      /**
       * Singleton for this class.
       *
       * @return The static class.
       */
      static AbstractCashCode609* getInstance();

      /**
       * Open connection with CashCode.
       *
       * @param comport - The name of the serial port.
       * @param baudrate - The data baud rate for the desired direction.
       * @param databit - Set the number of data bits used.
       * @param tparity - This property holds the parity checking mode.
       * @param stopbit - This property holds the number of stop bits in a frame.
       * @param flowcontrol - Set the flow control used.
       * @return Return true if command successful, others false.
       */
      virtual bool open(std::string comport = "COM3", unsigned int baudrate = 9600,
                        unsigned int databit = 8, unsigned int tparity = 0,
                        unsigned int stopbit = 3, unsigned int flowcontrol = 0);

      /**
       * Close all connections with CashCode.
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
       * Send command for reset CashCode.
       *
       * @return Return true if command successful, others false.
       */
      bool reset();

      /**
       * Send commmand to enable bill types and escrow.
       *
       * @return Return true if command successful, others false.
       */
      bool enableBillTypes();

      /**
       * Send commmand to poll (BV) status, case the holdnote is enable
       * store note in escrow.
       *
       * @see Check too hold command.
       * @return Return true if command successful, others false.
       */
      bool poll();

      /**
       * Send commmand to hold bill note in escrow. This command hold allows the
       * controller to hold Bill Validator in Escrow during 10s. After this time the
       * controller should send the STACK or RETURN command. For continued holding in an
       * Escrow state it is necessary to resend this command. Otherwise the Bill Validator
       * will execute return of a bill.
       *
       * @see Check too poll command.
       * @return Return true if command successful, others false.
       */
      bool hold();

      /**
       * Send command to return the bill from escrow position.
       *
       * @return Return true if command successful, others false.
       */
      bool returnBill();

      /**
       * Send commmand to stack the bill from escrow position.
       *
       * @return Return true if command successful, others false.
       */
      bool stackBill();

      /**
       * Send command to get version the Bill Validator.
       *
       * @return Return true if command successful, others false.
       */
      bool version();

      /**
       * Print device version.
       *
       * @return Return string device version.
       */
      const std::string printVersion();

      /**
       * Send command to get bill table disponible in firmeware the
       * Bill Validator.
       *
       * @return Return true if command successful, others false.
       */
      bool getBillTable();

      /**
       * Print the value note loader.
       *
       * Number 0 = The note value not found;\n
       * Number 1 = The note value found (check comand bill table);\n
       * Number 2 = The note value found (check comand bill table);\n
       * Number 3 = The note value found (check comand bill table);\n
       * Number 4 = The note value found (check comand bill table);\n
       * Number 5 = The note value found (check comand bill table);\n
       * Number 6 = The note value found (check comand bill table);\n
       *
       * @return Return the number the note loader.
       */
      unsigned int printNoteLoadedValue();

      /**
       * Clear value note loaded.
       */
      void clearNoteLoadedValue();

      /**
       * Check current status CashCode.
       *
       * @return Return true if command successful, others false.
       */
      bool statusRequest();

      /**
       * Check if bill returing in escrow for user.
       *
       * @return Return true if bill in position escrow.
       */
      bool billReturning();

    private:
      /**
       * Get data in CashCode.
       *
       * @param cmd - The command for get data.
       */
      void getData(eAbstractCashCode609GetData cmd = ABSTRACT_CASHCODE_GET_MONEY);

      /**
       * Check if data command is ok.
       *
       * @return Return true if command ok; otherwise false.
       */
      bool getDataIsOk();

    protected:
      /// Pointer for class using singleton.
      static AbstractCashCode609 *m_instance;

      /// Check if serial COM port is open or close.
      static bool m_ComIsOpen;

      /// Store number value note.
      unsigned int m_statusStoreNoteValue;

      /// Check bill retunrning in escrow.
      bool m_billReturning;

      /// Check data is ok.
      bool m_getDataIsOk;

      /// Store version the cachcode.
      std::string m_version;

      /// Store serial control.
      utils::SerialPort *m_serial;
    };

  }
}

#endif // DEVICES_BILLVALIDATOR_ABSTRACTCRT609_HPP
