/**
 * @file multiiofit.hpp
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

#ifndef MULTIIOFIT_HPP
#define MULTIIOFIT_HPP

#include <QWidget>
#include <QTimer>
#include <QCloseEvent>
#include <QIcon>

// Override!
namespace Ui {
    class MultiIoFit;
}

// Override!
class SerialCommandsFit;

/**
 * @class MultiIoFit
 *
 * @brief Widget UI that executes commands for the Multi/IO.
 *
 * @details This class contains the UI, it executes commands related the
 * Multi/IO device.
 */
class MultiIoFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit MultiIoFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~MultiIoFit();

private slots:    
    /**
     * Count the time connection elapsed.
     */
    void countUp();

    /**
     * Opens the connection to the device.
     */
    void deviceConnect();

    /**
     * Close the connection to the device.
     */
    void deviceDisconnect();

    /**
     * Power ON/OFF device: Printer.
     */
    void powerPrinter();

    /**
     * Power ON/OFF device: Dispenser.
     */
    void powerCoinValidator();

    /**
     * Power ON/OFF device: Dispenser.
     */
    void powerDispenser();

    /**
     * Power ON/OFF device: BillValidator.
     */
    void powerBillValidator();

    /**
     * Start (timer) for monitor sensors.
     */
    void startMonitorSensors();

    /**
     * Stop (timer) monitor sensors.
     */
    void stopMonitorSensors();

    /**
     * Monitor sensors actived.
     */
    void monitorSensors();

    /**
     * Called by combo box, change the type sensor for tests.
     *
     * @param index - Command number.
     */
    void handleSelectionSensorsChanged(int index = 0);

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
    Ui::MultiIoFit *ui;

    /// The timer for count.
    QTimer *m_timer;

    /// Monitor sensors.
    QTimer *m_timerSensors;

    /// Store the count time.
    int m_counttime;

    /// Check if device printer is ON or OFF.
    bool m_powerPrinter;

    /// Check if device coin validator is ON or OFF.
    bool m_powerCoinValidator;

    /// Check if device dispenser is ON or OFF.
    bool m_powerDispenser;

    /// Check if device bill validator is ON or OFF.
    bool m_powerBillValidator;

    /// Check the sensor superior door.
    bool m_sensorInternalDoor;

    /// Check the sensor inferior door.
    bool m_sensorExternalDoor;

    /// Check the sensor internal vibration.
    bool m_sensorInternalVibration;

    /// Set icon device ON.
    QIcon m_iconOn;

    /// Set icon device OFF.
    QIcon m_iconOff;
};

#endif // MULTIIOFIT_HPP
