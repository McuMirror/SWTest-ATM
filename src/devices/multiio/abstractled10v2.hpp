/**
 * @file abstractled10v2.hpp
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

#ifndef DEVICES_MULTIIO_ABSTRACTLED10V2_HPP
#define DEVICES_MULTIIO_ABSTRACTLED10V2_HPP

#include <utils/serialport.hpp>

#include <string>

// Override!
namespace utils {
    class SerialPort;
}

/**
 * Option for name class.
 *
 * \enum eAbstractLed10v2Class
 */
enum eAbstractLed10v2Class {
    ABSTRACTLED10V2
};

/**
 * Options for get data the commands in Multi/IO board.
 *
 * \enum eAbstractMei9520eGetData
 */
enum eAbstractLed10v2GetData {
    ABSTRACT_LED10V2_GET_STATUS_SENSOR,     //! Get sensors status.
    ABSTRACT_LED10V2_GET_STATUS_SENSOR_VBR, //! Get sensor vibration status.
    ABSTRACT_LED10V2_GET_STATUS_ONOFF       //! Get ON/OFF status.
};

namespace devices {
  /**
   * @brief Namespace for the access to Multi/IO.
   */
  namespace multiio {
    /**
     * @class AbstractLed10v2
     *
     * @brief Multi/IO Board Control.
     *
     * @details This multi board can close and open 20 pcs output pin.
     * Under the model image of the Multi/IO used:
     *
     * @image html multi-io-rele.png
     */
    class AbstractLed10v2 {
    public:
      /**
       * Constructor default.
       */
      explicit AbstractLed10v2();

      /**
       * Destructor.
       */
      ~AbstractLed10v2();

      /**
       * Singleton for this class.
       *
       * @return The static class.
       */
      static AbstractLed10v2* getInstance();

      /**
       * Open connection with Multi/IO board.
       *
       * @param comport - The name of the serial port.
       * @param baudrate - The data baud rate for the desired direction.
       * @param databit - Set the number of data bits used.
       * @param tparity - This property holds the parity checking mode.
       * @param stopbit - This property holds the number of stop bits in a frame.
       * @param flowcontrol - Set the flow control used.
       * @return Return true if command successful, others false.
       */
      virtual bool open(std::string comport = "COM5", unsigned int baudrate = 9600,
                        unsigned int databit = 8, unsigned int tparity = 0,
                        unsigned int stopbit = 3, unsigned int flowcontrol = 0);

      /**
       * Close all connections with Multi/IO board.
       *
       * @return Return true if the multi/io is closed with success.
       */
      bool close();

      /**
       * Check the COM port is open or close.
       *
       * @return Return true if com is open, others false.
       */
      bool comIsOpen();

      /**
       * Check sensor (door and vibration) status.
       *
       * @param pin - The number output pin connected in device.
       * @return Return true if device is ok; otherwise false.
       */
      bool sensorStatus(unsigned int pin);

      /**
       * Check current sensor (door and vibration) status.
       *
       * @return Return true if command successful, others false.
       */
      bool checkSensorStatus();

      /**
       * @brief Power On devices.
       *
       * @param pin - The number output pin connected in device.
       * @return Return true if power on ok, others return false.
       */
      bool powerOn(unsigned int pin = 0);

      /**
       * @brief Power Off devices.
       *
       * @param pin - The number output pin connected in device.
       * @return Return true if power off ok, others return false.
       */
      bool powerOff(unsigned int pin = 0);

      /**
       * Print device version.
       *
       * @return Return string device version.
       */
      const std::string printVersion();

    private:
      /**
       * Get data in Multi/IO board.
       *
       * @param cmd - The command for get data.
       */
      void getData(eAbstractLed10v2GetData cmd = ABSTRACT_LED10V2_GET_STATUS_SENSOR);

      /**
       * Check if data command is ok.
       *
       * @return Return true if command ok; otherwise false.
       */
      bool getDataIsOk();

    protected:
      /// Pointer for class using singleton.
      static AbstractLed10v2 *m_instance;

      /// Check if serial COM port is open or close.
      static bool m_ComIsOpen;

      /// Check data is ok.
      bool m_getDataIsOk;

      /// Check status sensor.
      bool m_statusSensor;

      /// Store serial control.
      utils::SerialPort *m_serial;
    };

  }
}

#endif // DEVICES_MULTIIO_ABSTRACTLED10V2_HPP



