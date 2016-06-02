/**
 * @file abstractcashcode609.cpp
 *
 * Implementations for a SWTest-ATM.
 *
 * ATM for banks Project.
 *
 * Copyright (c) 2015-2016 BlueSalute Company.
 *
 * @author Ederson Moura - <ederson.moura@bluesalute.com>
 * @mainteiner: ederson.moura@resource.com.br
 *
 * $Id: Exp$
 */

#include <test/unit_e1.hpp>
#include <atm/e1.hpp>

using namespace atm;

#ifdef TESTATM_DEBUG
void UnitE1::autoLog() {
    QMap<QVariant, QVariantList>::Iterator it;
    QMap<QVariant, QVariantList> data;
    QVariantList logDev;

    E1::insertLogAutoBillValidator("open+AUTO=PASSED");
    E1::insertLogAutoBillValidator("close+AUTO=PASSED");

    data = atm::E1::printLogAutoBillValidator();

    if (!data.isEmpty()) {
        for (it = data.begin(); it != data.end(); it++) {
            logDev.append(it.value());
        }
    }

    Q_ASSERT(logDev.size() >= 0);
    QCOMPARE(logDev.at(0), QVariant("open+AUTO=PASSED"));
    QCOMPARE(logDev.at(1), QVariant("close+AUTO=PASSED"));

    data.clear();
    logDev.clear();

    E1::insertLogAutoCoinValidator("open+AUTO=PASSED");
    E1::insertLogAutoCoinValidator("close+AUTO=PASSED");

    data = atm::E1::printLogAutoCoinValidator();

    if (!data.isEmpty()) {
        for (it = data.begin(); it != data.end(); it++) {
            logDev.append(it.value());
        }
    }

    Q_ASSERT(logDev.size() >= 0);
    QCOMPARE(logDev.at(0), QVariant("open+AUTO=PASSED"));
    QCOMPARE(logDev.at(1), QVariant("close+AUTO=PASSED"));

    data.clear();
    logDev.clear();

    E1::insertLogAutoCamera("open+AUTO=PASSED");
    E1::insertLogAutoCamera("close+AUTO=PASSED");

    data = atm::E1::printLogAutoCamera();

    if (!data.isEmpty()) {
        for (it = data.begin(); it != data.end(); it++) {
            logDev.append(it.value());
        }
    }

    Q_ASSERT(logDev.size() >= 0);
    QCOMPARE(logDev.at(0), QVariant("open+AUTO=PASSED"));
    QCOMPARE(logDev.at(1), QVariant("close+AUTO=PASSED"));

    data.clear();
    logDev.clear();

    E1::insertLogAutoDispenser("open+AUTO=PASSED");
    E1::insertLogAutoDispenser("close+AUTO=PASSED");

    data = atm::E1::printLogAutoDispenser();

    if (!data.isEmpty()) {
        for (it = data.begin(); it != data.end(); it++) {
            logDev.append(it.value());
        }
    }

    Q_ASSERT(logDev.size() >= 0);
    QCOMPARE(logDev.at(0), QVariant("open+AUTO=PASSED"));
    QCOMPARE(logDev.at(1), QVariant("close+AUTO=PASSED"));

    data.clear();
    logDev.clear();

    E1::insertLogAutoMultiIo("open+AUTO=PASSED");
    E1::insertLogAutoMultiIo("close+AUTO=PASSED");

    data = atm::E1::printLogAutoMultiIo();

    if (!data.isEmpty()) {
        for (it = data.begin(); it != data.end(); it++) {
            logDev.append(it.value());
        }
    }

    Q_ASSERT(logDev.size() >= 0);
    QCOMPARE(logDev.at(0), QVariant("open+AUTO=PASSED"));
    QCOMPARE(logDev.at(1), QVariant("close+AUTO=PASSED"));

    data.clear();
    logDev.clear();

    E1::insertLogAutoPrinter("open+AUTO=PASSED");
    E1::insertLogAutoPrinter("close+AUTO=PASSED");

    data = atm::E1::printLogAutoPrinter();

    if (!data.isEmpty()) {
        for (it = data.begin(); it != data.end(); it++) {
            logDev.append(it.value());
        }
    }

    Q_ASSERT(logDev.size() >= 0);
    QCOMPARE(logDev.at(0), QVariant("open+AUTO=PASSED"));
    QCOMPARE(logDev.at(1), QVariant("close+AUTO=PASSED"));

    data.clear();
    logDev.clear();

    E1::insertLogAutoSmartCard("open+AUTO=PASSED");
    E1::insertLogAutoSmartCard("close+AUTO=PASSED");

    data = atm::E1::printLogAutoSmartCard();

    if (!data.isEmpty()) {
        for (it = data.begin(); it != data.end(); it++) {
            logDev.append(it.value());
        }
    }

    Q_ASSERT(logDev.size() >= 0);
    QCOMPARE(logDev.at(0), QVariant("open+AUTO=PASSED"));
    QCOMPARE(logDev.at(1), QVariant("close+AUTO=PASSED"));

    data.clear();
    logDev.clear();
}
#endif
