/**
 * @file tools.cpp
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

#include <utils/tools.hpp>
#include <utils/osinfo.hpp>

#include <QCoreApplication>
#include <QList>
#include <QtNetwork/QNetworkInterface>

#include <iostream>
#include <sstream>

// Start static variable and objects.
utils::Tools* utils::Tools::m_instance = 0;

/// Map in HEX.
char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                 '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

namespace utils {

    Tools* Tools::getInstance() {
        if (m_instance == 0) {
            m_instance = new Tools();
        }
        return m_instance;
    }

    Tools::Tools() {
    }

    Tools::~Tools() {
    }

    std::string Tools::hexToStr(unsigned char *data, int len) {
      std::string s(len * 2, ' ');
      for (int i = 0; i < len; ++i) {
        s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
      }
      return s;
    }

    std::string Tools::hexToAscii(const std::string data) {
        std::string s = "";
        s.reserve(data.size() / 2);
        for (unsigned int i = 0; i < data.size(); i += 2) {
            std::istringstream iss(data.substr(i, 2));
            int temp = 0;
            iss >> std::hex >> temp;
            s += static_cast<char>(temp);
        }
        return s;
    }

    std::string Tools::printAppInfo() {
        std::string info = "";

        // Get data.
        const std::string organizationName   = QCoreApplication::organizationName().toStdString();
        const std::string organizationDomain = QCoreApplication::organizationDomain().toStdString();
        const std::string applicationName    = QCoreApplication::applicationName().toStdString();
        const std::string applicationVersion = QCoreApplication::applicationVersion().toStdString();
        const std::string qtVersion = qVersion();

        QString appDetails     = QObject::tr("Application Details:");
        QString orgName        = QObject::tr("Organization Name:");
        QString orgDomain      = QObject::tr("Organization Domain:");
        QString appName        = QObject::tr("Application Name:");
        QString appVersion     = QObject::tr("Application Version:");
        QString qtCheckVersion = QObject::tr("Qt Version:");

        info += appDetails.toStdString() + "\n\n";
        info += "      " + orgName.toStdString() + " " + organizationName + "\n";
        info += "      " + orgDomain.toStdString() + " " + organizationDomain + "\n";
        info += "      " + appName.toStdString() + " "  + applicationName + "\n";
        info += "      " + appVersion.toStdString() + " " + applicationVersion + "\n";
        info += "      " + qtCheckVersion.toStdString() + " " + qtVersion + "\n";
        return info;
    }

    std::string Tools::printBuildInfo() {
        std::string info = "";

        QString compBDetails = QObject::tr("Compilation Details:");
        QString compBDate    = QObject::tr("Compiler Build Date:");
        QString compBVersion = QObject::tr("Compiler Build Version:");
        QString compBPlat    = QObject::tr("Platform Build Application:");
        QString compBCPU     = QObject::tr("CPU Build Application:");
        QString compBOpt0    = QObject::tr("Build Optimized: No");
        QString compBOpt1    = QObject::tr("Build Optimized: Yes");
        QString compIso0     = QObject::tr("ISO Standard: No");
        QString compIso1     = QObject::tr("ISO Standard: Yes");

        info += compBDetails.toStdString() + "\n\n";
        info += "      " + compBDate.toStdString() + " " + __DATE__  + " " +  __TIME__ + "\n";
        info += "      " + compBVersion.toStdString() + " " + __VERSION__ + "\n";
        info += "      " + compBPlat.toStdString() + " " + utils::OsInfo::getInstance()->printProductName().toStdString() + "\n";
        info += "      " + compBCPU.toStdString() + " " + utils::OsInfo::getInstance()->printCurrentCpuArchitecture().toStdString() + "\n";
#ifdef __OPTIMIZE__
#define __OPT__ 1
        info += "      " + compBOpt1.toStdString() + "\n";
#else
#define __OPT__ 0
        info += "      " + compBOpt0.toStdString() + "\n";
#endif
#ifdef __STDC__
        info += "      " + compIso1.toStdString() + "\n";
#else
        info += "      " + compIso0.toStdString() + "\n";
#endif
        return info;
    }

    std::string Tools::printTeamInfo() {
        std::string info = "";

        QString teamInfo = QObject::tr("Team Information:");
        QString teamDepartName = QObject::tr("Department Name: Software development");
        QString teamCellName = QObject::tr("Cell Name: ATM");
        QString teamCityName = QObject::tr("City Name: Belo Horizonte - MG");
        QString teamCountry = QObject::tr("Country: Brazil");
        QString teamContact = QObject::tr("Contact email: atm@empresa1.com.br");
        QString teamPhone   = QObject::tr("Phone number: +55 31 3516-5200");

        info += teamInfo.toStdString() + "\n\n";
        info += "      " + teamDepartName.toStdString() + "\n";
        info += "      " + teamCellName.toStdString() + "\n";
        info += "      " + teamCityName.toStdString() + "\n";
        info += "      " + teamCountry.toStdString() + "\n";
        info += "      " + teamContact.toStdString() + "\n";
        info += "      " + teamPhone.toStdString() + "\n";
        return info;
    }

    void Tools::startElapsedTimer() {
        this->m_elapsedTimer = new QElapsedTimer;
        this->m_elapsedTimer->start();
    }

    void Tools::restartElapsedTimer() {
        this->m_elapsedTimer->restart();
    }

    qint64 Tools::printElapsedTimer() {
        return this->m_elapsedTimer->elapsed();
    }

    bool Tools::isConnectedToNetwork() {
        this->m_faceCount = 0;
        this->m_netFaceName.clear();
        this->m_netMacAddresses.clear();
        this->m_netIpAddresses.clear();
        this->m_netMaskAddresses.clear();

        QList<QNetworkInterface> ifaces = QNetworkInterface::allInterfaces();
        this->m_faceCount = ifaces.count();
        bool result = false;

        for (register int i = 0; i < ifaces.count(); i++) {
            QNetworkInterface iface = ifaces.at(i);
            if (iface.flags().testFlag(QNetworkInterface::IsUp) &&
                    !iface.flags().testFlag(QNetworkInterface::IsLoopBack)) {
                // Details of connection.
                this->m_netFaceName.append(iface.name());
                this->m_netMacAddresses.append(iface.hardwareAddress());
                for (register int j = 0; j < iface.addressEntries().count(); j++) {
                    // Ip and mask address.
                    this->m_netIpAddresses.append(iface.addressEntries().at(j).ip().toString());
                    this->m_netMaskAddresses.append(iface.addressEntries().at(j).netmask().toString());
                    // Got an interface which is up, and has an ip address.
                    if (result == false) {
                        result = true;
                    }
                }
            }

        }
        return result;
    }

    QString Tools::getPartStr(QVariantList data, QString searchedword, QString replacedword) {
        QVariantList li = data;
        QString str = QString();
        for (QVariantList::iterator j = li.begin(); j != li.end(); j++) {
            register int i = 0;
            while ((i = (*j).toString().indexOf(searchedword, i, Qt::CaseSensitive)) != -1) {
                str = (*j).toString().replace(replacedword, "");
                ++i;
            }
        }
        return str;
    }

}
