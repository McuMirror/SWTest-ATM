/**
 * @file mainwindow.hpp
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

#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QTreeWidgetItem>

// Override!
namespace Ui {
    class MainWindow;
}

// Override!
class AtmModelSelect;
class AtmFit;
class DispenserFit;
class BillValidatorFit;
class CoinValidatorFit;
class PrinterFit;
class SmartCardReaderFit;
class MultiIoFit;
class CameraFit;
class SerialLoopBackFit;
class Console;
class LogPreviewer;

/**
 * @class MainWindow
 *
 * @brief The main widget UI.
 *
 * @details This class contains the main UI.
 */
class MainWindow : public QMainWindow {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - The widget parent.
     */
    explicit MainWindow(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~MainWindow();

public slots:
    /**
     * Show form: ATM select.
     */
    void showAtmSelect();

    /**
     * Show form: ATM select menu.
     */
    void showAtmSelectMenu();

    /**
     * Show form: ATM console.
     */
    void showAtmConsole();

private slots:
    /**
     * Update objects.
     */
    void updateObjects();

    /**
     * Show about widget.
     */
    void about();

    /**
     * Show widget log previewer.
     */
    void showLogPreviewer();

    /**
     * Active objects.
     */
    void activeObjects();

    /**
     * Calls a determinete item to show in display.
     *
     * @param item - Number item.
     * @param col - Column number.
     */
    void handleItemClickedTreeWidgetMain(QTreeWidgetItem *item, int col);

private:
    /**
     * Disable objects.
     *
     * @param enabled - If set true enable objects, false disable objects.
     */
    void disableObjects(bool enabled);

    /**
     * Prepare the TreeWidget.
     */
    void setupTreeWidget();

    /**
     * Shows the message to the user if he really wants to close the SW.
     *
     * @return Returns true if close application; otherwize return false.
     */
    bool maybeClose();

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
     * Reimplemented method, triggered automatically when the widget
     * is repainted.
     *
     * @param event - Some event.
     */
    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

    /**
     * This method is called when you change the language of the software.
     */
    void updateWidgetItems();

    /// Store instance (ui interface).
    Ui::MainWindow *ui;

    /// Store widget: Model Select.
    AtmModelSelect *m_atmModelSelect;

    /**
     * Structure used for the widget fit (devices) from the list.
     *
     * \struct fitWidgetItems
     */
    struct fitWidgetItems {
        /*@{*/
        AtmFit *atmFit;                         /**< Store widget: Atm Fit. */
        DispenserFit *dispenserFit;             /**< Store widget: Dispenser Fit. */
        BillValidatorFit *billValidatorFit;     /**< Store widget: Bill Validator Fit. */
        CoinValidatorFit *coinValidatorFit;     /**< Store widget: Coin Validator Fit. */
        PrinterFit *printerFit;                 /**< Store widget: Printer Fit. */
        SmartCardReaderFit *smartCardReaderFit; /**< Store widget: Smart Card Reader Fit. */
        MultiIoFit *multiIoFit;                 /**< Store widget: Multi/IO Fit. */
        CameraFit *cameraFit;                   /**< Store widget: Camera Fit. */
        SerialLoopBackFit *serialLoopBackFit;   /**< Store widget: Serial Loop Back Fit. */
        /*@}*/
    };

    /// Store instance fit devices widget items.
    fitWidgetItems m_fitWidgetItems;

    /// Store widget: Log Previewer.
    LogPreviewer  *m_logPreviewer;

    /// Store widget: Console.
    Console *m_console;

    /**
     * Structure used for the items (devices) from the list.
     *
     * \struct treeWidgetItems
     */
    struct treeWidgetItems {
        /*@{*/
        QTreeWidgetItem *qtreewidgetitem0; /**< Item: Devices. */
        QTreeWidgetItem *qtreewidgetitem1; /**< Item: ATM Generic. */
        QTreeWidgetItem *qtreewidgetitem2; /**< Item: Dispenser. */
        QTreeWidgetItem *qtreewidgetitem3; /**< Item: BillValidator. */
        QTreeWidgetItem *qtreewidgetitem4; /**< Item: Printer. */
        QTreeWidgetItem *qtreewidgetitem5; /**< Item: CoinValidator. */
        QTreeWidgetItem *qtreewidgetitem6; /**< Item: SmartCard. */
        QTreeWidgetItem *qtreewidgetitem7; /**< Item: Multi/IO. */
        QTreeWidgetItem *qtreewidgetitem8; /**< Item: Camera. */
        QTreeWidgetItem *qtreewidgetitem9; /**< Item: SerialLoopBack. */
        /*@}*/
    };

    /// Store instance widget items.
    treeWidgetItems m_treeWidgetItems;
};

#endif // MAINWINDOW_HPP
