/**
 * @file atmmodelselect.cpp
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

#include <atmmodelselect.hpp>
#include "ui_atmmodelselect.h"

#include <settingsserial.hpp>
#include <console.hpp>
#include <searchdevices.hpp>
#include <utils/log.hpp>
#include <atm/e1.hpp>

#include <QPainter>
#include <QMessageBox>
#include <QStringList>
#include <QDesktopWidget>

// Dispenser.
QString AtmModelSelect::m_portDispenser = QString();
unsigned int AtmModelSelect::m_baudRateDispenser;
unsigned int AtmModelSelect::m_dataBitDispenser;
unsigned int AtmModelSelect::m_parityDispenser;
unsigned int AtmModelSelect::m_stopBitDispenser;
unsigned int AtmModelSelect::m_flowControlDispenser;

// BillValidator.
QString AtmModelSelect::m_portBillValidator = QString();
unsigned int AtmModelSelect::m_baudRateBillValidator = 0;
unsigned int AtmModelSelect::m_dataBitBillValidator = 0;
unsigned int AtmModelSelect::m_parityBillValidator = 0;
unsigned int AtmModelSelect::m_stopBitBillValidator = 0;
unsigned int AtmModelSelect::m_flowControlBillValidator = 0;

// CoinValidator.
QString AtmModelSelect::m_portCoinValidator = QString();
unsigned int AtmModelSelect::m_baudRateCoinValidator = 0;
unsigned int AtmModelSelect::m_dataBitCoinValidator = 0;
unsigned int AtmModelSelect::m_parityCoinValidator = 0;
unsigned int AtmModelSelect::m_stopBitCoinValidator = 0;
unsigned int AtmModelSelect::m_flowControlCoinValidator = 0;

// Multi/IO.
QString AtmModelSelect::m_portMultiIO = QString();
unsigned int AtmModelSelect::m_baudRateMultiIO = 0;
unsigned int AtmModelSelect::m_dataBitMultiIO = 0;
unsigned int AtmModelSelect::m_parityMultiIO = 0;
unsigned int AtmModelSelect::m_stopBitMultiIO = 0;
unsigned int AtmModelSelect::m_flowControlMultiIO = 0;

// Serial LoopBack.
QString AtmModelSelect::m_portSerialLoopBack = QString();
unsigned int AtmModelSelect::m_baudRateSerialLoopBack = 0;
unsigned int AtmModelSelect::m_dataBitSerialLoopBack = 0;
unsigned int AtmModelSelect::m_paritySerialLoopBack = 0;
unsigned int AtmModelSelect::m_stopBitSerialLoopBack = 0;
unsigned int AtmModelSelect::m_flowControlSerialLoopBack = 0;

// Others variables.
QString AtmModelSelect::m_portPrinter = QString();
QString AtmModelSelect::m_portUps = QString();
QString AtmModelSelect::m_languageSelected = QString();
QStringList AtmModelSelect::m_portName = QStringList();

AtmModelSelect::AtmModelSelect(QWidget *parent) :
    QWidget(parent), ui(new Ui::AtmModelSelect), m_countSerialPorts(0) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Set option widget.
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    // Load Ui.
    ui->setupUi(this);

    // Create instance translator.
    this->m_translator = new QTranslator(this);

    // Create instance serial settings.
    this->m_settingsserial = new SettingsSerial(this);

    // Hide objects.
    this->hideAllTabs();

    // Slots and signals.
    QObject::connect(ui->comboBoxChangeLanguage, SIGNAL(currentIndexChanged(int)), 
                     this, SLOT(handleSelectionLangChanged(int)));
    QObject::connect(ui->comboBoxChangeModelATM, SIGNAL(currentIndexChanged(int)), 
                     this,SLOT(handleSelectionModelATMChanged(int)));
    QObject::connect(ui->pushButtonStart, SIGNAL (clicked()),
                     this, SLOT(handleConsole()));
    QObject::connect(ui->pushButtonSearchDevices, SIGNAL (clicked()),
                     this, SLOT(handleSearchDevices()));
    QObject::connect(ui->pushButtonCancel, SIGNAL (clicked()),
                     this, SLOT(handleCancel()));
    QObject::connect(ui->comboBoxDispenser, SIGNAL(activated(int)),
                     this, SLOT(handleComboBoxDispenserActivated(int)));
    QObject::connect(ui->comboBoxMultiIo, SIGNAL(activated(int)),
                     this, SLOT(handleComboBoxMultiActivated(int)));
    QObject::connect(ui->comboBoxBillValidator, SIGNAL(activated(int)),
                     this, SLOT(handleComboBoxBoxBillValidatorActivated(int)));
    QObject::connect(ui->comboBoxCoinValidator, SIGNAL(activated(int)),
                     this, SLOT(handleComboBoxCoinValidatorActivated(int)));
    QObject::connect(ui->toolButtonDispenser, SIGNAL (clicked()),
                     this, SLOT(handleSerialSettingsDispenser()));
    QObject::connect(ui->toolButtonBillValidator, SIGNAL (clicked()),
                     this, SLOT(handleSerialSettingsBillValidator()));
    QObject::connect(ui->toolButtonCoinValidator, SIGNAL (clicked()),
                     this, SLOT(handleSerialSettingsCoinValidator()));
    QObject::connect(ui->toolButtonMultiIo, SIGNAL (clicked()),
                     this, SLOT(handleSerialSettingsMultiIo()));
}

AtmModelSelect::~AtmModelSelect() {
    delete ui;
}

void AtmModelSelect::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void AtmModelSelect::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void AtmModelSelect::handleSelectionLangChanged(int index) {
    if (index == 1) {
        this->changeLanguageTo("EN");
        ui->comboBoxChangeModelATM->setEnabled(true);
    } else if (index == 2) {
        this->changeLanguageTo("pt_BR");
        ui->comboBoxChangeModelATM->setEnabled(true);
    } else {
        ui->comboBoxChangeModelATM->setEnabled(false);
    }
}

void AtmModelSelect::changeLanguageTo(const QString &language) {
    const QString ts =  ("TestATM_" + language);

    if (ts == "TestATM_pt_BR") {
        if (this->m_translator->load(ts, ":/ts")) {
            QCoreApplication::removeTranslator(this->m_translator);
            QCoreApplication::installTranslator(this->m_translator);
            AtmModelSelect::m_languageSelected = "pt_BR";
        }
    } else if (ts == "TestATM_EN") {
        QCoreApplication::removeTranslator(this->m_translator);
        AtmModelSelect::m_languageSelected = "EN";
    }
}

QString AtmModelSelect::printLanguageSelect() {
    return AtmModelSelect::m_languageSelected;
}

void AtmModelSelect::hideAllTabs() {
    for (register int i = 0; i < ui->tabWidget->count(); ++i) {
        ui->tabWidget->removeTab(i);
    }

    ui->tabWidget->hide();
}

void AtmModelSelect::handleSearchDevices() {
    this->m_searchDevices = new SearchDevices(0);
    this->m_searchDevices->adjustSize();
    this->m_searchDevices->resize(900, 450);
    this->m_searchDevices->move(QApplication::desktop()->screen()->rect().center() -
                                this->m_searchDevices->rect().center());

    // Slots and signals.
    QObject::connect(this->m_searchDevices, SIGNAL(signalUpdateSerialPorts()), this, SLOT(updatePortsATMEmpresa1()));
    
    this->m_searchDevices->setWindowModality(Qt::WindowModal);
    this->m_searchDevices->show();
    this->m_searchDevices->activateWindow();
}

void AtmModelSelect::handleSelectionModelATMChanged(int index) {
    this->getSerialPorts();
    const QString msg = QObject::tr("Sorry! Not found any serial port enabled in the system.");

    if (msg.isEmpty()) {}

    // Check serial only in mode release.
#ifdef TESTATM_RELEASE
    if (m_countSerialPorts == 0) {
        ui->tabWidget->hide();
        this->hideAllTabs();
        QMessageBox::critical(this, QCoreApplication::applicationName(), msg);
        return;
    }
#endif

    this->hideAllTabs();

    if (index == 0) {

        this->repaint();
        this->resize(900, 150);
        this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
        this->show();

        return;
    } else if (index == 1) {
        ui->tabWidget->addTab(ui->tabModelEmpresa1, ui->comboBoxChangeModelATM->currentText());
        this->setPortsATMEmpresa1();
        ui->tabWidget->adjustSize();
        ui->tabWidget->show();
        ui->pushButtonSearchDevices->setEnabled(true);
        ui->pushButtonStart->setEnabled(true);
    } else {

        this->repaint();
        this->resize(900, 150);
        this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
        this->show();

        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("Sorry! Functionality not implemented for this release."));
        return;
    }

    ui->tabWidget->setCurrentIndex(0);

    this->repaint();
    this->resize(900,450);
    this->move(QApplication::desktop()->screen()->rect().center() - this->rect().center());
    this->show();
}

void AtmModelSelect::getSerialPorts() {
    // Get all serial data
    QList<QSerialPortInfo> serialList = utils::SerialPort::printAvaliablePorts();
    this->m_countSerialPorts = serialList.count();
    foreach (const QSerialPortInfo &serialInfo, serialList) {
        AtmModelSelect::m_portName << serialInfo.portName();
    }
}

void AtmModelSelect::setPortsATMEmpresa1() {
    register int val = 0;
#ifdef TESTATM_DEBUG
#if defined(Q_OS_WIN)
    AtmModelSelect::m_portName << "COM1" << "COM2" << "COM3" << "COM4" << "COM5"
                               << "COM6" << "COM7" << "COM8" << "COM9" << "COM10";
#elif defined(Q_OS_LINUX)
    AtmModelSelect::m_portName << "ttyS0" << "ttyS1" << "ttyS2" << "ttyS3" << "ttyS4"
                              << "ttyS5" << "ttyS6" << "ttyS7" << "ttyS8" << "ttyS9";
#endif
#endif

    QIcon icon;
    icon.addFile(QStringLiteral(":/res/images/serialport.png"), QSize(), QIcon::Normal, QIcon::Off);

    for (register int i = 0; i < AtmModelSelect::m_portName.size(); ++i) {
        ui->comboBoxDispenser->addItem(icon, AtmModelSelect::m_portName[i]);
        ui->comboBoxMultiIo->addItem(icon, AtmModelSelect::m_portName[i]);
        ui->comboBoxBillValidator->addItem(icon, AtmModelSelect::m_portName[i]);
        ui->comboBoxCoinValidator->addItem(icon, AtmModelSelect::m_portName[i]);
        ui->comboBoxUps->addItem(icon, AtmModelSelect::m_portName[i]);
    }

    // Dispenser.
#if defined(Q_OS_WIN)
    val = AtmModelSelect::m_portName.indexOf("COM4");
#elif defined(Q_OS_LINUX)
    val = AtmModelSelect::m_portName.indexOf("ttyS4");
#endif
    if (val >= 0) {
        ui->comboBoxDispenser->setCurrentIndex(ui->comboBoxDispenser->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));

        // Set serial comunication with Dispenser.
        AtmModelSelect::m_portDispenser = ui->comboBoxDispenser->currentText();
        AtmModelSelect::m_baudRateDispenser = 38400;
        AtmModelSelect::m_dataBitDispenser = 8;
        AtmModelSelect::m_parityDispenser = 0;
        AtmModelSelect::m_stopBitDispenser = 1;
        AtmModelSelect::m_flowControlDispenser = 0;
    } else {
        QLOG_ERROR() << "AtmModelSelect: Error serial port COM4 not found.";
    }

    // Multi/IO.
#if defined(Q_OS_WIN)
    val = AtmModelSelect::m_portName.indexOf("COM5");
#elif defined(Q_OS_LINUX)
    val = AtmModelSelect::m_portName.indexOf("ttyS5");
#endif
    if (val >= 0) {
        ui->comboBoxMultiIo->setCurrentIndex(ui->comboBoxMultiIo->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));

        // Set serial comunication with Multi/IO.
        AtmModelSelect::m_portMultiIO = ui->comboBoxMultiIo->currentText();
        AtmModelSelect::m_baudRateMultiIO = 9600;
        AtmModelSelect::m_dataBitMultiIO = 8;
        AtmModelSelect::m_parityMultiIO = 0;
        AtmModelSelect::m_stopBitMultiIO = 1;
        AtmModelSelect::m_flowControlMultiIO = 0;
    } else {        
        QLOG_ERROR() << "AtmModelSelect: Error serial port COM5 not found.";
    }

    // Bill validator.
#if defined(Q_OS_WIN)
    val = AtmModelSelect::m_portName.indexOf("COM3");
#elif defined(Q_OS_LINUX)
    val = AtmModelSelect::m_portName.indexOf("ttyS3");
#endif
    if (val >= 0) {
        ui->comboBoxBillValidator->setCurrentIndex(ui->comboBoxBillValidator->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));

        // Set serial comunication with BillValidator.
        AtmModelSelect::m_portBillValidator = ui->comboBoxBillValidator->currentText();
        AtmModelSelect::m_baudRateBillValidator = 9600;
        AtmModelSelect::m_dataBitBillValidator = 8;
        AtmModelSelect::m_parityBillValidator = 0;
        AtmModelSelect::m_stopBitBillValidator = 1;
        AtmModelSelect::m_flowControlBillValidator = 0;
    } else {
        QLOG_ERROR() << "AtmModelSelect: Error serial port COM3 not found.";
    }

    // Coin Validator.
#if defined(Q_OS_WIN)
    val = AtmModelSelect::m_portName.indexOf("COM2");
#elif defined(Q_OS_LINUX)
    val = AtmModelSelect::m_portName.indexOf("ttyS2");
#endif
    if (val >= 0) {
        ui->comboBoxCoinValidator->setCurrentIndex(ui->comboBoxCoinValidator->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));

        // Set serial comunication with CoinValidator.
        AtmModelSelect::m_portCoinValidator = ui->comboBoxCoinValidator->currentText();
        AtmModelSelect::m_baudRateCoinValidator = 9600;
        AtmModelSelect::m_dataBitCoinValidator = 8;
        AtmModelSelect::m_parityCoinValidator = 0;
        AtmModelSelect::m_stopBitCoinValidator = 1;
        AtmModelSelect::m_flowControlCoinValidator = 0;
    } else {
        QLOG_ERROR() << "AtmModelSelect: Error serial port COM2 not found.";
    }

    // UPS.
#if defined(Q_OS_WIN)
    val = AtmModelSelect::m_portName.indexOf("COM6");
#elif defined(Q_OS_LINUX)
    val = AtmModelSelect::m_portName.indexOf("ttyS6");
#endif
    if (val >= 0) {
        ui->comboBoxUps->setCurrentIndex(ui->comboBoxUps->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));
        AtmModelSelect::m_portUps = ui->comboBoxUps->currentIndex();
    } else {
        QLOG_ERROR() << "AtmModelSelect: Error serial port COM6 not found.";
    }

    // Serial LoopBack.
    QString portLoopBack = QString();
#if defined(Q_OS_WIN)
    portLoopBack = QString("COM3");
#elif defined(Q_OS_LINUX)
    portLoopBack = "ttyS7";
#endif
    // Set serial comunication for test Loopback.
    AtmModelSelect::m_portSerialLoopBack  = portLoopBack;
    AtmModelSelect::m_baudRateSerialLoopBack = 9600;
    AtmModelSelect::m_dataBitSerialLoopBack = 8;
    AtmModelSelect::m_paritySerialLoopBack = 0;
    AtmModelSelect::m_stopBitSerialLoopBack = 1;
    AtmModelSelect::m_flowControlSerialLoopBack = 0;

    // Printer.
    QIcon iconP;
    iconP.addFile(QStringLiteral(":/res/images/desktop-driver.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->comboBoxPrinter->addItem(iconP, QString("EPSON-T-500"));
    AtmModelSelect::m_portPrinter = "ESDPRT001/COM1";

    // Camera.
    QIcon iconC;
    iconC.addFile(QStringLiteral(":/res/images/desktop-driver.png"), QSize(), QIcon::Normal, QIcon::Off);
    ui->comboBoxCamera->addItem(iconC, QString("USB Generic"));

    // Smart Card.
    bool readers = devices::cardreader::AbstractCrt603Cz1::getInstance()->getReaders();

    if (!readers) {
        ui->comboBoxSmartCardReader->setDisabled(true);
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("SmartCard reader not found in system."));
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

    // Set icons size.
    ui->comboBoxDispenser->setIconSize(QSize(32, 32));
    ui->comboBoxMultiIo->setIconSize(QSize(32, 32));
    ui->comboBoxBillValidator->setIconSize(QSize(32, 32));
    ui->comboBoxCoinValidator->setIconSize(QSize(32, 32));
    ui->comboBoxPrinter->setIconSize(QSize(29, 29));
    ui->comboBoxUps->setIconSize(QSize(32, 32));
    ui->comboBoxSmartCardReader->setIconSize(QSize(29, 29));
    ui->comboBoxCamera->setIconSize(QSize(29, 29));
}

void AtmModelSelect::updatePortsATMEmpresa1() {
    register int val = 0;

    // Dispenser.    
    if (!this->m_searchDevices->printFoundPortDispenser().isEmpty()) {
        val = AtmModelSelect::m_portName.indexOf(this->m_searchDevices->printFoundPortDispenser());
        if (val >= 0) {
            ui->comboBoxDispenser->setCurrentIndex(ui->comboBoxDispenser->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));
            ui->comboBoxDispenser->setStyleSheet("QWidget {background-color:blue;}");

            // Set serial comunication with Dispenser.
            AtmModelSelect::m_portDispenser = ui->comboBoxDispenser->currentText();
            AtmModelSelect::m_baudRateDispenser = 38400;
            AtmModelSelect::m_dataBitDispenser = 8;
            AtmModelSelect::m_parityDispenser = 0;
            AtmModelSelect::m_stopBitDispenser = 1;
            AtmModelSelect::m_flowControlDispenser = 0;
        }
    }
    
    // Multi/IO.
    if (!this->m_searchDevices->printFoundPortMultiIo().isEmpty()) {
        val = AtmModelSelect::m_portName.indexOf(this->m_searchDevices->printFoundPortMultiIo());
        if (val >= 0) {
            ui->comboBoxMultiIo->setCurrentIndex(ui->comboBoxMultiIo->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));
            ui->comboBoxMultiIo->setStyleSheet("QWidget {background-color:blue;}");

            // Set serial comunication with Multi/IO.
            AtmModelSelect::m_portMultiIO = ui->comboBoxMultiIo->currentText();
            AtmModelSelect::m_baudRateMultiIO = 9600;
            AtmModelSelect::m_dataBitMultiIO = 8;
            AtmModelSelect::m_parityMultiIO = 0;
            AtmModelSelect::m_stopBitMultiIO = 1;
            AtmModelSelect::m_flowControlMultiIO = 0;
        }
    }

    // Bill validator.
    if (!this->m_searchDevices->printFoundPortBillValidator().isEmpty()) {
        val = AtmModelSelect::m_portName.indexOf(this->m_searchDevices->printFoundPortBillValidator());
        if (val >= 0) {
            ui->comboBoxBillValidator->setCurrentIndex(ui->comboBoxBillValidator->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));
            ui->comboBoxBillValidator->setStyleSheet("QWidget {background-color:blue;}");

            // Set serial comunication with BillValidator.
            AtmModelSelect::m_portBillValidator = ui->comboBoxBillValidator->currentText();
            AtmModelSelect::m_baudRateBillValidator = 9600;
            AtmModelSelect::m_dataBitBillValidator = 8;
            AtmModelSelect::m_parityBillValidator = 0;
            AtmModelSelect::m_stopBitBillValidator = 1;
            AtmModelSelect::m_flowControlBillValidator = 0;
        }
    }

    // Coin Validator.
    if (!this->m_searchDevices->printFoundPortCoinValidator().isEmpty()) {
        val = AtmModelSelect::m_portName.indexOf(this->m_searchDevices->printFoundPortCoinValidator());
        if (val >= 0) {
            ui->comboBoxCoinValidator->setCurrentIndex(ui->comboBoxCoinValidator->findData(AtmModelSelect::m_portName[val], Qt::DisplayRole));
            ui->comboBoxCoinValidator->setStyleSheet("QWidget {background-color:blue;}");

            // Set serial comunication with CoinValidator.
            AtmModelSelect::m_portCoinValidator = ui->comboBoxCoinValidator->currentText();
            AtmModelSelect::m_baudRateCoinValidator = 9600;
            AtmModelSelect::m_dataBitCoinValidator = 8;
            AtmModelSelect::m_parityCoinValidator = 0;
            AtmModelSelect::m_stopBitCoinValidator = 1;
            AtmModelSelect::m_flowControlCoinValidator = 0;
        }
    }
}

void AtmModelSelect::handleSerialSettingsDispenser() {
    this->m_settingsserial->setDevice("dispenser");
    this->m_settingsserial->show();
}

void AtmModelSelect::handleSerialSettingsBillValidator() {
    this->m_settingsserial->setDevice("billvalidator");
    this->m_settingsserial->show();
}

void AtmModelSelect::handleSerialSettingsCoinValidator() {
    this->m_settingsserial->setDevice("coinvalidator");
    this->m_settingsserial->show();
}

void AtmModelSelect::handleSerialSettingsMultiIo() {
    this->m_settingsserial->setDevice("multiio");
    this->m_settingsserial->show();
}

void AtmModelSelect::handleComboBoxDispenserActivated(int index) {
    ui->comboBoxDispenser->setCurrentIndex(index);
    ui->comboBoxDispenser->setStyleSheet("");
}

void AtmModelSelect::handleComboBoxMultiActivated(int index) {
    ui->comboBoxMultiIo->setCurrentIndex(index);
    ui->comboBoxMultiIo->setStyleSheet("");
}

void AtmModelSelect::handleComboBoxBoxBillValidatorActivated(int index) {
    ui->comboBoxBillValidator->setCurrentIndex(index);
    ui->comboBoxBillValidator->setStyleSheet("");
}

void AtmModelSelect::handleComboBoxCoinValidatorActivated(int index) {
    ui->comboBoxCoinValidator->setCurrentIndex(index);
    ui->comboBoxCoinValidator->setStyleSheet("");
}

bool AtmModelSelect::checkComboBoxSerialPorts() {
    bool showMsg = false;
    const QString comboDispenser = ui->comboBoxDispenser->currentText();
    const QString comboMultiIo = ui->comboBoxMultiIo->currentText();
    const QString comboBillValidator = ui->comboBoxBillValidator->currentText();
    const QString comboCoinValidator = ui->comboBoxCoinValidator->currentText();

    // Check combo dispenser.
    if (comboDispenser == comboMultiIo || comboDispenser == comboBillValidator ||
        comboDispenser == comboCoinValidator) {
        ui->comboBoxDispenser->setStyleSheet("QWidget {background-color:red;}");

        if(comboDispenser == comboMultiIo) {
            ui->comboBoxMultiIo->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboDispenser == comboBillValidator) {
            ui->comboBoxBillValidator->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboDispenser == comboCoinValidator) {
            ui->comboBoxCoinValidator->setStyleSheet("QWidget {background-color:red;}");
        }

        showMsg = true;
    }

    // Check combo multiio.
    if (comboMultiIo == comboDispenser || comboMultiIo == comboBillValidator ||
        comboMultiIo == comboCoinValidator) {
        ui->comboBoxMultiIo->setStyleSheet("QWidget {background-color:red;}");

        if(comboMultiIo == comboDispenser) {
            ui->comboBoxDispenser->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboMultiIo == comboBillValidator) {
            ui->comboBoxBillValidator->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboMultiIo == comboCoinValidator) {
            ui->comboBoxCoinValidator->setStyleSheet("QWidget {background-color:red;}");
        }

        showMsg = true;
    }

    // Check combo bill validator
    if (comboBillValidator == comboDispenser || comboBillValidator == comboMultiIo ||
        comboBillValidator == comboCoinValidator) {
        ui->comboBoxBillValidator->setStyleSheet("QWidget {background-color:red;}");

        if(comboBillValidator == comboDispenser) {
            ui->comboBoxDispenser->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboBillValidator == comboMultiIo) {
            ui->comboBoxMultiIo->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboBillValidator == comboCoinValidator) {
            ui->comboBoxCoinValidator->setStyleSheet("QWidget {background-color:red;}");
        }

        showMsg = true;
    }

    // Check combo coin validator
    if (comboCoinValidator == comboDispenser || comboCoinValidator == comboMultiIo ||
        comboCoinValidator == comboBillValidator) {
        ui->comboBoxCoinValidator->setStyleSheet("QWidget {background-color:red;}");

        if(comboCoinValidator == comboDispenser) {
            ui->comboBoxDispenser->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboCoinValidator == comboMultiIo) {
            ui->comboBoxMultiIo->setStyleSheet("QWidget {background-color:red;}");
        }

        if(comboCoinValidator == comboBillValidator) {
            ui->comboBoxCoinValidator->setStyleSheet("QWidget {background-color:red;}");
        }

        showMsg = true;
    }

    if (showMsg) {
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                             QObject::tr("Sorry! There duplicate serial port. Please verify."));
        return false;
    }

    return true;
}

void AtmModelSelect::handleConsole() {
    this->handleShow();
}

void AtmModelSelect::handleCancel() {
    Q_EMIT signalCancel();
    this->close();
}

void AtmModelSelect::handleShow() {
    Q_EMIT signalShow();
    this->close();
}
