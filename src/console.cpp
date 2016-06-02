/**
 * @file about.cpp
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

#include <console.hpp>
#include "ui_console.h"

#include <mainwindow.hpp>
#include <atmmodelselect.hpp>

#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QScrollBar>
#include <QTimer>
#include <QTime>

#include <QFutureWatcher>

Console::Console(QWidget *parent) :
    QWidget(parent), ui(new Ui::Console) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Set option widget.
    this->setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);

    // Load ui.
    ui->setupUi(this);

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

    // Hide objects.
    ui->labelTestsFaileds->hide();

    // Clock timer.
    QTimer *timer = new QTimer(this);
    QObject::connect(timer, SIGNAL(timeout()), this, SLOT(showTime()));
    timer->start(1000);

    // Set objects.
    ui->progressBar->setAlignment(Qt::AlignCenter);

    // Slots and signals.
    QObject::connect(ui->progressBar, SIGNAL(valueChanged(int)),
                     this, SLOT(valueChangedBar(int)));
    QObject::connect(ui->pushButtonOk, SIGNAL (clicked()),
                     this, SLOT(handleMainWindow()));
    QObject::connect(ui->pushButtonBack, SIGNAL (clicked()),
                     this, SLOT(handleAtmSelect()));
}

Console::~Console() {
    delete ui;
}

void Console::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void Console::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void Console::valueChangedBar(int value) {
    ui->progressBar->setValue(value);
}

void Console::putData(const QString &data) {
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
    } else if (str == "PASSED") {
        QString msg = "Command was successful: [<font color=\"green\">PASSED</font>]";
        ui->plainTextEdit->appendHtml(msg);
    } else if (str == "FAILED") {
        QString msg = "Command was unsuccessful: [<font color=\"red\">FAILED</font>]";
        ui->plainTextEdit->appendHtml(msg);
    } else {
        ui->plainTextEdit->appendHtml(QString(data));
    }

    QScrollBar *bar = ui->plainTextEdit->verticalScrollBar();
    bar->setValue(bar->maximum());
}

void Console::showTime() {
    QTime time = QTime::currentTime();
    QString text = time.toString("hh:mm");
    if ((time.second() % 2) == 0) {
        text[2] = ' ';
    }
    ui->lcdNumber->display(text);
}

void Console::handleMainWindow() {
    Q_EMIT signalUpdateObjs();
    this->close();
}

void Console::handleAtmSelect() {
    Q_EMIT signalShow();
    this->close();
}

void Console::runDeviceAutoTest() {
    QLOG_INFO() << "Start automatic testing devices...";

    bool cmd = false;
    register int totalfailed = 0;
    int elapsedtimer = 0;

    utils::Tools::getInstance()->startElapsedTimer();
    this->putData("START");

    //
    // Device Multi/IO.
    //
    {
        this->putData("MULTIIO");
        this->putData("Trying to connect the multi/io in " + AtmModelSelect::printPortMultiIO() + "...");
        bool dopen = true;
        int elapsedtimer = 0;
        utils::Tools::getInstance()->startElapsedTimer();
        cmd = devices::multiio::AbstractLed10v2::getInstance()->open(AtmModelSelect::printPortMultiIO().toStdString(),
                                                                     AtmModelSelect::printBaudRateMultiIO(),
                                                                     AtmModelSelect::printDataBitMultiIO(),
                                                                     AtmModelSelect::printParityMultiIO(),
                                                                     AtmModelSelect::printStopBitMultiIO(),
                                                                     AtmModelSelect::printFlowControlMultiIO());
        if (cmd) {
            atm::E1::insertLogAutoMultiIo("open+AUTO=PASSED");
            this->putData("PASSED");
        } else {
            atm::E1::insertLogAutoMultiIo("open+AUTO=FAILED");
            this->putData("FAILED");
            dopen = false;
            totalfailed++;
        }
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogAutoMultiIo("open+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        if (dopen) {
            this->putData("Send command power on device (printer) V24 pin 1...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::multiio::AbstractLed10v2::getInstance()->powerOn(1);
            if (cmd) {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+PRINTER=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+PRINTER=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoMultiIo("powerOn+AUTO+PRINTER+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command power off device (coinvalidator) V12 pin 3...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::multiio::AbstractLed10v2::getInstance()->powerOn(3);
            if (cmd) {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+COINVALIDATOR=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+COINVALIDATOR=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoMultiIo("powerOn+AUTO+COINVALIDATOR+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command power on device (billvalidator) V24 pin 9...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::multiio::AbstractLed10v2::getInstance()->powerOn(9);
            if (cmd) {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+BILLVALIDATOR=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+BILLVALIDATOR=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoMultiIo("powerOn+AUTO+BILLVALIDATOR+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command power off device (dispenser) pine 11...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::multiio::AbstractLed10v2::getInstance()->powerOn(11);
            if (cmd) {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+DISPENSER=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoMultiIo("powerOn+AUTO+DISPENSER=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoMultiIo("powerOn+AUTO+DISPENSER+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command close...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::multiio::AbstractLed10v2::getInstance()->close();
            if (cmd) {
                atm::E1::insertLogAutoMultiIo("close+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoMultiIo("close+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoMultiIo("close+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");
        }  else {
            QLOG_ERROR() << "Its not possible to connect device (MultiIO). The remaining tests are aborted.";
            this->putData("Its not possible to connect device (MultiIO). The remaining tests are aborted.");
        }

        this->putData("--------------------------------------------------");
        this->putData("");
    }
    ui->progressBar->setValue(10);

    //
    // Device Dispenser.
    //
    {
        this->putData("DISPENSER");
        this->putData("Trying to connect the dispenser in " + AtmModelSelect::printPortDispenser() + "...");
        bool dopen = true;
        int elapsedtimer = 0;
        utils::Tools::getInstance()->startElapsedTimer();
        cmd = devices::dispenser::AbstractCrt591::getInstance()->open(AtmModelSelect::printPortDispenser().toStdString(),
                                                                      AtmModelSelect::printBaudRateDispenser(),
                                                                      AtmModelSelect::printDataBitDispenser(),
                                                                      AtmModelSelect::printParityDispenser(),
                                                                      AtmModelSelect::printStopBitDispenser(),
                                                                      AtmModelSelect::printFlowControlDispenser());
        if (cmd) {
            atm::E1::insertLogAutoDispenser("open+AUTO=PASSED");
            this->putData("PASSED");
        } else {
            atm::E1::insertLogAutoDispenser("open+AUTO=FAILED");
            this->putData("FAILED");
            dopen = false;
            totalfailed++;
        }
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogAutoDispenser("open+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        if (dopen) {
            this->putData("Send command init...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::dispenser::AbstractCrt591::getInstance()->init(ABSTRACT_CRT591_INIT_NOT_MOVE_CARD_POSITION);
            if (cmd) {
                atm::E1::insertLogAutoDispenser("init+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoDispenser("init+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoDispenser("init+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command card status...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd =  devices::dispenser::AbstractCrt591::getInstance()->cardStatus();
            if (cmd) {
                atm::E1::insertLogAutoDispenser("cardStatus+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoDispenser("cardStatus+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoDispenser("cardStatus+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command sensor status...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::dispenser::AbstractCrt591::getInstance()->sensorStatus();
            if (cmd) {
                atm::E1::insertLogAutoDispenser("sensorStatus+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoDispenser("sensorStatus+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoDispenser("sensorStatus+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command reset...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd =  devices::dispenser::AbstractCrt591::getInstance()->reset();
            if (cmd) {
                atm::E1::insertLogAutoDispenser("reset+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoDispenser("reset+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoDispenser("reset+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command close...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd =  devices::dispenser::AbstractCrt591::getInstance()->close();
            if (cmd) {
                atm::E1::insertLogAutoDispenser("close+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoDispenser("close+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoDispenser("close+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");
        } else {
            QLOG_ERROR() << "Its not possible to connect device (Dispenser). The remaining tests are aborted.";
            this->putData("Its not possible to connect device (Dispenser). The remaining tests are aborted.");
        }

        this->putData("--------------------------------------------------");
        this->putData("");
    }    
    ui->progressBar->setValue(20);

    //
    // Device BillValidator.
    //
    {
        this->putData("BILLVALIDATOR");
        this->putData("Trying to connect the bill validator in " + AtmModelSelect::printPortBillValidator() + "...");
        bool dopen = true;
        int elapsedtimer = 0;
        utils::Tools::getInstance()->startElapsedTimer();
        cmd = devices::billvalidator::AbstractCashCode609::getInstance()->open(AtmModelSelect::printPortBillValidator().toStdString(),
                                                                               AtmModelSelect::printBaudRateBillValidator(),
                                                                               AtmModelSelect::printDataBitBillValidator(),
                                                                               AtmModelSelect::printParityBillValidator(),
                                                                               AtmModelSelect::printStopBitBillValidator(),
                                                                               AtmModelSelect::printFlowControlBillValidator());
        if (cmd) {
            atm::E1::insertLogAutoBillValidator("open+AUTO=PASSED");
            this->putData("PASSED");
        } else {            
            atm::E1::insertLogAutoBillValidator("open+AUTO=FAILED");
            this->putData("FAILED");
            dopen = false;
            totalfailed++;
        }
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogAutoBillValidator("open+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        if (dopen) {
            this->putData("Send command reset...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::billvalidator::AbstractCashCode609::getInstance()->reset();
            if (cmd) {
                atm::E1::insertLogAutoBillValidator("reset+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoBillValidator("reset+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoBillValidator("reset+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command get firmeware version...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::billvalidator::AbstractCashCode609::getInstance()->version();
            if (cmd) {
                const QString ver = QString::fromStdString( devices::billvalidator::AbstractCashCode609::getInstance()->printVersion());
                if (!ver.isEmpty()) {
                    atm::E1::insertLogAutoBillValidator("version+AUTO=PASSED");
                    this->putData("PASSED");
                } else {
                    atm::E1::insertLogAutoBillValidator("version+AUTO=FAILED");
                    this->putData("FAILED");
                }
            } else {
                atm::E1::insertLogAutoBillValidator("version+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoBillValidator("version+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command close...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd =  devices::billvalidator::AbstractCashCode609::getInstance()->close();
            if (cmd) {
                atm::E1::insertLogAutoBillValidator("close+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoBillValidator("close+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoBillValidator("close+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");
        } else {
            QLOG_ERROR() << "Its not possible to connect device (BillValidator). The remaining tests are aborted.";
            this->putData("Its not possible to connect device (BillValidator). The remaining tests are aborted.");
        }

        this->putData("--------------------------------------------------");
        this->putData("");
    }
    ui->progressBar->setValue(30);

    //
    // Device CoinValidator.
    //
    { 
        this->putData("COINVALIDATOR");
        this->putData("Trying to connect the coin validator in " + AtmModelSelect::printPortCoinValidator() + "...");
        bool dopen = true;
        int elapsedtimer = 0;
        utils::Tools::getInstance()->startElapsedTimer();
        cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->open(AtmModelSelect::printPortCoinValidator().toStdString(),
                                                                            AtmModelSelect::printBaudRateCoinValidator(),
                                                                            AtmModelSelect::printDataBitCoinValidator(),
                                                                            AtmModelSelect::printParityCoinValidator(),
                                                                            AtmModelSelect::printStopBitCoinValidator(),
                                                                            AtmModelSelect::printFlowControlCoinValidator());

        if (cmd) {
            atm::E1::insertLogAutoCoinValidator("open+AUTO=PASSED");
            this->putData("PASSED");
        } else {
            atm::E1::insertLogAutoCoinValidator("open+AUTO=FAILED");
            this->putData("FAILED");
            dopen = false;
            totalfailed++;
        }
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogAutoCoinValidator("open+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        if (dopen) {
            this->putData("Send command reset...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->reset();
            if (cmd) {
                atm::E1::insertLogAutoCoinValidator("reset+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoCoinValidator("reset+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoCoinValidator("reset+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command get firmeware version...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->version();
            if (cmd) {
                const QString ver = QString::fromStdString(devices::coinvalidator::AbstractMei9520e::getInstance()->printVersion());
                if (!ver.isEmpty()) {
                    atm::E1::insertLogAutoCoinValidator("version+AUTO=PASSED");
                    this->putData("PASSED");
                } else {
                    atm::E1::insertLogAutoCoinValidator("version+AUTO=FAILED");
                    this->putData("FAILED");
                }
            } else {
                atm::E1::insertLogAutoCoinValidator("version+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoCoinValidator("version+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command close...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::coinvalidator::AbstractMei9520e::getInstance()->close();
            if (cmd) {
                atm::E1::insertLogAutoCoinValidator("close+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoCoinValidator("close+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoCoinValidator("close+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        } else {
            QLOG_ERROR() << "Its not possible to connect device (CoinValidator). The remaining tests are aborted.";
            this->putData("Its not possible to connect device (CoinValidator). The remaining tests are aborted.");
        }

        this->putData("--------------------------------------------------");
        this->putData("");
    }
    ui->progressBar->setValue(40);

    //
    // Device Printer.
    //
    {
        this->putData("PRINTER");
        bool nofail = true;
        this->putData("Checks for printer on your ATM system....");
        int elapsedtimer = 0;
        utils::Tools::getInstance()->startElapsedTimer();
        cmd = devices::printer::AbstractT500::getInstance()->showPrinters();
        if (cmd) {
            atm::E1::insertLogAutoPrinter("showPrinter+AUTO=PASSED");
            this->putData("PASSED");
        } else {
            atm::E1::insertLogAutoPrinter("showPrinter+AUTO=FAILED");
            this->putData("FAILED");
            nofail = false;
            totalfailed++;
        }
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogAutoPrinter("showPrinter+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        this->putData("Send command get information printer EPSON T-500...");
        utils::Tools::getInstance()->startElapsedTimer();
        cmd = devices::printer::AbstractT500::getInstance()->info(true, "EPSON EU-T500 Receipt");
        if (cmd && nofail) {
            atm::E1::insertLogAutoPrinter("info+AUTO=PASSED");
            this->putData("PASSED");
        } else {
            atm::E1::insertLogAutoPrinter("info+AUTO=FAILED");
            this->putData("FAILED");
            totalfailed++;
        }
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogAutoPrinter("info+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        this->putData("--------------------------------------------------");
        this->putData("");
    }
    ui->progressBar->setValue(50);
    ui->progressBar->setValue(60);
    ui->progressBar->setValue(70);

    //
    // Device Reader Smartcard.
    //
    {
        this->putData("SMARTCARD");
        this->putData("Trying to connect the USB  smartcard reader");
        bool dopen = true;
        int elapsedtimer = 0;
        utils::Tools::getInstance()->startElapsedTimer();
        cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->open();
        if (cmd) {
            atm::E1::insertLogAutoSmartCard("open+AUTO=PASSED");
            this->putData("PASSED");
        } else {
            atm::E1::insertLogAutoSmartCard("open+AUTO=FAILED");
            this->putData("FAILED");
            dopen = false;
            totalfailed++;
        }
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogAutoSmartCard("open+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        if (dopen) {
            this->putData("Send command card status...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->cardStatus();
            if (cmd) {
                atm::E1::insertLogAutoSmartCard("cardStatus+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoSmartCard("cardStatus+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }            
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoSmartCard("cardStatus+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

            this->putData("Send command close...");
            utils::Tools::getInstance()->startElapsedTimer();
            cmd = devices::cardreader::AbstractCrt603Cz1::getInstance()->close();
            if (cmd) {
                atm::E1::insertLogAutoSmartCard("close+AUTO=PASSED");
                this->putData("PASSED");
            } else {
                atm::E1::insertLogAutoSmartCard("close+AUTO=FAILED");
                this->putData("FAILED");
                totalfailed++;
            }
            elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
            atm::E1::insertLogAutoSmartCard("close+AUTO+ELAPSEDTIMER=" + QString::number(elapsedtimer));
            this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");

        } else {
            QLOG_ERROR() << "Its not possible to connect device (SmartCard). The remaining tests are aborted.";
            this->putData("Its not possible to connect device (SmartCard). The remaining tests are aborted.");
        }

        this->putData("--------------------------------------------------");
        this->putData("");
    }
    ui->progressBar->setValue(80);
    ui->progressBar->setValue(90);
    ui->progressBar->setValue(100);

    // Check total tests failed.
    if (totalfailed >= 1) {
        ui->labelTestsFaileds->show();
        const QString hmsg = "<font color='red' size='5'>" + QObject::tr("Failed: ") + QString::number(totalfailed) + "</font>";
        ui->labelTestsFaileds->setText(hmsg);
    }

    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    this->putData("Time to do this: " + QString::number(elapsedtimer) + " in milliseconds.");
    this->putData("END");
}
