/**
 * @file logpreviewer.cpp
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

#include <logpreviewer.hpp>
#include "ui_logpreviewer.h"

#include <atmmodelselect.hpp>
#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/osinfo.hpp>
#include <utils/tools.hpp>

#include <QPrinter>
#include <QtWidgets>
#include <QWebSettings>
#include <QStandardPaths>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>

LogPreviewer::LogPreviewer(QWidget *parent)
    : QWidget(parent), ui(new Ui::LogPreviewer) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Report.
    this->report();

    // Slots and signals.
    QObject::connect(ui->pushButtonExportPdf, SIGNAL (clicked()),
                     this, SLOT(exportToPdf()));
}

LogPreviewer::~LogPreviewer() {
    delete ui;
}

void LogPreviewer::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void LogPreviewer::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void LogPreviewer::exportToPdf() {
    QString id = utils::OsInfo::getInstance()->printHardDiskSerial();
    QString dateTimeString = this->m_dateTime.toString("yyyy-MM-dd_hh-mm-ss");

    ui->webView->settings()->setAttribute(QWebSettings::PrintElementBackgrounds, true);
    QPrinter printer;
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setDocName(QObject::tr("Report Test ATM E-1"));
    printer.setPaperSize(QPrinter::A4);

    const QString savePath = QCoreApplication::applicationDirPath() + QString("/report/");

    QDir dir(savePath);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    const QString filename = ("E1_" + id + "_" + dateTimeString + ".pdf");
    printer.setOutputFileName(savePath + filename);
    ui->webView->print(&printer);

    QFileInfo checkFile(savePath + filename);
    if (checkFile.exists() && checkFile.isFile()) {
        QMessageBox::information(this, QCoreApplication::applicationName(),
                                 QObject::tr("The file <b> ") + savePath + filename +
                                 QObject::tr("</b> was exported!"));
    } else {
        QMessageBox::critical(this, QCoreApplication::applicationName(),
                              QObject::tr("Sorry, error exporting the file <b>") + savePath + filename +
                              QObject::tr("</b> to your desktop."));
    }
}

void LogPreviewer::report() {
    const QUrl baseUrl = QUrl::fromLocalFile(QDir::current().absoluteFilePath("tmp.html"));
    QMap<QVariant, QVariantList>::Iterator it;
    QMap<QVariant, QVariantList> data;
    QString ht = QString();
    const QString heads1 = QObject::tr("Heads Up! ");
    const QString heads2 = QObject::tr("The following manual testing was not performed: ");

    ht =  "<!DOCTYPE html>";
    ht += "<html>";
    ht += "<head>";
    ht += "<style>";
    ht += "table, th, td {";
    ht += "    border: 1px solid black;";
    ht += "    border-collapse: collapse;";
    ht += "}";
    ht += "th, td {";
    ht += "    padding: 5px;";
    ht += "}";
    ht += "</style>";
    ht += "</head>";
    ht += "<body>";
    ht += "<img style='display: inline;' src='qrc:res/images/report/logo.jpg'/>";

    const QString h1 = QObject::tr("Test Result - ATM-E1");
    ht += "<h1 style='text-align: center;'>" + h1 + "</h1>";

    QString id = utils::OsInfo::getInstance()->printHardDiskSerial();
    this->m_dateTime = this->m_dateTime.currentDateTime();
    const QString dateTimeString = this->m_dateTime.toString("yyyy-MM-dd hh:mm:ss");

    if (!id.isEmpty()) {
        const QString h4 = QObject::tr("Date:");
        ht += "<h4 style='text-align: center;'>ID: " + id + " " + h4 + dateTimeString + "</h4>";
    } else {
        ht += "<h4 style='text-align: center;'>ID: 000000 Date: 0000-00-00 00:00:00</h4>";
    }

    ht += "<br>";

    //
    // Basic System Information.
    //
    {
        const QString h3 = QObject::tr("Operating System Information");

        ht += "<img style='display: inline;' src='qrc:res/images/atm-icon.png' height='60' width='60'/>";
        ht += "<h3 style='display: inline;'>" + h3 + "</h3>";
        ht += "<br><br>";

        // Get information.
        const QString osName = utils::OsInfo::getInstance()->printProductName();
        const QString kerType = utils::OsInfo::getInstance()->printKernelType();
        const QString kerVer = utils::OsInfo::getInstance()->printKernelVersion();
        const QString joinKer = kerType + "-" + kerVer;
        const QString cpu = utils::OsInfo::getInstance()->printCurrentCpuArchitecture();
        const QString build = utils::OsInfo::getInstance()->printBuildAbi();
        const QString mem = utils::OsInfo::getInstance()->printTotalMemory();
        const QList<QSerialPortInfo> serialList = utils::SerialPort::printAvaliablePorts();
        const int tserial = serialList.count();

        // Translate.
        const QString msg1 = QObject::tr("Name OS System");
        const QString msg2 = QObject::tr("Kernel OS");
        const QString msg3 = QObject::tr("CPU Architecture");
        const QString msg4 = QObject::tr("Build ABI");
        const QString msg5 = QObject::tr("Memory");
        const QString msg6 = QObject::tr("Serial Ports");

        ht += "<table style='width:100%'' align='center'>";
        ht += " <tr>";
        ht += "   <th><img src='qrc:res/images/report/os-win-logo.png' height='32' width='32'></img>" + msg1 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/kernel.png' height='32' width='32'></img>" + msg2 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/cpu.png' height='32' width='32'></img>" + msg3 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/build.jpg' height='32' width='32'></img>" + msg4 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/memory.png' height='32' width='32'></img>" + msg5 + "</th>";
        ht += "   <th><img src='qrc:res/images/serialport.png' height='32' width='32'></img>" + msg6 + "</th>";
        ht += " </tr>";
        ht += " <tr>";
        ht += "   <td>" + osName + "</td>";
        ht += "   <td>" + joinKer + "</td>";
        ht += "   <td>" + cpu + "</td>";
        ht += "   <td>" + build + "</td>";
        ht += "   <td>" + mem + "</td>";
        ht += "   <td>" + QString::number(tserial) + "</td>";
        ht += " </tr>";
        ht += "</table>";
        ht += "<br><br><br>";
    }

    //
    // Network Information.
    //
    {
        // Translate.
        const QString h3 = QObject::tr("Network Information");

        ht += "<img style='display: inline;' src='qrc:res/images/report/ethernet-card.png' height='60' width='60'/>";
        ht += "<h3 style='display: inline;'>" + h3 + "</h3>";
        ht += "<br><br>";

        bool net = utils::Tools::getInstance()->isConnectedToNetwork();
        int tNic = 0;
        QStringList netName;
        QStringList netMac;
        QStringList netIp;
        QStringList netMask;

        if (net) {
            tNic = utils::Tools::getInstance()->printNetFaceCount();
            netName = utils::Tools::getInstance()->printNetFaceName();
            netMac = utils::Tools::getInstance()->printNetMacAddresses();
            netIp = utils::Tools::getInstance()->printNetIpAddresses();
            netMask = utils::Tools::getInstance()->printNetMaskAddresses();
        } else {
            netName << "0";
            netMac << "0";
            netIp << "0";
            netMask << "0";
        }

        // Translate.
        const QString msg1 = QObject::tr("NIC Internal ID");
        const QString msg2 = QObject::tr("Mac Address");
        const QString msg3 = QObject::tr("IP Address");
        const QString msg4 = QObject::tr("Mask Address");
        const QString msg5 = QObject::tr("Status");

        // Get information.
        ht += "<table style='width:100%'' align='center'>";
        ht += " <tr>";
        ht += "   <th><img src='qrc:res/images/report/id-network.png' height='32' width='32'></img>" + msg1 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/net-mac.png' height='32' width='32'></img>" + msg2 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/ip-connection.png' height='32' width='32'></img>" + msg3 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/net-mask.png' height='32' width='32'></img>" + msg4 + "</th>";
        ht += "   <th><img src='qrc:res/images/report/net-status.png' height='32' width='32'></img>" + msg5 + "</th>";
        ht += " </tr>";

        if (tNic >= 1) {
            ht += " <tr>";
            ht += "   <td>" + netName.last() + "</td>";
            ht += "   <td>" + netMac.last() + "</td>";
            ht += "   <td>" + netIp.last() + "</td>";
            ht += "   <td>" + netMask.last() + "</td>";
            if (net) {
                ht += "    <td><font color=\"green\">CONNECTED</font></td>";
            } else {
                ht += "    <td><font color=\"red\">DISCONNECTED</font></td>";
            }
            ht += " </tr>";
        }
        ht += "</table>";
        ht += "<br><br><br>";
    }

    //
    // Multi/IO - Auto
    //
    {
        data.clear();
        data = atm::E1::printLogAutoMultiIo();
        QVariantList logMultiIo;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logMultiIo.append(it.value());
            }

            QString ver = QString::fromStdString(devices::multiio::AbstractLed10v2::getInstance()->printVersion());
            QString port =  AtmModelSelect::printPortMultiIO();
            QString elapsedtimer = QString();

            // Translate.
            const QString h3 = QObject::tr("Device: Multi/IO");
            const QString h4 = QObject::tr("Model: ") + ver + " " + QObject::tr("Port: ") + port;

            ht += "<img style='display: inline;' src='qrc:res/images/multiio-empresa1.png' ";
            ht += "alt='Multi/IO Logo' height='60' width='60'/>";
            ht += "<h3 style='display: inline;'>" + h3 + " </h3>";
            ht += "<h4 style='display: inline;'>" + h4 + "</h4>";
            ht += "<br><br>";

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Volts");
            const QString msg5 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/volts.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg5 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Automatic");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "open+AUTO+ELAPSEDTIMER=", "open+AUTO+ELAPSEDTIMER=");
            if (logMultiIo.contains("open+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("open+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+AUTO+PRINTER+ELAPSEDTIMER=",
                                                    "powerOn+AUTO+PRINTER+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+AUTO+PRINTER=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(PRINTER) P1</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+AUTO+PRINTER=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(PRINTER-P1)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(PRINTER)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+AUTO+COINVALIDATOR+ELAPSEDTIMER=",
                                                    "powerOn+AUTO+COINVALIDATOR+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+AUTO+COINVALIDATOR=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(COINVALIDATOR-P3)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>12V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+AUTO+COINVALIDATOR=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(COINVALIDATOR-P3)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>12V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(COINVALIDATOR)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+AUTO+BILLVALIDATOR+ELAPSEDTIMER=",
                                                    "powerOn+AUTO+BILLVALIDATOR+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+AUTO+BILLVALIDATOR=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(BILLVALIDATOR-P9)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+AUTO+BILLVALIDATOR=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(BILLVALIDATOR-P9)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(BILLVALIDATOR)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+AUTO+DISPENSER+ELAPSEDTIMER=",
                                                    "powerOn+AUTO+DISPENSER+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+AUTO+DISPENSER=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(DISPENSER-P11)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+AUTO+DISPENSER=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(DISPENSER-P11)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(DISPENSER)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "close+AUTO+ELAPSEDTIMER=", "close+AUTO+ELAPSEDTIMER=");
            if (logMultiIo.contains("close+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("close+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Multi/IO - Manual
    //
    {
        data.clear();
        data = atm::E1::printLogManualMultiIo();
        QVariantList logMultiIo;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logMultiIo.append(it.value());
            }

            QString elapsedtimer = QString();
            QString notested = QString();

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Volts");
            const QString msg5 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/volts.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg5 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Manual");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "open+MANUAL+ELAPSEDTIMER=", "open+MANUAL+ELAPSEDTIMER=");
            if (logMultiIo.contains("open+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("open+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "open ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+MANUAL+PRINTER+ELAPSEDTIMER=",
                                                    "powerOn+MANUAL+PRINTER+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+MANUAL+PRINTER=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(PRINTER) P1</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+MANUAL+PRINTER=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(PRINTER) P1</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOn(PRINTER) P1 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(PRINTER)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOff+MANUAL+PRINTER+ELAPSEDTIMER=",
                                                    "powerOff+MANUAL+PRINTER+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOff+MANUAL+PRINTER=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOff(PRINTER) P1</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOff+MANUAL+PRINTER=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOff(PRINTER) P1</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOff(PRINTER) P1 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOff(PRINTER)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+MANUAL+COINVALIDATOR+ELAPSEDTIMER=",
                                                    "powerOn+MANUAL+COINVALIDATOR+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+MANUAL+COINVALIDATOR=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(COINVALIDATOR) P3</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>12V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+MANUAL+COINVALIDATOR=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(COINVALIDATOR) P3</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>12V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOn(COINVALIDATOR) P3 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(COINVALIDATOR)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOff+MANUAL+COINVALIDATOR+ELAPSEDTIMER=",
                                                    "powerOff+MANUAL+COINVALIDATOR+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOff+MANUAL+COINVALIDATOR=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOff(COINVALIDATOR) P3</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>12V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOff+MANUAL+COINVALIDATOR=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(COINVALIDATOR) P3</td>";                
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>12V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOff(COINVALIDATOR) P3 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOff(COINVALIDATOR)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+MANUAL+BILLVALIDATOR+ELAPSEDTIMER=",
                                                    "powerOn+MANUAL+BILLVALIDATOR+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+MANUAL+BILLVALIDATOR=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(BILLVALIDATOR) P9</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+MANUAL+BILLVALIDATOR=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(BILLVALIDATOR) P9</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOn(BILLVALIDATOR) P9 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(BILLVALIDATOR)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOff+MANUAL+BILLVALIDATOR+ELAPSEDTIMER=",
                                                    "powerOff+MANUAL+BILLVALIDATOR+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+MANUAL+BILLVALIDATOR=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOff(BILLVALIDATOR) P9</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+MANUAL+BILLVALIDATOR=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOff(BILLVALIDATOR) P9</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOff(BILLVALIDATOR) P9 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOff(BILLVALIDATOR)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOn+MANUAL+DISPENSER+ELAPSEDTIMER=",
                                                    "powerOn+MANUAL+DISPENSER+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOn+MANUAL+DISPENSER=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(DISPENSER) P11</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOn+MANUAL+DISPENSER=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOn(DISPENSER) P11)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOn(DISPENSER) P11 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOn(DISPENSER)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "powerOff+MANUAL+DISPENSER+ELAPSEDTIMER=",
                                                    "powerOff+MANUAL+DISPENSER+ELAPSEDTIMER=");
            if (logMultiIo.contains("powerOff+MANUAL+DISPENSER=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOff(DISPENSER-P11)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("powerOff+MANUAL+DISPENSER=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>powerOff(DISPENSER-P11)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>24V</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "powerOff(DISPENSER) P11 ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (powerOff(DISPENSER)) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logMultiIo, "close+MANUAL+ELAPSEDTIMER=", "close+MANUAL+ELAPSEDTIMER=");
            if (logMultiIo.contains("close+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logMultiIo.contains("close+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>Manual</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td>--</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "close ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            if (!notested.isEmpty()) {
                ht += "<br><b><font color=\"red\">" + heads1 + "</font></b>";
                ht += heads2 + notested.toUpper() + ".";
            }
            ht += "<br><br>";
            ht += "<hr width='80%'>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Dispenser - Auto
    //
    {
        data.clear();
        data = atm::E1::printLogAutoDispenser();
        QVariantList logDispenser;        

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logDispenser.append(it.value());
            }

            QString ver = QString::fromStdString(devices::dispenser::AbstractCrt591::getInstance()->printVersion());
            QString port =  AtmModelSelect::printPortDispenser();
            QString elapsedtimer = QString();

            // Translate.
            const QString h3 = QObject::tr("Device: Dispenser");
            const QString h4 = QObject::tr("Model: ") + ver + " " +  QObject::tr("Port: ") + port;

            ht += "<img style='display: inline;' src='qrc:res/images/dispenser-empresa1.png' ";
            ht += "alt='Multi/IO Logo' height='60' width='60'/>";
            ht += "<h3 style='display: inline;'>" + h3 + " </h3>";
            ht += "<h4 style='display: inline;'>" + h4 + "</h4>";
            ht += "<br><br>";

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Automatic");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "open+AUTO+ELAPSEDTIMER=", "open+AUTO+ELAPSEDTIMER=");
            if (logDispenser.contains("open+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("open+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "init+AUTO+ELAPSEDTIMER=", "init+AUTO+ELAPSEDTIMER=");
            if (logDispenser.contains("init+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>init</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("init+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>init</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (init) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "cardStatus+AUTO+ELAPSEDTIMER=", "cardStatus+AUTO+ELAPSEDTIMER=");
            if (logDispenser.contains("cardStatus+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardStatus</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("cardStatus+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardStatus</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (cardStatus) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "sensorStatus+AUTO+ELAPSEDTIMER=", "sensorStatus+AUTO+ELAPSEDTIMER=");
            if (logDispenser.contains("sensorStatus+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>sensorStatus</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("sensorStatus+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>sensorStatus</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (sensorsStatus) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "reset+AUTO+ELAPSEDTIMER=", "reset+AUTO+ELAPSEDTIMER=");
            if (logDispenser.contains("reset+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("reset+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (reset) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "close+AUTO+ELAPSEDTIMER=", "close+AUTO+ELAPSEDTIMER=");
            if (logDispenser.contains("close+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("close+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Dispenser - Manual
    //
    {
        data.clear();
        data = atm::E1::printLogManualDispenser();
        QVariantList logDispenser;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logDispenser.append(it.value());
            }

            QString elapsedtimer = QString();
            QString notested = QString();

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Manual");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "open+MANUAL+ELAPSEDTIMER=", "open+MANUAL+ELAPSEDTIMER=");
            if (logDispenser.contains("open+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("open+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "open ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "close+MANUAL+ELAPSEDTIMER=", "close+MANUAL+ELAPSEDTIMER=");
            if (logDispenser.contains("close+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("close+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "close ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "cardPositionChanged+MANUAL+ELAPSEDTIMER=",
                                                    "cardPositionChanged+MANUAL+ELAPSEDTIMER=");
            if (logDispenser.contains("cardPositionChanged(READING)+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardPositionChanged(READING)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("cardPositionChanged(READING)+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardPositionChanged(READING)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "cardPositionChanged(READING) ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (cardPositionChanged) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "cardPositionChanged+MANUAL+ELAPSEDTIMER=",
                                                    "cardPositionChanged+MANUAL+ELAPSEDTIMER=");
            if (logDispenser.contains("cardPositionChanged(EJECT)+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardPositionChanged(EJECT)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("cardPositionChanged(EJECT)+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardPositionChanged(EJECT)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "cardPositionChanged(EJECT) ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (cardPositionChanged) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "cardPositionChanged+MANUAL+ELAPSEDTIMER=",
                                                    "cardPositionChanged+MANUAL+ELAPSEDTIMER=");
            if (logDispenser.contains("cardPositionChanged(REJECT)+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardPositionChanged(REJECT)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("cardPositionChanged(REJECT)+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardPositionChanged(REJECT)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "cardPositionChanged(REJECT) ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (cardPositionChanged) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "readSerialCardNumber+MANUAL+ELAPSEDTIMER=",
                                                    "readSerialCardNumber+MANUAL+ELAPSEDTIMER=");
            if (logDispenser.contains("readSerialCardNumber+MANUAL=PASSED")) {
                QString str =  utils::Tools::getPartStr(logDispenser, "readSerialCardNumber+MANUAL+VALUE=",
                                                        "readSerialCardNumber+MANUAL+VALUE=");
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>readSerialNumber(" + str + ")</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("readSerialCardNumber+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>readSerialNumber(ERROR)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "readSerialCardNumber ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (readSerialCardNumber) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logDispenser, "sendAPDUSamCard+MANUAL+ELAPSEDTIMER=",
                                                    "sendAPDUSamCard+MANUAL+ELAPSEDTIMER=");
            if (logDispenser.contains("sendAPDUSamCard+MANUAL=PASSED")) {
                QString str =  utils::Tools::getPartStr(logDispenser, "sendAPDUSamCard+MANUAL+VALUE=",
                                                        "sendAPDUSamCard+MANUAL+VALUE=");
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>sendAPDUSamCard(" + str + ")</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logDispenser.contains("sendAPDUSamCard+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>sendAPDUSamCard(ERROR)</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "sendAPDUSamCard ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (sendAPDUSamCard) command.";
            }

            ht += "</table>";
            if (!notested.isEmpty()) {
                ht += "<br><b><font color=\"red\">" + heads1 + "</font></b>";
                ht += heads2 + notested.toUpper() + ".";
            }
            ht += "<br><br>";
            ht += "<hr width='80%'>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Bill Validator - Auto
    //
    {
        data.clear();
        data = atm::E1::printLogAutoBillValidator();
        QVariantList logBillValidator;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logBillValidator.append(it.value());
            }

            QString ver = QString::fromStdString(devices::billvalidator::AbstractCashCode609::getInstance()->printVersion());
            QString port =  AtmModelSelect::printPortBillValidator();
            QString elapsedtimer = QString();

            // Translate.
            const QString h3 = QObject::tr("Device: BillValidator");
            const QString h4 = QObject::tr("Model: ") + ver + " " + QObject::tr("Port: ") + port;

            ht += "<img style='display: inline;' src='qrc:res/images/billvalidator-empresa1.png' ";
            ht += "alt='Multi/IO Logo' height='60' width='60'/>";
            ht += "<h3 style='display: inline;'>" + h3 + " </h3>";
            ht += "<h4 style='display: inline;'>" + h4 + "</h4>";
            ht += "<br><br>";

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Automatic");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "open+AUTO+ELAPSEDTIMER=", "open+AUTO+ELAPSEDTIMER=");
            if (logBillValidator.contains("open+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("open+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "reset+AUTO+ELAPSEDTIMER=", "reset+AUTO+ELAPSEDTIMER=");
            if (logBillValidator.contains("reset+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + " milliseconds</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("reset+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (reset) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "version+AUTO+ELAPSEDTIMER=", "version+AUTO+ELAPSEDTIMER=");
            if (logBillValidator.contains("version+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("version+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (version) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "close+AUTO+ELAPSEDTIMER=", "close+AUTO+ELAPSEDTIMER=");
            if (logBillValidator.contains("close+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("close+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // BillValidador - Manual.
    //
    {
        data.clear();
        data = atm::E1::printLogManualBillValidator();
        QVariantList logBillValidator;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logBillValidator.append(it.value());
            }

            QString elapsedtimer = QString();
            QString notested = QString();

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Manual");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "open+MANUAL+ELAPSEDTIMER=", "open+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("open+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("open+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "open ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "close+MANUAL+ELAPSEDTIMER=", "close+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("close+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("close+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "close ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "reset+MANUAL+ELAPSEDTIMER=", "reset+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("reset+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("reset+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "reset ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (reset) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "poll+MANUAL+ELAPSEDTIMER=", "poll+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("poll+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>poll</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("poll+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>poll</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "poll ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (poll) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "stackBill+MANUAL+ELAPSEDTIMER=",
                                                    "stackBill+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("stackBill+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>stackBill</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("stackBill+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>stackBill</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "stackBill ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (stackBill) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "returnBill+MANUAL+ELAPSEDTIMER=",
                                                    "returnBill+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("returnBill+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>returnBill</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("returnBill+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>returnBill</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "returnBill ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (returnBill) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "billTables+MANUAL+ELAPSEDTIMER=",
                                                    "billTables+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("billTables+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>billTables</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("billTables+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>billTables</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "billTables ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (billTables) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logBillValidator, "version+MANUAL+ELAPSEDTIMER=",
                                                    "version+MANUAL+ELAPSEDTIMER=");
            if (logBillValidator.contains("version+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logBillValidator.contains("version+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "version ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (version) command.";
            }

            ht += "</table>";
            if (!notested.isEmpty()) {
                ht += "<br><b><font color=\"red\">" + heads1 + "</font></b>";
                ht += heads2 + notested.toUpper() + ".";
            }
            ht += "<br><br>";
            ht += "<hr width='80%'>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Coin Validator - Auto
    //
    {
        data.clear();
        data = atm::E1::printLogAutoCoinValidator();
        QVariantList logCoinValidator;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logCoinValidator.append(it.value());
            }

            QString ver = "MEI";
            QString port =  AtmModelSelect::printPortCoinValidator();
            QString elapsedtimer = QString();

            // Translate.
            const QString h3 = QObject::tr("Device: CoinValidator");
            const QString h4 = QObject::tr("Model: ") + ver + " " + QObject::tr("Port: ") + port;

            ht += "<img style='display: inline;' src='qrc:res/images/coinvalidator-empresa1.png' ";
            ht += "alt='Multi/IO Logo' height='60' width='60'/>";
            ht += "<h3 style='display: inline;'>" + h3 + " </h3>";
            ht += "<h4 style='display: inline;'>" + h4 + "</h4>";
            ht += "<br><br>";

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Automatic");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "open+AUTO+ELAPSEDTIMER=", "open+AUTO+ELAPSEDTIMER=");
            if (logCoinValidator.contains("open+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("open+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "reset+AUTO+ELAPSEDTIMER=", "reset+AUTO+ELAPSEDTIMER=");
            if (logCoinValidator.contains("reset+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("reset+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (reset) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "version+AUTO+ELAPSEDTIMER=", "version+AUTO+ELAPSEDTIMER=");
            if (logCoinValidator.contains("version+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("version+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (version) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "close+AUTO+ELAPSEDTIMER=", "close+AUTO+ELAPSEDTIMER=");
            if (logCoinValidator.contains("close+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("close+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // CoinValidator - Manual
    //
    {
        data.clear();
        data = atm::E1::printLogManualCoinValidator();
        QVariantList logCoinValidator;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logCoinValidator.append(it.value());
            }

            QString elapsedtimer = QString();
            QString notested = QString();

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Manual");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "open+MANUAL+ELAPSEDTIMER=", "open+MANUAL+ELAPSEDTIMER=");
            if (logCoinValidator.contains("open+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("open+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "open ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "reset+MANUAL+ELAPSEDTIMER=", "reset+MANUAL+ELAPSEDTIMER=");
            if (logCoinValidator.contains("reset+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("reset+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>reset</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "reset ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (reset) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "billTables+MANUAL+ELAPSEDTIMER=", "billTables+MANUAL+ELAPSEDTIMER=");
            if (logCoinValidator.contains("billTables+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>billTables</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("billTables+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>billTables</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "billTables ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (billTables) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "version+MANUAL+ELAPSEDTIMER=", "version+MANUAL+ELAPSEDTIMER=");
            if (logCoinValidator.contains("version+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("version+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>version</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "version ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (version) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "poll+MANUAL+ELAPSEDTIMER=", "poll+MANUAL+ELAPSEDTIMER=");
            if (logCoinValidator.contains("poll+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>poll</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"gree\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("poll+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>poll</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "poll ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (poll) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logCoinValidator, "close+MANUAL+ELAPSEDTIMER=", "close+MANUAL+ELAPSEDTIMER=");
            if (logCoinValidator.contains("close+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCoinValidator.contains("close+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "close ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            if (!notested.isEmpty()) {
                ht += "<br><b><font color=\"red\">" + heads1 + "</font></b>";
                ht += heads2 + notested.toUpper() + ".";
            }
            ht += "<br><br>";
            ht += "<hr width='80%'>";
            ht += "<br><br>";

        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Printer - Auto
    //
    {
        data.clear();
        data = atm::E1::printLogAutoPrinter();
        QVariantList logPrinter;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logPrinter.append(it.value());
            }

            QString ver = "EPSON-T500";
            QString port = AtmModelSelect::printPortPrinter();
            QString elapsedtimer = QString();

            // Translate.
            const QString h3 = QObject::tr("Device: Printer");
            const QString h4 = QObject::tr("Model: ") + ver + " " + QObject::tr("Port: ") + port;

            ht += "<img style='display: inline;' src='qrc:res/images/printer-empresa1.png' ";
            ht += "alt='Multi/IO Logo' height='60' width='60'/>";
            ht += "<h3 style='display: inline;'>" + h3 + " </h3>";
            ht += "<h4 style='display: inline;'>" + h4 + "</h4>";
            ht += "<br><br>";

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Automatic");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logPrinter, "showPrinter+AUTO+ELAPSEDTIMER=", "showPrinter+AUTO+ELAPSEDTIMER=");
            if (logPrinter.contains("showPrinter+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>showPrinter</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logPrinter.contains("showPrinter+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>showPrinter</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (showPrinter) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logPrinter, "info+AUTO+ELAPSEDTIMER=", "info+AUTO+ELAPSEDTIMER=");
            if (logPrinter.contains("info+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>info</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logPrinter.contains("info+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>info</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (info) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logPrinter, "testPage+AUTO+ELAPSEDTIMER=", "testPage+AUTO+ELAPSEDTIMER=");
            if (logPrinter.contains("testPage+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>testPage</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logPrinter.contains("testPage+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>testPage</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (testPage) command.";
            }

            ht += "</table>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Printer - Manual
    //
    {
        data.clear();
        data = atm::E1::printLogManualPrinter();
        QVariantList logPrinter;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logPrinter.append(it.value());
            }

            QString elapsedtimer = QString();
            QString notested = QString();

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Manual");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logPrinter, "printerSelected+MANUAL+ELAPSEDTIMER=", "printerSelected+MANUAL+ELAPSEDTIMER=");
            if (logPrinter.contains("printerSelected+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>printerSelected</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logPrinter.contains("printerSelected+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>printerSelected</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "open ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (printerSelected) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logPrinter, "testPage+MANUAL+ELAPSEDTIMER=", "testPage+MANUAL+ELAPSEDTIMER=");
            if (logPrinter.contains("testPage+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>testPage</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logPrinter.contains("testPage+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>testPage</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "testPage ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (testPage) command.";
            }

            ht += "</table>";
            if (!notested.isEmpty()) {
                ht += "<br><b><font color=\"red\">" + heads1 + "</font></b>";
                ht += heads2 + notested.toUpper() + ".";
            }
            ht += "<br><br>";
            ht += "<hr width='80%'>";
            ht += "<br><br>";

        } else {
            qDebug() << "Error...";
        }
    }


    //
    // SmartCard Reader - Auto
    //
    {
        data.clear();
        data = atm::E1::printLogAutoSmartCard();
        QVariantList logSmartCard;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logSmartCard.append(it.value());
            }

            QString ver = "CRT-603-CZ1";
            QString port = AtmModelSelect::printPortPrinter();
            QString elapsedtimer = QString();

            // Translate.
            const QString h3 = QObject::tr("Device: SmartCard");
            const QString h4 = QObject::tr("Model: ") + ver + " " +  QObject::tr("Port: ") + port;

            ht += "<img style='display: inline;' src='qrc:res/images/cardreader-empresa1.png' ";
            ht += "alt='Multi/IO Logo' height='60' width='60'/>";
            ht += "<h3 style='display: inline;'>" + h3 + " </h3>";
            ht += "<h4 style='display: inline;'>" + h4 + "</h4>";
            ht += "<br><br>";

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Automatic");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logSmartCard, "open+AUTO+ELAPSEDTIMER=", "open+AUTO+ELAPSEDTIMER=");
            if (logSmartCard.contains("open+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logSmartCard.contains("open+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logSmartCard, "cardStatus+AUTO+ELAPSEDTIMER=", "cardStatus+AUTO+ELAPSEDTIMER=");
            if (logSmartCard.contains("cardStatus+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardStatus</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logSmartCard.contains("cardStatus+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cardStatus</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (cardStatus) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logSmartCard, "close+AUTO+ELAPSEDTIMER=", "close+AUTO+ELAPSEDTIMER=");
            if (logSmartCard.contains("close+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logSmartCard.contains("close+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            ht += "<br><br>";
        } else {
            qDebug() << "Error...";
        }
    }

    //
    // SmartCard Reader - Manual
    //
    {
        data.clear();
        data = atm::E1::printLogManualSmartCard();
        QVariantList logSmartCard;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logSmartCard.append(it.value());
            }

            QString elapsedtimer = QString();
            QString notested = QString();

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Manual");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logSmartCard, "open+MANUAL+ELAPSEDTIMER=", "open+MANUAL+ELAPSEDTIMER=");
            if (logSmartCard.contains("open+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logSmartCard.contains("open+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>open</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "open ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (open) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logSmartCard, "readSerialCardNumber+MANUAL+ELAPSEDTIMER=",
                                                    "readSerialCardNumber+MANUAL+ELAPSEDTIMER=");
            if (logSmartCard.contains("readSerialCardNumber+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>readSerialCardNumber</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logSmartCard.contains("readSerialCardNumber+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>readSerialCardNumber</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "readSerialCardNumber ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (readSerialCardNumber) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logSmartCard, "sendAPDUSamCard+MANUAL+ELAPSEDTIMER=",
                                                    "sendAPDUSamCard+MANUAL+ELAPSEDTIMER=");
            if (logSmartCard.contains("sendAPDUSamCard+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>sendAPDUSamCard</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logSmartCard.contains("sendAPDUSamCard+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>sendAPDUSamCard</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "sendAPDUSamCard ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (sendAPDUSamCard) command.";
            }

            elapsedtimer = utils::Tools::getPartStr(logSmartCard, "close+MANUAL+ELAPSEDTIMER=", "close+MANUAL+ELAPSEDTIMER=");
            if (logSmartCard.contains("close+MANUAL=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logSmartCard.contains("close+MANUAL=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>close</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                notested += "close ";
                QLOG_ERROR() << "LogPreviewer: Does not contain (close) command.";
            }

            ht += "</table>";
            if (!notested.isEmpty()) {
                ht += "<br><b><font color=\"red\">" + heads1 + "</font></b>";
                ht += heads2 + notested.toUpper() + ".";
            }
            ht += "<br><br>";
            ht += "<hr width='80%'>";
            ht += "<br><br>";

        } else {
            qDebug() << "Error...";
        }
    }

    //
    // Camera - Auto
    //
    {
        data.clear();
        data = atm::E1::printLogAutoCamera();
        QVariantList logCamera;

        if (!data.isEmpty()) {
            for (it = data.begin(); it != data.end(); it++) {
                logCamera.append(it.value());
            }

            QString ver = "USB/Generic";
            QString port = "";
            QString elapsedtimer = QString();

            // Translate.
            const QString h3 = QObject::tr("Device: Camera");
            const QString h4 = QObject::tr("Model: ") + ver + " " + QObject::tr("Port: ") + port;

            ht += "<img style='display: inline;' src='qrc:res/images/camera-empresa1.png' ";
            ht += "alt='Multi/IO Logo' height='60' width='60'/>";
            ht += "<h3 style='display: inline;'>" + h3 + " </h3>";
            ht += "<h4 style='display: inline;'>" + h4 + "</h4>";
            ht += "<br><br>";

            // Translate.
            const QString msg1 = QObject::tr("Type Test");
            const QString msg2 = QObject::tr("Command");
            const QString msg3 = QObject::tr("Execution Time");
            const QString msg4 = QObject::tr("Result");

            ht += "<table style='width:100%' align='center'>";
            ht += " <tr>";
            ht += "    <th><img src='qrc:res/images/report/test-icon.jpg' height='32' width='32'></img>" + msg1 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/command.png' height='32' width='32'></img>" + msg2 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/execution.png' height='32' width='32'></img>" + msg3 + "</th>";
            ht += "    <th><img src='qrc:res/images/report/test-icon-check.png' height='32' width='32'></img>" + msg4 + "</th>";
            ht += " </tr>";

            // Translate.
            const QString t1 = QObject::tr("Automatic");
            const QString t2 = QObject::tr(" milliseconds");
            const QString t3 = QObject::tr("PASSED");
            const QString t4 = QObject::tr("FAILED");

            elapsedtimer = utils::Tools::getPartStr(logCamera, "cameraStarted+AUTO+ELAPSEDTIMER=",
                                                    "cameraStarted+AUTO+ELAPSEDTIMER=");
            if (logCamera.contains("cameraStarted+AUTO=PASSED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cameraStarted</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"green\">" + t3 + "</font></td>";
                ht += "</tr>";
            } else if(logCamera.contains("cameraStarted+AUTO=FAILED")) {
                ht += "<tr>";
                ht += "    <td>" + t1 + "</td>";
                ht += "    <td>cameraStarted</td>";
                ht += "    <td>" + elapsedtimer + t2 + "</td>";
                ht += "    <td><font color=\"red\">" + t4 + "</font></td>";
                ht += "</tr>";
            } else {
                QLOG_ERROR() << "LogPreviewer: Does not contain (cameraStarted) command.";
            }

            ht += "</table>";
            ht += "<br><br>";

        } else {
            qDebug() << "Error...";
        }
    }

    ht += "<body></html>";
    ui->webView->setHtml(ht, baseUrl);
}
