/**
 * @file smartcardreaderfit.cpp
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

#include <smartcardreaderfit.hpp>
#include "ui_smartcardreaderfit.h"

#include <serialcommandsfit.hpp>
#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QTime>
#include <QMessageBox>
#include <QProcess>
#include <QFileInfo>

SmartCardReaderFit::SmartCardReaderFit(QWidget *parent) :
    QWidget(parent), ui(new Ui::SmartCardReaderFit), m_counttime(0), m_slotNumberChanged(1) {
    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Set labels.
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");

    // Set palette color.
    QPalette *pred = new QPalette();
    pred->setColor(QPalette::Text, Qt::red);
    ui->lineEditCommandResult->setPalette(*pred);
    ui->lineEditPrintDataCard->setPalette(*pred);

    // Timer clock count UP.
    this->m_timer = new QTimer(this);

    // Slots and signals.
    QObject::connect(this->m_timer, SIGNAL(timeout()),
                     this, SLOT(countUp()));
    QObject::connect(ui->pushButtonReadSerialCard, SIGNAL (clicked()),
                     this, SLOT(printSerialCardNumber()));
    QObject::connect(ui->pushButtonConnect, SIGNAL (clicked()),
                     this, SLOT(deviceConnect()));
    QObject::connect(ui->pushButtonDisconnect, SIGNAL (clicked()),
                     this, SLOT(deviceDisconnect()));
    QObject::connect(ui->pushButtonChangeSlot1, SIGNAL (clicked()),
                     this, SLOT(changeSlot1()));
    QObject::connect(ui->pushButtonChangeSlot2, SIGNAL (clicked()),
                     this, SLOT(changeSlot2()));
    QObject::connect(ui->pushButtonReadBlock, SIGNAL (clicked()),
                     this, SLOT(readBlock()));
    QObject::connect(ui->pushButtonWriteBlock, SIGNAL (clicked()),
                     this, SLOT(writeBlock()));
    QObject::connect(ui->comboBoxLoadKey, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(handleSelectionKeyChanged(int)));
    QObject::connect(ui->comboBoxWriteData, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(handleSelectionWriteDataChanged(int)));
}

SmartCardReaderFit::~SmartCardReaderFit() {
    delete ui;
}

void SmartCardReaderFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void SmartCardReaderFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void SmartCardReaderFit::countUp() {
    m_counttime++;
    ui->lcdNumber->display(this->m_counttime);
}

void SmartCardReaderFit::handleSelectionWriteDataChanged(int index) {
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

void SmartCardReaderFit::handleSelectionKeyChanged(int index) {
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

void SmartCardReaderFit::deviceConnect() {
    ui->comboBoxSmartCardReader->clear();

    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool readers = devices::cardreader::AbstractCrt603Cz1::getInstance()->getReaders();
    if (!readers) {
        atm::E1::insertLogManualSmartCard("open+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualSmartCard("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("SmartCard reader not found in system."));
        return;
    } else {
        QIcon icon;
        icon.addFile(QStringLiteral(":/res/images/usbport.png"), QSize(), QIcon::Normal, QIcon::Off);
        QVector<LPTSTR> v = QVector<LPTSTR>::fromStdVector(devices::cardreader::AbstractCrt603Cz1::getInstance()->printReaders());
        for (register int i = 0; i < v.size(); ++i) {
            if (v.size() >= 2) {
                ui->comboBoxSmartCardReader->addItem(icon, "Creator CRT-603 RF");
                ui->comboBoxSmartCardReader->addItem(icon, "Creator CRT-603 SAM");
                break;
            }
        }
    }

    bool cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->open();
    if (!cmd) {
        atm::E1::insertLogManualSmartCard("open+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualSmartCard("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error connecting, please insert the card in the reader."));
        return;
    }

    utils::Tools::getInstance()->startElapsedTimer();
    atm::E1::insertLogManualSmartCard("open+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->start(1000);
    ui->lcdNumber->display(this->m_counttime);
    const QString hmsg = "<font color='green' size='2'>" + QObject::tr("YES") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("CRT-603-CZ1");
    ui->labelVersion->setText("1.0");
    ui->tabWidget->setEnabled(true);
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->pushButtonReadBlock->setEnabled(true);
    ui->pushButtonWriteBlock->setEnabled(true);
    ui->comboBoxLoadKey->setEnabled(true);
    ui->lineEditPrintDataCard->setReadOnly(true);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualSmartCard("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void SmartCardReaderFit::deviceDisconnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->close();
    if (!cmd) {
        atm::E1::insertLogManualSmartCard("close+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualSmartCard("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error when disconnecting the device."));
        return;
    }

    atm::E1::insertLogManualSmartCard("close+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->stop();
    ui->lcdNumber->display(this->m_counttime);
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    ui->tabWidget->setEnabled(false);
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonReadBlock->setEnabled(false);
    ui->pushButtonWriteBlock->setEnabled(false);
    ui->comboBoxLoadKey->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualSmartCard("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void SmartCardReaderFit::printSerialCardNumber() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->open();
    if (!cmd) {
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                             QObject::tr("Error connecting, please insert the card in the reader."));
        return;
    }

    cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->cardStatus();
    if (!cmd) {
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error card status, please insert the card in the reader."));
    }

    cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->getCardMifareSerialNumber();
    if (!cmd) {
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error read serial, please insert the card in the reader."));
    }

    utils::Tools::getInstance()->startElapsedTimer();
    const QString serial = QString::fromStdString(devices::cardreader::AbstractCrt603Cz1::getInstance()->printCardMifareSerialNumber());

    if (!serial.isEmpty()) {
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualSmartCard("readSerialCardNumber+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        // Set serial number card.
        ui->lineEditPrintSerialCardNumber->setText(serial);
    }
}


void SmartCardReaderFit::changeSlot1() {
    //bool cmd = devices::cardreader::AbstractCrt603Cz1Sam::getInstance()->connectSamSlot(1);

    // Remove file.
    QFile::remove("samS.log");

    QProcess process;
    process.start("CrtSAM.exe --sam1");
    process.waitForFinished();
    process.close();

    if (QFileInfo::exists("samS.log")) {
        ui->lineEditCommandResult->setText("SLOT1: 6300");
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error executing the command, make sure the SAM card is connected in SLOT-1."));

        return;
    }

    ui->lineEditCommandResult->setText("SLOT1: 9090");
    QMessageBox::information(this, QCoreApplication::applicationName(),
                             QObject::tr("Please check the reader CRT-603 if the SLOT-1 this with the RED LED on."));

}

void SmartCardReaderFit::changeSlot2() {
    //bool cmd = devices::cardreader::AbstractCrt603Cz1Sam::getInstance()->connectSamSlot(2);

    // Remove file.
    QFile::remove("samS.log");

    QProcess process;
    process.start("CrtSAM.exe --sam2");
    process.waitForFinished();
    process.close();

    if (QFileInfo::exists("samS.log")) {
        ui->lineEditCommandResult->setText("SLOT2: 6300");
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error executing the command, make sure the SAM card is connected in SLOT-2."));

        return;
    }

    ui->lineEditCommandResult->setText("SLOT2: 9090");
    QMessageBox::information(this, QCoreApplication::applicationName(),
                             QObject::tr("Please check the reader CRT-603 if the SLOT-2 this with the RED LED on."));
}

void SmartCardReaderFit::readBlock() {
    const QString keyCombo = ui->comboBoxLoadKey->currentText();

    if (keyCombo.isEmpty()) {
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

    bool cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->loadKey(this->m_loadKey);
    if (!cmd) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error load key, please try other key."));
        return;
    }

    cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->mifareReadDataBlock(0x01, this->m_loadKey);
    if (!cmd) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error reading block!"));
        return;
    }

    const QString readBlock = QString::fromStdString(devices::cardreader::AbstractCrt603Cz1::getInstance()->printCardMifareReadDataBlock());
    if (!readBlock.isEmpty()) {
        ui->lineEditPrintDataCard->setText(readBlock);
    }
}

void SmartCardReaderFit::writeBlock() {
    bool cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->loadKey(this->m_loadKey);
    if (!cmd) {
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

    cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->mifareWriteDataBlock(0x01, this->m_writeData);
    if (!cmd) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error writing block!"));
        return;
    }

    this->readBlock();
}
