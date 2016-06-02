/**
 * @file  coinvalidatorfit.hpp
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

#ifndef COINVALIDATORFIT_HPP
#define COINVALIDATORFIT_HPP

#include <QWidget>
#include <QTimer>
#include <QCloseEvent>

// Override!
namespace Ui {
    class CoinValidatorFit;
}

// Override!
class SerialCommandsFit;

/**
 * @class CoinValidatorFit
 *
 * @brief Widget UI that executes commands for the CoinValidator.
 *
 * @details This class contains the UI, it executes commands related the
 * CoinValidator device.
 */
class CoinValidatorFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - The widget parent.
     */
    explicit CoinValidatorFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~CoinValidatorFit();

private slots:
    /**
     * Count the time connection elapsed.
     */
    void countUp();

    /**
     * Start command to put the note.
     */
    void startPollCoinValues();

    /**
     * Stop command to put the note.
     */
    void stopPollCoinValues();

    /**
     * Command to reset coin validator.
     */
    void reset();

    /**
     * Command that prints the coin value on the display, when it
     * is identified.
     */
    void monitorCoinValuesLoaded();

    /**
     * Opens the connection to the device.
     */
    void deviceConnect();

    /**
     * Close the connection to the device.
     */
    void deviceDisconnect();

    /**
     * Execute command init/poll.
     */
    void poll();

    /**
     * Command to get version in bill validator.
     */
    void version();

    /**
     * Command to display the type of accepted currency.
     */
    void billTables();

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
    Ui::CoinValidatorFit *ui;

    /// The timer for count.
    QTimer *m_timer;

    /// Ther timer for command poll.
    QTimer *m_timerPoll;

    /// Ther timer for command hold.
    QTimer *m_timerHold;

    /// Ther timer for command monitor value.
    QTimer *m_monitorCoinLoaded;

    /// Store last commands (serial).
    SerialCommandsFit *m_serialfittab1;

    /// Store last commands (serial).
    SerialCommandsFit *m_serialfittab2;

    /// Store the count time.
    int m_counttime;
};

#endif // COINVALIDATORFIT_HPP
