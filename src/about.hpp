/**
 * @file about.hpp
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

#ifndef ABOUT_HPP
#define ABOUT_HPP

#include <QDialog>

// Override!
namespace Ui {
    class About;
}

/**
 * @class About
 *
 * @brief Class about informations.
 *
 * @details Form containing information about the system.
 */
class About : public QDialog {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit About(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~About();

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
    Ui::About *ui;
};

#endif // ABOUT_HPP
