/**
 * @file serialcommandsfit.hpp
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

#ifndef SERIALCOMMANDSFIT_HPP
#define SERIALCOMMANDSFIT_HPP

#include <QWidget>
#include <QCloseEvent>

//! Override.
namespace Ui {
   class SerialCommandsFit;
}

/**
 * @class SerialCommandsFit
 *
 * @brief Widget UI that print serial commands in display.
 *
 * @details This class print serial commands in display, command send
 * to serial/device and commands anwser to serial/device.
 */
class SerialCommandsFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit SerialCommandsFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~SerialCommandsFit();

private slots:
    /**
     * Show the serial commands in lineedit.
     */
    void showSerialCommands();

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
    Ui::SerialCommandsFit *ui;
};

#endif // SERIALCOMMANDSFIT_HPP
