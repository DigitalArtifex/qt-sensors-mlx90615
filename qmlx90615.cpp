#include "qmlx90615.h"
#include "qmlx90615_p.h"

IMPLEMENT_READING(QMLX90615Reading)

QMLX90615::QMLX90615(QObject *parent) : QSensor(sensorType, parent) {}

QMLX90615Reading *QMLX90615::reading() const
{
    return qobject_cast<QMLX90615Reading*>(QSensor::reading());
}

QString QMLX90615::bus() const
{
    return m_bus;
}

void QMLX90615::setBus(const QString &bus)
{
    if (m_bus == bus)
        return;

    m_bus = bus;
    emit busChanged();
}

quint8 QMLX90615::address() const
{
    return m_address;
}

void QMLX90615::setAddress(quint8 address)
{
    if (m_address == address)
        return;

    m_address = address;
    emit addressChanged();
}
