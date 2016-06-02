/**
 * @file billvalidatorfit.cpp
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

#include <billvalidatorfit.hpp>
#include "ui_billvalidatorfit.h"

#include <serialcommandsfit.hpp>
#include <atmmodelselect.hpp>
#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QMessageBox>

BillValidatorFit::BillValidatorFit(QWidget *parent) :
    QWidget(parent), ui(new Ui::BillValidatorFit), m_counttime(0) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Set labels.
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    ui->labelCassetteLevel->setText("");
    ui->lcdNumberNoteLoaded->display("0");

    // Set combo serial.
    if (!AtmModelSelect::printPortBillValidator().isEmpty()) {
        ui->comboBoxSerial->insertItem(0, AtmModelSelect::printPortBillValidator());
    }

    // Timer clock count UP.
    this->m_timer = new QTimer(this);
    QObject::connect(this->m_timer, SIGNAL(timeout()), this, SLOT(countUp()));

    // Poll command timer.
    this->m_timerPoll = new QTimer(this);
    QObject::connect(this->m_timerPoll, SIGNAL(timeout()),
                     this, SLOT(poll()));

    // Hold command timer.
    this->m_timerHold = new QTimer(this);
    QObject::connect(this->m_timerHold, SIGNAL(timeout()),
                     this, SLOT(hold()));

    // Monitor bill notes.
    this->m_monitorNoteLoader = new QTimer(this);
    QObject::connect(this->m_monitorNoteLoader, SIGNAL(timeout()),
                     this, SLOT(monitorNoteLoaded()));

    // Slots and signals.
    QObject::connect(ui->pushButtonConnect, SIGNAL (clicked()),
                     this, SLOT(deviceConnect()));
    QObject::connect(ui->pushButtonDisconnect, SIGNAL (clicked()),
                     this, SLOT(deviceDisconnect()));
    QObject::connect(ui->pushButtonStartPoll, SIGNAL (clicked()),
                     this, SLOT(startPollBillNotes()));
    QObject::connect(ui->pushButtonStack , SIGNAL (clicked()),
                     this, SLOT(stackBillNotes()));
    QObject::connect(ui->pushButtonReturn, SIGNAL (clicked()),
                     this, SLOT(returnBillNotes()));
    QObject::connect(ui->pushButtonReset, SIGNAL (clicked()),
                     this, SLOT(reset()));
    QObject::connect(ui->pushButtonVersion, SIGNAL (clicked()),
                     this, SLOT(version()));
    QObject::connect(ui->pushButtonBillTables, SIGNAL (clicked()),
                     this, SLOT(billTables()));
}

BillValidatorFit::~BillValidatorFit() {
    delete ui;
}

void BillValidatorFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void BillValidatorFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void BillValidatorFit::deviceConnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::billvalidator::AbstractCashCode609::getInstance()->open(AtmModelSelect::printPortBillValidator().toStdString(),
                                                                                AtmModelSelect::printBaudRateBillValidator(),
                                                                                AtmModelSelect::printDataBitBillValidator(),
                                                                                AtmModelSelect::printParityBillValidator(),
                                                                                AtmModelSelect::printStopBitBillValidator(),
                                                                                AtmModelSelect::printFlowControlBillValidator());
    if (!cmd) {
        atm::E1::insertLogManualBillValidator("open+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualBillValidator("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error connecting the device! Connection serial port: ") +
                              AtmModelSelect::printPortBillValidator());
        return;
    }

    atm::E1::insertLogManualBillValidator("open+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->start(1000);
    ui->lcdNumber->display(this->m_counttime);
    ui->lcdNumberNoteLoaded->display("0");
    const QString hmsg = "<font color='green' size='2'>" + QObject::tr("YES") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("CashCode-SM-BR1615");
    ui->labelVersion->setText("1.0");
    ui->labelCassetteLevel->setText("GOOD");
    ui->tabWidget->setEnabled(true);
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->pushButtonReturn->setEnabled(false);
    ui->pushButtonStack->setEnabled(false);
    ui->pushButtonStartPoll->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void BillValidatorFit::deviceDisconnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::billvalidator::AbstractCashCode609::getInstance()->close();
    if (!cmd) {
        atm::E1::insertLogManualBillValidator("close+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualBillValidator("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error when disconnecting the device."));
        return;
    }

    atm::E1::insertLogManualBillValidator("close+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->stop();
    ui->lcdNumber->display(this->m_counttime);
    ui->lcdNumberNoteLoaded->display("0");
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    ui->labelCassetteLevel->setText("");
    ui->tabWidget->setEnabled(false);
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void BillValidatorFit::countUp() {
    m_counttime++;
    ui->lcdNumber->display(this->m_counttime);
}

void BillValidatorFit::startPollBillNotes() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualBillValidator("poll+MANUAL=PASSED");

    devices::billvalidator::AbstractCashCode609::getInstance()->enableBillTypes();
    this->m_timerPoll->start(500);
    this->m_monitorNoteLoader->start(100);
    this->m_timerHold->start(9000);
    ui->pushButtonReturn->setEnabled(true);
    ui->pushButtonStack->setEnabled(true);
    ui->pushButtonStartPoll->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("poll+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void BillValidatorFit::poll() {
    devices::billvalidator::AbstractCashCode609::getInstance()->poll();
}

void BillValidatorFit::hold() {
    devices::billvalidator::AbstractCashCode609::getInstance()->hold();
}

void BillValidatorFit::stackBillNotes() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualBillValidator("stackBill+MANUAL=PASSED");

    this->m_timerPoll->stop();
    this->m_monitorNoteLoader->stop();
    this->m_timerHold->stop();
    ui->lcdNumberNoteLoaded->display("0");

    devices::billvalidator::AbstractCashCode609::getInstance()->stackBill();
    this->startPollBillNotes();

    ui->pushButtonStartPoll->setEnabled(false);
    ui->pushButtonStack->setEnabled(false);
    ui->pushButtonReturn->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("stackBill+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void BillValidatorFit::returnBillNotes() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualBillValidator("returnBill+MANUAL=PASSED");

    this->m_timerPoll->stop();
    this->m_monitorNoteLoader->stop();
    this->m_timerHold->stop();
    ui->lcdNumberNoteLoaded->display("0");

    devices::billvalidator::AbstractCashCode609::getInstance()->returnBill();
    this->startPollBillNotes();

    ui->pushButtonStartPoll->setEnabled(false);
    ui->pushButtonStack->setEnabled(false);
    ui->pushButtonReturn->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("returnBill+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void BillValidatorFit::reset() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualBillValidator("reset+MANUAL=PASSED");

    devices::billvalidator::AbstractCashCode609::getInstance()->reset();
    ui->pushButtonStartPoll->setEnabled(true);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("reset+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void BillValidatorFit::version() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualBillValidator("version+MANUAL=PASSED");

    devices::billvalidator::AbstractCashCode609::getInstance()->version();

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("version+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

    QMessageBox::information(this, QCoreApplication::applicationName(),
                             QObject::tr("Firmeware version is: <b>SM-BR1615</b>."));
}

void BillValidatorFit::billTables() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualBillValidator("billTables+MANUAL=PASSED");

    devices::billvalidator::AbstractCashCode609::getInstance()->getBillTable();

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualBillValidator("billTables+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

    QString notes = QString();
    notes += "1- 2-BRA, ";
    notes += "2- 5-BRA, ";
    notes += "3- 10-BRA, ";
    notes += "4- 20-BRA, ";
    notes += "5- 50-BRA, ";
    notes += "6- 100-BRA";

    QMessageBox::information(this, QCoreApplication::applicationName(),
                             QObject::tr("Bill table is: ") + notes + ".");
}

void BillValidatorFit::monitorNoteLoaded() {
    unsigned int val = devices::billvalidator::AbstractCashCode609::getInstance()->printNoteLoadedValue();

    if (val == 1) {
        ui->lcdNumberNoteLoaded->display("2.00");
    } else if (val == 2) {
        ui->lcdNumberNoteLoaded->display("5.00");
    } else if (val == 3) {
        ui->lcdNumberNoteLoaded->display("10.00");
    } else if (val == 4) {
        ui->lcdNumberNoteLoaded->display("20.00");
    } else if (val == 5) {
        ui->lcdNumberNoteLoaded->display("50.00");
    } else if (val == 6) {
        ui->lcdNumberNoteLoaded->display("100.00");
    }

    if (val > 0) {
        devices::billvalidator::AbstractCashCode609::getInstance()->clearNoteLoadedValue();
        QLOG_INFO() << "Bill note: " << val;
    }
}
