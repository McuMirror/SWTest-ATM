/**
 * @file smartcardreaderfit.hpp
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

#ifndef SMARTCARDREADERFIT_HPP
#define SMARTCARDREADERFIT_HPP

#include <QWidget>
#include <QTimer>
#include <QCloseEvent>

// Override!
namespace Ui {
    class SmartCardReaderFit;
}

// Override!
class SerialCommandsFit;

/**
 * @class SmartCardReaderFit
 *
 * @brief Widget UI that executes commands for the SmartCard.
 *
 * @details This class contains the UI, it executes commands related the
 * SmartCard device.
 */
class SmartCardReaderFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit SmartCardReaderFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~SmartCardReaderFit();

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
     * Print serial card number.
     */
    void printSerialCardNumber();

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
     * Select SAM slot number 1.
     */
    void changeSlot1();

    /**
     * Select SAM slot number 2.
     */
    void changeSlot2();

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
    Ui::SmartCardReaderFit *ui;

    /// The timer for count.
    QTimer *m_timer;

    /// Store the count time.
    int m_counttime;

    /// Check slot number changed.
    unsigned int m_slotNumberChanged;

    /// Store load key.
    std::vector<unsigned char> m_loadKey;

    /// Store write data.
    std::vector<unsigned char> m_writeData;
};

#endif // SMARTCARDREADERFIT_HPP
