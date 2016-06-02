/**
 * @file log.hpp
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

#ifndef UTILS_LOG_HPP
#define UTILS_LOG_HPP

#include <qslog/QsLog.h>
#include <qslog/QsLogDest.h>

#include <QString>
#include <QDebug>

#include <iostream>
#include <typeinfo>

/**
 * @brief Namespace for the access to utils.
 */
namespace utils {
    /**
     * @class Log
     *
     * @brief Class that contains the logging mechanisms.
     *
     * @details This class is the control of the log in the program makes
     * use of "QsLog2".
     */
    class Log {
    public:
        /**
         * Constructor default.
         *
         * @param logmode - Type log generate: Off, Debug, Trace.
         * @param dirpath - Set directory generate log.
         * @param filepath - Set file name the log.
         */
        explicit Log(const int logmode = 2, const QString dirpath = QString(),
                     const QString filepath = QString());

        /**
         * Destructor.
         */
        ~Log();

        /**
         * Singleton for this class.
         *
         * @param logmode - Type log generate: Off, Debug, Trace.
         * @param dirpath - Set directory generate log.
         * @param filepath - Set file name the log.
         * @return The static class.
         */
        static Log* getInstance(const int logmode = 2, const QString dirpath = QString(),
                                const QString filepath = QString());

        /**
         * Set log printer.
         *
         * @param message - Message printer log.
         * @param level - Type log generate: Off, Debug, Trace.
         */
        void logPrint(const QString &message, QsLogging::Level level);

        /**
         * Destroy instance this class.
         */
        void destroy();

    protected:
        /// Store the log instance.
        static Log *m_instance;
    };
}

#endif // UTILS_LOG_HPP
