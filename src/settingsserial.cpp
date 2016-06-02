/**
 * @file settingsserial.cpp
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

#include <settingsserial.hpp>
#include "ui_settingsserial.h"

#include <atmmodelselect.hpp>
#include <serialloopbackfit.hpp>
#include <utils/log.hpp>

#include <QtSerialPort/QSerialPortInfo>
#include <QIntValidator>
#include <QLineEdit>

SettingsSerial::SettingsSerial(QWidget *parent) :
    QDialog(parent), ui(new Ui::SettingsSerial) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load UI.
    ui->setupUi(this);

    // Validator.
    this->m_intValidator = new QIntValidator(0, 4000000, this);
    ui->comboBoxBaudRate->setInsertPolicy(QComboBox::NoInsert);

    QObject::connect(ui->pushButtonApply, SIGNAL(clicked()),
                     this, SLOT(apply()));
    QObject::connect(ui->comboBoxSerial, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(showPortInfo(int)));
    QObject::connect(ui->comboBoxBaudRate, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(checkCustomBaudRatePolicy(int)));
    QObject::connect(ui->pushButtonLoopback, SIGNAL(clicked()),
                     this, SLOT(handleLoopbackSerial()));

    // Set serial informations.
    this->fillPortsParameters();
    this->fillPortsInfo();
}

SettingsSerial::~SettingsSerial() {
    delete ui;
}

void SettingsSerial::closeEvent(QCloseEvent *event) {
    this->m_deviceBillValidator = false;
    this->m_deviceCoinValidator = false;
    this->m_deviceDispenser = false;
    this->m_deviceMultiIo = false;
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void SettingsSerial::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void SettingsSerial::showPortInfo(int index) {
    if (index != -1) {
        QStringList list = ui->comboBoxSerial->itemData(index).toStringList();
        ui->descriptionLabel->setText(QObject::tr("Description: %1").arg(list.at(1)));
        ui->manufacturerLabel->setText(QObject::tr("Manufacturer: %1").arg(list.at(2)));
        ui->serialNumberLabel->setText(QObject::tr("Serial number: %1").arg(list.at(3)));
        ui->locationLabel->setText(QObject::tr("Location: %1").arg(list.at(4)));
        ui->vidLabel->setText(QObject::tr("Vendor Identifier: %1").arg(list.at(5)));
        ui->pidLabel->setText(QObject::tr("Product Identifier: %1").arg(list.at(6)));
    }
}

void SettingsSerial::checkCustomBaudRatePolicy(int index) {
    bool isCustomBaudRate = !ui->comboBoxBaudRate->itemData(index).isValid();
    ui->comboBoxBaudRate->setEditable(isCustomBaudRate);
    if (isCustomBaudRate) {
        ui->comboBoxBaudRate->clearEditText();
        QLineEdit *edit = ui->comboBoxBaudRate->lineEdit();
        edit->setValidator(this->m_intValidator);
    }
}

void SettingsSerial::fillPortsParameters() {
    ui->comboBoxBaudRate->addItem(QStringLiteral("9600"), QSerialPort::Baud9600);
    ui->comboBoxBaudRate->addItem(QStringLiteral("19200"), QSerialPort::Baud19200);
    ui->comboBoxBaudRate->addItem(QStringLiteral("38400"), QSerialPort::Baud38400);
    ui->comboBoxBaudRate->addItem(QStringLiteral("115200"), QSerialPort::Baud115200);
    ui->comboBoxBaudRate->addItem(QStringLiteral("Custom"));

    ui->comboBoxDataBits->addItem(QStringLiteral("5"), QSerialPort::Data5);
    ui->comboBoxDataBits->addItem(QStringLiteral("6"), QSerialPort::Data6);
    ui->comboBoxDataBits->addItem(QStringLiteral("7"), QSerialPort::Data7);
    ui->comboBoxDataBits->addItem(QStringLiteral("8"), QSerialPort::Data8);
    ui->comboBoxDataBits->setCurrentIndex(3);

    ui->comboBoxParity->addItem(QStringLiteral("None"), QSerialPort::NoParity);
    ui->comboBoxParity->addItem(QStringLiteral("Even"), QSerialPort::EvenParity);
    ui->comboBoxParity->addItem(QStringLiteral("Odd"), QSerialPort::OddParity);
    ui->comboBoxParity->addItem(QStringLiteral("Mark"), QSerialPort::MarkParity);
    ui->comboBoxParity->addItem(QStringLiteral("Space"), QSerialPort::SpaceParity);

    ui->comboBoxBits->addItem(QStringLiteral("1"), QSerialPort::OneStop);
#ifdef Q_OS_WIN
    ui->comboBoxBits->addItem(QStringLiteral("1.5"), QSerialPort::OneAndHalfStop);
#endif
    ui->comboBoxBits->addItem(QStringLiteral("2"), QSerialPort::TwoStop);

    ui->comboBoxFlowControl->addItem(QStringLiteral("None"), QSerialPort::NoFlowControl);
    ui->comboBoxFlowControl->addItem(QStringLiteral("RTS/CTS"), QSerialPort::HardwareControl);
    ui->comboBoxFlowControl->addItem(QStringLiteral("XON/XOFF"), QSerialPort::SoftwareControl);
}

void SettingsSerial::setDevice(const QString device) {
    this->m_deviceDispenser = false;
    this->m_deviceBillValidator = false;
    this->m_deviceCoinValidator = false;
    this->m_deviceMultiIo = false;
    this->m_deviceSerialLoopBack = false;

    QString port = QString();
    unsigned int baudrate = 0;
    unsigned int databits = 0;
    unsigned int parity = 0;
    unsigned int bits = 0;
    unsigned int flow = 0;

    if (device == "dispenser") {
        this->m_deviceDispenser = true;
        port = AtmModelSelect::printPortDispenser();
        baudrate = AtmModelSelect::printBaudRateDispenser();
        databits = AtmModelSelect::printDataBitDispenser();
        parity = AtmModelSelect::printParityDispenser();
        bits = AtmModelSelect::printStopBitDispenser();
        flow = AtmModelSelect::printFlowControlDispenser();
    } else if (device == "billvalidator") {
        this->m_deviceBillValidator = true;
        port = AtmModelSelect::printPortBillValidator();
        baudrate = AtmModelSelect::printBaudRateBillValidator();
        databits = AtmModelSelect::printDataBitBillValidator();
        parity = AtmModelSelect::printParityBillValidator();
        bits = AtmModelSelect::printStopBitBillValidator();
        flow = AtmModelSelect::printFlowControlBillValidator();
    } else if (device == "coinvalidator") {
        this->m_deviceCoinValidator = true;
        port = AtmModelSelect::printPortCoinValidator();
        baudrate = AtmModelSelect::printBaudRateCoinValidator();
        databits = AtmModelSelect::printDataBitCoinValidator();
        parity = AtmModelSelect::printParityCoinValidator();
        bits = AtmModelSelect::printStopBitCoinValidator();
        flow = AtmModelSelect::printFlowControlCoinValidator();
    } else if (device == "multiio") {
        this->m_deviceMultiIo = true;
        port = AtmModelSelect::printPortMultiIO();
        baudrate = AtmModelSelect::printBaudRateMultiIO();
        databits = AtmModelSelect::printDataBitMultiIO();
        parity = AtmModelSelect::printParityMultiIO();
        bits = AtmModelSelect::printStopBitMultiIO();
        flow = AtmModelSelect::printFlowControlMultiIO();
    } else if (device == "serialloopback" ) {
        this->m_deviceSerialLoopBack = true;
        ui->pushButtonLoopback->hide();
        ui->comboBoxSerial->setDisabled(true);
        port = AtmModelSelect::printPortSerialLoopBack();
        baudrate = AtmModelSelect::printBaudRateSerialLoopBack();
        databits = AtmModelSelect::printDataBitSerialLoopBack();
        parity = AtmModelSelect::printParitySerialLoopBack();
        bits = AtmModelSelect::printStopBitSerialLoopBack();
        flow = AtmModelSelect::printFlowControlSerialLoopBack();
    }

    ui->comboBoxSerial->setCurrentIndex(ui->comboBoxSerial->findData(port, Qt::DisplayRole));
    ui->comboBoxBaudRate->setCurrentIndex(ui->comboBoxBaudRate->findData(QString::number(baudrate), Qt::DisplayRole));
    ui->comboBoxDataBits->setCurrentIndex(ui->comboBoxDataBits->findData(QString::number(databits), Qt::DisplayRole));

    if (parity == 0) {
        ui->comboBoxParity->setCurrentIndex(0); // None Parity.
    } else if(parity == 2) {
        ui->comboBoxParity->setCurrentIndex(1); // Even Parity.
    } else if(parity == 3) {
        ui->comboBoxParity->setCurrentIndex(2); // Odd Parity.
    } else if(parity == 4) {
        ui->comboBoxParity->setCurrentIndex(4); // Space Parity.
    } else if(parity == 5) {
        ui->comboBoxParity->setCurrentIndex(3); // Mark Parity.
    }

    if (bits == 0) {
        ui->comboBoxBits->setCurrentIndex(0); // One Stop.
    } else if(bits == 2) {
        ui->comboBoxBits->setCurrentIndex(2); // Two Stop.
    } else if(bits == 3) {
        ui->comboBoxBits->setCurrentIndex(1); // One Half.
    }

    if (flow == 0) {
        ui->comboBoxFlowControl->setCurrentIndex(0); // None.
    } else if(flow == 1) {
        ui->comboBoxFlowControl->setCurrentIndex(1); // RTS/CTS.
    } else if(flow == 2) {
        ui->comboBoxFlowControl->setCurrentIndex(2); // XON/XOFF.
    }
}

void SettingsSerial::apply() {
    QString port = ui->comboBoxSerial->currentText();
    unsigned int baudrate = ui->comboBoxBaudRate->currentText().toInt();
    unsigned int databits = ui->comboBoxDataBits->currentText().toInt();
    unsigned int parity = 0;
    unsigned int bits = 0;
    unsigned int flow = 0;

    if (ui->comboBoxParity->currentIndex() == 0) {
        parity = 0; // None Parity.
    } else if (ui->comboBoxParity->currentIndex() == 1) {
        parity = 2; // Even Parity.
    } else if (ui->comboBoxParity->currentIndex() == 2) {
        parity = 3; // Odd Parity.
    } else if (ui->comboBoxParity->currentIndex() == 4) {
        parity = 4; // Space Parity.
    } else if (ui->comboBoxParity->currentIndex() == 3) {
        parity = 5; // Mark Parity.
    }

    if (ui->comboBoxBits->currentIndex() == 0) {
        bits = 0; // One Stop.
    } else if (ui->comboBoxBits->currentIndex() == 2) {
        bits = 2; // // Two Stop.
    } else if (ui->comboBoxBits->currentIndex() == 1) {
        bits = 3; // One Half.
    }

    if (ui->comboBoxFlowControl->currentIndex() == 0) {
        flow = 0; // None.
    } else if (ui->comboBoxFlowControl->currentIndex() == 1) {
        flow = 1; // RTS/CTS.
    } else if (ui->comboBoxFlowControl->currentIndex() == 2) {
        flow = 2; // XON/XOFF.
    }

    if (this->m_deviceDispenser) {
        AtmModelSelect::setPortDispenser(port);
        AtmModelSelect::setBaudRateDispenser(baudrate);
        AtmModelSelect::setDataBitDispenser(databits);
        AtmModelSelect::setParityDispenser(parity);
        AtmModelSelect::setStopBitsDispenser(bits);
        AtmModelSelect::setFlowControlDispenser(flow);
    } else if (this->m_deviceBillValidator) {
        AtmModelSelect::setPortBillValidator(port);
        AtmModelSelect::setBaudRateBillValidator(baudrate);
        AtmModelSelect::setDataBitBillValidator(databits);
        AtmModelSelect::setParityBillValidator(parity);
        AtmModelSelect::setStopBitsBillValidator(bits);
        AtmModelSelect::setFlowControlBillValidator(flow);
    } else if (this->m_deviceCoinValidator) {
        AtmModelSelect::setPortCoinValidator(port);
        AtmModelSelect::setBaudRateCoinValidator(baudrate);
        AtmModelSelect::setDataBitCoinValidator(databits);
        AtmModelSelect::setParityCoinValidator(parity);
        AtmModelSelect::setStopBitsCoinValidator(bits);
        AtmModelSelect::setFlowControlCoinValidator(flow);
    } else if (this->m_deviceMultiIo) {
        AtmModelSelect::setPortMultiIO(port);
        AtmModelSelect::setBaudRateMultiIO(baudrate);
        AtmModelSelect::setDataBitMultiIO(databits);
        AtmModelSelect::setParityMultiIO(parity);
        AtmModelSelect::setStopBitsMultiIO(bits);
        AtmModelSelect::setFlowControlMultiIO(flow);
    } else if (this->m_serialLoopBackFit) {
        AtmModelSelect::setPortSerialLoopBack(port);
        AtmModelSelect::setBaudRateSerialLoopBack(baudrate);
        AtmModelSelect::setDataBitSerialLoopBack(databits);
        AtmModelSelect::setParitySerialLoopBack(parity);
        AtmModelSelect::setStopBitsSerialLoopBack(bits);
        AtmModelSelect::setFlowControlSerialLoopBack(flow);
    }

    this->close();
}

void SettingsSerial::fillPortsInfo() {
    ui->comboBoxSerial->clear();
    static const QString blankString = QObject::tr("N/A");
    QString description = QString();
    QString manufacturer = QString();
    QString serialNumber = QString();

    QIcon icon;
    icon.addFile(QStringLiteral(":/res/images/serialport.png"), QSize(), QIcon::Normal, QIcon::Off);

    // Get all serial data
    QList<QSerialPortInfo> serialList = utils::SerialPort::printAvaliablePorts();
    foreach (const QSerialPortInfo &info, serialList) {
        QStringList list;
        description = info.description();
        manufacturer = info.manufacturer();
        serialNumber = info.serialNumber();
        list << info.portName()
             << (!description.isEmpty() ? description : blankString)
             << (!manufacturer.isEmpty() ? manufacturer : blankString)
             << (!serialNumber.isEmpty() ? serialNumber : blankString)
             << info.systemLocation()
             << (info.vendorIdentifier() ? QString::number(info.vendorIdentifier(), 16) : blankString)
             << (info.productIdentifier() ? QString::number(info.productIdentifier(), 16) : blankString);
        ui->comboBoxSerial->addItem(icon, list.first(), list);
    }

    // Set icons size.
    ui->comboBoxSerial->setIconSize(QSize(32, 32));
}

void SettingsSerial::handleLoopbackSerial() {
    m_serialLoopBackFit = new SerialLoopBackFit(0, true);

    QObject::connect(this->m_serialLoopBackFit, SIGNAL(errorConnection()),
                     this, SLOT(closeSerialLoopBack()));

    QString port = ui->comboBoxSerial->currentText();
    unsigned int baudrate = ui->comboBoxBaudRate->currentText().toInt();
    unsigned int databits = ui->comboBoxDataBits->currentText().toInt();
    unsigned int parity = 0;
    unsigned int stopbits = 0;
    unsigned int flowcontrol = 0;

    if (ui->comboBoxParity->currentIndex() == 0) {
        parity = 0; // None Parity.
    } else if (ui->comboBoxParity->currentIndex() == 1) {
        parity = 2; // Even Parity.
    } else if (ui->comboBoxParity->currentIndex() == 2) {
        parity = 3; // Odd Parity.
    } else if (ui->comboBoxParity->currentIndex() == 4) {
        parity = 4; // Space Parity.
    } else if (ui->comboBoxParity->currentIndex() == 3) {
        parity = 5; // Mark Parity.
    }

    if (ui->comboBoxBits->currentIndex() == 0) {
        stopbits = 0; // One Stop.
    } else if (ui->comboBoxBits->currentIndex() == 2) {
        stopbits = 2; // // Two Stop.
    } else if (ui->comboBoxBits->currentIndex() == 1) {
        stopbits = 3; // One Half.
    }

    if (ui->comboBoxFlowControl->currentIndex() == 0) {
        flowcontrol = 0; // None.
    } else if (ui->comboBoxFlowControl->currentIndex() == 1) {
        flowcontrol = 1; // RTS/CTS.
    } else if (ui->comboBoxFlowControl->currentIndex() == 2) {
        flowcontrol = 2; // XON/XOFF.
    }

    this->m_serialLoopBackFit->setSerialPort(ui->comboBoxSerial->currentText(), baudrate,
                     databits, parity,
                     stopbits, flowcontrol);
    this->m_serialLoopBackFit->show();
    this->m_serialLoopBackFit->serialConnect();
}

void SettingsSerial::closeSerialLoopBack() {
    this->m_serialLoopBackFit->close();
}
