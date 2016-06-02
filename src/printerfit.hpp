/**
 * @file printerfit.hpp
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

#ifndef PRINTERFIT_HPP
#define PRINTERFIT_HPP

#include <QWidget>
#include <QUrl>
#include <QCloseEvent>

// Override!
namespace Ui {
    class PrinterFit;
}

/**
 * @class PrinterFit
 *
 * @brief Widget UI that executes commands for the Printer.
 *
 * @details This class contains the UI, it executes commands related the
 * Printer device.
 */
class PrinterFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit PrinterFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~PrinterFit();

private slots:
    /**
     * Get information about printer. Eg: Name printer, idle,
     * description, etc...
     */
    void getInfo();

    /**
     * Call page test, for send to printer.
     */
    void handlePrinterSetup();

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
    Ui::PrinterFit *ui;
};

#endif // PRINTERFIT_HPP
