#ifndef QMLX9_615READING_H
#define QMLX9_615READING_H

#include <QSensor>
#include <QSensorReading>

#include "QMLX90615_global.h"

QT_BEGIN_NAMESPACE

class QMLX90615ReadingPrivate;

class QMLX90615_EXPORT QMLX90615Reading : public QSensorReading
{
    Q_OBJECT
    Q_PROPERTY(qreal temperature READ temperature)
    Q_PROPERTY(qreal ambientTemperature READ ambientTemperature)
    DECLARE_READING(QMLX90615Reading)
public:
    qreal temperature() const;
    void setTemperature(qreal temperature);
    qreal ambientTemperature() const;
    void setAmbientTemperature(qreal temperature);
};

class QMLX90615_EXPORT QMLX90615Filter : public QSensorFilter
{
public:
    virtual bool filter(QMLX90615Reading *reading) = 0;
private:
    bool filter(QSensorReading *reading) override { return filter(static_cast<QMLX90615Reading*>(reading)); }
};

QT_END_NAMESPACE
#endif // QMLX9_615READING_H
