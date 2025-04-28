#ifndef QMLX9_615PLUGIN_H
#define QMLX9_615PLUGIN_H

#include <QObject>
#include <QSensorBackendFactory>
#include <QSensorPluginInterface>
#include <QSensorChangesInterface>

#include "QMLX90615_global.h"
#include "qmlx90615backend.h"
#include "qmlx90615.h"

QT_BEGIN_NAMESPACE

class QMLX90615_EXPORT QMLX90615Plugin : public QObject, public QSensorPluginInterface, public QSensorChangesInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.qt-project.Qt.QSensorPluginInterface/1.0")
    Q_INTERFACES(QSensorPluginInterface QSensorChangesInterface)
public:
    void registerSensors() override
    {
        QSensor::defaultSensorForType(QMLX90615::sensorType);
        QSensorManager::registerBackend(QMLX90615::sensorType, QMLX90615Backend::id, this);
    }

    void sensorsChanged() override
    {
        //register backend on initial load
        if(!QSensorManager::isBackendRegistered(QMLX90615::sensorType, QMLX90615Backend::id))
            QSensorManager::registerBackend(QMLX90615::sensorType, QMLX90615Backend::id, this);
    }

    QSensorBackend *createBackend(QSensor *sensor) override
    {
        if (sensor->identifier() == QMLX90615Backend::id)
            return new QMLX90615Backend(sensor);

        return 0;
    }
};

QT_END_NAMESPACE
#endif // QMLX9_615PLUGIN_H
