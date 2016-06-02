/**
 * @file osinfo.hpp
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

#ifndef UTILS_OSINFO_HPP
#define UTILS_OSINFO_HPP

#include <QSysInfo>
#include <QString>

#ifdef Q_OS_WIN
extern "C" {
#include <windows.h>
}
#endif

// Override!
class QSysInfo;

namespace utils {
    /**
     * @class OsInfo
     *
     * @brief Class that contains the informations about system.
     *
     * @details It contains information about the system and the program.
     */
    class OsInfo {
    public:
        /**
         * Contructor.
         */
        explicit OsInfo();

        /**
         * Destructor.
         */
        ~OsInfo();

        /**
         * Singleton for this class.
         *
         * @return The static class.
         */
        static OsInfo* getInstance();

        /**
         * Print name cpu architecture.
         *
         * @return Name the cpu architecture.
         */
        QString printCurrentCpuArchitecture() const;

        /**
         * Print OS version.
         *
         * @return Return number the OS version.
         */
        int printOsVersion() const;

        /**
         * Print type name buil ABI.
         *
         * @return Name the build ABI.
         */
        QString printBuildAbi() const;

        /**
         * Print name kernel type.
         *
         * @return Name the kernel.
         */
        QString printKernelType() const;

        /**
         * Print name kernel version.
         *
         * @return Name the kernel version.
         */
        QString printKernelVersion() const;

        /**
         * Print name the product name.
         *
         * @return Name the product.
         */
        QString printProductName() const;

        /**
         * Print name the product type.
         *
         * @return Name the product type.
         */
        QString printProductType() const;

        /**
         * Print total memory disponible in system.
         *
         * @return Total memory disponible.
         */
        QString printTotalMemory();

        /**
         * Print serial numbber the harddisk.
         *
         * @return Disk serial number.
         */
        QString printHardDiskSerial();

    protected:
        /// Store the OsInfo instance.
        static OsInfo *m_instance;

        /// Get information about system.
        QSysInfo m_sys;
    };
}

#endif // UTILS_LOG_HPP
