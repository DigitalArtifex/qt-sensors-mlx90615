#ifndef QMLX9_615BACKEND_H
#define QMLX9_615BACKEND_H

#include <QObject>

#include <QObject>
#include <QString>
#include <QSensorBackend>
#include <QTimer>
#include <QThread>
#include <QDateTime>

#include "QMLX90615_global.h"
#include "qmlx90615.h"

#include "fcntl.h"
#include "i2c/smbus.h"
#include "linux/i2c-dev.h"
#include "linux/errno.h"
#include "sys/ioctl.h"

QT_BEGIN_NAMESPACE

class QMLX90615Backend : public QSensorBackend
{
    Q_OBJECT

    enum class Command : quint8
    {
        EEPROM = 0x10,
        RAM = 0x20,
        Sleep = 0xC6
    };

    enum class Register : quint8
    {
        Address = 0x00, //writeable i2c address
        PWMRange = 0x01,
        Configuration = 0x02,
        Emissivity = 0x03,
        ID1 = 0x0E,
        ID2 = 0x0F
    };

    enum class RAM : quint8
    {
        IRData = 0x05,
        AmbientTemperature = 0x06,
        ObjectTemperature = 0x07
    };

    struct Configuration {
        Configuration() :
            I2C(1),
            PWM10khz(0),
            PWMMode(0),
            Reserved0(0),
            InternalShunt(1),
            IIRMode(1),
            Reserved1(0) { }
        Configuration(quint16 data):
            I2C(data & 0x01),
            PWM10khz((data >> 1) & 0x01),
            PWMMode((data >> 2) & 0x01),
            Reserved0((data >> 3) & 0xFF),
            InternalShunt((data >> 11) & 0x01),
            IIRMode((data >> 12) & 0x07),
            Reserved1((data >> 15) & 0x01) { }

        quint16 I2C: 1; /**< 1 = i2c 0 = pwm */
        quint16 PWM10khz: 1; /**< 1 = 10hkz 0 = 1khz */
        quint16 PWMMode: 1;
        quint16 Reserved0: 8;
        quint16 InternalShunt: 1;
        quint16 IIRMode: 3;
        quint16 Reserved1: 1;
        quint16 flatten()
        {
            quint16 result = 0;
            result |= (I2C & 0x01);
            result |= ((PWM10khz & 0x01) << 1);
            result |= ((PWMMode & 0x01) << 2);
            result |= ((Reserved0 & 0xFF) << 3);
            result |= ((InternalShunt & 0x01) << 11);
            result |= ((IIRMode & 0x07) << 12);
            result |= ((Reserved1 & 0x01) << 15);

            return result;
        }
    };

public:
    static inline const char* id = "QMLX90615-Backend";
    static inline const quint8 m_chipId = 0x58; //i2c chip id

    explicit QMLX90615Backend(QSensor *sensor = nullptr);
    ~QMLX90615Backend();

    virtual void start() override;
    virtual void stop() override;
    virtual bool isFeatureSupported(QSensor::Feature feature) const override;

signals:

protected slots:
    void poll();

protected:
    bool initialize();
    bool startI2C();
    bool endI2C();
    bool readTemperature();
    bool readAmbientTemperature();
    bool readConfiguration();
    void handleFault();
    void reportEvent(QString message);
    void reportError(QString message);
    void newLine();
    quint8 crc(quint8 *data, quint8 length, quint8 polynomial = 0x07);
    quint8 reverseBits(quint8 data);

    void onSensorBusChanged();
    void onSensorAddressChanged();
    void onSesnorDataRateChanged();

private:
    Configuration m_configuration;

    int m_i2c = -1;
    int m_errno;
    QString m_bus;
    quint8 m_address;

    bool m_initialized = false;
    bool m_backendDebug = false;
    QTimer *m_pollTimer = nullptr;

    qreal m_temperature = 0;
    qreal m_ambientTemperature = 0;
    QMLX90615Reading m_reading;
};

QT_END_NAMESPACE
#endif // QMLX9_615BACKEND_H
