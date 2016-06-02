/**
 * @file multiio.cpp
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

#include <multiiofit.hpp>
#include "ui_multiiofit.h"

#include <serialcommandsfit.hpp>
#include <atmmodelselect.hpp>
#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QMessageBox>
#include <QThread>

MultiIoFit::MultiIoFit(QWidget *parent) :
    QWidget(parent), ui(new Ui::MultiIoFit), m_counttime(0),
    m_powerPrinter(false), m_powerCoinValidator(false), m_powerDispenser(false),
    m_powerBillValidator(false), m_sensorInternalDoor(false), m_sensorExternalDoor(false),
    m_sensorInternalVibration(false) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Set labels.
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("");
    ui->labelVersion->setText("");
    const QString indoor = "<font color='orange' size='3'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                           "&nbsp;&nbsp;&nbsp;<b>" + QObject::tr("Internal Door") + "</b></font>";
    ui->labelINDoor->setText(indoor);
    ui->labelINDoor->repaint();

    const QString exdoor = "<font color='orange' size='3'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                           "&nbsp;&nbsp;&nbsp;<b>" + QObject::tr("External Door") + "</b></font>";
    ui->labelEXDoor->setText(exdoor);
    ui->labelEXDoor->repaint();

    const QString invibration = "<font color='orange' size='3'>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
                                "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<b>" + QObject::tr("Vibration") + "</b></font>";
    ui->labelINVibration->setText(invibration);
    ui->labelINVibration->repaint();

    // Set combo serial.
    if (!AtmModelSelect::printPortMultiIO().isEmpty()) {
        ui->comboBoxSerial->insertItem(0, AtmModelSelect::printPortMultiIO());
    }

    // Timer clock count UP.
    this->m_timer = new QTimer(this);
    QObject::connect(this->m_timer, SIGNAL(timeout()), this, SLOT(countUp()));

    // Timer check sensors command timer.
    this->m_timerSensors = new QTimer(this);
    QObject::connect(this->m_timerSensors, SIGNAL(timeout()),
                     this, SLOT(monitorSensors()));

    // Set icons.
    this->m_iconOn.addFile(QStringLiteral(":/res/images/switch-on-128x128.png"), QSize(),
                           QIcon::Normal, QIcon::Off);
    this->m_iconOff.addFile(QStringLiteral(":/res/images/switch-off-128x128.png"), QSize(),
                            QIcon::Normal, QIcon::Off);

    // Slots and signals.
    QObject::connect(ui->pushButtonConnect, SIGNAL (clicked()),
                     this, SLOT(deviceConnect()));
    QObject::connect(ui->pushButtonDisconnect, SIGNAL (clicked()),
                     this, SLOT(deviceDisconnect()));
    QObject::connect(ui->pushButtonPrint, SIGNAL (clicked()),
                     this, SLOT(powerPrinter()));
    QObject::connect(ui->pushButtonCoinValidator, SIGNAL (clicked()),
                     this, SLOT(powerCoinValidator()));
    QObject::connect(ui->pushButtonDispenser, SIGNAL (clicked()),
                     this, SLOT(powerDispenser()));
    QObject::connect(ui->pushButtonBillValidator, SIGNAL (clicked()),
                     this, SLOT(powerBillValidator()));
    QObject::connect(ui->pushButtonStartSensors, SIGNAL (clicked()),
                     this, SLOT(startMonitorSensors()));
    QObject::connect(ui->pushButtonStopSensors, SIGNAL (clicked()),
                     this, SLOT(stopMonitorSensors()));
    QObject::connect(ui->comboBoxChangeSensor, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(handleSelectionSensorsChanged(int)));
}

MultiIoFit::~MultiIoFit() {
    delete ui;
}

void MultiIoFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void MultiIoFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void MultiIoFit::countUp() {
    m_counttime++;
    ui->lcdNumber->display(this->m_counttime);
}

void MultiIoFit::deviceConnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::multiio::AbstractLed10v2::getInstance()->open(AtmModelSelect::printPortMultiIO().toStdString(),
                                                                      AtmModelSelect::printBaudRateMultiIO(),
                                                                      AtmModelSelect::printDataBitMultiIO(),
                                                                      AtmModelSelect::printParityMultiIO(),
                                                                      AtmModelSelect::printStopBitMultiIO(),
                                                                      AtmModelSelect::printFlowControlMultiIO());
    if (!cmd) {
        atm::E1::insertLogManualMultiIo("open+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Error connecting the device! Connection serial port: ") +
                              AtmModelSelect::printPortMultiIO());
        return;
    }

    atm::E1::insertLogManualMultiIo("open+MANUAL=PASSED");

    m_counttime = 0;
    m_timer->start(1000);
    ui->lcdNumber->display(this->m_counttime);
    const QString hmsg = "<font color='green' size='2'>" + QObject::tr("YES") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelFirmware->setText("LED10V2");
    ui->labelVersion->setText("1.0");
    ui->tabWidget->setEnabled(true);
    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->pushButtonStartSensors->setEnabled(false);
    ui->pushButtonStopSensors->setEnabled(false);
    ui->labelImgExternalDoor->setEnabled(false);
    ui->labelImgInternalDoor->setEnabled(false);
    ui->labelImgInternalVibration->setEnabled(false);

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualMultiIo("open+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void MultiIoFit::deviceDisconnect() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    bool cmd = devices::multiio::AbstractLed10v2::getInstance()->close();
    if (!cmd) {
        atm::E1::insertLogManualMultiIo("close+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Error when disconnecting the device."));
        return;
    }

    atm::E1::insertLogManualMultiIo("close+MANUAL=PASSED");

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
    ui->pushButtonStartSensors->setEnabled(false);
    ui->pushButtonStopSensors->setEnabled(false);    
    ui->labelImgExternalDoor->setEnabled(false);
    ui->labelImgInternalDoor->setEnabled(false);
    ui->labelImgInternalVibration->setEnabled(false);

    this->m_sensorInternalDoor = false;
    this->m_sensorExternalDoor = false;
    this->m_sensorInternalVibration = false;

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualMultiIo("close+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}

void MultiIoFit::powerPrinter() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    if (this->m_powerPrinter) {
        this->m_powerPrinter = false;
        devices::multiio::AbstractLed10v2::getInstance()->powerOn(1);
        ui->pushButtonPrint->setIcon(this->m_iconOn);
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+PRINTER=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+PRINTER+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    } else {
        this->m_powerPrinter = true;
        devices::multiio::AbstractLed10v2::getInstance()->powerOff(1);
        ui->pushButtonPrint->setIcon(this->m_iconOff);
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+PRINTER=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+PRINTER+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    }
}

void MultiIoFit::powerCoinValidator() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    if (this->m_powerCoinValidator) {
        this->m_powerCoinValidator = false;
        devices::multiio::AbstractLed10v2::getInstance()->powerOn(3);
        ui->pushButtonCoinValidator->setIcon(this->m_iconOn);    
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+COINVALIDATOR=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+COINVALIDATOR+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    } else {
        this->m_powerCoinValidator = true;
        devices::multiio::AbstractLed10v2::getInstance()->powerOff(3);
        ui->pushButtonCoinValidator->setIcon(this->m_iconOff);
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+COINVALIDATOR=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+COINVALIDATOR+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    }
}

void MultiIoFit::powerDispenser() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    if (this->m_powerDispenser) {
        this->m_powerDispenser = false;
        devices::multiio::AbstractLed10v2::getInstance()->powerOn(7);
        ui->pushButtonDispenser->setIcon(this->m_iconOn);
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+DISPENSER=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+DISPENSER+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    } else {
        this->m_powerDispenser = true;
        devices::multiio::AbstractLed10v2::getInstance()->powerOff(7);
        ui->pushButtonDispenser->setIcon(this->m_iconOff);
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+DISPENSER=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+DISPENSER+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    }
}

void MultiIoFit::powerBillValidator() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    if (this->m_powerBillValidator) {
        this->m_powerBillValidator = false;
        devices::multiio::AbstractLed10v2::getInstance()->powerOn(7);
        ui->pushButtonBillValidator->setIcon(this->m_iconOn);
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+BILLVALIDATOR=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOn+MANUAL+BILLVALIDATOR+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    } else {
        this->m_powerBillValidator = true;
        devices::multiio::AbstractLed10v2::getInstance()->powerOff(7);
        ui->pushButtonBillValidator->setIcon(this->m_iconOff);
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+BILLVALIDATOR=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualMultiIo("powerOff+MANUAL+BILLVALIDATOR+ELAPSEDTIMER=" + QString::number(elapsedtimer));
    }
}

void MultiIoFit::handleSelectionSensorsChanged(int index) {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    this->m_sensorExternalDoor = false;
    this->m_sensorInternalDoor = false;
    this->m_sensorInternalVibration = false;

    ui->labelImgExternalDoor->setEnabled(false);
    ui->labelImgInternalDoor->setEnabled(false);
    ui->labelImgInternalVibration->setEnabled(false);

    if (index == 0) {
        ui->pushButtonStartSensors->setEnabled(false);
        ui->pushButtonStopSensors->setEnabled(false);
        return;
    } else if (index == 1) {
        ui->pushButtonStartSensors->setEnabled(true);
        ui->labelImgInternalDoor->setEnabled(true);
        this->m_sensorInternalDoor = true;
    } else if (index == 2) {
        ui->pushButtonStartSensors->setEnabled(true);
        ui->labelImgExternalDoor->setEnabled(true);
        this->m_sensorExternalDoor = true;
    } else if (index == 3) {
        ui->pushButtonStartSensors->setEnabled(true);
        ui->labelImgInternalVibration->setEnabled(true);
        this->m_sensorInternalVibration = true;
    } else {
        ui->pushButtonStartSensors->setEnabled(false);
        ui->pushButtonStopSensors->setEnabled(false);
        ui->labelImgExternalDoor->setEnabled(false);
        ui->labelImgInternalDoor->setEnabled(false);
        ui->labelImgInternalVibration->setEnabled(false);
    }

    QApplication::restoreOverrideCursor();
}

void MultiIoFit::startMonitorSensors() {
    if (this->m_sensorInternalDoor) {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("You triggered the internal door sensor! To test press the internal door "
                                             "button, and make sure the port icon changes color."));
    }

    if (this->m_sensorExternalDoor) {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("You triggered the external door sensor! To test press the external door "
                                             "button, and make sure the port icon changes color."));
    }

    if (this->m_sensorInternalVibration) {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("You triggered the vibration sensor! To test tap the left side of the "
                                             "ATM and check that the vibration button changes color."));
    }

    ui->comboBoxChangeSensor->setEnabled(false);
    ui->pushButtonStartSensors->setEnabled(false);
    ui->pushButtonStopSensors->setEnabled(true);
    this->m_timerSensors->start(500);
}

void MultiIoFit::stopMonitorSensors() {
    ui->comboBoxChangeSensor->setEnabled(true);
    ui->pushButtonStartSensors->setEnabled(false);
    ui->pushButtonStopSensors->setEnabled(false);

    ui->labelImgExternalDoor->setEnabled(false);
    ui->labelImgInternalDoor->setEnabled(false);
    ui->labelImgInternalVibration->setEnabled(false);

    this->m_timerSensors->stop();
}

void MultiIoFit::monitorSensors() {
    bool status = false;
    bool checkSensor = false;

    if (this->m_sensorInternalDoor) {
        // Check status sensors: Inferior door.
        status = devices::multiio::AbstractLed10v2::getInstance()->sensorStatus(12);

        if (status) {
            checkSensor = devices::multiio::AbstractLed10v2::getInstance()->checkSensorStatus();
            if (checkSensor) {
                ui->labelImgInternalDoor->setPixmap(QPixmap(QString::fromUtf8(":/res/images/green-led-sensor.png")));
                ui->labelImgInternalDoor->repaint();
            } else {
                ui->labelImgInternalDoor->setPixmap(QPixmap(QString::fromUtf8(":/res/images/red-led-sensor.png")));
                ui->labelImgInternalDoor->repaint();
            }
        } else {
            QLOG_ERROR() << "Error get CPU status sensors: Internal door.";
        }
    } else {
        this->m_sensorInternalDoor = false;
    }

    status = false;
    checkSensor = false;

    if (this->m_sensorExternalDoor) {
        // Check status sensors: External door.
        status = devices::multiio::AbstractLed10v2::getInstance()->sensorStatus(13);

        if (status) {
            checkSensor = devices::multiio::AbstractLed10v2::getInstance()->checkSensorStatus();
            if (checkSensor) {
                ui->labelImgExternalDoor->setPixmap(QPixmap(QString::fromUtf8(":/res/images/green-led-sensor.png")));
                ui->labelImgExternalDoor->repaint();
            } else {
                ui->labelImgExternalDoor->setPixmap(QPixmap(QString::fromUtf8(":/res/images/red-led-sensor.png")));
                ui->labelImgExternalDoor->repaint();
            }
        } else {
            QLOG_ERROR() << "Error get CPU status sensors: External door.";
        }
    } else {
        this->m_sensorExternalDoor = false;
    }

    status = false;
    checkSensor = false;

    if (this->m_sensorInternalVibration) {
        // Check status sensors: Internal vibration.
        status = devices::multiio::AbstractLed10v2::getInstance()->sensorStatus(14);

        if (status) {
            checkSensor = devices::multiio::AbstractLed10v2::getInstance()->checkSensorStatus();
            if (checkSensor) {
                ui->labelImgInternalVibration->setPixmap(QPixmap(QString::fromUtf8(":/res/images/green-led-sensor.png")));
                ui->labelImgInternalVibration->repaint();
            } else {
                ui->labelImgInternalVibration->setPixmap(QPixmap(QString::fromUtf8(":/res/images/red-led-sensor.png")));
                ui->labelImgInternalVibration->repaint();
            }
        } else {
            QLOG_ERROR() << "Error get CPU status sensors: Vibration.";
        }
    } else {
        this->m_sensorInternalVibration = false;
    }
}
