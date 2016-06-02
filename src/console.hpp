/**
 * @file  console.hpp
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

#ifndef CONSOLE_HPP
#define CONSOLE_HPP

#include <QWidget>
#include <QCloseEvent>

// Override!
namespace Ui {
    class Console;
}

/**
 * @class Console
 *
 * @brief Widget UI that executes automaticaly tests in devices.
 *
 * @details This class contains the UI, it responsible for performing
 * automatic tests with the devices.
 */
class Console : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit Console(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~Console();

    /**
     * Write text in black console.
     *
     * @param data - Text to write.
     */
    void putData(const QString &data);

    /**
     * Start automatic testing devices.
     */
    void runDeviceAutoTest();

Q_SIGNALS:
    /**
     * Signal triggered when the widget is showed.
     */
    void signalUpdateObjs();

    /**
     * Signal triggered when the widget is canceled.
     */
    void signalShow();

private slots:
    /**
     * Call MainWindow widget.
     */
    void handleMainWindow();

    /**
     * Call AtmSelect widget.
     */
    void handleAtmSelect();

    /**
     * Show clock time in display.
     */
    void showTime();

    /**
     * Change status progress bar.
     *
     * @param value - The number percent.
     */
    void valueChangedBar(int value = 0);

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
    Ui::Console *ui;
};

#endif // CONSOLE_HPP
