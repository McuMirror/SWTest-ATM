/**
 * @file printerfit.cpp
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

#include <printerfit.hpp>
#include "ui_printerfit.h"

#include <atm/e1.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QtWidgets>
#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QPainter>
#include <QTextEdit>
#include <QMessageBox>

PrinterFit::PrinterFit(QWidget *parent)
    : QWidget(parent), ui(new Ui::PrinterFit) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Load printes.
    ui->comboBoxPrinters->addItems(QPrinterInfo::availablePrinterNames());
    ui->comboBoxPrinters->setCurrentIndex(ui->comboBoxPrinters->findData("EPSON EU-T500 Receipt", Qt::DisplayRole));

    // Signals and slots.
    QObject::connect(ui->comboBoxPrinters,SIGNAL(currentIndexChanged(int)),
                     this, SLOT(getInfo()));
    QObject::connect(ui->pushButtonPrintSetup, SIGNAL(clicked()),
                     this, SLOT(handlePrinterSetup()));


    // Initially display information for first printer.
    this->getInfo();
}

PrinterFit::~PrinterFit() {
    delete ui;
}

void PrinterFit::closeEvent(QCloseEvent *event) {
    QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    event->accept();
}

void PrinterFit::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
    }
    QWidget::changeEvent(event);
}

void PrinterFit::getInfo() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    QString name = ui->comboBoxPrinters->currentText();
    QPrinterInfo info = QPrinterInfo::printerInfo(name);

    if (info.isNull()) {
        atm::E1::insertLogManualPrinter("printerSelected+MANUAL=FAILED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualPrinter("printerSelected+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
        return;
    }

    atm::E1::insertLogManualPrinter("printerSelected+MANUAL=PASSED");
    ui->labelDescription->setText(QObject::tr("Description: %1").arg(info.description()));
    ui->labelDefaultPrinter->setText(QObject::tr("Default Printer: %1").arg(info.isDefault() ? QObject::tr("Yes") : QObject::tr("No")));
    ui->labelIsNull->setText(QObject::tr("Is Null: %1").arg(info.isNull() ? QObject::tr("Yes") : QObject::tr("No")));
    ui->labelIsRemote->setText(QObject::tr("Is Remote: %1").arg(info.isRemote() ? QObject::tr("Yes") : QObject::tr("No")));
    ui->labelLocation->setText(QObject::tr("Location: %1").arg(info.location()));
    ui->labelMakeAndModel->setText(QObject::tr("Make and Model: %1").arg(info.makeAndModel()));
    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualPrinter("printerSelected+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

    // Get state.
    switch (info.state()) {
    case QPrinter::Idle:
        ui->labelState->setText(QObject::tr("State: %1").arg(QObject::tr("Idle")));
        break;
    case QPrinter::Active:
        ui->labelState->setText(QObject::tr("State: %1").arg(QObject::tr("Active")));
        break;
    case QPrinter::Aborted:
        ui->labelState->setText(QObject::tr("State: %1").arg(QObject::tr("Aborted")));
        break;
    case QPrinter::Error:
        ui->labelState->setText(QObject::tr("State: %1").arg(QObject::tr("Error")));
        break;
    default:
        ui->labelState->setText(QObject::tr("State: %1").arg(QObject::tr("Unknow")));
        break;
    }

    ui->labelDefaultPageSize->setText(QObject::tr("Default Page: %1").arg(info.defaultPageSize().name()));
    ui->labelMinimumPageSize->setText(QObject::tr("Minimum Page Size: %1").arg(info.minimumPhysicalPageSize().name()));
    ui->labelMaximumPageSize->setText(QObject::tr("Maximum Page Size: %1").arg(info.maximumPhysicalPageSize().name()));
    ui->labelCustomPageSizes->setText(QObject::tr("Support Custom Pages Size: %1").arg(info.supportsCustomPageSizes() ? QObject::tr("Yes") : QObject::tr("No")));

    // We fill in the combox box with a description of the screen resolutions.
    ui->comboBoxResolutions->clear();
    foreach (int res, info.supportedResolutions()) {
        ui->comboBoxResolutions->addItem(QString::number(res) + QObject::tr(" dpi"));
    }

    // We do the same for the page sizes.
    ui->comboBoxPageSizes->clear();
    foreach (QPageSize size, info.supportedPageSizes()) {
        ui->comboBoxPageSizes->addItem(size.name());
    }
}

void PrinterFit::handlePrinterSetup() {
    int elapsedtimer = 0;
    utils::Tools::getInstance()->startElapsedTimer();

    QString text = "<html>";
            text +="<head>";
            text += "<title>Test Page - Printer</title>";
            text +="</head>";
            text += "<body>";
            text += "<img src='logo-top-printer.png'</img>";
            text += "<h1>SELF-TEST</h1>";
            text += "<hr>";
            text += "<h2>Configurations</h2>";
            text += "Name Printer: " + ui->comboBoxPrinters->currentText() + "<br>";
            text += ui->labelDescription->text() + "<br>";
            text += ui->labelIsNull->text() + "<br>";
            text += ui->labelIsRemote->text() + "<br>";
            text += ui->labelDefaultPrinter->text() + "<br>";
            text += ui->labelLocation->text() + "<br>";
            text += ui->labelMakeAndModel->text() + "<br>";
            text += ui->labelState->text().toUpper() + "<br>";
            text += ui->labelDefaultPageSize->text() + "<br>";
            text += ui->labelMinimumPageSize->text() + "<br>";
            text += ui->labelMaximumPageSize->text() + "<br>";
            text += ui->labelCustomPageSizes->text() + "<br>";
            text += "DPI Resolutions: </br>" + ui->comboBoxResolutions->currentText() + "<br>";
            text += "Page size:" + ui->comboBoxPageSizes->currentText() + "<br>";
            text += "<h2>Draw Ascii</h2>";
            text += "<hr>";
            text += "</body>";
            text += "</html>";

    QPrinter printer(QPrinter::HighResolution);
    const QString htmlText(text);
    QPrintDialog printDialog(&printer, this);
    if (printDialog.exec()) {
        atm::E1::insertLogManualPrinter("testPage+MANUAL=PASSED");
        elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
        atm::E1::insertLogManualPrinter("testPage+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));

        //printer.setFullPage(true);
        QTextDocument textDocument;
        textDocument.setHtml(htmlText);
        textDocument.print(&printer);
        return;
    }

    atm::E1::insertLogManualPrinter("testPage+MANUAL=FAILED");
    elapsedtimer = utils::Tools::getInstance()->printElapsedTimer();
    atm::E1::insertLogManualPrinter("testPage+MANUAL+ELAPSEDTIMER=" + QString::number(elapsedtimer));
}
