/**
 * @file e1.hpp
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

#ifndef ATM_E1_HPP
#define ATM_E1_HPP

#include <devices/billvalidator/abstractcashcode609.hpp>
#include <devices/dispenser/abstractcrt591.hpp>
#include <devices/multiio/abstractled10v2.hpp>
#include <devices/printer/abstractT500.hpp>
#include <devices/cardreader/abstractcrt603cz1.hpp>
#include <devices/coinvalidator/abstractmei9520e.hpp>

#include <QMap>
#include <QVariant>
#include <QVariantList>

#include <cstddef>
#include <iostream>

// Override!
namespace devices {
   namespace billvalidator {
        class AbstractCashCode609;
    }
    namespace cardreader {
        class AbstractCrt603Cz1;
    }
    namespace coinvalidator {
       class AbstractMei9520e;
    }
    namespace dispenser {
        class AbstractCrt591;
    }
    namespace printer {
        class AbstractT500;
    }
    namespace multiio {
        class AbstractLed10v2;
    }
}

/**
 * Options for set device in class model ATM E1.
 *
 * \enum eAtmE1
 */
enum eAtmE1 {
    E1_DEVICE_DISPENSER,     ///! Device Dispenser.
    E1_DEVICE_PRINTER,       ///! Device Printer.
    E1_DEVICE_BILLVALIDATOR, ///! Device Bill Validator.
    E1_DEVICE_MULTIIO,       ///! Device Multi/IO.
    E1_DEVICE_COINVALIDATOR  ///! Device Coin Validator.
};

/**
 * @brief Namespace for the access to Model ATM.
 */
