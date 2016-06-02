/**
 * @file serialloopback.cpp
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

#include <serialloopbackfit.hpp>
#include "ui_serialloopbackfit.h"

#include <atm/e1.hpp>
#include <atmmodelselect.hpp>
#include <settingsserial.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QScrollBar>
#include <QTimer>
#include <QTime>

SerialLoopBackFit::SerialLoopBackFit(QWidget *parent, bool type) :
    QWidget(parent), ui(new Ui::SerialLoopBackFit), m_countSerialPorts(0),
    m_counttime(0), m_answerAscii(""), m_answerHex("") {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Set option widget.
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    // Load ui.
    ui->setupUi(this);

    // Set labels.
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelPortName->setText("");
    ui->labelBaudRate->setText("");
    ui->labelFlowControl->setText("");

    // Set text edit.
    ui->plainTextEditRequest->document()->setMaximumBlockCount(1000);
    ui->plainTextEditAnswer->document()->setMaximumBlockCount(1000);
    ui->plainTextEditResponseCommands->document()->setMaximumBlockCount(1000);
    QFont font = QFont("Courier");
    font.setStyleHint(QFont::Monospace);
    font.setPointSize(12);
    font.setFixedPitch(true);
    font.setBold(true);
    ui->plainTextEditRequest->setFont(font);
    ui->plainTextEditAnswer->setFont(font);
    ui->plainTextEditResponseCommands->setFont(font);
    ui->plainTextEditRequest->setReadOnly(true);
    ui->plainTextEditAnswer->setReadOnly(true);
    ui->plainTextEditResponseCommands->setReadOnly(true);

    // Set palette color.
    QPalette p = palette();
    p.setColor(QPalette::Base, Qt::black);
    p.setColor(QPalette::Text, Qt::red);
    ui->plainTextEditRequest->setPalette(p);

    // Set palette color.
    QPalette p1 = palette();
    p1.setColor(QPalette::Base, Qt::black);
    p1.setColor(QPalette::Text, Qt::blue);
    ui->plainTextEditAnswer->setPalette(p1);

    // Set palette color.
    QPalette p2 = palette();
    p2.setColor(QPalette::Base, Qt::black);
    p2.setColor(QPalette::Text, Qt::green);
    ui->plainTextEditResponseCommands->setPalette(p2);
    ui->progressBar->setPalette(p2);

    // Set objects.
    ui->progressBar->setAlignment(Qt::AlignCenter);
    ui->progressBar->setValue(0);
    ui->progressBar->hide();

    // Timer clock count UP.
    this->m_timer = new QTimer(this);

    // Slots and signals.    
    QObject::connect(this->m_timer, SIGNAL(timeout()),
                     this, SLOT(countUp()));
    QObject::connect(ui->pushButtonConnect, SIGNAL(clicked()),
                     this, SLOT(serialConnect()));
    QObject::connect(ui->pushButtonBack, SIGNAL(clicked()),
                     this, SLOT(closeForm()));
    QObject::connect(ui->pushButtonDisconnect, SIGNAL(clicked()),
                     this, SLOT(serialDisconnect()));
    QObject::connect(ui->pushButtonOk, SIGNAL(clicked()),
                     this, SLOT(runLoopback()));
    QObject::connect(this, SIGNAL(finishSerial()), this,
                     SLOT(serialReport()));
    QObject::connect(ui->comboBoxSerial, SIGNAL(currentTextChanged(const QString&)),
                     this, SLOT(handleSelectionSerialChanged(const QString&)));
    QObject::connect(ui->toolButtonLoopBack, SIGNAL (clicked()),
                     this, SLOT(handleSerialSettings()));

    // Create instance serial port.
    this->m_serial = new utils::SerialPort;

    // Create instance serial settings.
    this->m_settingsserial = new SettingsSerial(this);

    // Get serial ports.
    this->getSerialPorts();

    if (type) {
        ui->labelMsg->hide();
        ui->selectBoxConnection->hide();
        ui->selectBoxStatus->hide();
        ui->selectBoxTimeConnection->hide();
        ui->pushButtonBack->show();
        ui->pushButtonBack->setEnabled(true);
    } else {
        ui->pushButtonBack->hide();
        ui->labelMsg->hide();

        if (!AtmModelSelect::printPortSerialLoopBack().isEmpty()) {
            // Set default serial com.
            this->setSerialPort(AtmModelSelect::printPortSerialLoopBack(), AtmModelSelect::printBaudRateSerialLoopBack(),
                                AtmModelSelect::printDataBitSerialLoopBack(), AtmModelSelect::printParitySerialLoopBack(),
                                AtmModelSelect::printStopBitSerialLoopBack(), AtmModelSelect::printFlowControlSerialLoopBack());

            // Set port serial default.
            this->handleSelectionSerialChanged("COM1");

        } else {
            // Set default serial com.
            this->setSerialPort(m_cmdParamSerial.comPort, m_cmdParamSerial.baudRate, m_cmdParamSerial.dataBits,
                                m_cmdParamSerial.parity, m_cmdParamSerial.stopBits, m_cmdParamSerial.flowControl);

            this->m_answerAscii.clear();
            this->m_answerHex.clear();
            ui->comboBoxSerial->setCurrentIndex(ui->comboBoxSerial->findData(m_cmdParamSerial.comPort, Qt::DisplayRole));
        }
        this->m_settingsserial->setDevice("serialloopback");
    }
}

SerialLoopBackFit::~SerialLoopBackFit() {
    delete ui;
}

void SerialLoopBackFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void SerialLoopBackFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void SerialLoopBackFit::countUp() {
    m_counttime++;
    ui->lcdNumber->display(this->m_counttime);
}

void SerialLoopBackFit::setSerialPort(const QString comPort, const unsigned int baudRate,
                                      const unsigned int dataBits, const unsigned int parity,
                                      const unsigned int stopBits, const unsigned int flowControl) {
    // Set serial connection.
    m_cmdParamSerial.comPort = comPort;
    m_cmdParamSerial.baudRate = baudRate;
    m_cmdParamSerial.dataBits = dataBits;
    m_cmdParamSerial.parity = parity;
    m_cmdParamSerial.stopBits = stopBits;
    m_cmdParamSerial.flowControl = flowControl;

    this->m_answerAscii.clear();
    this->m_answerHex.clear();
    ui->comboBoxSerial->setCurrentIndex(ui->comboBoxSerial->findData(comPort, Qt::DisplayRole));
}

void SerialLoopBackFit::handleSelectionSerialChanged(const QString &text) {
    // Set serial connection.
    m_cmdParamSerial.comPort = text;
    m_cmdParamSerial.baudRate = 9600;
    m_cmdParamSerial.dataBits = 8;
    m_cmdParamSerial.parity = 0;
    m_cmdParamSerial.stopBits = 1;
    m_cmdParamSerial.flowControl = 0;

    // Set default connection.
    AtmModelSelect::setPortSerialLoopBack(m_cmdParamSerial.comPort);
    AtmModelSelect::setBaudRateSerialLoopBack(m_cmdParamSerial.baudRate);
    AtmModelSelect::setDataBitSerialLoopBack(m_cmdParamSerial.dataBits);
    AtmModelSelect::setParitySerialLoopBack(m_cmdParamSerial.parity);
    AtmModelSelect::setStopBitsSerialLoopBack(m_cmdParamSerial.stopBits);
    AtmModelSelect::setFlowControlSerialLoopBack(m_cmdParamSerial.flowControl);
}

void SerialLoopBackFit::handleSerialSettings() {
    if (ui->comboBoxSerial->currentText().isEmpty()) {
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                              QObject::tr("Please select a serial communication port!"));
        return;
    }
    this->m_settingsserial->setDevice("serialloopback");
    this->m_settingsserial->show();
}

void SerialLoopBackFit::getSerialPorts() {
    // Get all serial data
    QList<QSerialPortInfo> serialList = utils::SerialPort::printAvaliablePorts();
    this->m_countSerialPorts = serialList.count();
    foreach (const QSerialPortInfo &serialInfo, serialList) {
        this->m_portName << serialInfo.portName();
    }

    QIcon icon;
    icon.addFile(QStringLiteral(":/res/images/serialport.png"), QSize(), QIcon::Normal, QIcon::Off);

    // Add the sort proxy.
    QSortFilterProxyModel* proxy = new QSortFilterProxyModel(ui->comboBoxSerial);
    proxy->setSourceModel(ui->comboBoxSerial->model());
    ui->comboBoxSerial->model()->setParent(proxy);
    ui->comboBoxSerial->setModel(proxy);

    for (register int i = 0; i < m_portName.size(); ++i) {
        ui->comboBoxSerial->addItem(icon, m_portName[i], QComboBox::InsertAlphabetically);
    }

    // Sort proxy and set index.
    proxy->sort(0);
    ui->comboBoxSerial->setCurrentIndex(0);

    // Set icons size.
    ui->comboBoxSerial->setIconSize(QSize(32, 32));
}

void SerialLoopBackFit::serialConnect() {
    if (ui->comboBoxSerial->currentText().isEmpty()) {
        QMessageBox::warning(this, QCoreApplication::applicationName(),
                              QObject::tr("Please select a serial communication port!"));
        return;
    }

    this->m_answerAscii.clear();
    this->m_answerHex.clear();

    bool ok = this->m_serial->open(m_cmdParamSerial.comPort.toStdString(), m_cmdParamSerial.baudRate,
                                   m_cmdParamSerial.dataBits, m_cmdParamSerial.parity,
                                   m_cmdParamSerial.stopBits, m_cmdParamSerial.flowControl);
    if (!ok) {
        QLOG_ERROR() << "SerialLoopback: Error open connection.";
        QMessageBox::StandardButton ret;
        ret = QMessageBox::critical(this, QCoreApplication::applicationName(),
                                    QObject::tr("Error open connection in serial port: ") +  m_cmdParamSerial.comPort +
                                    QObject::tr("! If you are using a DB9 connector try to change the value of databit "
                                                "for 1 or 2."),
                                    QMessageBox::Ok);
        if (ret == QMessageBox::Ok) {
            emit errorConnection();
        }
        return;
    } else {
        QLOG_INFO() << "SerialLoopback: open connection successful.";
    }

    m_counttime = 0;
    m_timer->start(1000);
    ui->lcdNumber->display(this->m_counttime);
    const QString hmsg = "<font color='green' size='2'>" + QObject::tr("YES") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelPortName->setText(ui->comboBoxSerial->currentText());
    ui->labelBaudRate->setText(QString::number(AtmModelSelect::printBaudRateSerialLoopBack()));

    if (AtmModelSelect::printFlowControlSerialLoopBack() == 0) {
        ui->labelFlowControl->setText(QObject::tr("None"));
    } else if (AtmModelSelect::printFlowControlSerialLoopBack() == 1) {
        ui->labelFlowControl->setText(QObject::tr("RTS/CTS"));
    } else if (AtmModelSelect::printFlowControlSerialLoopBack() == 2) {
        ui->labelFlowControl->setText(QObject::tr("XON/XOFF"));
    }

    ui->pushButtonConnect->setEnabled(false);
    ui->pushButtonDisconnect->setEnabled(true);
    ui->pushButtonOk->setEnabled(true);
    ui->plainTextEditAnswer->setEnabled(true);
    ui->plainTextEditRequest->setEnabled(true);
    ui->plainTextEditResponseCommands->setEnabled(true);
    ui->toolButtonLoopBack->setEnabled(false);
}

void SerialLoopBackFit::serialDisconnect() {
    QLOG_INFO() << "SerialLoopback: close connection successful.";
    this->m_serial->close();
    this->m_answerAscii.clear();
    this->m_answerHex.clear();
    ui->labelMsg->clear();
    ui->label->hide();

    m_counttime = 0;
    m_timer->stop();
    ui->lcdNumber->display(this->m_counttime);
    const QString hmsg = "<font color='red' size='2'>" + QObject::tr("NO") + "</font>";
    ui->labelConnect->setText(hmsg);
    ui->labelPortName->setText("");
    ui->labelBaudRate->setText("");
    ui->labelFlowControl->setText("");
    ui->plainTextEditAnswer->clear();
    ui->plainTextEditResponseCommands->clear();
    ui->progressBar->setValue(0);
    ui->progressBar->hide();
    ui->pushButtonConnect->setEnabled(true);
    ui->pushButtonDisconnect->setEnabled(false);
    ui->pushButtonOk->setEnabled(false);
    ui->plainTextEditAnswer->setEnabled(false);
    ui->plainTextEditRequest->setEnabled(false);
    ui->plainTextEditResponseCommands->setEnabled(false);
    ui->toolButtonLoopBack->setEnabled(true);
}

void SerialLoopBackFit::closeForm() {
    this->serialDisconnect();
    this->close();
}

void SerialLoopBackFit::runLoopback() {
    QApplication::setOverrideCursor(Qt::WaitCursor);

    ui->progressBar->show();
    ui->plainTextEditAnswer->clear();
    ui->plainTextEditResponseCommands->clear();
    this->m_answerAscii.clear();
    this->m_answerHex.clear();
    ui->labelMsg->clear();
    ui->label->hide();

    QByteArray cmd;

    // Write the alphabet.
    for (register int i = 0; i < 10; ++i) {
        cmd.clear();
        cmd.resize(48);

        cmd[0]  = 0x41;
        cmd[1]  = 0x20;
        cmd[2]  = 0x42;
        cmd[3]  = 0x20;
        cmd[4]  = 0x43;
        cmd[5]  = 0x20;
        cmd[6]  = 0x44;
        cmd[7]  = 0x20;
        cmd[8]  = 0x45;
        cmd[9]  = 0x20;
        cmd[10] = 0x46;
        cmd[11] = 0x20;
        cmd[12] = 0x47;
        cmd[13] = 0x20;
        cmd[14] = 0x48;
        cmd[15] = 0x20;
        cmd[16] = 0x49;
        cmd[17] = 0x20;
        cmd[18] = 0x4a;
        cmd[19] = 0x20;
        cmd[20] = 0x4c;
        cmd[21] = 0x20;
        cmd[22] = 0x4d;
        cmd[23] = 0x20;
        cmd[24] = 0x4e;
        cmd[25] = 0x20;
        cmd[26] = 0x4f;
        cmd[27] = 0x20;
        cmd[28] = 0x50;
        cmd[29] = 0x20;
        cmd[30] = 0x51;
        cmd[31] = 0x20;
        cmd[32] = 0x52;
        cmd[33] = 0x20;
        cmd[34] = 0x53;
        cmd[35] = 0x20;
        cmd[36] = 0x54;
        cmd[37] = 0x20;
        cmd[38] = 0x55;
        cmd[39] = 0x20;
        cmd[40] = 0x56;
        cmd[41] = 0x20;
        cmd[42] = 0x58;
        cmd[43] = 0x20;
        cmd[44] = 0x5a;
        cmd[45] = 0x20;
        cmd[46] = 0x2f;
        cmd[47] = 0x20;

        // Store size pkg.
        this->m_cmdSerial.requestAlphabet = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command alphabet.
        this->getData(1);

        // Progress.
        ui->progressBar->setValue(i+1);

        if (i == 9 || i >= 9) {
            //emit finishSerial();
        }
    }

    // Jump line 1.
    {
        cmd.clear();
        cmd.resize(2);
        cmd[0] = 0x0a;
        cmd[1] = 0x0a;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData();
    }

    // Write the number.
    for (register int i = 0; i < 10; ++i) {
        cmd.clear();
        cmd.resize(19);

        cmd[0]  = 0x31;
        cmd[1]  = 0x20;
        cmd[2]  = 0x32;
        cmd[3]  = 0x20;
        cmd[4]  = 0x33;
        cmd[5]  = 0x20;
        cmd[6]  = 0x34;
        cmd[7]  = 0x20;
        cmd[8]  = 0x35;
        cmd[9]  = 0x20;
        cmd[10] = 0x36;
        cmd[11] = 0x20;
        cmd[12] = 0x37;
        cmd[13] = 0x20;
        cmd[14] = 0x38;
        cmd[15] = 0x20;
        cmd[16] = 0x39;
        cmd[17] = 0x20;
        cmd[18] = 0x2f;

        // Store size pkg.
        this->m_cmdSerial.requestNumbers = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command numbers.
        this->getData(2);

        // Progress.
        ui->progressBar->setValue(i+1);

        if (i == 9 || i >= 9) {
            //emit finishSerial();
        }
    }

    // Jump line 2.
    {
        cmd.clear();
        cmd.resize(2);
        cmd[0] = 0x0a;
        cmd[1] = 0x0a;

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 10);

        // Get response command.
        this->getData();
    }

    // Write Imperial Probe Droid.
    for (register int i = 0; i < 2; ++i) {
        cmd.clear();
        cmd.resize(21);

        // Step 1.
        cmd[0] =  0x20; cmd[1] =  0x20; cmd[2]  = 0x20;
        cmd[3] =  0x20; cmd[4] =  0x20; cmd[5]  = 0x5f;
        cmd[6] =  0x2e; cmd[7] =  0x2e; cmd[8]  = 0x2e;
        cmd[9] =  0x2d; cmd[10] = 0x2d; cmd[11] = 0x7e;
        cmd[12] = 0x7e; cmd[13] = 0x2d; cmd[14] = 0x2d;
        cmd[15] = 0x2e; cmd[16] = 0x2e; cmd[17] = 0x2e;
        cmd[18] = 0x5f; cmd[19] = 0x0a; cmd[20] = 0x20;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 2.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x20; cmd[5]  = 0x2f;
        cmd[6]  = 0x20; cmd[7]  = 0x20; cmd[8]  = 0x5f;
        cmd[9]  = 0x20; cmd[10] = 0x20; cmd[11] = 0x2c;
        cmd[12] = 0x2d; cmd[13] = 0x2e; cmd[14] = 0x20;
        cmd[15] = 0x20; cmd[16] = 0x20; cmd[17] = 0x5f;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x5c;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Progress.
        ui->progressBar->setValue(1);

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 3.
        cmd[0]  = 0x0a; cmd[1]  = 0x20; cmd[2]  = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x20; cmd[5]  = 0x20;
        cmd[6]  = 0x20; cmd[7]  = 0x5c; cmd[8]  = 0x20;
        cmd[9]  = 0x28; cmd[10] = 0x5f; cmd[11] = 0x29;
        cmd[12] = 0x20; cmd[13] = 0x60; cmd[14] = 0x2d;
        cmd[15] = 0x60; cmd[16] = 0x6f; cmd[17] = 0x4f;
        cmd[18] = 0x28; cmd[19] = 0x5f; cmd[20] = 0x29;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 4.
        cmd[0]  = 0x20; cmd[1]  = 0x2f; cmd[2]  = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x20; cmd[5]  = 0x20;
        cmd[6]  = 0x20; cmd[7]  = 0x20; cmd[8]  = 0x20;
        cmd[9]  = 0x49; cmd[10] = 0x6d; cmd[11] = 0x70;
        cmd[12] = 0x65; cmd[13] = 0x72; cmd[14] = 0x69;
        cmd[15] = 0x61; cmd[16] = 0x6c; cmd[17] = 0x0a;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Progress.
        ui->progressBar->setValue(2);

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 5.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x22; cmd[5]  = 0x7e;
        cmd[6]  = 0x2e; cmd[7]  = 0x5f; cmd[8]  = 0x20;
        cmd[9]  = 0x20; cmd[10] = 0x20; cmd[11] = 0x20;
        cmd[12] = 0x20; cmd[13] = 0x20; cmd[14] = 0x5f;
        cmd[15] = 0x2e; cmd[16] = 0x7e; cmd[17] = 0x22;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 6.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x20; cmd[5]  = 0x50;
        cmd[6]  = 0x72; cmd[7]  = 0x6f; cmd[8]  = 0x62;
        cmd[9]  = 0x65; cmd[10] = 0x20; cmd[11] = 0x44;
        cmd[12] = 0x72; cmd[13] = 0x6f; cmd[14] = 0x69;
        cmd[15] = 0x64; cmd[16] = 0x0a; cmd[17] = 0x20;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;

        // Progress.
        ui->progressBar->setValue(3);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 7.
        cmd[0]  = 0x20; cmd[1]  = 0x2c; cmd[2]  = 0x2d;
        cmd[3]  = 0x2d; cmd[4]  = 0x2d; cmd[5]  = 0x2d;
        cmd[6]  = 0x2d; cmd[7]  = 0x22; cmd[8]  = 0x22;
        cmd[9]  = 0x22; cmd[10] = 0x22; cmd[11] = 0x22;
        cmd[12] = 0x22; cmd[13] = 0x2d; cmd[14] = 0x2d;
        cmd[15] = 0x2d; cmd[16] = 0x2d; cmd[17] = 0x2e;
        cmd[18] = 0x0a; cmd[19] = 0x20; cmd[20] = 0x5f;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 8.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x28;
        cmd[3]  = 0x20; cmd[4]  = 0x7c; cmd[5]  = 0x20;
        cmd[6]  = 0x7c; cmd[7]  = 0x20; cmd[8]  = 0x7c;
        cmd[9]  = 0x20; cmd[10] = 0x7c; cmd[11] = 0x20;
        cmd[12] = 0x7c; cmd[13] = 0x20; cmd[14] = 0x7c;
        cmd[15] = 0x20; cmd[16] = 0x7c; cmd[17] = 0x20;
        cmd[18] = 0x7c; cmd[19] = 0x20; cmd[20] = 0x29;

        // Progress.
        ui->progressBar->setValue(4);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 9.
        cmd[0]  = 0x7e; cmd[1]  = 0x7e; cmd[2]  = 0x7e;
        cmd[3]  = 0x3c; cmd[4]  = 0x0a; cmd[5]  = 0x4f;
        cmd[6]  = 0x2d; cmd[7]  = 0x60; cmd[8]  = 0x2d;
        cmd[9]  = 0x2d; cmd[10] = 0x5c; cmd[11] = 0x7c;
        cmd[12] = 0x5f; cmd[13] = 0x7c; cmd[14] = 0x5f;
        cmd[15] = 0x7c; cmd[16] = 0x5f; cmd[17] = 0x7c;
        cmd[18] = 0x5f; cmd[19] = 0x7c; cmd[20] = 0x5f;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 10.
        cmd[0]  = 0x7c; cmd[1]  = 0x5f; cmd[2]  = 0x7c;
        cmd[3]  = 0x5f; cmd[4]  = 0x7c; cmd[5]  = 0x2f;
        cmd[6]  = 0x27; cmd[7]  = 0x2e; cmd[8]  = 0x53;
        cmd[9]  = 0x53; cmd[10] = 0x74; cmd[11] = 0x0a;
        cmd[12] = 0x20; cmd[13] = 0x20; cmd[14] = 0x20;
        cmd[15] = 0x20; cmd[16] = 0x2e; cmd[17] = 0x5f;
        cmd[18] = 0x2c; cmd[19] = 0x27; cmd[20] = 0x20;

        // Progress.
        ui->progressBar->setValue(5);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 11.
        cmd[0]  = 0x7c; cmd[1]  = 0x23; cmd[2]  = 0x7c;
        cmd[3]  = 0x3a; cmd[4]  = 0x20; cmd[5]  = 0x7c;
        cmd[6]  = 0x4a; cmd[7]  = 0x7c; cmd[8]  = 0x23;
        cmd[9]  = 0x7c; cmd[10] = 0x27; cmd[11] = 0x2e;
        cmd[12] = 0x5f; cmd[13] = 0x0a; cmd[14] = 0x20;
        cmd[15] = 0x20; cmd[16] = 0x20; cmd[17] = 0x20;
        cmd[18] = 0x20; cmd[19] = 0x22; cmd[20] = 0x20;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 12.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x2f;
        cmd[3]  = 0x2f; cmd[4]  = 0x20; cmd[5]  = 0x3b;
        cmd[6]  = 0x20; cmd[7]  = 0x60; cmd[8]  = 0x20;
        cmd[9]  = 0x20; cmd[10] = 0x5c; cmd[11] = 0x5c;
        cmd[12] = 0x20; cmd[13] = 0x20; cmd[14] = 0x22;
        cmd[15] = 0x0a; cmd[16] = 0x20; cmd[17] = 0x20;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;

        // Progress.
        ui->progressBar->setValue(6);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 13.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x20;
        cmd[3]  = 0x28; cmd[4]  = 0x2f; cmd[5]  = 0x20;
        cmd[6]  = 0x28; cmd[7]  = 0x5f; cmd[8]  = 0x20;
        cmd[9]  = 0x20; cmd[10] = 0x20; cmd[11] = 0x20;
        cmd[12] = 0x28; cmd[13] = 0x2f; cmd[14] = 0x0a;
        cmd[15] = 0x20; cmd[16] = 0x20; cmd[17] = 0x20;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;

        // Progress.
        ui->progressBar->setValue(7);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 14.
        cmd[0]  = 0x20; cmd[1]  = 0x5f; cmd[2]  = 0x5f;
        cmd[3]  = 0x5c; cmd[4]  = 0x20; cmd[5]  = 0x20;
        cmd[6]  = 0x20; cmd[7]  = 0x20; cmd[8]  = 0x20;
        cmd[9]  = 0x20; cmd[10] = 0x20; cmd[11] = 0x2f;
        cmd[12] = 0x0a; cmd[13] = 0x20; cmd[14] = 0x20;
        cmd[15] = 0x20; cmd[16] = 0x20; cmd[17] = 0x20;
        cmd[18] = 0x5f; cmd[19] = 0x7c; cmd[20] = 0x20;

        // Progress.
        ui->progressBar->setValue(8);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 15.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2] = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x20; cmd[5] = 0x20;
        cmd[6]  = 0x20; cmd[7]  = 0x20; cmd[8] = 0x28;
        cmd[9]  = 0x0a; cmd[10] = 0x20; cmd[11] = 0x20;
        cmd[12] = 0x20; cmd[13] = 0x20; cmd[14] = 0x20;
        cmd[15] = 0x2f; cmd[16] = 0x20; cmd[17] = 0x20;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 1.
        this->getData(3);

        cmd.clear();
        cmd.resize(21);

        // Step 16.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x20; cmd[5]  = 0x20;
        cmd[6]  = 0x5c; cmd[7]  = 0x0a; cmd[8]  = 0x20;
        cmd[9]  = 0x20; cmd[10] = 0x20; cmd[11] = 0x20;
        cmd[12] = 0x20; cmd[13] = 0x20; cmd[14] = 0x20;
        cmd[15] = 0x20; cmd[16] = 0x20; cmd[17] = 0x20;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;

        // Progress.
        ui->progressBar->setValue(9);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP1 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command.
        this->getData();

        cmd.clear();
        cmd.resize(31);

        // Step 17.
        cmd[0]  = 0x20; cmd[1]  = 0x20; cmd[2]  = 0x20;
        cmd[3]  = 0x20; cmd[4]  = 0x20; cmd[5]  = 0x5c;
        cmd[6]  = 0x5f; cmd[7]  = 0x0a; cmd[8]  = 0x20;
        cmd[9]  = 0x20; cmd[10] = 0x20; cmd[11] = 0x20;
        cmd[12] = 0x20; cmd[13] = 0x20; cmd[14] = 0x20;
        cmd[15] = 0x20; cmd[16] = 0x20; cmd[17] = 0x20;
        cmd[18] = 0x20; cmd[19] = 0x20; cmd[20] = 0x20;
        cmd[21] = 0x20; cmd[22] = 0x20; cmd[23] = 0x20;
        cmd[24] = 0x20; cmd[25] = 0x20; cmd[26] = 0x20;
        cmd[27] = 0x5c; cmd[28] = 0x5c; cmd[29] = 0x0a;
        cmd[30] = 0x0a;

        // Progress.
        ui->progressBar->setValue(10);

        // Store size pkg.
        this->m_cmdSerial.requestProbeDroidP2 = cmd.size();

        // Send commands.
        this->m_serial->writeData(cmd, cmd.size(), 100);

        // Get response command prob droid 2.
        this->getData(4);

        cmd.clear();
    }

    emit finishSerial();
}

void SerialLoopBackFit::getData(int cmd) {
    const QByteArray data = m_serial->getData();

    if (data.isEmpty()) {
        return; // Avoid segmentation fault.
    }

    switch (cmd) {
    case 1:
        // Store size cmd alphabet.
        this->m_cmdSerial.answerAlphabet = data.size();
        break;
    case 2:
        // Store size cmd numbers.
        this->m_cmdSerial.answerNumbers = data.size();
        break;
    case 3:
        // Store size cmd probe droid 1.
        this->m_cmdSerial.answerProbeDroidP1 = data.size();
        break;
    case 4:
        // Store size cmd probe droid 2.
        this->m_cmdSerial.answerProbeDroidP2 = data.size();
        break;
    default:
        break;
    }

    this->m_answerAscii += QString::fromStdString(utils::Tools::hexToAscii(data.toHex().toStdString()));
    ui->plainTextEditAnswer->setPlainText(this->m_answerAscii);

    this->m_answerHex += "PKG> " + data.toHex().toUpper() + "\n";
    ui->plainTextEditResponseCommands->setPlainText(this->m_answerHex);

    QScrollBar *bar = ui->plainTextEditResponseCommands->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void SerialLoopBackFit::serialReport() {    
    bool cmdAlphabet = false;
    if (m_cmdSerial.requestAlphabet == m_cmdSerial.answerAlphabet) {
        cmdAlphabet = true;
    }

    bool cmdNumbers = false;
    if (m_cmdSerial.requestNumbers == m_cmdSerial.answerNumbers ) {
        cmdNumbers = true;
    }

    bool cmdProbeDroidP1 = false;
    if (m_cmdSerial.requestProbeDroidP1 == m_cmdSerial.answerProbeDroidP1) {
        cmdProbeDroidP1 = true;
    }

    bool cmdProbeDroidP2 = false;
    if (m_cmdSerial.requestProbeDroidP2 == m_cmdSerial.requestProbeDroidP2) {
        cmdProbeDroidP2 = true;
    }

    ui->progressBar->hide();
    QApplication::restoreOverrideCursor();

    size_t ts = m_cmdSerial.requestAlphabet + m_cmdSerial.requestNumbers +
                m_cmdSerial.requestProbeDroidP1 + m_cmdSerial.requestProbeDroidP2;
    size_t re = m_cmdSerial.answerAlphabet + m_cmdSerial.answerNumbers +
                m_cmdSerial.answerProbeDroidP1 + m_cmdSerial.answerProbeDroidP2;

    const QString hm1 = "<font color='red' size='3'>" + QString::number(ts) + " </font>";
    const QString hm2 = "<font color='red' size='3'>" + QString::number(re) + ".</font>";

    if(!this->m_answerAscii.isEmpty() && !this->m_answerAscii.isEmpty()) {
        if (cmdAlphabet == true && cmdNumbers == true &&
                cmdProbeDroidP1 == true && cmdProbeDroidP2 == true) {
            const QString lmsg = "<font color='green' size='5'>" + QObject::tr("PASSED") + "</font>";
            ui->labelMsg->setText(lmsg);
            ui->labelMsg->show();
            QMessageBox::information(this, QCoreApplication::applicationName(),
                                     QObject::tr("Congratulations! Test passed: total request package is ") + hm1 +
                                     QObject::tr("and total anwser package is ") + hm2);
        }
    } else {
        const QString lmsg = "<font color='red' size='5'>" + QObject::tr("FAILED") + "</font>";
        ui->labelMsg->setText(lmsg);
        ui->labelMsg->show();
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Sorry! Test failed: total request package is ") + hm1 +
                              QObject::tr("and total anwser package is ") + hm2);
    }

}
