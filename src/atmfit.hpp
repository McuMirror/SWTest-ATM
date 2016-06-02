/**
 * @file atmfit.hpp
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

#ifndef ATMFIT_HPP
#define ATMFIT_HPP

#include <QWidget>
#include <QUrl>
#include <QCloseEvent>

// Override!
namespace Ui {
    class AtmFit;
}

/**
 * @class AtmFit
 *
 * @brief Contains the home page of the company.
 *
 * @details Contains homepage/website of the company.
 */
class AtmFit : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - Some widget.
     */
    explicit AtmFit(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~AtmFit();

    /**
     * It can be passed the URL to be displayed.
     *
     * @param url - Page/site url.
     */
    void setBaseUrl(const QUrl &url);

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
     * Call the URL default.
     */
    void setUrl();

    /// Store instance (ui interface).
    Ui::AtmFit *ui;

    /// Store the url.
    QUrl m_baseUrl;
};

#endif // ATMFIT_H
