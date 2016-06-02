/**
 * @file logpreviewer.hpp
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

#ifndef LOGPREVIEWER_HPP
#define LOGPREVIEWER_HPP

#include <QWidget>
#include <QCloseEvent>
#include <QDateTime>

// Override!
namespace Ui {
    class LogPreviewer;
}

/**
 * @class LogPreviewer
 *
 * @brief Widget UI that generate log report in HTML.
 *
 * @details This class contains the UI, class used to generate the
 * HTML report.
 */
class LogPreviewer : public QWidget {

    Q_OBJECT

public:
    /**
     * Constructor.
     *
     * @param parent - The widget parent.
     */
    explicit LogPreviewer(QWidget *parent = 0);

    /**
     * Destructor.
     */
    ~LogPreviewer();

private slots:
    /**
     * Export an HTML file type to a PDF file type.
     */
    void exportToPdf();

private:
    /**
     * Make the assembly (pieces) of reporting information in html format.
     */
    void report();

    //XXXXXXXXXXXXXXX
    bool saveAs();

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
    Ui::LogPreviewer *ui;

    /// Store datetime.
    QDateTime m_dateTime;
};

#endif // LOGPREVIEWER_H
