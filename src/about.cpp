/**
 * @file about.cpp
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

#include <about.hpp>
#include "ui_about.h"

#include <utils/log.hpp>
#include <utils/tools.hpp>
#include <QDebug>

About::About(QWidget *parent)
    : QDialog(parent), ui(new Ui::About) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Get infos.
    const QString appInfo = QString::fromStdString(utils::Tools::printAppInfo());
    const QString buildInfo = QString::fromStdString(utils::Tools::printBuildInfo());
    const QString teamInfo = QString::fromStdString(utils::Tools::printTeamInfo());

    // Set infos.
    ui->plainTextEdit->insertPlainText(appInfo + "\n" + buildInfo + "\n" + teamInfo);
}

About::~About() {
    delete ui;
}

void About::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void About::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}
