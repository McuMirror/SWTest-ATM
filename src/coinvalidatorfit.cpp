/**
 * @file coinvalidator.cpp
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

#include <coinvalidatorfit.hpp>
#include "ui_coinvalidatorfit.h"

#include <serialcommandsfit.hpp>
#include <atmmodelselect.hpp>
#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QMessageBox>

CoinValidatorFit::CoinValidatorFit(QWidget *parent) :
    QWidget(parent), ui(new Ui::CoinValidatorFit), m_counttime(0) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Set labels.
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    ui->lcdNumberNoteLoaded->display("0");

    // Set combo serial.
    if (!AtmModelSelect::printPortCoinValidator().isEmpty()) {
        ui->comboBoxSerial->insertItem(0, AtmModelSelect::printPortCoinValidator());
    }

    // Timer clock count UP.
    this->m_timer = new QTimer(this);
    QObject::connect(this->m_timer, SIGNAL(timeout()), this, SLOT(countUp()));

    // Poll command timer.
    this->m_timerPoll = new QTimer(this);
    QObject::connect(this->m_timerPoll, SIGNAL(timeout()),
                     this, SLOT(poll()));

    // Monitor coin values.
    this->m_monitorCoinLoaded = new QTimer(this);
    QObject::connect(this->m_monitorCoinLoaded, SIGNAL(timeout()),
                     this, SLOT(monitorCoinValuesLoaded()));

    // Signals and slots.
    QObject::connect(ui->pushButtonConnect, SIGNAL (clicked()),
                     this, SLOT(deviceConnect()));
    QObject::connect(ui->pushButtonDisconnect, SIGNAL (clicked()),
                     this, SLOT(deviceDisconnect()));
    QObject::connect(ui->pushButtonStartPoll, SIGNAL (clicked()),
                     this, SLOT(startPollCoinValues()));
    QObject::connect(ui->pushButtonStopPoll, SIGNAL (clicked()),
                     this, SLOT(stopPollCoinValues()));
    QObject::connect(ui->pushButtonReset, SIGNAL (clicked()),
                     this, SLOT(reset()));
    QObject::connect(ui->pushButtonVersion, SIGNAL (clicked()),
                     this, SLOT(version()));
    QObject::connect(ui->pushButtonBillTables, SIGNAL (clicked()),
                     this, SLOT(billTables()));
}

CoinValidatorFit::~CoinValidatorFit() {
    delete ui;
}

void CoinValidatorFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void CoinValidatorFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void CoinValidatorFit::countUp() {
    m_counttime++;
    ui->lcdNumber->display(this->m_counttime);
}

void CoinValidatorFit::deviceConnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->open(AtmModelSelect::printPortCoinValidator().toStdString(),
                                                                             AtmModelSelect::printBaudRateCoinValidator(),
                                                                             AtmModelSelect::printDataBitCoinValidator(),
                                                                             AtmModelSelect::printParityCoinValidator(),
                                                                             AtmModelSelect::printStopBitCoinValidator(),
                                                                             AtmModelSelect::printFlowControlCoinValidator());
    if (!cmd) {
        atm::E1::insertLogManualCoinValidator("open+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualCoinValidator("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error connecting the device! Connection serial port: ") +
                              AtmModelSelect::printPortCoinValidator());
        return;
    }

    atm::E1::insertLogManualCoinValidator("open+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->start(1000);
    ui->lcdNumber->display(this->m_counttime);
    ui->lcdNumberNoteLoaded->display("0");
    const QString hmsg = "<font color='green' size='2'>" + QObject::tr("YES") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("MEI");
    ui->labelVersion->setText("0");
    ui->tabWidget->setEnabled(true);
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->pushButtonStartPoll->setEnabled(false);
    ui->pushButtonStopPoll->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualCoinValidator("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

    // Reset command.
    devices::coinvalidator::AbstractMei9520e::getInstance()->reset();

    // Set coin version.
    QString ver = QString("");
    cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->version();
    if (cmd) {
        ver = QString::fromStdString(devices::coinvalidator::AbstractMei9520e::getInstance()->printVersion());
        if (!ver.isEmpty()) {
            ui->labelVersion->setText(ver);
        } else {
            ui->labelVersion->setText("");
        }
    }
}

void CoinValidatorFit::deviceDisconnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->close();
    if (!cmd) {
        atm::E1::insertLogManualCoinValidator("close+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualCoinValidator("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error when disconnecting the device."));
        return;
    }

    atm::E1::insertLogManualCoinValidator("close+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->stop();
    ui->lcdNumber->display(this->m_counttime);
    ui->lcdNumberNoteLoaded->display("0");
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    ui->tabWidget->setEnabled(false);
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonStartPoll->setEnabled(false);
    ui->pushButtonStopPoll->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualCoinValidator("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void CoinValidatorFit::poll() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualCoinValidator("poll+MANUAL=PASSED");

    devices::coinvalidator::AbstractMei9520e::getInstance()->poll();

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualCoinValidator("poll+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void CoinValidatorFit::startPollCoinValues() {
    this->m_timerPoll->start(500);
    this->m_monitorCoinLoaded->start(100);
    ui->pushButtonStartPoll->setEnabled(false);
    ui->pushButtonStopPoll->setEnabled(true);
    ui->pushButtonReset->setEnabled(false);
}

void CoinValidatorFit::stopPollCoinValues() {
    this->m_timerPoll->stop();
    this->m_monitorCoinLoaded->stop();
    ui->pushButtonStartPoll->setEnabled(true);
    ui->pushButtonStopPoll->setEnabled(false);
    ui->pushButtonReset->setEnabled(true);
}

void CoinValidatorFit::monitorCoinValuesLoaded() {
    unsigned int val = devices::coinvalidator::AbstractMei9520e::getInstance()->printCoinLoadedValue();

    if (val == 1) {
        ui->lcdNumberNoteLoaded->display("00.5");
    } else if (val == 2) {
        ui->lcdNumberNoteLoaded->display("00.10");
    } else if (val == 3) {
        ui->lcdNumberNoteLoaded->display("00.25");
    } else if (val == 4 || val == 6) {
        ui->lcdNumberNoteLoaded->display("00.50");
    } else if (val == 5) {
        ui->lcdNumberNoteLoaded->display("1.00");
    }

    if (val > 0) {
        devices::coinvalidator::AbstractMei9520e::getInstance()->clearCoinLoadedValue();
        QLOG_INFO() << "Coin value: " << val;
    }
}

void CoinValidatorFit::reset() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualCoinValidator("reset+MANUAL=PASSED");

    devices::coinvalidator::AbstractMei9520e::getInstance()->reset();
    ui->pushButtonStartPoll->setEnabled(true);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualCoinValidator("reset+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void CoinValidatorFit::version() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualCoinValidator("version+MANUAL=PASSED");

    QString ver = QString("");
    bool cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->version();
    if (cmd) {
        ver = QString::fromStdString(devices::coinvalidator::AbstractMei9520e::getInstance()->printVersion());
        if (!ver.isEmpty()) {
            cmd = true;
        } else {
            cmd = false;
        }
    } else {
        cmd = false;
    }

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualCoinValidator("version+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

    if (cmd) {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("Firmeware version is: ") + ver + ".");
    } else {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error execute command get version!"));
    }

}

void CoinValidatorFit::billTables() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualCoinValidator("billTables+MANUAL=PASSED");

    QString notes = QString();
    notes += "1- 00.5-BRA, ";
    notes += "2- 00.10-BRA, ";
    notes += "3- 00.25-BRA, ";
    notes += "4- 00.50-BRA, ";
    notes += "5- 1.00-BRA";

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualCoinValidator("billTables+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

    QMessageBox::information(this, QCoreApplication::applicationName(),
                             QObject::tr("Bill table is: ") + notes + ".");
}
