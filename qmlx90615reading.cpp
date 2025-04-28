#include "qmlx90615reading.h"
#include "qmlx90615_p.h"

qreal QMLX90615Reading::temperature() const
{
    return d->temperature;
}

void QMLX90615Reading::setTemperature(qreal temperature)
{
    d->temperature = temperature;
}

qreal QMLX90615Reading::ambientTemperature() const
{
    return d->ambientTemperature;
}

void QMLX90615Reading::setAmbientTemperature(qreal temperature)
{
    d->ambientTemperature = temperature;
}