namespace atm {
    /**
     * @class E1
     *
     * @brief Class model ATM-E1.
     *
     * @details Class model to access devices attached to the ATM-E1.
     */
    class E1 : public devices::dispenser::AbstractCrt591, public devices::printer::AbstractT500,
               public devices::billvalidator::AbstractCashCode609, public devices::multiio::AbstractLed10v2,
               public devices::cardreader::AbstractCrt603Cz1, public devices::coinvalidator::AbstractMei9520e {
    public:
        /**
         * Constructor default.
         *
         * @param dev - Name device.
         */
        explicit E1(const eAtmE1 dev);

        /**
         * Singleton for class E1.
         *
         * @param dev - Name device.
         * @return The static class.
         */
        static E1 *getInstance(const eAtmE1 dev);

        /**
         * Destructor.
         */
        virtual ~E1();

        /**
         * Stores the automatic log for the device (Bill Validator), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: open+AUTO=PASSED;
         * 2) Test failed: open+AUTO=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogAutoBillValidator(const QVariant value);

        /**
         * Print the automatic log device (Bill Validator).
         *
         * @see insertLogAutoBillValidator();
         * @return Return automatic logs.
         */
        static QMap<QVariant, QVariantList> printLogAutoBillValidator();

        /**
         * Stores the automatic log for the device (Coin Validator), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: reset+AUTO=PASSED;
         * 2) Test failed: reset+AUTO=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogAutoCoinValidator(const QVariant value);

        /**
         * Print the automatic log device (Coin Validator).
         *
         * @see insertLogAutoCoinValidator();
         * @return Return automatic logs.
         */
        static QMap<QVariant, QVariantList> printLogAutoCoinValidator();

        /**
         * Stores the automatic log for the device (Dispenser), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: cardStatus+AUTO=PASSED;
         * 2) Test failed: cardStatus+AUTO=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogAutoDispenser(const QVariant value);

        /**
         * Print the automatic log device (Dispenser).
         *
         * @see insertLogAutoDispenser();
         * @return Return automatic logs.
         */
        static QMap<QVariant, QVariantList> printLogAutoDispenser();

        /**
         * Stores the automatic log for the device (Printer), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: showPrinter+AUTO=PASSED;
         * 2) Test failed: showPrinter+AUTO=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogAutoPrinter(const QVariant value);

        /**
         * Print the automatic log device (Printer).
         *
         * @see insertLogAutoPrinter();
         * @return Return automatic logs.
         */
        static QMap<QVariant, QVariantList> printLogAutoPrinter();

        /**
         * Stores the automatic log for the device (Multi/IO), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: powerOn+AUTO=PASSED;
         * 2) Test failed: powerOn+AUTO=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogAutoMultiIo(const QVariant value);

        /**
         * Print the automatic log device (Multi/IO).
         *
         * @see insertLogAutoMultiIo();
         * @return Return automatic logs.
         */
        static QMap<QVariant, QVariantList> printLogAutoMultiIo();

        /**
         * Stores the automatic log for the device (SmartCard), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: open+AUTO=PASSED;
         * 2) Test failed: open+AUTO=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogAutoSmartCard(const QVariant value);

        /**
         * Print the automatic log device (SmartCard).
         *
         * @see insertLogAutoSmartCard();
         * @return Return automatic logs.
         */
        static QMap<QVariant, QVariantList> printLogAutoSmartCard();

        /**
         * Stores the automatic log for the device (Camera), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: open+AUTO=PASSED;
         * 2) Test failed: open+AUTO=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogAutoCamera(const QVariant value);

        /**
         * Print the automatic log device (Camera).
         *
         * @see insertLogAutoCamera();
         * @return Return automatic logs.
         */
        static QMap<QVariant, QVariantList> printLogAutoCamera();

        /**
         * Stores the manual log for the device (BillValidator), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: open+MANUAL=PASSED;
         * 2) Test failed: open+MANUAL=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogManualBillValidator(const QVariant value);

        /**
         * Print the manual log device (BillValidator).
         *
         * @see insertLogManualBillValidator();
         * @return Return manual logs.
         */
        static QMap<QVariant, QVariantList> printLogManualBillValidator();

        /**
         * Stores the manual log for the device (CoinValidator), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: open+MANUAL=PASSED;
         * 2) Test failed: open+MANUAL=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogManualCoinValidator(const QVariant value);

        /**
         * Print the manual log device (CoinValidator).
         *
         * @see insertLogManualCoinValidator();
         * @return Return manual logs.
         */
        static QMap<QVariant, QVariantList> printLogManualCoinValidator();

        /**
         * Stores the manual log for the device (Dispenser), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: reset+MANUAL=PASSED;
         * 2) Test failed: reset+MANUAL=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogManualDispenser(const QVariant value);

        /**
         * Print the manual log device (Dispenser).
         *
         * @see insertLogManualDispenser();
         * @return Return manual logs.
         */
        static QMap<QVariant, QVariantList> printLogManualDispenser();

        /**
         * Stores the manual log for the device (Printer), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: open+MANUAL=PASSED;
         * 2) Test failed: open+MANUAL=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogManualPrinter(const QVariant value);

        /**
         * Print the manual log device (Printer).
         *
         * @see insertLogManualPrinter();
         * @return Return manual logs.
         */
        static QMap<QVariant, QVariantList> printLogManualPrinter();

        /**
         * Stores the manual log for the device (Multi/IO), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: powerOff+MANUAL=PASSED;
         * 2) Test failed: powerOff+MANUAL=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogManualMultiIo(const QVariant value);

        /**
         * Print the manual log device (Multi/IO).
         *
         * @see insertLogManualMultiIo();
         * @return Return manual logs.
         */
        static QMap<QVariant, QVariantList> printLogManualMultiIo();

        /**
         * Stores the manual log for the device (SmartCard), this
         * information is used to generate the HTML/PDF report. The default is
         * used log: command device; type of log; test run with success or failure.
         *
         * Eg:
         *
         * 1) Test passed: close+MANUAL=PASSED;
         * 2) Test failed: close+MANUAL=FAILED;
         *
         * @param value - The string with the type of logging.
         */
        static void insertLogManualSmartCard(const QVariant value);

        /**
         * Print the manual log device (SmartCard).
         *
         * @see insertLogManualSmartCard();
         * @return Return manual logs.
         */
        static QMap<QVariant, QVariantList> printLogManualSmartCard();

    private:
        /// Pointer for class using singleton.
        static E1 *m_instance;

        /// Store model device.
        eAtmE1 m_dev;

        /// Store automatic log bill validator.
        static QVariantList m_insertLogAutoBillValidator;

        /// Store automatic log: Bill Validator.
        static QVariantList m_insertLogAutoCoinValidator;

        /// Store automatic log: Dispenser.
        static QVariantList m_insertLogAutoDispenser;

        /// Store automatic log: Printer.
        static QVariantList m_insertLogAutoPrinter;

        /// Store automatic log: Multi/IO.
        static QVariantList m_insertLogAutoMultiIo;

        /// Store automatic log: SmartCard.
        static QVariantList m_insertLogAutoSmartCard;

        /// Store automatic log: Bill Validator.
        static QVariantList m_insertLogAutoCamera;

        /// Store manual log: Bill Validator.
        static QVariantList m_insertLogManualBillValidator;

        /// Store manual log: Coin Validator.
        static QVariantList m_insertLogManualCoinValidator;

        /// Store manual log: Dispeser.
        static QVariantList m_insertLogManualDispenser;

        /// Store manual log: Printer.
        static QVariantList m_insertLogManualPrinter;

        /// Store manual log: Maulti/IO.
        static QVariantList m_insertLogManualMultiIo;

        /// Store manual log: SmartCard.
        static QVariantList m_insertLogManualSmartCard;
    };
}

#endif // ATM_E1_HPP
