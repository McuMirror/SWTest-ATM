/**
 * @file log.cpp
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

#include <utils/log.hpp>

#include <QLibrary>
#include <QCoreApplication>
#include <QDir>
#include <QDateTime>

// Start static variable and objects.
utils::Log* utils::Log::m_instance = 0;

namespace utils {

    Log* Log::getInstance(const int logmode, const QString dirpath, const QString filepath) {
        if (m_instance == 0) {
            m_instance = new Log(logmode, dirpath, filepath);
        }
        return m_instance;
    }

    Log::Log(const int logmode, const QString dirpath, const QString filepath) {
        // Init the logging mechanism.
        QsLogging::Logger& logger = QsLogging::Logger::instance();

        switch (logmode) {
        case 0:
            logger.setLoggingLevel(QsLogging::OffLevel);
            break;
        case 1:
            logger.setLoggingLevel(QsLogging::DebugLevel);
            break;
        case 2:
            logger.setLoggingLevel(QsLogging::TraceLevel);
            break;
        default:
            logger.setLoggingLevel(QsLogging::TraceLevel);
            break;
        }

        // Log name.
        QDateTime dateTime = dateTime.currentDateTime();
        QString dateTimeString = dateTime.toString("yyyy-MM-dd_hh-mm-ss");
        QString sLogPath = QCoreApplication::applicationDirPath() + QString("/log/");

        QDir dir(sLogPath);
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        if ((!dirpath.isEmpty() || !dirpath.isNull()) &&
            (!filepath.isEmpty() || !filepath.isNull())) {
            sLogPath = (QDir(dirpath).filePath(filepath));
        } else {
            sLogPath = (QDir(QCoreApplication::applicationDirPath()).filePath(sLogPath + QCoreApplication::applicationName() + "-" + dateTimeString + ".log"));
        }

        // Add two destinations.
        QsLogging::DestinationPtr fileDestination(QsLogging::DestinationFactory::MakeFileDestination(sLogPath,
                                                                                                     QsLogging::EnableLogRotation,
                                                                                                     QsLogging::MaxSizeBytes(100122),
                                                                                                     QsLogging::MaxOldLogCount(2)));
        QsLogging::DestinationPtr debugDestination(QsLogging::DestinationFactory::MakeDebugOutputDestination());
        logger.addDestination(debugDestination);
        logger.addDestination(fileDestination);
    }

    Log::~Log() {
    }

    void Log::logPrint(const QString &message, QsLogging::Level level) {
        std::cout << "From log function: " << qPrintable(message) << " " << static_cast<int>(level)
                  << std::endl;
    }

    void Log::destroy() {
        QsLogging::Logger::destroyInstance();
    }

}
