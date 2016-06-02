/**
 * @file main.cpp
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
#include <utils/log.hpp>

#ifdef TESTATM_DEBUG
#include <test/unit_e1.hpp>
#endif

#include <QApplication>
#include <QDesktopWidget>
#include <QStyleFactory>
#include <QSplashScreen>
#include <QCommandLineParser>
#include <QTime>
#include <QFont>

/**
 * Run unit tests case, in mode debug only.
 *
 * @return return true is success; otherwize false.
 */
static bool runUnitTests() {
#ifdef TESTATM_DEBUG
    QLOG_INFO() << "Call unit tests...";

    register int runtest = 0;

    UnitE1 e1;
    runtest += QTest::qExec(&e1);
#endif
    return true;
}

/**
 * The main.
 *
 * @param argc - Argument count.
 * @param argv - Argument vector.
 * @return Return widget application.
 */
int main(int argc, char *argv[]) {
    // Initializes the resources.
    Q_INIT_RESOURCE(images);

    QApplication app(argc, argv);

    // About this app.
    app.setOrganizationName("BlueSalute Company");
    app.setOrganizationDomain("bluesalute.com");
    app.setApplicationName("SW-TestATM");
    app.setApplicationVersion("1.0");

    // Start log instance.
    utils::Log::getInstance();

    QLOG_INFO() << "Application started...";

    // Show splashscreen only in mode release.
#ifdef TESTATM_RELEASE
    QPixmap pixmap(":/res/images/splashscreen.jpg");
    QSplashScreen splash(pixmap);
    splash.show();

    app.processEvents();
    splash.showMessage(QObject::tr("Loading..."));

    QTime dieTime= QTime::currentTime().addSecs(2);
    while( QTime::currentTime() < dieTime )
    QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
#endif
    // Set style application.
    app.setStyle(QStyleFactory::create("Fusion"));

    // Set font.
    QFont font;
    font.setFamily(font.defaultFamily());
    app.setFont(font);

    // Check unit options.
    if (argc > 1) {
        if (QCoreApplication::arguments().at(1) == "--unittest") {
            runUnitTests();
        }
    } else {
#ifdef TESTATM_DEBUG
        runUnitTests();
#endif
        // Open the main window SW.
        MainWindow *w = new MainWindow(0);
        w->adjustSize();
        w->move(QApplication::desktop()->screen()->rect().center() - w->rect().center());
        w->showMaximized();
        w->showAtmSelect();  
#ifdef TESTATM_RELEASE
        splash.finish(w);
#endif
    }
    register int rc = app.exec();
    if (rc == 0) {
        QLOG_INFO() << "Application ending.";
        utils::Log::getInstance()->destroy();
    }
    return rc;
}
