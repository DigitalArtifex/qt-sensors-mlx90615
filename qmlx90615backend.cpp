#include "qmlx90615backend.h"

QMLX90615Backend::QMLX90615Backend(QSensor *sensor)
    : QSensorBackend{sensor}
{
    m_pollTimer = new QTimer(this);
    QObject::connect(m_pollTimer, SIGNAL(timeout()), this, SLOT(poll()));

    reportEvent("QMLX90615 BACKEND CREATED");

    setReading<QMLX90615Reading>(&m_reading);
    reading();

    addDataRate(1, 157);
}

QMLX90615Backend::~QMLX90615Backend()
{
    if(m_pollTimer)
    {
        if(m_pollTimer->isActive())
            m_pollTimer->stop();

        delete m_pollTimer;
    }

    QSensorBackend::~QSensorBackend();
}

void QMLX90615Backend::start()
{
    if(m_pollTimer && m_pollTimer->isActive())
        return;

    if(!m_initialized && !initialize())
    {
        reportError("COULD NOT INITIALIZE SENSOR");
        handleFault();
        return;
    }

    m_pollTimer->setInterval(1000 / sensor()->dataRate());
    m_pollTimer->start();
}

void QMLX90615Backend::stop()
{
    if(!m_pollTimer || !m_pollTimer->isActive())
        return;

    m_pollTimer->stop();
}

bool QMLX90615Backend::isFeatureSupported(QSensor::Feature feature) const
{
    return false;
}

bool QMLX90615Backend::initialize()
{
    QMLX90615 *sensor = qobject_cast<QMLX90615*>(this->sensor());

    if(!sensor)
        return false;

    m_bus = sensor->bus();
    m_address = sensor->address();

    QObject::connect(sensor, &QMLX90615::busChanged, this, &QMLX90615Backend::onSensorBusChanged);
    QObject::connect(sensor, &QMLX90615::addressChanged, this, &QMLX90615Backend::onSensorAddressChanged);
    QObject::connect(sensor, &QMLX90615::dataRateChanged, this, &QMLX90615Backend::onSesnorDataRateChanged);

    startI2C();
    readConfiguration();
    endI2C();

    m_initialized = true;

    return true;
}

bool QMLX90615Backend::startI2C()
{
    if(m_i2c < 0)
    {
        if((m_i2c = open(m_bus.toStdString().c_str(), O_RDWR)) < 0)
        {
            reportError("COULD NOT OPEN I2C BUS");
            m_errno = errno;

            return false;
        }
    }

    if(ioctl(m_i2c, I2C_SLAVE, m_address) < 0)
    {
        reportError("DEVICE NOT FOUND");
        m_errno = errno;

        endI2C();

        return false;
    }

    reportEvent("I2C STARTED");

    return true;
}

bool QMLX90615Backend::endI2C()
{
    if(m_i2c < 0)
        return true;

    if(close(m_i2c) < 0)
    {
        reportError("COULD NOT CLOSE I2C BUS");
        m_errno = errno;
        m_i2c = -1;
        return false;
    }

    m_i2c = -1;
    reportEvent("I2C ENDED");

    return true;
}

void QMLX90615Backend::poll()
{
    //start i2c
    if(!startI2C() || !readTemperature() || !readAmbientTemperature() || !endI2C())
    {
        m_errno = errno; //errno is not set by endI2C()
        handleFault();
        return;
    }

    m_reading.setAmbientTemperature(m_ambientTemperature);
    m_reading.setTemperature(m_temperature);
    newReadingAvailable();
}

//Data is sent as LSB/MSB/PEC
bool QMLX90615Backend::readTemperature()
{
    quint8 reg = (quint8)Command::RAM | (quint8)RAM::ObjectTemperature;
    quint8 *buffer = new quint8[3];

    struct i2c_msg messages[]
    {
        {
            .addr = m_address,
            .flags = 0,
            .len = 1,
            .buf = &reg
        },
        {
            .addr = m_address,
            .flags = I2C_M_RD,
            .len = 3,
            .buf = buffer
        }
    };

    struct i2c_rdwr_ioctl_data payload =
    {
        .msgs = messages,
        .nmsgs = 2
    };

    if(ioctl(m_i2c, I2C_RDWR, &payload) < 0)
    {
        reportError("COULD NOT READ TEMPERATURE REGISTER");
        m_errno = errno;
        return false;
    }

    qint16 result = (qint16)buffer[1] << 8 | buffer[0];

    m_temperature = ((qreal)result * 0.02) - 273.15;

    return true;
}

