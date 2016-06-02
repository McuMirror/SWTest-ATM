/**
 * @file e1.cpp
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

#include <atm/e1.hpp>
#include <utils/log.hpp>

#include <typeinfo>

// Start static variable and objects.
atm::E1* atm::E1::m_instance = 0;

// Start statics variables, used for store auto logs.
QVariantList atm::E1::m_insertLogAutoBillValidator = QVariantList();
QVariantList atm::E1::m_insertLogAutoCoinValidator = QVariantList();
QVariantList atm::E1::m_insertLogAutoDispenser = QVariantList();
QVariantList atm::E1::m_insertLogAutoPrinter = QVariantList();
QVariantList atm::E1::m_insertLogAutoMultiIo = QVariantList();
QVariantList atm::E1::m_insertLogAutoSmartCard = QVariantList();
QVariantList atm::E1::m_insertLogAutoCamera = QVariantList();

// Start statics variables, used for store manual logs.
QVariantList atm::E1::m_insertLogManualBillValidator = QVariantList();
QVariantList atm::E1::m_insertLogManualCoinValidator = QVariantList();
QVariantList atm::E1::m_insertLogManualDispenser = QVariantList();
QVariantList atm::E1::m_insertLogManualPrinter = QVariantList();
QVariantList atm::E1::m_insertLogManualMultiIo = QVariantList();
QVariantList atm::E1::m_insertLogManualSmartCard = QVariantList();

namespace atm {

    E1* E1::getInstance(const eAtmE1 dev) {
        if (m_instance == 0) {
            m_instance = new E1(dev);
        }
        return m_instance;
    }

    E1::E1(const eAtmE1 dev) {
        // First log class.
        QLOG_INFO() << "Constructor class:" << "E1";
        this->m_dev = dev;
    }

    E1::~E1() {
        QLOG_INFO() << "Destructor class:" << "E1";
        E1::m_instance = 0;
    }

    void E1::insertLogAutoBillValidator(const QVariant value) {
        atm::E1::m_insertLogAutoBillValidator.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogAutoBillValidator() {
        QMap<QVariant, QVariantList> data;
        data["BILLVALIDATOR"] = atm::E1::m_insertLogAutoBillValidator;
        return data;
    }

    void E1::insertLogAutoCoinValidator(const QVariant value) {
        atm::E1::m_insertLogAutoCoinValidator.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogAutoCoinValidator() {
        QMap<QVariant, QVariantList> data;
        data["COINVALIDATOR"] = atm::E1::m_insertLogAutoCoinValidator;
        return data;
    }

    void E1::insertLogAutoDispenser(const QVariant value) {
        atm::E1::m_insertLogAutoDispenser.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogAutoDispenser() {
        QMap<QVariant, QVariantList> data;
        data["DISPENSER"] = atm::E1::m_insertLogAutoDispenser;
        return data;
    }

    void E1::insertLogAutoPrinter(const QVariant value) {
        atm::E1::m_insertLogAutoPrinter.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogAutoPrinter() {
        QMap<QVariant, QVariantList> data;
        data["PRINTER"] = atm::E1::m_insertLogAutoPrinter;
        return data;
    }

    void E1::insertLogAutoMultiIo(const QVariant value) {
        atm::E1::m_insertLogAutoMultiIo.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogAutoMultiIo() {
        QMap<QVariant, QVariantList> data;
        data["MULTIIO"] = atm::E1::m_insertLogAutoMultiIo;
        return data;
    }

    void E1::insertLogAutoSmartCard(const QVariant value) {
        atm::E1::m_insertLogAutoSmartCard.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogAutoSmartCard() {
        QMap<QVariant, QVariantList> data;
        data["SMARTCARD"] = atm::E1::m_insertLogAutoSmartCard;
        return data;
    }

    void E1::insertLogAutoCamera(const QVariant value) {
        atm::E1::m_insertLogAutoCamera.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogAutoCamera() {
        QMap<QVariant, QVariantList> data;
        data["CAMERA"] = atm::E1::m_insertLogAutoCamera;
        return data;
    }

    void E1::insertLogManualBillValidator(const QVariant value) {
         atm::E1::m_insertLogManualBillValidator.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogManualBillValidator() {
        QMap<QVariant, QVariantList> data;
        data["BILLVALIDATOR"] = atm::E1::m_insertLogManualBillValidator;
        return data;
    }

    void E1::insertLogManualCoinValidator(const QVariant value) {
        atm::E1::m_insertLogManualCoinValidator.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogManualCoinValidator() {
        QMap<QVariant, QVariantList> data;
        data["COINVALIDATOR"] = atm::E1::m_insertLogManualCoinValidator;
        return data;
    }

    void E1::insertLogManualDispenser(const QVariant value) {
         atm::E1::m_insertLogManualDispenser.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogManualDispenser() {
        QMap<QVariant, QVariantList> data;
        data["DISPENSER"] = atm::E1::m_insertLogManualDispenser;
        return data;
    }

    void E1::insertLogManualPrinter(const QVariant value) {
        atm::E1::m_insertLogManualPrinter.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogManualPrinter() {
        QMap<QVariant, QVariantList> data;
        data["PRINTER"] = atm::E1::m_insertLogManualPrinter;
        return data;
    }

    void E1::insertLogManualMultiIo(const QVariant value) {
        atm::E1::m_insertLogManualMultiIo.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogManualMultiIo() {
        QMap<QVariant, QVariantList> data;
        data["MULTIIO"] = atm::E1::m_insertLogManualMultiIo;
        return data;
    }

    void E1::insertLogManualSmartCard(const QVariant value) {
        atm::E1::m_insertLogManualSmartCard.append(value);
    }

    QMap<QVariant, QVariantList> E1::printLogManualSmartCard() {
        QMap<QVariant, QVariantList> data;
        data["SMARTCARD"] = atm::E1::m_insertLogManualSmartCard;
        return data;
    }
}
