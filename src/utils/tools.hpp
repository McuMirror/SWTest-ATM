/**
 * @file tools.hpp
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

#ifndef UTILS_TOOLS_HPP
#define UTILS_TOOLS_HPP

#include <QElapsedTimer>
#include <QStringList>
#include <QVariantList>
#include <QString>

#include <string>

namespace utils {
    /**
     * @class Tools
     *
     * @brief Class containing tools and utilities.
     *
     * @details This class containing tools and utilities that are used
     * in the system.
     */
    class Tools {
    public:
        /**
         * Constructor.
         */
        explicit Tools();

        /**
         * Destructor.
         */
        ~Tools();

        /**
         * Singleton for this class.
         *
         * @return The static class.
         */
        static Tools* getInstance();

        /**
         * Conversion hex to string.
         *
         * @param data - Data hex.
         * @param len - Size hex.
         * @return String conversion.
         */
        static std::string hexToStr(unsigned char *data, int len);

        /**
         * Conversion hex to ascii.
         *
         * @param data - Data.
         * @return String conversion.
         */
        static std::string hexToAscii(const std::string data);

        /**
         * Print app information.
         *
         * @return App information.
         */
        static std::string printAppInfo();

        /**
         * Print information about build software.
         *
         * @return Build info.
         */
        static std::string printBuildInfo();

        /**
         * Print team information.
         *
         * @return Team information.
         */
        static std::string printTeamInfo();

        /**
         * Get the part (piece) the string, inside the variant list.
         *
         * @param data - The data.
         * @param searchedword - The word search.
         * @param replacedword - The word replace.
         * @return Return the part string.
         */
        static QString getPartStr(QVariantList data, QString searchedword, QString replacedword);

        /**
         * Method is usually used to quickly calculate how much time has elapsed
         * between two events.
         */
        void startElapsedTimer();

        /**
         * Restart elapsed timer.
         *
         * @see See startElapsedTimer().
         */
        void restartElapsedTimer();

        /**
         * Print number (elapsed timer).
         *
         * @return Time epapsed.
         */
        qint64 printElapsedTimer();

        /**
         * Check if exist network connections.
         *
         * @return Return ok if connection exist; otherwize false.
         */
        bool isConnectedToNetwork();

        /**
         * Print total net interface found.
         *
         * @see See isConnectedToNetwork().
         * @return Total interfaces found.
         */
        int printNetFaceCount() {
            return this->m_faceCount;
        }

        /**
         * Print interfaces names.
         *
         * @see See isConnectedToNetwork().
         * @return Interfaces found.
         */
        QStringList printNetFaceName() {
            return this->m_netFaceName;
        }

        /**
         * Print mac addresses.
         *
         * @see See isConnectedToNetwork().
         * @return Mac addresses found.
         */
        QStringList printNetMacAddresses() {
            return this->m_netMacAddresses;
        }

        /**
         * Print net ip addresses.
         *
         * @see See isConnectedToNetwork().
         * @return Net ip addresses found.
         */
        QStringList printNetIpAddresses() {
            return this->m_netIpAddresses;
        }

        /**
         * Print net mask addresses.
         *
         * @see See isConnectedToNetwork().
         * @return Net mask found.
         */
        QStringList printNetMaskAddresses() {
            return this->m_netMaskAddresses;
        }

    protected:
        /// Store the log instance.
        static Tools *m_instance;

        /// Store the elapsed timer.
        QElapsedTimer* m_elapsedTimer;

        /// Store net face name found.
        QStringList m_netFaceName;

        /// Store net mac addresses found.
        QStringList m_netMacAddresses;

        /// Store net ip addresses found.
        QStringList m_netIpAddresses;

        /// Store net mask addressed found.
        QStringList m_netMaskAddresses;

        /// Store net face name found.
        int m_faceCount;
    };

};

#endif // UTILS_TOOLS_HPP