bool QMLX90615Backend::readAmbientTemperature()
{
    quint8 reg = (quint8)Command::RAM | (quint8)RAM::AmbientTemperature;
    quint8 *buffer = new quint8[3];

    struct i2c_msg messages[]
        {
            {
                .addr = m_address,
                .flags = 0,
                .len = 1,
                .buf = &reg
            },
            {
                .addr = m_address,
                .flags = I2C_M_RD,
                .len = 3,
                .buf = buffer
            }
        };

    struct i2c_rdwr_ioctl_data payload =
        {
            .msgs = messages,
            .nmsgs = 2
        };

    if(ioctl(m_i2c, I2C_RDWR, &payload) < 0)
    {
        reportError("COULD NOT READ TEMPERATURE REGISTER");
        m_errno = errno;
        return false;
    }

    qint16 result = (qint16)buffer[1] << 8 | buffer[0];

    m_ambientTemperature = ((qreal)result * 0.02) - 273.15;

    return true;
}

bool QMLX90615Backend::readConfiguration()
{
    quint8 reg = (quint8)Command::EEPROM | (quint8)Register::Configuration;
    quint8 *buffer = new quint8[3];

    struct i2c_msg messages[]
    {
        {
            .addr = m_address,
            .flags = 0,
            .len = 1,
            .buf = &reg
        },
        {
            .addr = m_address,
            .flags = I2C_M_RD,
            .len = 3,
            .buf = buffer
        }
    };

    struct i2c_rdwr_ioctl_data payload =
    {
        .msgs = messages,
        .nmsgs = 2
    };

    if(ioctl(m_i2c, I2C_RDWR, &payload) < 0)
    {
        reportError("COULD NOT READ CONFIGURATION REGISTER");
        m_errno = errno;
        qDebug() << m_errno;
    }

    m_configuration = Configuration((qint16)buffer[1] << 8 | buffer[0]);

    return true;
}

void QMLX90615Backend::handleFault()
{
    //TODO
}

void QMLX90615Backend::reportEvent(QString message)
{
    //report event if backendDebug is true
    if(m_backendDebug)
        qDebug() << QString("** %1 - (QMLX90615@%2:0x%3)").arg(message, m_bus).arg(m_address, 2, 16);
}

void QMLX90615Backend::reportError(QString message)
{
    qDebug() << QString("!! ERROR: %1 - (QBMP280@%2:0x%3)").arg(message, m_bus).arg(m_address, 2, 16);
}

void QMLX90615Backend::newLine()
{
    //add newline if backendDebug is true
    if(m_backendDebug)
        qDebug() << "";
}

quint8 QMLX90615Backend::crc(quint8 *data, quint8 length, quint8 polynomial)
{
    uint8_t crc = 0x00;

    size_t i, j;
    for (i = 0; i < length; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x80)
                crc = ((crc << 1) ^ polynomial);
            else
                crc <<= 1;
        }
    }

    return crc;
}

quint8 QMLX90615Backend::reverseBits(quint8 data)
{
    return 0;
}

void QMLX90615Backend::onSensorBusChanged()
{
    QMLX90615 *sensor = qobject_cast<QMLX90615*>(this->sensor());

    if(!sensor)
        return;

    m_bus = sensor->bus();
}

void QMLX90615Backend::onSensorAddressChanged()
{
    QMLX90615 *sensor = qobject_cast<QMLX90615*>(this->sensor());

    if(!sensor)
        return;

    m_bus = sensor->bus();
}

void QMLX90615Backend::onSesnorDataRateChanged()
{
    stop();
    start();
}
