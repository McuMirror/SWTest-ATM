/**
 * @file mainwindow.cpp
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

#include <mainwindow.hpp>
#include "ui_mainwindow.h"

#include <atmmodelselect.hpp>
#include <atmfit.hpp>
#include <about.hpp>
#include <billvalidatorfit.hpp>
#include <coinvalidatorfit.hpp>
#include <camerafit.hpp>
#include <console.hpp>
#include <dispenserfit.hpp>
#include <logpreviewer.hpp>
#include <multiiofit.hpp>
#include <printerfit.hpp>
#include <smartcardreaderfit.hpp>
#include <serialloopbackfit.hpp>
#include <utils/log.hpp>
#include <utils/tools.hpp>

#include <QtWidgets>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent), ui(new Ui::MainWindow) {

    // First log class.
    QLOG_INFO() << "Constructor class:" << this->metaObject()->className();

    // Load ui.
    ui->setupUi(this);

    // Instance objects.
    this->m_atmModelSelect = new AtmModelSelect(0);
    this->m_console = new Console(0);

    // Instance objects (devices).
    this->m_fitWidgetItems.atmFit = new AtmFit(this);
    this->m_fitWidgetItems.dispenserFit = new DispenserFit(this);
    this->m_fitWidgetItems.billValidatorFit = new BillValidatorFit(this);
    this->m_fitWidgetItems.printerFit = new PrinterFit(this);
    this->m_fitWidgetItems.coinValidatorFit = new CoinValidatorFit(this);
    this->m_fitWidgetItems.smartCardReaderFit = new SmartCardReaderFit(this);
    this->m_fitWidgetItems.multiIoFit = new MultiIoFit(this);
    this->m_fitWidgetItems.cameraFit = new CameraFit(this);
    this->m_fitWidgetItems.serialLoopBackFit = new SerialLoopBackFit(this);

    // Hide objects.
    this->m_atmModelSelect->hide();
    this->m_console->hide();
    this->m_fitWidgetItems.atmFit->hide();
    this->m_fitWidgetItems.dispenserFit->hide();
    this->m_fitWidgetItems.billValidatorFit->hide();
    this->m_fitWidgetItems.printerFit->hide();
    this->m_fitWidgetItems.coinValidatorFit->hide();
    this->m_fitWidgetItems.smartCardReaderFit->hide();
    this->m_fitWidgetItems.multiIoFit->hide();
    this->m_fitWidgetItems.cameraFit->hide();
    this->m_fitWidgetItems.serialLoopBackFit->hide();

    // Set setup.
    this->setupTreeWidget();

    // Slots and signals.
    QObject::connect(ui->treeWidgetMain, SIGNAL(itemClicked(QTreeWidgetItem*, int)),
                     this, SLOT(handleItemClickedTreeWidgetMain(QTreeWidgetItem*, int)));
    QObject::connect(ui->actionCloseMenu, SIGNAL(triggered()), this, SLOT(close()));
    QObject::connect(ui->actionAboutMenu, SIGNAL(triggered()), this, SLOT(about()));
    QObject::connect(ui->actionWizardBar, SIGNAL(triggered()), this, SLOT(showAtmSelectMenu()));
    QObject::connect(ui->actionWizardMenu, SIGNAL(triggered()), this, SLOT(showAtmSelectMenu()));
    QObject::connect(ui->actionShowReportLogBar, SIGNAL(triggered()), this, SLOT(showLogPreviewer()));
    QObject::connect(ui->actionShowReportLogMenu, SIGNAL(triggered()), this, SLOT(showLogPreviewer()));
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::paintEvent(QPaintEvent *) {
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}

void MainWindow::changeEvent(QEvent *event) {
    // Event change language.
    if(event->type() == QEvent::LanguageChange) {
        ui->retranslateUi(this);
        ui->tabWidgetDevices->repaint();
        ui->treeWidgetMain->repaint();
        this->updateWidgetItems();

        this->m_fitWidgetItems.atmFit->repaint();
        this->m_fitWidgetItems.dispenserFit->repaint();
        this->m_fitWidgetItems.billValidatorFit->repaint();
        this->m_fitWidgetItems.printerFit->repaint();
        this->m_fitWidgetItems.coinValidatorFit->repaint();
        this->m_fitWidgetItems.smartCardReaderFit->repaint();
        this->m_fitWidgetItems.multiIoFit->repaint();
        this->m_fitWidgetItems.cameraFit->repaint();
        this->m_fitWidgetItems.serialLoopBackFit->repaint();
    }
    QWidget::changeEvent(event);
}

void MainWindow::closeEvent(QCloseEvent *event) {
    if (maybeClose()) {
        // Close objects.
        this->m_fitWidgetItems.atmFit->close();
        this->m_fitWidgetItems.dispenserFit->close();
        this->m_fitWidgetItems.billValidatorFit->close();
        this->m_fitWidgetItems.printerFit->close();
        this->m_fitWidgetItems.coinValidatorFit->close();
        this->m_fitWidgetItems.smartCardReaderFit->close();
        this->m_fitWidgetItems.multiIoFit->close();
        this->m_fitWidgetItems.cameraFit->close();
        this->m_fitWidgetItems.serialLoopBackFit->close();

        event->accept();
        QLOG_INFO() << "Destructor class:" << this->metaObject()->className();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeClose() {
    // Show message only in mode release.
#ifdef TESTATM_RELEASE
    QMessageBox::StandardButton ret;
    ret = QMessageBox::information(this, QCoreApplication::applicationName(),
                                   QObject::tr("You really want to close the program?"),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        return true;
    } else if (ret == QMessageBox::Cancel) {
        return false;
    }
#endif
    return true;
}

void MainWindow::showAtmSelect() {
    this->disableObjects(true);
    ui->tabWidgetDevices->hide();
    ui->treeWidgetMain->hide();

    this->m_atmModelSelect->hideAllTabs();
    this->m_atmModelSelect->adjustSize();
    this->m_atmModelSelect->resize(900, 150);

    m_atmModelSelect->move(QApplication::desktop()->screen()->rect().center() -
                           m_atmModelSelect->rect().center());

    // Slots and signals.
    QObject::connect(this->m_atmModelSelect, SIGNAL(signalShow()),
                     this, SLOT(showAtmConsole()));
    QObject::connect(this->m_atmModelSelect, SIGNAL(signalCancel()),
                     this, SLOT(activeObjects()));

    m_atmModelSelect->setWindowModality(Qt::WindowModal);
    m_atmModelSelect->show();
    m_atmModelSelect->activateWindow();
}

void MainWindow::showAtmSelectMenu() {
    QMessageBox::StandardButton ret;
    ret = QMessageBox::information(this, QCoreApplication::applicationName(),
                                   QObject::tr("You want to run the tests again? The current tests will be lost!"),
                                   QMessageBox::Ok | QMessageBox::Cancel);
    if (ret == QMessageBox::Ok) {
        this->showAtmSelect();
    } else if (ret == QMessageBox::Cancel) {
        return;
    }
}

void MainWindow::updateObjects() {
    this->disableObjects(false);
    ui->tabWidgetDevices->adjustSize();
    ui->treeWidgetMain->adjustSize();
    ui->tabWidgetDevices->show();
    ui->treeWidgetMain->show();
}

void MainWindow::showAtmConsole() {
    m_console->adjustSize();
    m_console->resize(900, 450);

    m_console->move(QApplication::desktop()->screen()->rect().center() -
                    m_console->rect().center());

    // Slots and signals.
    QObject::connect(this->m_console, SIGNAL(signalUpdateObjs()),
                     this, SLOT(updateObjects()));
    QObject::connect(this->m_console, SIGNAL(signalShow()),
                     this, SLOT(showAtmSelect()));

    m_console->show();
    m_atmModelSelect->activateWindow();
    m_console->runDeviceAutoTest();
}

void MainWindow::activeObjects() {
    this->disableObjects(false);
}

void MainWindow::disableObjects(bool enabled) {
    if (enabled) {
        ui->treeWidgetMain->setEnabled(false);
        this->setEnabled(false);
    } else {
        ui->treeWidgetMain->setEnabled(true);
        this->setEnabled(true);
    }
}

void MainWindow::handleItemClickedTreeWidgetMain(QTreeWidgetItem *item, int col) {
    const QVariant data = item->data(col, Qt::UserRole);

    // Hide all objects.
    this->m_fitWidgetItems.atmFit->hide();
    this->m_fitWidgetItems.dispenserFit->hide();
    this->m_fitWidgetItems.billValidatorFit->hide();
    this->m_fitWidgetItems.printerFit->hide();
    this->m_fitWidgetItems.coinValidatorFit->hide();
    this->m_fitWidgetItems.smartCardReaderFit->hide();
    this->m_fitWidgetItems.multiIoFit->hide();
    this->m_fitWidgetItems.cameraFit->hide();
    this->m_fitWidgetItems.serialLoopBackFit->hide();

    if (data == "root") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("ATM Generic"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/atm-icon.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.atmFit);
        this->m_fitWidgetItems.atmFit->show();
    } else if (data == "serial") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Serial Ports"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/serial-device-256x256.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.serialLoopBackFit);
        this->m_fitWidgetItems.serialLoopBackFit->show();
    } else if (data == "dispenser") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Dispenser"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/dispenser-empresa1.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.dispenserFit);
        this->m_fitWidgetItems.dispenserFit->show();
    } else if (data == "billvalidator") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Bill Validator"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/billvalidator-empresa1.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.billValidatorFit);
        this->m_fitWidgetItems.billValidatorFit->show();
    } else if (data == "printer") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Printer"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/printer-empresa1.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.printerFit);
        this->m_fitWidgetItems.printerFit->show();
    } else if (data == "coinvalidator") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Coin Validator"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/coinvalidator-empresa1.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.coinValidatorFit);
        this->m_fitWidgetItems.coinValidatorFit->show();
    } else if (data == "smartcardreader") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Smart Card Reader"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/cardreader-empresa1.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.smartCardReaderFit);
        this->m_fitWidgetItems.smartCardReaderFit->show();
    } else if (data == "multiio") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Multi/IO"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/multiio-empresa1.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.multiIoFit);
        this->m_fitWidgetItems.multiIoFit->show();
    } else if (data == "camera") {
        ui->tabWidgetDevices->setTabText(0, QObject::tr("Camera"));
        ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/camera-empresa1.png"));
        ui->gridLayout->addWidget(this->m_fitWidgetItems.cameraFit);
        this->m_fitWidgetItems.cameraFit->setCamera(QCameraInfo::defaultCamera());
        CameraFit::isCameraStarted();
        this->m_fitWidgetItems.cameraFit->show();
    }
}

void MainWindow::setupTreeWidget() {
    ui->treeWidgetMain->header()->setStyleSheet("font: bold 12px; font-size: 12pt; font-family: Arial;");
    QIcon icon1;
    icon1.addFile(QStringLiteral(":/res/images/atm-icon.png"), QSize(), QIcon::Normal, QIcon::Off);

    QTreeWidgetItem *qtreewidgetitem = new QTreeWidgetItem(ui->treeWidgetMain);
    qtreewidgetitem->setIcon(0, icon1);
    new QTreeWidgetItem(qtreewidgetitem);
    new QTreeWidgetItem(qtreewidgetitem);
    new QTreeWidgetItem(qtreewidgetitem);
    new QTreeWidgetItem(qtreewidgetitem);
    new QTreeWidgetItem(qtreewidgetitem);
    new QTreeWidgetItem(qtreewidgetitem);
    new QTreeWidgetItem(qtreewidgetitem);
    new QTreeWidgetItem(qtreewidgetitem);

    ui->treeWidgetMain->setObjectName(QStringLiteral("treeWidgetMain"));
    ui->treeWidgetMain->setGeometry(QRect(20, 20, 256, 321));
    ui->treeWidgetMain->setIconSize(QSize(60, 60));

    m_treeWidgetItems.qtreewidgetitem0 = ui->treeWidgetMain->headerItem();
    m_treeWidgetItems.qtreewidgetitem0->setText(0, QApplication::translate("MainWindow", "Devices", 0));

    m_treeWidgetItems.qtreewidgetitem1 = ui->treeWidgetMain->topLevelItem(0);
    m_treeWidgetItems.qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "ATM Generic", 0));
    m_treeWidgetItems.qtreewidgetitem1->setExpanded(true);
    m_treeWidgetItems.qtreewidgetitem1->setSelected(true);
    m_treeWidgetItems.qtreewidgetitem1->setData(0, Qt::UserRole, QVariant("root"));
    ui->tabWidgetDevices->setTabText(0, QObject::tr("ATM generic"));

    QIcon icon2;
    icon2.addFile(QStringLiteral(":/res/images/dispenser-empresa1.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem2 = m_treeWidgetItems.qtreewidgetitem1->child(0);
    m_treeWidgetItems.qtreewidgetitem2->setText(0, QApplication::translate("MainWindow", "Dispenser", 0));
    m_treeWidgetItems.qtreewidgetitem2->setIcon(0, icon2);
    m_treeWidgetItems.qtreewidgetitem2->setData(0, Qt::UserRole, QVariant("dispenser"));

    QIcon icon3;
    icon3.addFile(QStringLiteral(":/res/images/billvalidator-empresa1.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem3 = m_treeWidgetItems.qtreewidgetitem1->child(1);
    m_treeWidgetItems.qtreewidgetitem3->setText(0, QApplication::translate("MainWindow", "Bill Validator", 0));
    m_treeWidgetItems.qtreewidgetitem3->setIcon(0, icon3);
    m_treeWidgetItems.qtreewidgetitem3->setData(0, Qt::UserRole, QVariant("billvalidator"));

    QIcon icon4;
    icon4.addFile(QStringLiteral(":/res/images/printer-empresa1.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem4 = m_treeWidgetItems.qtreewidgetitem1->child(2);
    m_treeWidgetItems.qtreewidgetitem4->setText(0, QApplication::translate("MainWindow", "Printer", 0));
    m_treeWidgetItems.qtreewidgetitem4->setIcon(0, icon4);
    m_treeWidgetItems.qtreewidgetitem4->setData(0, Qt::UserRole, QVariant("printer"));

    QIcon icon5;
    icon5.addFile(QStringLiteral(":/res/images/coinvalidator-empresa1.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem5 = m_treeWidgetItems.qtreewidgetitem1->child(3);
    m_treeWidgetItems.qtreewidgetitem5->setText(0, QApplication::translate("MainWindow", "CoinValidator", 0));
    m_treeWidgetItems.qtreewidgetitem5->setIcon(0, icon5);
    m_treeWidgetItems.qtreewidgetitem5->setData(0, Qt::UserRole, QVariant("coinvalidator"));

    QIcon icon6;
    icon6.addFile(QStringLiteral(":/res/images/cardreader-empresa1.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem6 = m_treeWidgetItems.qtreewidgetitem1->child(4);
    m_treeWidgetItems.qtreewidgetitem6->setText(0, QApplication::translate("MainWindow", "SmartCardReader", 0));
    m_treeWidgetItems.qtreewidgetitem6->setIcon(0, icon6);
    m_treeWidgetItems.qtreewidgetitem6->setData(0, Qt::UserRole, QVariant("smartcardreader"));

    QIcon icon7;
    icon7.addFile(QStringLiteral(":/res/images/multiio-empresa1.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem7 = m_treeWidgetItems.qtreewidgetitem1->child(5);
    m_treeWidgetItems.qtreewidgetitem7->setText(0, QApplication::translate("MainWindow", "MultiIO", 0));
    m_treeWidgetItems.qtreewidgetitem7->setIcon(0, icon7);
    m_treeWidgetItems.qtreewidgetitem7->setData(0, Qt::UserRole, QVariant("multiio"));

    QIcon icon8;
    icon8.addFile(QStringLiteral(":/res/images/camera-empresa1.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem8 = m_treeWidgetItems.qtreewidgetitem1->child(6);
    m_treeWidgetItems.qtreewidgetitem8->setText(0, QApplication::translate("MainWindow", "Camera", 0));
    m_treeWidgetItems.qtreewidgetitem8->setIcon(0, icon8);
    m_treeWidgetItems.qtreewidgetitem8->setData(0, Qt::UserRole, QVariant("camera"));

    QIcon icon9;
    icon9.addFile(QStringLiteral(":/res/images/serial-device-256x256.png"), QSize(), QIcon::Normal, QIcon::Off);

    m_treeWidgetItems.qtreewidgetitem9 = m_treeWidgetItems.qtreewidgetitem1->child(7);
    m_treeWidgetItems.qtreewidgetitem9->setText(0, QApplication::translate("MainWindow", "SerialPorts", 0));
    m_treeWidgetItems.qtreewidgetitem9->setIcon(0, icon9);
    m_treeWidgetItems.qtreewidgetitem9->setData(0, Qt::UserRole, QVariant("serial"));

    // Set ATM fit.
    ui->tabWidgetDevices->setTabText(0, "ATM Generic");
    ui->tabWidgetDevices->setTabIcon(0, QIcon(":/res/images/atm-icon.png"));
    ui->gridLayout->addWidget(this->m_fitWidgetItems.atmFit);
    this->m_fitWidgetItems.atmFit->show();

    // Sorting.
    ui->treeWidgetMain->sortByColumn(0, Qt::AscendingOrder);
    ui->treeWidgetMain->setSortingEnabled(true);

    statusBar()->showMessage(QObject::tr("Ready"));
}

void MainWindow::updateWidgetItems() {
    m_treeWidgetItems.qtreewidgetitem0->setText(0, QApplication::translate("MainWindow", "Devices", 0));

    m_treeWidgetItems.qtreewidgetitem1->setText(0, QApplication::translate("MainWindow", "ATM Generic", 0));
    ui->tabWidgetDevices->setTabText(0, QObject::tr("ATM Generic"));

    m_treeWidgetItems.qtreewidgetitem2->setText(0, QApplication::translate("MainWindow", "Dispenser", 0));
    m_treeWidgetItems.qtreewidgetitem2->setData(0, Qt::UserRole, QVariant("dispenser"));

    m_treeWidgetItems.qtreewidgetitem3->setText(0, QApplication::translate("MainWindow", "Bill Validator", 0));
    m_treeWidgetItems.qtreewidgetitem3->setData(0, Qt::UserRole, QVariant("billvalidator"));

    m_treeWidgetItems.qtreewidgetitem4->setText(0, QApplication::translate("MainWindow", "Printer", 0));
    m_treeWidgetItems.qtreewidgetitem4->setData(0, Qt::UserRole, QVariant("printer"));

    m_treeWidgetItems.qtreewidgetitem5->setText(0, QApplication::translate("MainWindow", "CoinValidator", 0));
    m_treeWidgetItems.qtreewidgetitem5->setData(0, Qt::UserRole, QVariant("coinvalidator"));

    m_treeWidgetItems.qtreewidgetitem6->setText(0, QApplication::translate("MainWindow", "SmartCardReader", 0));
    m_treeWidgetItems.qtreewidgetitem6->setData(0, Qt::UserRole, QVariant("smartcardreader"));

    m_treeWidgetItems.qtreewidgetitem7->setText(0, QApplication::translate("MainWindow", "MultiIO", 0));
    m_treeWidgetItems.qtreewidgetitem7->setData(0, Qt::UserRole, QVariant("multiio"));

    m_treeWidgetItems.qtreewidgetitem8->setText(0, QApplication::translate("MainWindow", "Camera", 0));
    m_treeWidgetItems.qtreewidgetitem8->setData(0, Qt::UserRole, QVariant("camera"));

    m_treeWidgetItems.qtreewidgetitem9->setText(0, QApplication::translate("MainWindow", "SerialPorts", 0));
    m_treeWidgetItems.qtreewidgetitem9->setData(0, Qt::UserRole, QVariant("serial"));

    ui->tabWidgetDevices->setTabText(0, "ATM Generic");
    ui->treeWidgetMain->repaint();
}

void MainWindow::about() {
    About *about = new About(this);
    about->activateWindow();
    about->show();
}

void MainWindow::showLogPreviewer() {
    this->m_logPreviewer = new LogPreviewer(0);
    this->m_logPreviewer->showMaximized();
}
