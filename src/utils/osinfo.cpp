/**
 * @file osinfo.cpp
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

#include <utils/osinfo.hpp>

#include <QProcess>
#include <QStringList>

// Start static variables and objects.
utils::OsInfo* utils::OsInfo::m_instance = 0;

namespace utils {

    OsInfo* OsInfo::getInstance() {
        if (m_instance == 0) {
            m_instance = new OsInfo();
        }
        return m_instance;
    }

    OsInfo::OsInfo() {
    }

    OsInfo::~OsInfo() {
    }

    QString OsInfo::printCurrentCpuArchitecture() const {
        return m_sys.currentCpuArchitecture();
    }

    int OsInfo::printOsVersion() const {
#ifdef Q_OS_WIN
        return m_sys.windowsVersion();
#elif Q_OS_MAC
        return m_sys.macVersion();
#endif
    }

    QString OsInfo::printBuildAbi() const {
        return m_sys.buildAbi();
    }

    QString OsInfo::printKernelType() const {
        return m_sys.kernelType();
    }

    QString OsInfo::printKernelVersion() const {
        return m_sys.kernelVersion();
    }

    QString OsInfo::printProductName() const {
        return m_sys.prettyProductName();
    }

    QString OsInfo::printProductType() const {
        return m_sys.productType();
    }

    QString OsInfo::printTotalMemory() {
        QString sysinfo = QString();
#if defined(Q_OS_WIN)
        MEMORYSTATUSEX memstatus;
        ZeroMemory(&memstatus, sizeof(MEMORYSTATUSEX));
        memstatus.dwLength = sizeof(MEMORYSTATUSEX);
        if (GlobalMemoryStatusEx(&memstatus)) {
            sysinfo.append(QString("%1 MB").arg(memstatus.ullTotalPhys / (1024 * 1024)));
        } else {
            sysinfo.append("Unknown RAM");
        }
#elif defined(Q_OS_LINUX)
        QProcess p;
        p.start("awk", QStringList() << "/MemTotal/ { print $2 }" << "/proc/meminfo");
        p.waitForFinished();
        const QString memory = p.readAllStandardOutput();
        sysinfo.append(QString("; RAM: %1 MB").arg(memory.toLong() / 1024));
        p.close();
#elif defined(Q_OS_MAC)
        QProcess p;
        p.start("sysctl", QStringList() << "kern.version" << "hw.physmem");
        p.waitForFinished();
        sysinfo.append(p.readAllStandardOutput());
        p.close();
#endif
        return sysinfo;
    }

    QString OsInfo::printHardDiskSerial() {
        QString val = QString();
#if defined(Q_OS_WIN)
        DWORD dwVolSerial;
        val = QString::number(DWORD(dwVolSerial), 16);
#elif defined(Q_OS_LINUX)
        // @TODO: Add get serial hd in Linux platafom.
#endif
        return val.toUpper();
    }
}
