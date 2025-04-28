#ifndef QMLX9_615_H
#define QMLX9_615_H

#include <QObject>
#include <QSensor>
#include <QString>

#include "QMLX90615_global.h"
#include "qmlx90615reading.h"

QT_BEGIN_NAMESPACE

class QMLX90615_EXPORT QMLX90615 : public QSensor
{
    Q_OBJECT
public:
    static inline char const * const sensorType = "QMLX90615";

    explicit QMLX90615(QObject *parent = nullptr);

    QMLX90615Reading *reading() const;

    QString bus() const;
    void setBus(const QString &bus);

    quint8 address() const;
    void setAddress(quint8 address);

signals:
    void busChanged();
    void addressChanged();

private:
    QString m_bus = "/dev/i2c-1"; //i2c bus path
    quint8 m_address = 0x5B; //i2c device address

    Q_PROPERTY(QString bus READ bus WRITE setBus NOTIFY busChanged FINAL)
    Q_PROPERTY(quint8 address READ address WRITE setAddress NOTIFY addressChanged FINAL)
};

QT_END_NAMESPACE
#endif // QMLX9_615_H
