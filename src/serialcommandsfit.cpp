/**
 * @file serialcommandsfit.cpp
 *
 * Implementations for a SWTest-ATM.
 *
 * ATM for banks Project.
 *
 * Copyright (c) 2015-2016 BlueSalute Company.
 *
 * @author Ederson Moura - <ederson.moura@bluesalute.com>
 *
 * $Id: Exp$
 */

#include <serialcommandsfit.hpp>
#include "ui_serialcommandsfit.h"

#include <utils/serialport.hpp>
#include <utils/log.hpp>

#include <QTimer>

SerialCommandsFit::SerialCommandsFit(QWidget *parent) :
    QWidget(parent), ui(new Ui::SerialCommandsFit) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load UI.
    ui->setupUi(this);

    // Set lineedit color.
    QPalette *pblue = new QPalette();
    pblue->setColor(QPalette::Text, Qt::blue);
    ui->lineEditSerialCommandSend->setPalette(*pblue);

    QPalette *pred = new QPalette();
    pred->setColor(QPalette::Text,Qt::red);
    ui->lineEditSerialCommandReceived->setPalette(*pred);

    // Timer monitor serial commands.
    QTimer *timerSerialCommands = new QTimer(this);
    QObject::connect(timerSerialCommands, SIGNAL(timeout()), this, SLOT(showSerialCommands()));
    timerSerialCommands->start(500);
}

SerialCommandsFit::~SerialCommandsFit() {
    delete ui;
}

void SerialCommandsFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void SerialCommandsFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void SerialCommandsFit::showSerialCommands() {
    this->ui->lineEditSerialCommandSend->clear();
    this->ui->lineEditSerialCommandReceived->clear();
    this->ui->lineEditSerialCommandSend->setText(utils::SerialPort::printCommandSend());
    this->ui->lineEditSerialCommandReceived->setText(utils::SerialPort::printCommandResponse());
}
