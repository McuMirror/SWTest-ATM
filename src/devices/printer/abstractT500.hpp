/**
 * @file abstractT500.hpp
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

#ifndef DEVICES_PRINTER_ABSTRACTT500_HPP
#define DEVICES_PRINTER_ABSTRACTT500_HPP

#include <QPrinterInfo>

namespace devices {
  /**
   * @brief Namespace for the access to printer.
   */
  namespace printer {
    /**
     * @class AbstractT500
     *
     * @brief Class model ATM-E1.
     *
     * @details Class that controls the printer information.
     *
     * @image html printer-T500.png
     */
    class AbstractT500  {
    public:
      /**
       * Constructor default.
       */
      explicit AbstractT500();
	  
      /**
       * Singleton for class AbstractT500.
       *
       * @return The static class.
       */
      static AbstractT500* getInstance();

      /**
       * Destructor.
       */
      ~AbstractT500();

      /**
       * Get all information about the printer.
       *
       * @param ok - Set ok.
       * @param printerName - Set name the printer.
       * @return Return true if get information is ok; otherwize false.
       */
      bool info(bool ok, const std::string printerName);

      /**
       * Get information about the printer.
       *
       * @param printerName - Set name the printer.
       * @return Return data if get information is ok; otherwiser return data empty.
       */
      QPrinterInfo info(const std::string printerName);

      /**
       * Send simple page test for printer, using file PRN.
       *
       * @return Return true if send printer is ok; otherwize false.
       */
      bool sendTestPageToPrinter();

      /**
       * Show information all printers.
       *
       * @return Return true if show printer is ok; otherwize false.
       */
      bool showPrinters();
	  
    protected:
      /// Pointer for class using singleton.
      static AbstractT500 *m_instance;
    };

  }
}

#endif // DEVICES_PRINTER_ABSTRACTT500_HPP
