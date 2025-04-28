# qt-sensors-mlx90615
Qt Sensors Plugin &amp; Library For MLX90615 Infrared Temperature Sensor

# Installation

Run the following commands in a terminal session to download and setup the plugin
```console
git clone https://github.com/DigitalArtifex/qt-sensors-mlx90615.git
mkdir qt-sensors-mlx90615/build
cd qt-sensors-mlx90615/build
```

Next, we will need to configure the project against your Qt Installation by executing `qt-cmake` located in `<Qt Path>/<Version>/<Arch>/bin`. Typically, the Qt Path will be `~/Qt`, but can be `/opt/Qt` if installed with sudo, or elsewhere if configured differently. The example assumes Qt Path to be `/opt/Qt`, Qt Version to be `6.9.0` and the arch to be `gcc_arm64`

```
/opt/Qt/6.9.0/gcc_arm64/bin/qt-cmake -S ../ -B ./
```

Once configured, we will switch to the system provided cmake. If Qt is installed to `~/` there is no need to execute `cmake install` with sudo

```
cmake --build ./
sudo cmake --install ./
```

# Usage

## Adding the reference (CMake)

```cmake
target_link_libraries(
  MyApp
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Sensors
  i2c
  qt${QT_VERSION_MAJOR}-sensors-mlx90615
)
```

## Using in code

```cpp
QMLX90615 mlx90615 = new QMLX90615(this);
mlx90615->connectToBackend();
mlx90615->setDataRate(1);
mlx90615->setBus("/dev/i2c-1");
mlx90615->setAddress(0x5B);

connect(mlx90615, SIGNAL(readingChanged()), this, SLOT(onSensorReadingAvailable()));
mlx90615->start();
```
