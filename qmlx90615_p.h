#ifndef QMLX9_615_P_H
#define QMLX9_615_P_H
#include "QMLX90615_global.h"

QT_BEGIN_NAMESPACE

class QMLX90615_EXPORT QMLX90615ReadingPrivate
{
public:
    QMLX90615ReadingPrivate() : temperature(0.0) { }

    qreal temperature;
    qreal ambientTemperature;
};

QT_END_NAMESPACE
#endif // QMLX9_615_P_H
