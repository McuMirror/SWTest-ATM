/**
 * @file atmmodelselect.hpp
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


#ifndef ATMMODELSELECT_HPP
#define ATMMODELSELECT_HPP

#include <utils/serialport.hpp>

#include <QWidget>
#include <QTranslator>
#include <QCloseEvent>

// Override!
namespace Ui {
    class AtmModelSelect;
}

// Override!
class SettingsSerial;
class Console;
class SearchDevices;

/**
 * @class AtmModelSelect
 *
 * @brief Make is the selection to be used for the tests.
 *
 * @details Makes the ATM selection that will be used for automatic
 * and manual testing device.
 */
class AtmModelSelect : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - The widget parent.
     */
    explicit AtmModelSelect(QWidget *parent = 0);

    /**
     * Desctructor.
     */
    ~AtmModelSelect();

    /**
     * Hide tab widgets.
     */
    void hideAllTabs();

    /**
     * Print dispenser serial port.
     *
     * @return Return name serial port.
     */
    static QString printPortDispenser() {
        return AtmModelSelect::m_portDispenser;
    }

    /**
     * Set dispenser serial port. Eg: COM1, COM2, etc...
     */
    static void setPortDispenser(const QString port) {
        AtmModelSelect::m_portDispenser = port;
    }

    /**
     * Print dispenser baudrate.
     *
     * @return Return baudrate.
     */
    static unsigned int printBaudRateDispenser() {
        return AtmModelSelect::m_baudRateDispenser;
    }

    /**
     * Set dispenser baudrate. Eg: 9600, 38400, etc...
     */
    static void setBaudRateDispenser(const unsigned int baudrate) {
        AtmModelSelect::m_baudRateDispenser = baudrate;
    }

    /**
     * Print dispenser databit.
     *
     * @return Return databit.
     */
    static unsigned int printDataBitDispenser() {
        return AtmModelSelect::m_dataBitDispenser;
    }

    /**
     * Set dispenser databit. Eg: 9600, 38400, etc...
     */
    static void setDataBitDispenser(const unsigned int databit) {
        AtmModelSelect::m_dataBitDispenser = databit;
    }

    /**
     * Print dispenser parity.
     *
     * @return Return parity.
     */
    static unsigned int printParityDispenser() {
        return AtmModelSelect::m_parityDispenser;
    }

    /**
     * Set dispenser parity. Eg: NoParity, EvenParity, etc...
     */
    static void setParityDispenser(const unsigned int parity) {
        AtmModelSelect::m_parityDispenser = parity;
    }

    /**
     * Print dispenser stopbit.
     *
     * @return Return stopbit.
     */
    static unsigned int printStopBitDispenser() {
        return AtmModelSelect::m_stopBitDispenser;
    }

    /**
     * Set dispenser stopdabits. Eg: OneStop, TwoStop, etc...
     */
    static void setStopBitsDispenser(const unsigned int bits) {
        AtmModelSelect::m_stopBitDispenser = bits;
    }

    /**
     * Print dispenser flow control.
     *
     * @return Return flow control.
     */
    static unsigned int printFlowControlDispenser() {
        return AtmModelSelect::m_flowControlDispenser;
    }

    /**
     * Set dispenser flow control. Eg: NoFlowControl, HardwareFlowControl, etc...
     */
    static void setFlowControlDispenser(const unsigned int flow) {
        AtmModelSelect::m_flowControlDispenser = flow;
    }

    /**
     * Print bill validator serial port.
     *
     * @return Return name serial port.
     */
    static QString printPortBillValidator() {
        return AtmModelSelect::m_portBillValidator;
    }

    /**
     * Set bill validator serial port. Eg: COM1, COM2, etc...
     */
    static void setPortBillValidator(const QString port) {
        AtmModelSelect::m_portBillValidator = port;
    }

    /**
     * Print bill validator baudrate.
     *
     * @return Return baudrate.
     */
    static unsigned int printBaudRateBillValidator() {
        return AtmModelSelect::m_baudRateBillValidator;
    }

    /**
     * Set bill validator baudrate. Eg: 9600, 38400, etc...
     */
    static void setBaudRateBillValidator(const unsigned int baudrate) {
        AtmModelSelect::m_baudRateBillValidator = baudrate;
    }

    /**
     * Print bill validator databit.
     *
     * @return Return databit.
     */
    static unsigned int printDataBitBillValidator() {
        return AtmModelSelect::m_dataBitBillValidator;
    }

    /**
     * Set bill validaotr databit. Eg: 9600, 38400, etc...
     */
    static void setDataBitBillValidator(const unsigned int databit) {
        AtmModelSelect::m_dataBitBillValidator = databit;
    }

    /**
     * Print bill validator parity.
     *
     * @return Return parity.
     */
    static unsigned int printParityBillValidator() {
        return AtmModelSelect::m_parityBillValidator;
    }

    /**
     * Set bill validaotr parity. Eg: NoParity, EvenParity, etc...
     */
    static void setParityBillValidator(const unsigned int parity) {
        AtmModelSelect::m_parityBillValidator = parity;
    }

    /**
     * Print bill validator stopbit.
     *
     * @return Return stopbit.
     */
    static unsigned int printStopBitBillValidator() {
        return AtmModelSelect::m_stopBitBillValidator;
    }

    /**
     * Set bill validator stopdabits. Eg: OneStop, TwoStop, etc...
     */
    static void setStopBitsBillValidator(const unsigned int bits) {
        AtmModelSelect::m_stopBitBillValidator = bits;
    }

    /**
     * Print bill validator flow control.
     *
     * @return Return flow control.
     */
    static unsigned int printFlowControlBillValidator() {
        return AtmModelSelect::m_flowControlBillValidator;
    }

    /**
     * Set bill validator flow control. Eg: NoFlowControl, HardwareFlowControl, etc...
     */
    static void setFlowControlBillValidator(const unsigned int flow) {
        AtmModelSelect::m_flowControlBillValidator = flow;
    }

    /**
     * Print coin validator serial port.
     *
     * @return Return name serial port.
     */
    static QString printPortCoinValidator() {
        return AtmModelSelect::m_portCoinValidator;
    }

    /**
     * Set coin validator serial port. Eg: COM1, COM2, etc...
     */
    static void setPortCoinValidator(const QString port) {
        AtmModelSelect::m_portCoinValidator = port;
    }

    /**
     * Print coin validator baudrate.
     *
     * @return Return baudrate.
     */
    static unsigned int printBaudRateCoinValidator() {
        return AtmModelSelect::m_baudRateCoinValidator;
    }

    /**
     * Set coin validator baudrate. Eg: 9600, 38400, etc...
     */
    static void setBaudRateCoinValidator(const unsigned int baudrate) {
        AtmModelSelect::m_baudRateCoinValidator = baudrate;
    }

    /**
     * Print coin validator databit.
     *
     * @return Return databit.
     */
    static unsigned int printDataBitCoinValidator() {
        return AtmModelSelect::m_dataBitCoinValidator;
    }

    /**
     * Set coin validator databit. Eg: 9600, 38400, etc...
     */
    static void setDataBitCoinValidator(const unsigned int databit) {
        AtmModelSelect::m_dataBitCoinValidator = databit;
    }

    /**
     * Print coin validator parity.
     *
     * @return Return parity.
     */
    static unsigned int printParityCoinValidator() {
        return AtmModelSelect::m_parityCoinValidator;
    }

    /**
     * Set coin validator parity. Eg: NoParity, EvenParity, etc...
     */
    static void setParityCoinValidator(const unsigned int parity) {
        AtmModelSelect::m_parityCoinValidator = parity;
    }

    /**
     * Print coin validator stopbit.
     *
     * @return Return stopbit.
     */
    static unsigned int printStopBitCoinValidator() {
        return AtmModelSelect::m_stopBitCoinValidator;
    }

    /**
     * Set coin validator stopdabits. Eg: OneStop, TwoStop, etc...
     */
    static void setStopBitsCoinValidator(const unsigned int bits) {
        AtmModelSelect::m_stopBitCoinValidator = bits;
    }

    /**
     * Print coin validator flow control.
     *
     * @return Return flow control.
     */
    static unsigned int printFlowControlCoinValidator() {
        return AtmModelSelect::m_flowControlCoinValidator;
    }

    /**
     * Set coin validator flow control. Eg: NoFlowControl, HardwareFlowControl, etc...
     */
    static void setFlowControlCoinValidator(const unsigned int flow) {
        AtmModelSelect::m_flowControlCoinValidator = flow;
    }

    /**
     * Print multi/io serial port.
     *
     * @return Return name serial port.
     */
    static QString printPortMultiIO() {
        return AtmModelSelect::m_portMultiIO;
    }

    /**
     * Set multi/io serial port. Eg: COM1, COM2, etc...
     */
    static void setPortMultiIO(const QString port) {
        AtmModelSelect::m_portMultiIO = port;
    }

    /**
     * Print multi/io baudrate.
     *
     * @return Return baudrate.
     */
    static unsigned int printBaudRateMultiIO() {
        return AtmModelSelect::m_baudRateMultiIO;
    }

    /**
     * Set multi/io baudrate. Eg: 9600, 38400, etc...
     */
    static void setBaudRateMultiIO(const unsigned int baudrate) {
        AtmModelSelect::m_baudRateMultiIO = baudrate;
    }

    /**
     * Print multi/io databit.
     *
     * @return Return databit.
     */
    static unsigned int printDataBitMultiIO() {
        return AtmModelSelect::m_dataBitMultiIO;
    }

    /**
     * Set multi/io databit. Eg: 9600, 38400, etc...
     */
    static void setDataBitMultiIO(const unsigned int databit) {
        AtmModelSelect::m_dataBitMultiIO = databit;
    }

    /**
     * Print multi/io parity.
     *
     * @return Return parity.
     */
    static unsigned int printParityMultiIO() {
        return AtmModelSelect::m_parityMultiIO;
    }

    /**
     * Set multi/io parity. Eg: NoParity, EvenParity, etc...
     */
    static void setParityMultiIO(const unsigned int parity) {
        AtmModelSelect::m_parityMultiIO = parity;
    }

    /**
     * Print multi/io stopbit.
     *
     * @return Return stopbit.
     */
    static unsigned int printStopBitMultiIO() {
        return AtmModelSelect::m_stopBitMultiIO;
    }

    /**
     * Set multi/io stopdabits. Eg: OneStop, TwoStop, etc...
     */
    static void setStopBitsMultiIO(const unsigned int bits) {
        AtmModelSelect::m_stopBitMultiIO = bits;
    }

    /**
     * Print multi/io flow control.
     *
     * @return Return flow control.
     */
    static unsigned int printFlowControlMultiIO() {
        return AtmModelSelect::m_flowControlMultiIO;
    }

    /**
     * Set multi/io flow control. Eg: NoFlowControl, HardwareFlowControl, etc...
     */
    static void setFlowControlMultiIO(const unsigned int flow) {
        AtmModelSelect::m_flowControlMultiIO = flow;
    }

    /**
     * Print loopback serial port.
     *
     * @return Return name serial port.
     */
    static QString printPortSerialLoopBack() {
        return AtmModelSelect::m_portSerialLoopBack;
    }

    /**
     * Set loopback serial port. Eg: COM1, COM2, etc...
     */
    static void setPortSerialLoopBack(const QString port) {
        AtmModelSelect::m_portSerialLoopBack = port;
    }

    /**
     * Print loopback baudrate.
     *
     * @return Return baudrate.
     */
    static unsigned int printBaudRateSerialLoopBack() {
        return AtmModelSelect::m_baudRateSerialLoopBack;
    }

    /**
     * Set loopback baudrate. Eg: 9600, 38400, etc...
     */
    static void setBaudRateSerialLoopBack(const unsigned int baudrate) {
        AtmModelSelect::m_baudRateSerialLoopBack = baudrate;
    }

    /**
     * Print loopback databit.
     *
     * @return Return databit.
     */
    static unsigned int printDataBitSerialLoopBack() {
        return AtmModelSelect::m_dataBitSerialLoopBack;
    }

    /**
     * Set loopback databit. Eg: 9600, 38400, etc...
     */
    static void setDataBitSerialLoopBack(const unsigned int databit) {
        AtmModelSelect::m_dataBitSerialLoopBack = databit;
    }

    /**
     * Print loopback parity.
     *
     * @return Return parity.
     */
    static unsigned int printParitySerialLoopBack() {
        return AtmModelSelect::m_paritySerialLoopBack;
    }

    /**
     * Set loopback parity. Eg: NoParity, EvenParity, etc...
     */
    static void setParitySerialLoopBack(const unsigned int parity) {
        AtmModelSelect::m_paritySerialLoopBack = parity;
    }

    /**
     * Print loopback stopbit.
     *
     * @return Return stopbit.
     */
    static unsigned int printStopBitSerialLoopBack() {
        return AtmModelSelect::m_stopBitSerialLoopBack;
    }

    /**
     * Set loopback stopdabits. Eg: OneStop, TwoStop, etc...
     */
    static void setStopBitsSerialLoopBack(const unsigned int bits) {
        AtmModelSelect::m_stopBitSerialLoopBack = bits;
    }

    /**
     * Print loopback flow control.
     *
     * @return Return flow control.
     */
    static unsigned int printFlowControlSerialLoopBack() {
        return AtmModelSelect::m_flowControlSerialLoopBack;
    }

    /**
     * Set loopback flow control. Eg: NoFlowControl, HardwareFlowControl, etc...
     */
    static void setFlowControlSerialLoopBack(const unsigned int flow) {
        AtmModelSelect::m_flowControlSerialLoopBack = flow;
    }

    /**
     * Print printer serial port.
     *
     * @return Return name serial port.
     */
    static QString printPortPrinter() {
        return AtmModelSelect::m_portPrinter;
    }

    /**
     * Print UPS serial port.
     *
     * @return Return name serial port.
     */
    static QString printPortUps() {
        return AtmModelSelect::m_portUps;
    }

    /**
     * Print the name language selected by user.
     *
     * Eg:
     *
     * 1) EN = Englisg language;
     * 2) pt_BR = Portuguese language.
     *
     * @return Return name the language selected.
     */
    static QString printLanguageSelect();

