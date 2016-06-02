/**
 * @file dispenserfit.hpp
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

#ifndef DISPENSERFIT_HPP
#define DISPENSERFIT_HPP

#include <serialcommandsfit.hpp>

#include <QWidget>
#include <QCloseEvent>
#include <QTimer>

// Override!
namespace Ui {
    class DispenserFit;
}

/**
 * @class DispenserFit
 *
 * @brief Widget UI that executes commands for the Dispenser.
 *
 * @details This class contains the UI, it executes commands related the
 * Dispenser device.
 */
class DispenserFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - The widget parent.
     */
    explicit DispenserFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~DispenserFit();

private slots:
    /**
     * Show level the cards in tower.
     */
    void showTowerLevel();

    /**
     * Check if exist cards rejected in box reject.
     */
    void checkCardRejectBox();

    /**
     * Called by combo box, change position cards in dispenser.
     *
     * @param index - Command number.
     */
    void handleSelectionCardPositionChanged(int index = 0);

    /**
     * Called by combo box, change the key, used to read or write
     * in mifare card.
     *
     * @param index - Type key.
     */
    void handleSelectionKeyChanged(int index = 0);

    /**
     * Called by combo box, change the key, used to write data in
     * mifare card.
     *
     * @param index - Type key.
     */
    void handleSelectionWriteDataChanged(int index = 0);

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
     * Read the number serial card.
     */
    void readSerialCardNumber();

    /**
     * Send APDU to SAM.
     */
    void sendAPDUSamCard();

    /**
     * Read card block.
     */
    void readBlock();

    /**
     * Write card block.
     */
    void writeBlock();

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
    Ui::DispenserFit *ui;

    /// The timer for count.
    QTimer *m_timer;

    /// Ther timer for command monitor value.
    QTimer *m_monitorCoinLoaded;

    /// Store last commands (serial).
    SerialCommandsFit *m_serialfittab1;

    /// Store last commands (serial).
    SerialCommandsFit *m_serialfittab2;

    /// Store the count time.
    int m_counttime;

    /// Store load key.
    std::vector<unsigned char> m_loadKey;

    /// Store write data.
    std::vector<unsigned char> m_writeData;

    /// Store style in progress bar.
    QString m_progressRed;

    /// Store style in progress bar.
    QString m_progressGreen;

    /// Store style in progress bar.
    QString m_progressBlue;
};

#endif // DISPENSERFIT_HPP
