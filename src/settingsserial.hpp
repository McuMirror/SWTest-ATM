/**
 * @file settingsserial.hpp
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

#ifndef SETTINGSDIALOG_HPP
#define SETTINGSDIALOG_HPP

#include <QDialog>
#include <QtSerialPort/QSerialPort>
#include <QWidget>
#include <QCloseEvent>

// Override!
namespace Ui {
    class SettingsSerial;
}

// Override!
class QIntValidator;
class SerialLoopBackFit;

/**
 * @class SettingsSerial
 *
 * @brief Widget UI that making the settings of the serial ports.
 *
 * @details Class responsible for making the settings of the serial ports.
 */
class SettingsSerial : public QDialog {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit SettingsSerial(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~SettingsSerial();

    /**
     * Set device used in configuration:
     *
     * Eg. Dispenser, BillValidator, CoinValidator, etc...
     *
     * @param device - name device.
     */
    void setDevice(const QString device = QString());

private slots:
    /**
     * Called by combo box, show serial port information.
     *
     * @param index - Number index.
     */
    void showPortInfo(int index = 0);

    /**
     * Apply configuration.
     */
    void apply();

    /**
     * Check custom baudrate policy.
     *
     * @param index - Number index.
     */
    void checkCustomBaudRatePolicy(int index);

    /**
     * Call widget: LoopbackSerial.
     */
    void handleLoopbackSerial();

    /**
     * Call widget: SerialLoopBack.
     */
    void closeSerialLoopBack();

private:
    /**
     * Set ports parameters.
     */
    void fillPortsParameters();

    /**
     * Ports information.
     */
    void fillPortsInfo();

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

    /// Store instance (ui interface).
    Ui::SettingsSerial *ui;

    /// Store instance integer validator.
    QIntValidator *m_intValidator;

    /// Store instance (LoopBackFit).
    SerialLoopBackFit* m_serialLoopBackFit;

    /// Check dispenser active.
    bool m_deviceDispenser;

    /// Check bill validator active.
    bool m_deviceBillValidator;

    /// Check coin validator active.
    bool m_deviceCoinValidator;

    /// Check multi/io active.
    bool m_deviceMultiIo;

    /// Check loopback active.
    bool m_deviceSerialLoopBack;
};

#endif // SETTINGSSERIAL_HPP