Q_SIGNALS:
    /**
     * Signal triggered when the widget is showed.
     */
    void signalShow();

    /**
     * Signal triggered when the widget is canceled.
     */
    void signalCancel();

private slots:
    /**
     * Change the language selected in system.
     *
     * @param index - The number index.
     */
    void handleSelectionLangChanged(int index = 0);

    /**
     * Change the the ATM selected.
     *
     * @param index - The number index.
     */
    void handleSelectionModelATMChanged(int index = 0);

    /**
     * Call widget serial settings (dispenser).
     */
    void handleSerialSettingsDispenser();

    /**
     * Call widget serial settings (billvalidator).
     */
    void handleSerialSettingsBillValidator();

    /**
     * Call widget serial settings (coinvalidator).
     */
    void handleSerialSettingsCoinValidator();

    /**
     * Call widget serial settings (multi/io).
     */
    void handleSerialSettingsMultiIo();

    /**
     * Call console widget.
     */
    void handleConsole();

    /**
     * Call search widget.
     */
    void handleSearchDevices();

    /**
     * Call signal show and close this widget.
     *
     * @see See signalShow();
     */
    void handleShow();

    /**
     * Call signal show and cancel/close this widget.
     *
     * @see See signalCancel();
     */
    void handleCancel();

    /**
     * Activated in combobox the device dispenser.
     *
     * @param index - The number index.
     */
    void handleComboBoxDispenserActivated(int index = 0);

    /**
     * Activated in combobox the device multi/io.
     *
     * @param index - The number index.
     */
    void handleComboBoxMultiActivated(int index = 0);

    /**
     * Activated in combobox the device billvalidator.
     *
     * @param index - The number index.
     */
    void handleComboBoxBoxBillValidatorActivated(int index = 0);

    /**
     * Activated in combobox the device coinvalidator.
     *
     * @param index - The number index.
     */
    void handleComboBoxCoinValidatorActivated(int index = 0);

    /**
     * Make the update in serial ports.
     */
    void updatePortsATMEmpresa1();
    
