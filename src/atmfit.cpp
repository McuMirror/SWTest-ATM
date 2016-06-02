/**
 * @file atmfit.cpp
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

#include <atmfit.hpp>
#include "ui_atmfit.h"

#include <atmmodelselect.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QtWidgets>
#include <QTextStream>

AtmFit::AtmFit(QWidget *parent)
    : QWidget(parent), ui(new Ui::AtmFit) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Set URL default.
    this->setUrl();
}

AtmFit::~AtmFit() {
    delete ui;
}

void AtmFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void AtmFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        this->setUrl();
    }
    QWidget::changeEvent(event);
}

void AtmFit::setBaseUrl(const QUrl &url) {
    this->m_baseUrl = url;
}

void AtmFit::setUrl() {
    if (AtmModelSelect::printLanguageSelect() == "EN") {
        ui->webView->setUrl(QUrl("http://www.empresa1.com.br/en/"));
    } else if (AtmModelSelect::printLanguageSelect() == "pt_BR") {
        ui->webView->setUrl(QUrl("http://www.empresa1.com.br/"));
    } else {
        ui->webView->setUrl(QUrl("http://www.empresa1.com.br/en/"));
    }
}
