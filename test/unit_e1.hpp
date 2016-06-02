//
// WARNING: Please do NOT documenting TEST codes.
//
#ifndef TEST_UNIT_E1_HPP
#define TEST_UNIT_E1_HPP

#ifdef TESTATM_DEBUG
#include <QtTest/QTest>
#endif

#include <QObject>

class UnitE1: public QObject {
    Q_OBJECT
public:
    UnitE1() {};
    ~UnitE1() {};

#ifdef TESTATM_DEBUG
public slots:
    virtual void initTestCase() {
        qDebug() << "Init unit tests in class atm::E1...";
        this->autoLog();
    }
    virtual void cleanupTestCase() {
        qDebug("Done.");
    }    
    void autoLog();
#endif // TESTATM_DEBUG
};

#endif // TEST_UNIT_E1_HPP