private:
    /// Store instance (ui interface).
    Ui::AtmModelSelect *ui;

    /// When the language is changed.
    QTranslator *m_translator;

    /// Store the portname selected.
    static QStringList m_portName;

    /// Instance for class Settings Serial.
    SettingsSerial *m_settingsserial;

    /// Instance for class Console.
    Console *m_console;

    /// Instance for class Search Devices.
    SearchDevices* m_searchDevices;

    /// Store dispenser: Serial Port.
    static QString m_portDispenser;

    /// Store dispenser: Baud Rate.
    static unsigned int m_baudRateDispenser;

    /// Store dispenser: Data Bit.
    static unsigned int m_dataBitDispenser;

    /// Store dispenser: Parity.
    static unsigned int m_parityDispenser;

    /// Store dispenser: Stop Bit.
    static unsigned int m_stopBitDispenser;

    /// Store dispenser: Flow Control.
    static unsigned int m_flowControlDispenser;

    /// Store bill validator: Serial Port.
    static QString m_portBillValidator;

    /// Store bill validator: Baute Rate.
    static unsigned int m_baudRateBillValidator;

    /// Store bill validator: Data Bit.
    static unsigned int m_dataBitBillValidator;

    /// Store bill validator: Parity.
    static unsigned int m_parityBillValidator;

    /// Store bill validator: Stop Bit.
    static unsigned int m_stopBitBillValidator;

    /// Store bill validator: Flow Control.
    static unsigned int m_flowControlBillValidator;

    /// Store coin validator: Serial Port.
    static QString m_portCoinValidator;

    /// Store coin validator: Baute Rate.
    static unsigned int m_baudRateCoinValidator;

    /// Store coin validator: Data Bit.
    static unsigned int m_dataBitCoinValidator;

    /// Store coin validator: Parity.
    static unsigned int m_parityCoinValidator;

    /// Store coin validator: Stop Bit.
    static unsigned int m_stopBitCoinValidator;

    /// Store coin validator: Flow Control.
    static unsigned int m_flowControlCoinValidator;

    /// Store multi/io: Serial Port.
    static QString m_portMultiIO;

    /// Store multi/io: Baud Rate.
    static unsigned int m_baudRateMultiIO;

    /// Store multi/io: Data Bit.
    static unsigned int m_dataBitMultiIO;

    /// Store multi/io: Parity.
    static unsigned int m_parityMultiIO;

    /// Store multi/io: Stop Bit.
    static unsigned int m_stopBitMultiIO;

    /// Store multi/io: Flow Control.
    static unsigned int m_flowControlMultiIO;

    /// Store loopback: Serial Port.
    static QString m_portSerialLoopBack;

    /// Store loopback: Baude Rate.
    static unsigned int m_baudRateSerialLoopBack;

    /// Store loopback: Data Bit.
    static unsigned int m_dataBitSerialLoopBack;

    /// Store loopback: Parity.
    static unsigned int m_paritySerialLoopBack;

    /// Store loopback: Stop Bit.
    static unsigned int m_stopBitSerialLoopBack;

    /// Store loopback: Flow Control.
    static unsigned int m_flowControlSerialLoopBack;

    /// Store port: Printer.
    static QString m_portPrinter;

    /// Store port: UPS.
    static QString m_portUps;

    /// Store the name language seleted by user.
    static QString m_languageSelected;

    /// Store number serial ports found in system.
    int m_countSerialPorts;

protected:
    /**
     * Reimplemented method, triggered automatically when the widget
     * is closed.
     *
     * @param event - Some event.
     */
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

    /**
     * Reimplemented method, triggered automatically when the widget
     * is changed.
     *
     * @param event - Some event.
     */
    void changeEvent(QEvent *event) Q_DECL_OVERRIDE;

    /**
     * Used for change application language.
     *
     * @param language - Name languange to change. 
     */
    void changeLanguageTo(const QString &language);

    /**
     * Get serial ports avaliable in system.
     */
    void getSerialPorts(); 

    /**
     * Set default ports for ATM-E1.
     * 
     * @see See too updatePortsATMEmpresa1();
     */
    void setPortsATMEmpresa1();

    /**
     * Check and validate serial ports.
     *
     * @return Return true if check is ok; otherwize false.
     */
    bool checkComboBoxSerialPorts();
};

#endif // ATMMODELSELECT_HPP
