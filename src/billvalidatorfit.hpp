/**
 * @file billvalidatorfit.hpp
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

#ifndef BILLVALIDATORFIT_HPP
#define BILLVALIDATORFIT_HPP

#include <QWidget>
#include <QTimer>
#include <QCloseEvent>

// Override!
namespace Ui {
    class BillValidatorFit;
}

// Override!
class SerialCommandsFit;

/**
 * @class BillValidatorFit
 *
 * @brief Widget UI that executes commands for the BillValidator.
 *
 * @details This class contains the UI, it executes commands related the
 * BillValidator device.
 */
class BillValidatorFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - The widget parent.
     */
    explicit BillValidatorFit(QWidget *parent = 0);

    /**
     * Desctructor.
     */
    ~BillValidatorFit();

private slots:
    /**
     * Start command to put the note.
     */
    void startPollBillNotes();

    /**
     * Execute command init/poll.
     */
    void poll();

    /**
     * Command to push the note to the cassette.
     */
    void stackBillNotes();

    /**
     * Command to return the note to user.
     */
    void returnBillNotes();

    /**
     * Command to keep the stop note.
     */
    void hold();

    /**
     * Command to reset bill validator.
     */
    void reset();

    /**
     * Command to get version in bill validator.
     */
    void version();

    /**
     * Command to display the type of accepted currency.
     */
    void billTables();

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
     * Command that prints the note on the display, when it
     * is identified.
     */
    void monitorNoteLoaded();

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
    Ui::BillValidatorFit *ui;

    /// The timer for count.
    QTimer *m_timer;

    /// Ther timer for command poll.
    QTimer *m_timerPoll;

    /// Ther timer for command hold.
    QTimer *m_timerHold;

    /// Ther timer for command monitor value.
    QTimer *m_monitorNoteLoader;

    /// Store last commands (serial).
    SerialCommandsFit *m_serialfittab1;

    /// Store last commands (serial).
    SerialCommandsFit *m_serialfittab2;

    /// Store the count time.
    int m_counttime;
};

#endif // BILLVALIDATORFIT_HPP
