/**
 * @file abstractT500.cpp
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

#include <devices/printer/abstractT500.hpp>

#include <atm/e1.hpp>
#include <utils/log.hpp>

#include <QPrinter>
#include <QPrintDialog>
#include <QFileDialog>
#include <QFileInfo>
#include <QPainter>
#include <QTextEdit>
#include <QMessageBox>

// Start static variable and objects.
devices::printer::AbstractT500* devices::printer::AbstractT500::m_instance = 0;

namespace devices {
    namespace printer {

        AbstractT500* AbstractT500::getInstance() {
            if (m_instance == 0) {
               m_instance = new AbstractT500();
            }
            return m_instance;
        }
		
        AbstractT500::AbstractT500() {
            // First log class.
            QLOG_INFO() << "Constructor class:" << "AbstractT500";
        }
		
        AbstractT500::~AbstractT500() {
            QLOG_INFO() << "Destructor class:" << "AbstractT500";
            // Clean variables and objects.
            AbstractT500::m_instance = 0;
        }

        bool AbstractT500::showPrinters() {
            if (QPrinterInfo::availablePrinterNames().isEmpty()) {
                QLOG_ERROR() << "AbstractT500: Error get printer.";
                return false;
            }
            return true;
        }

        bool AbstractT500::info(bool ok, const std::string printerName) {
            Q_UNUSED(ok);

            QPrinterInfo info = QPrinterInfo::printerInfo(printerName.c_str());

            // It is good practice to make sure the object is valid, so we call isNull to do a sanity
            // check in case there is no printer info.
            if (info.isNull()) {
                return false;
            }

            // Printer status.
            switch (info.state()) {
            case QPrinter::Idle:
                QLOG_INFO() << "AbstractT500: Idle.";
                break;
            case QPrinter::Active:
                QLOG_INFO() << "AbstractT500: Active.";
                break;
            case QPrinter::Aborted:
                QLOG_INFO() << "AbstractT500: Aborted.";
                break;
            case QPrinter::Error:
                QLOG_ERROR() << "AbstractT500: Error.";
                break;
            default:
                QLOG_ERROR() << "AbstractT500: Unknow.";
                break;
            }

            // We fill in the combox box with a description of the screen resolutions.
            //foreach (int res, info.supportedResolutions()) {}

            // We do the same for the page sizes.
            //foreach (QPageSize size, info.supportedPageSizes()) {}
            return true;
        }

        QPrinterInfo AbstractT500::info(const std::string printerName) {
            QPrinterInfo info = QPrinterInfo::printerInfo(printerName.c_str());
            return info;
        }

        bool AbstractT500::sendTestPageToPrinter() {
            QFile file("PRN");
            if (!file.open(QIODevice::WriteOnly)) {
                qDebug() << "Error send print";
                return false;
            }
            file.write("Single line of text\n");
            file.close();
            return true;
        }
    }
}
