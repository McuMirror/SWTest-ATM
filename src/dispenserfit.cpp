/**
 * @file dispenserfit.cpp
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

#include <dispenserfit.hpp>
#include "ui_dispenserfit.h"

#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>
#include <atmmodelselect.hpp>

#include <QMessageBox>
#include <QThread>

DispenserFit::DispenserFit(QWidget *parent) :
    QWidget(parent), ui(new Ui::DispenserFit), m_counttime(0) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Set labels.
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    ui->labelCardLevel->setText("");

    // Set combo serial.
    if (!AtmModelSelect::printPortDispenser().isEmpty()) {
        ui->comboBoxSerial->insertItem(0, AtmModelSelect::printPortDispenser());
    }

    // Set lineedit color.
    QPalette *pblue = new QPalette();
    pblue->setColor(QPalette::Text, Qt::blue);
    ui->lineEditCommandAPDU->setPalette(*pblue);

    QPalette *pred = new QPalette();
    pred->setColor(QPalette::Text,Qt::red);
    ui->lineEditCommandResultAPDU->setPalette(*pred);
    ui->lineEditPrintSerialCardNumber->setPalette(*pred);
    ui->lineEditPrintDataCard->setPalette(*pred);

    // Timer clock count UP.
    this->m_timer = new QTimer(this);
    QObject::connect(this->m_timer, SIGNAL(timeout()), this, SLOT(countUp()));

    // Set progress.
    m_progressRed = ("QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 "
                     "#FF0350,stop: 0.4999 #FF0020,stop: 0.5 #FF0019,stop: 1 #FF0000 );}");
    m_progressBlue = ("QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 "
                      "#78d,stop: 0.4999 #46a,stop: 0.5 #45a,stop: 1 #238 );}");
    m_progressGreen = ("QProgressBar::chunk {background: QLinearGradient( x1: 0, y1: 0, x2: 1, y2: 0,stop: 0 "
                       "#008099,stop: 0.4999 #00FF20,stop: 0.5 #00FF19,stop: 1 #00FF00 );}");

    // Bar tower.
    ui->progressBarTowerCardLevel->setStyleSheet(m_progressRed);
    ui->progressBarTowerCardLevel->setValue(0);

    // Serial print commands 1.
    m_serialfittab1 = new SerialCommandsFit(this);
    ui->gridLayoutMoveCards->addWidget(m_serialfittab1);

    // Serial print commands 2.
    m_serialfittab2 = new SerialCommandsFit(this);
    ui->gridLayoutReadCards->addWidget(m_serialfittab2);

    // Slots and signals.
    QObject::connect(ui->pushButtonTowerCardLevel, SIGNAL(clicked()),
                     this, SLOT(showTowerLevel()));
    QObject::connect(ui->comboBoxCardPosition, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(handleSelectionCardPositionChanged(int)));
    QObject::connect(ui->pushButtonConnect, SIGNAL (clicked()),
                     this, SLOT(deviceConnect()));
    QObject::connect(ui->pushButtonDisconnect, SIGNAL (clicked()),
                     this, SLOT(deviceDisconnect()));
    QObject::connect(ui->pushButtonReadSerialCard, SIGNAL(clicked()),
                     this, SLOT(readSerialCardNumber()));
    QObject::connect(ui->pushButtonSendAPDUSamCard, SIGNAL(clicked()),
                     this, SLOT(sendAPDUSamCard()));
    QObject::connect(ui->pushButtonReadBlock, SIGNAL (clicked()),
                     this, SLOT(readBlock()));
    QObject::connect(ui->pushButtonWriteBlock, SIGNAL (clicked()),
                     this, SLOT(writeBlock()));
    QObject::connect(ui->pushButtonCardPresentRejectBox, SIGNAL (clicked()),
                     this, SLOT(checkCardRejectBox()));
    QObject::connect(ui->comboBoxLoadKey, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(handleSelectionKeyChanged(int)));
    QObject::connect(ui->comboBoxWriteData, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(handleSelectionWriteDataChanged(int)));
}

DispenserFit::~DispenserFit() {
    delete ui;
}

void DispenserFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void DispenserFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void DispenserFit::countUp() {
    m_counttime++;
    ui->lcdNumber->display(this->m_counttime);
}

void DispenserFit::handleSelectionKeyChanged(int index) {
    this->m_loadKey.clear();
    if (index == 0) {
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
    } else if (index == 1) {
        this->m_loadKey.push_back(0xA0);
        this->m_loadKey.push_back(0xA1);
        this->m_loadKey.push_back(0xA2);
        this->m_loadKey.push_back(0xA3);
        this->m_loadKey.push_back(0xA4);
        this->m_loadKey.push_back(0xA5);
    } else if (index == 2) {
        this->m_loadKey.push_back(0xB0);
        this->m_loadKey.push_back(0xB1);
        this->m_loadKey.push_back(0xB2);
        this->m_loadKey.push_back(0xB3);
        this->m_loadKey.push_back(0xB4);
        this->m_loadKey.push_back(0xB5);
    }
}

void DispenserFit::handleSelectionWriteDataChanged(int index) {
    this->m_writeData.clear();
    if (index == 0) {
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
    } else if (index == 1) {
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xAA);
    } else if (index == 2) {
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xCC);
    } else if (index == 3) {
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xBB);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xDD);
        this->m_writeData.push_back(0xEE);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xBB);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xDD);
        this->m_writeData.push_back(0xEE);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xAA);
        this->m_writeData.push_back(0xBB);
        this->m_writeData.push_back(0xCC);
        this->m_writeData.push_back(0xDD);
    }
}

void DispenserFit::showTowerLevel() {    
    devices::dispenser::AbstractCrt591::getInstance()->reset();

    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->cardStatus();

    if (!cmd) {        
        atm::E1::insertLogManualDispenser("towerLevelCard+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("towerLevelCard+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        return;
    }

    utils::Tools::getInstance()->startElapsedTimer();
    const int st =  devices::dispenser::AbstractCrt591::getInstance()->printStatusCardLevel();
    if (st == 0) {
        atm::E1::insertLogManualDispenser("towerLevelCard0+MANUAL=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("towerLevelCard0+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        ui->progressBarTowerCardLevel->setValue(0);
        ui->progressBarTowerCardLevel->setStyleSheet(m_progressRed);

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Level: No cards in dispenser tower!"));
    } else if (st == 1) {
        atm::E1::insertLogManualDispenser("towerLevelCard1+MANUAL=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("towerLevelCard1+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        ui->progressBarTowerCardLevel->setValue(50);
        ui->progressBarTowerCardLevel->setStyleSheet(m_progressGreen);

        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("Level: Low cards in dispenser tower!"));
    } else if (st == 2) {        
        atm::E1::insertLogManualDispenser("towerLevelCard2+MANUAL=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("towerLevelCard2+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        ui->progressBarTowerCardLevel->setValue(100);
        ui->progressBarTowerCardLevel->setStyleSheet(m_progressBlue);

        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("Level: High level cards in dispenser tower!"));
    }
}

void DispenserFit::checkCardRejectBox() {
    devices::dispenser::AbstractCrt591::getInstance()->reset();

    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    // Check reject box.
    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->cardStatus(true);

    if (!cmd) {
        atm::E1::insertLogManualDispenser("rejectCardBox+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("rejectCardBox+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        return;
    }

    utils::Tools::getInstance()->startElapsedTimer();
    bool rejectBox = devices::dispenser::AbstractCrt591::getInstance()->isCardRejectBox();
    if (rejectBox) {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("Has rejected cards in the box!"));
    } else {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("There is no card in the rejected box!"));
    }
}

void DispenserFit::deviceConnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->open(AtmModelSelect::printPortDispenser().toStdString(),
                                                                       AtmModelSelect::printBaudRateDispenser(),
                                                                       AtmModelSelect::printDataBitDispenser(),
                                                                       AtmModelSelect::printStopBitDispenser(),
                                                                       AtmModelSelect::printFlowControlDispenser());
    if (!cmd) {
        atm::E1::insertLogManualDispenser("open+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error connecting the device! Connection serial port: ") +
                              AtmModelSelect::printPortDispenser());
        return;
    }

    devices::dispenser::AbstractCrt591::getInstance()->reset();
    atm::E1::insertLogManualDispenser("open+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->start(1000);
    ui->lcdNumber->display(this->m_counttime);
    const QString hmsg = "<font color='green' size='2'>" + QObject::tr("YES") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("CRT-591");
    ui->labelVersion->setText("1.0");
    ui->labelCardLevel->setText("GOOD");
    ui->lineEditCommandAPDU->setText("00 A4 00 00 02 3F 00");
    ui->tabWidget->setEnabled(true);
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->pushButtonCardPresentRejectBox->setEnabled(true);

    // Set current tower level.
    this->showTowerLevel();

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualDispenser("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void  DispenserFit::deviceDisconnect() {
    devices::dispenser::AbstractCrt591::getInstance()->reset();

    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->close();
    if (!cmd) {
        atm::E1::insertLogManualDispenser("close+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error when disconnecting the device."));
        return;
    }

    atm::E1::insertLogManualDispenser("close+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->stop();
    ui->lcdNumber->display(this->m_counttime);
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    ui->labelCardLevel->setText("");
    ui->tabWidget->setEnabled(false);
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonCardPresentRejectBox->setEnabled(false);
    ui->lineEditPrintDataCard->setReadOnly(true);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualDispenser("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void DispenserFit::handleSelectionCardPositionChanged(int index) {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = false;

    switch (index) {
    case 0:
        cmd = true;
        break;
    case 1:
        cmd = devices::dispenser::AbstractCrt591::getInstance()->cardMovePosition(ABSTRACT_CRT591_MOVE_CARD_TO_IC_POSITION);
        if (cmd) {
            atm::E1::insertLogManualDispenser("cardPositionChanged(READING)+MANUAL=PASSED");
        } else {
            atm::E1::insertLogManualDispenser("cardPositionChanged(READING)+MANUAL=FAILED");
        }
        break;
    case 2:
        cmd = devices::dispenser::AbstractCrt591::getInstance()->cardMovePosition(ABSTRACT_CRT591_MOVE_CARD_TO_GATE_EJECT);
        if (cmd) {
            atm::E1::insertLogManualDispenser("cardPositionChanged(EJECT)+MANUAL=PASSED");
        } else {
            atm::E1::insertLogManualDispenser("cardPositionChanged(EJECT)+MANUAL=FAILED");
        }
        break;
    case 3:
        cmd = devices::dispenser::AbstractCrt591::getInstance()->cardMovePosition(ABSTRACT_CRT591_MOVE_CARD_TO_ERROR_BIN);
        cmd = true;
        if (cmd) {
            atm::E1::insertLogManualDispenser("cardPositionChanged(REJECT)+MANUAL=PASSED");
        } else {
            atm::E1::insertLogManualDispenser("cardPositionChanged(REJECT)+MANUAL=FAILED");
        }
        break;
    default:
        break;
    }

    QApplication::restoreOverrideCursor();

    if (!cmd) {
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("cardPositionChanged+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error position the card! Check the level tower in dispenser."));
    }

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualDispenser("cardPositionChanged+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void DispenserFit::readSerialCardNumber() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->cardMifareActivate();
    if (!cmd) {
        atm::E1::insertLogManualDispenser("readSerialCardNumber+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("readSerialCardNumber+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        QApplication::restoreOverrideCursor();

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error read serial, please position a card to read mode."));
    }

    const QString serial = QString::fromStdString(devices::dispenser::AbstractCrt591::getInstance()->printCardMifareSerialNumber());
    ui->lineEditPrintSerialCardNumber->setText(serial);

    if (!serial.isEmpty()) {
        atm::E1::insertLogManualDispenser("readSerialCardNumber+MANUAL=PASSED");
        atm::E1::insertLogManualDispenser("readSerialCardNumber+MANUAL+VALUE=" + serial);
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("readSerialCardNumber+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        ui->lineEditPrintSerialCardNumber->setText(serial.toUpper());
        QApplication::restoreOverrideCursor();
    }

    devices::dispenser::AbstractCrt591::getInstance()->cardMifareDesactivate();
    QApplication::restoreOverrideCursor();
}

void DispenserFit::sendAPDUSamCard() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->samActivate();
    if (!cmd) {
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        QApplication::restoreOverrideCursor();

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error execute command sam activate."));
        return;
    }

    cmd = devices::dispenser::AbstractCrt591::getInstance()->samCouldReset();
    if (!cmd) {
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        QApplication::restoreOverrideCursor();

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error execute command sam could reset."));
        return;
    }

    cmd = devices::dispenser::AbstractCrt591::getInstance()->samStatus();
    if (!cmd) {
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        QApplication::restoreOverrideCursor();

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error execute command sam status."));
        return;
    }

    cmd = devices::dispenser::AbstractCrt591::getInstance()->samIsPresent();
    if (!cmd) {
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        QApplication::restoreOverrideCursor();

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("SAM card not present."));
        return;
    }

    cmd = devices::dispenser::AbstractCrt591::getInstance()->samSendAPDU();
    if (!cmd) {
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        QApplication::restoreOverrideCursor();

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error send command APDU."));
        return;
    }

    const QString rapdu = QString::fromStdString(devices::dispenser::AbstractCrt591::getInstance()->printCardSamAPDU());
    if (!rapdu.isEmpty()) {
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL=PASSED");
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL+VALUE=" + rapdu);
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualDispenser("sendAPDUSamCard+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        QApplication::restoreOverrideCursor();
        ui->lineEditCommandResultAPDU->setText(rapdu);
    }

    QApplication::restoreOverrideCursor();
}

void DispenserFit::readBlock() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    const QString keyCombo = ui->comboBoxLoadKey->currentText();

    if (keyCombo.isEmpty()) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Please select the key first!"));
        return;
    }

    // Set key if key is empty.
    if (this->m_loadKey.empty()) {
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
        this->m_loadKey.push_back(0xFF);
    }

    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->cardMifareActivate();
    if (!cmd) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error read sector, please position a card to read mode."));
    }

    cmd = devices::dispenser::AbstractCrt591::getInstance()->loadKey(this->m_loadKey);
    if (!cmd) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error load key, please try other key."));
        return;
    }

    std::vector<unsigned char> blockSN;
    blockSN.push_back(0x00);

    cmd = devices::dispenser::AbstractCrt591::getInstance()->mifareReadDataBlock(0x01, blockSN);
    if (!cmd) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error reading block!"));
        return;
    }

    const QString readBlock = QString::fromStdString(devices::dispenser::AbstractCrt591::getInstance()->printCardMifareReadDataBlock());
    if (!readBlock.isEmpty()) {
        QApplication::restoreOverrideCursor();
        ui->lineEditPrintDataCard->setText(readBlock);
    }

    devices::dispenser::AbstractCrt591::getInstance()->cardMifareDesactivate();
    QApplication::restoreOverrideCursor();
}

void DispenserFit::writeBlock() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    bool cmd = devices::dispenser::AbstractCrt591::getInstance()->cardMifareActivate();
    if (!cmd) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error read sector, please position a card to read mode."));
    }

    cmd = devices::dispenser::AbstractCrt591::getInstance()->loadKey(this->m_loadKey);
    if (!cmd) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error load key, please try other key."));
        return;
    }


    // Set write data if empty.
    if (this->m_writeData.empty()) {
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
        this->m_writeData.push_back(0xFF);
    }

    cmd = devices::dispenser::AbstractCrt591::getInstance()->mifareWriteDataBlock(0x01, this->m_writeData);
    if (!cmd) {
        QApplication::restoreOverrideCursor();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error writing block!"));
        return;
    }

    devices::dispenser::AbstractCrt591::getInstance()->cardMifareDesactivate();
    this->readBlock();
    QApplication::restoreOverrideCursor();
}
