/**
 * @file searchdevices.cpp
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

#include <searchdevices.hpp>
#include "ui_searchdevices.h"

#include <utils/log.hpp>

#include <QMessageBox>
#include <QScrollBar>
#include <QThread>

SearchDevices::SearchDevices(QWidget *parent) :
    QWidget(parent), ui(new Ui::SearchDevices) {
	
    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Set option widget.
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    // Load ui.
    ui->setupUi(this);

    // Create instance serial port.
    this->m_serial = new utils::SerialPort;

    // Widget gauge.
    this->m_speedGauge = new QcGaugeWidget;
    this->m_speedGauge->addBackground(99);
    QcBackgroundItem *bkg1 = this->m_speedGauge->addBackground(92);
    bkg1->clearrColors();
    bkg1->addColor(0.1, Qt::black);
    bkg1->addColor(1.0, Qt::white);

    QcBackgroundItem *bkg2 = m_speedGauge->addBackground(88);
    bkg2->clearrColors();
    bkg2->addColor(0.1, Qt::gray);
    bkg2->addColor(1.0, Qt::darkGray);

    this->m_speedGauge->addArc(55);
    this->m_speedGauge->addDegrees(65)->setValueRange(0,100);
    this->m_speedGauge->addColorBand(50);
    this->m_speedGauge->addValues(80)->setValueRange(0,100);

    this->m_speedGauge->addLabel(70)->setText(" ");
    QcLabelItem *lab = this->m_speedGauge->addLabel(40);
    lab->setText("0");

    this->m_speedNeedle = this->m_speedGauge->addNeedle(60);
    this->m_speedNeedle->setLabel(lab);
    this->m_speedNeedle->setColor(Qt::white);
    this->m_speedNeedle->setValueRange(0,100);

    this->m_speedGauge->addBackground(7);
    this->m_speedGauge->addGlass(88);
    ui->verticalLayout->addWidget(this->m_speedGauge);

    // Set text edit.
    ui->plainTextEdit->document()->setMaximumBlockCount(100);
    QFont font = QFont("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(12);
    font.setFixedPitch(true);
    font.setBold(true);
    ui->plainTextEdit->setFont(font);
    ui->plainTextEdit->setReadOnly(true);

    // Set palette color.
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::green);
    ui->plainTextEdit->setPalette(p);
    ui->progressBar->setPalette(p);

    // Set default values in progress.
    this->m_speedNeedle->setCurrentValue(0);
    ui->progressBar->setValue(0);
    ui->progressBar->hide();

    // Slots and signals.
    QObject::connect(ui->pushButtonBack, SIGNAL(clicked()),
                     this, SLOT(closeForm()));
    QObject::connect(ui->pushButtonOk, SIGNAL(clicked()),
                     this, SLOT(tryTestDeviceCommunication()));
}

SearchDevices::~SearchDevices() {
    delete ui;
}

void SearchDevices::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void SearchDevices::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void SearchDevices::closeForm() {
    this->close();
}

void SearchDevices::putData(const QString &data) {
    const QString str = data;

    if (str == "START") {
        QString msg = "<b><font color=\"orange\"> " + str + "</font><b><br>";
        ui->plainTextEdit->appendHtml(msg);
        return;
    } else if (str == "END") {
        QString msg = "<br><b><font color=\"orange\"> " + str + "</font><b><br>";
        ui->plainTextEdit->appendHtml(msg);
        return;
    }

    if (str == "DISPENSER" || str == "MULTIIO" || str == "BILLVALIDATOR" ||
        str == "COINVALIDATOR" || str == "PRINTER" || str == "SMARTCARD") {
        QString msg = "Device: <font color=\"yellow\"> " + str + "</font>";
        ui->plainTextEdit->appendHtml(msg);
    } else if (str == "CONNECTED") {
        QString msg = "Connection was successful: [<font color=\"green\">PASSED</font>]";
        ui->plainTextEdit->appendHtml(msg);
    } else if (str == "FAILED") {
        QString msg = "Connection was unsuccessful: [<font color=\"red\">FAILED</font>]";
        ui->plainTextEdit->appendHtml(msg);
    } else {
        ui->plainTextEdit->appendHtml(QString(data));
    }

    QScrollBar *bar = ui->plainTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void SearchDevices::getData(eSearchDevicesGetData cmd) {
    const QByteArray data = this->m_serial->getData();

    if (data.isEmpty()) {
        QLOG_WARN() << "SearchDevices: Get data package is invalid.";
        m_getDataIsOk = false;
        return; // Avoid segmentation fault.
    }

    switch (cmd) {
    case SEARCHDEVICES_GETDATA_BILLVALIDATOR:
        QLOG_INFO() << "Get data bill validator";
        break;
    case SEARCHDEVICES_GETDATA_COINVALIDATOR:
        QLOG_INFO() << "Get data coin validator";
        break;
    case SEARCHDEVICES_GETDATA_DISPENSER:
        QLOG_INFO() << "Get data dispenser";
        break;
    case SEARCHDEVICES_GETDATA_MULTIIO:
        QLOG_INFO() << "Get data multi/io";
        break;
    default:
        break;
    }
}

bool SearchDevices::getDataIsOk() {
    return m_getDataIsOk;
}

bool SearchDevices::tryCommunicationCoinValidator(QString comport, unsigned int baudrate, unsigned int databit,
                                                  unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol) {

    bool open = this->m_serial->open(comport.toStdString(), baudrate, databit, tparity, stopbit,  flowcontrol);

    // Check open connection serial.
    if (open) {
        // Command simple pool.
        QByteArray cmd;
        cmd.resize(5);
        cmd[0] = 0x02;
        cmd[1] = 0x00;
        cmd[2] = 0x01;
        cmd[3] = 0xFE;
        cmd[4] = 0xFF;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 500);

        // Wait response device.
        QThread::msleep(200);

        // Get response command.
        this->getData(SEARCHDEVICES_GETDATA_COINVALIDATOR);

        if (this->getDataIsOk()) {
            // Communication ok.
            m_serialFound.coinvalidator = comport;
            this->m_serial->close();
            return true;
        } else {
            // Communication failed.
            this->m_serial->close();
            return false;
        }
    }
    this->m_serial->close();

    // Open connection serial failed.
    return false;
}

bool SearchDevices::tryCommunicationBillValidator(QString comport, unsigned int baudrate, unsigned int databit,
                                                  unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol) {

    bool open = this->m_serial->open(comport.toStdString(), baudrate, databit, tparity, stopbit,  flowcontrol);

    // Check open connection serial.
    if (open) {
        // Command reset.
        QByteArray cmd;
        cmd.resize(6);
        cmd[0] = 0x02;
        cmd[1] = 0x03;
        cmd[2] = 0x06;
        cmd[3] = 0x30;
        cmd[4] = 0x41;
        cmd[5] = 0xB3;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 500);

        // Wait response device.
        QThread::msleep(200);

        // Get response command.
        this->getData(SEARCHDEVICES_GETDATA_BILLVALIDATOR);

        if (this->getDataIsOk()) {
            // Communication ok.
            m_serialFound.billvalidator = comport;
            this->m_serial->close();
            return true;
        } else {
            // Communication failed.
            this->m_serial->close();
            return false;
        }
    }
    this->m_serial->close();

    // Open connection serial failed.
    return false;
}

bool SearchDevices::tryCommunicationDispenser(QString comport, unsigned int baudrate, unsigned int databit,
                                              unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol) {

    bool open = this->m_serial->open(comport.toStdString(), baudrate, databit, tparity, stopbit,  flowcontrol);

    // Check open connection serial.
    if (open) {
        // Command init.
        QByteArray cmd;
        cmd.resize(9);
        cmd[0] = 0xF2;
        cmd[1] = 0x00;
        cmd[2] = 0x00;
        cmd[3] = 0x03;
        cmd[4] = 0x43;
        cmd[5] = 0x31;
        cmd[6] = 0x30;
        cmd[7] = 0x03;
        cmd[8] = 0xB0;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 500);

        // Wait response device.
        QThread::msleep(500);

        // Get response command.
        this->getData(SEARCHDEVICES_GETDATA_DISPENSER);

        if (this->getDataIsOk()) {
            // Communication ok.
            m_serialFound.dispenser = comport;
            this->m_serial->close();
            return true;
        } else {
            // Communication failed.
            this->m_serial->close();
            return false;
        }
    }
    this->m_serial->close();

    // Open connection serial failed.
    return false;
}

bool SearchDevices::tryCommunicationMultiIo(QString comport, unsigned int baudrate, unsigned int databit,
                                            unsigned int tparity, unsigned int stopbit, unsigned int flowcontrol) {

    bool open = this->m_serial->open(comport.toStdString(), baudrate, databit, tparity, stopbit,  flowcontrol);

    // Check open connection serial.
    if (open) {
        // Command power on printer.
        QByteArray cmd;
        cmd.resize(11);
        cmd[0]  = 0x4F;
        cmd[1]  = 0x28;
        cmd[2]  = 0x30;
        cmd[3]  = 0x30;
        cmd[4]  = 0x2C;
        cmd[5]  = 0x30;
        cmd[6]  = 0x33;
        cmd[7]  = 0x2C;
        cmd[8]  = 0xFF;
        cmd[9]  = 0x29;
        cmd[10] = 0x45;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 500);

        // Wait response device.
        QThread::msleep(500);

        // Get response command.
        this->getData(SEARCHDEVICES_GETDATA_MULTIIO);

        if (this->getDataIsOk()) {
            // Communication ok.
            m_serialFound.multiIo = comport;
            this->m_serial->close();
            return true;
        } else {
            // Communication failed.
            this->m_serial->close();
            return false;
        }
    }
    this->m_serial->close();

    // Open connection serial failed.
    return false;
}

void SearchDevices::tryTestDeviceCommunication() {
    QStringList portName;
    QList<QSerialPortInfo> serialList = utils::SerialPort::printAvaliablePorts();
    foreach (const QSerialPortInfo &serialInfo, serialList) {
        portName << serialInfo.portName();
    }

    if (portName.isEmpty()) {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Sorry! Serial port not found in your system."));
        return;
    }

    QMessageBox::StandardButton ret;
    ret = QMessageBox::information(this, QCoreApplication::applicationName(),
                                   QObject::tr("Do you really want to scan the serial ports?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        QLOG_INFO() << "Start search devices in serial ports...";
        ui->progressBar->show();
    } else if (ret == QMessageBox::Cancel) {
        return;
    }

    // Set variable default for serial connection.
    QString comport = "";
    unsigned int baudrate = 0;
    unsigned int databit = 8;
    unsigned int tparity = 0;
    unsigned int stopbit = 1;
    unsigned int flowcontrol = 0;
    bool ok = false;

    ui->plainTextEdit->clear();
    this->putData("START");

    //
    // Try connection with device: Coin Validator.
    //    
    this->putData("COINVALIDATOR");
    for (register int i = 1; i < portName.count(); ++i) {
        comport = portName[i];
        baudrate = 9600;
        this->putData("1. Trying to connect the Coin Validator in " + comport + " baudrate " +
                      QString::number(baudrate));
        ok = tryCommunicationCoinValidator(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            break;
        } else {
            this->putData("FAILED");
        }

        // Set progress.
        ui->progressBar->setValue(20);
        this->m_speedNeedle->setCurrentValue(20);

        baudrate = 38400;
        ok = tryCommunicationCoinValidator(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        this->putData("2. Trying to connect the Coin Validator in " + comport + " baudrate " +
                      QString::number(baudrate));
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            this->m_foundDevice = true;
            break;
        } else {
            this->putData("FAILED");
            this->m_foundDevice = false;
        }
    }
    this->putData("-----------------------------------");
    this->putData("");

    //
    // Try connection with device: Bill Validator.
    //
    this->putData("BILLVALIDATOR");
    for (register int i = 1; i < portName.count(); ++i) {
        comport = portName[i];
        baudrate = 9600;
        this->putData("1. Trying to connect the Bill Validator in " + comport + " baudrate " +
                      QString::number(baudrate));
        ok = tryCommunicationBillValidator(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            break;
        } else {
            this->putData("FAILED");
        }

        // Set progress.
        ui->progressBar->setValue(40);
        this->m_speedNeedle->setCurrentValue(40);

        baudrate = 38400;
        this->putData("2. Trying to connect the Bill Validator in " + comport + " baudrate " +
                      QString::number(baudrate));
        ok = tryCommunicationBillValidator(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            break;
        } else {
            this->putData("FAILED");
        }
    }
    this->putData("-----------------------------------");
    this->putData("");

    /*
    //
    // Try connection with device: Dispenser.
    //
    this->putData("DISPENSER");
    for (register int i = 1; i < portName.count(); ++i) {
        comport = portName[i];
        //baudrate = 9600;
        baudrate = 38400;
        this->putData("1. Trying to connect the Dispenser in " + comport + " baudrate " +
                      QString::number(baudrate));
        ok = tryCommunicationDispenser(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            break;
        } else {
            this->putData("FAILED");
        }

        ui->progressBar->setValue(70);
        this->m_speedNeedle->setCurrentValue(70);

        baudrate = 38400;
        this->putData("2. Trying to connect the Dispenser in " + comport + " baudrate " +
                      QString::number(baudrate));
        ok = tryCommunicationDispenser(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            break;
        } else {
            this->putData("FAILED");
        }
    }
    this->putData("-----------------------------------");
    this->putData("");

    //
    // Try connection with device: Multi/IO.
    //
    this->putData("MULTIIO");
    for (register int i = 1; i < portName.count(); ++i) {
        comport = portName[i];
        baudrate = 9600;
        this->putData("1. Trying to connect the MultiIO in " + comport + " baudrate " +
                      QString::number(baudrate));
        ok = tryCommunicationMultiIo(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            break;
        } else {
            this->putData("FAILED");
        }

        // Set progress.
        ui->progressBar->setValue(80);
        this->m_speedNeedle->setCurrentValue(80);

        baudrate = 38400;
        this->putData("2. Trying to connect the MultiIO in " + comport + " baudrate " +
                      QString::number(baudrate));
        ok = tryCommunicationMultiIo(comport, baudrate, databit, tparity, stopbit, flowcontrol);
        if (ok) {
            this->putData("CONNECTED");
            portName.removeAt(i);
            break;
        } else {
            this->putData("FAILED");
        }
    }
    this->putData("-----------------------------------");
    this->putData("END");

    // Set progress.
    ui->progressBar->setValue(10);
    this->m_speedNeedle->setCurrentValue(100);
    ui->progressBar->hide();

    if (!m_serialFound.coinvalidator.isEmpty() || !m_serialFound.billvalidator.isEmpty()
        || !m_serialFound.dispenser.isEmpty() || !m_serialFound.multiIo.isEmpty()) {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("It was found devices! Click OK and try to do automatic testing."));
        emit signalUpdateSerialPorts();
        //this->close();
    } else {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("No device was found!"));
    }*/
}

