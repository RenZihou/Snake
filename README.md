# Snake

A simple Snake game implemented by qt6/c++

## Usage

You need to specify your `CMAKE_PREFIX_PATH` in `CMakeLists.txt` to the `cmake` folder provided
by Qt. It should look like `QT_INSTALL_FOLDER/QT_VERSION/mingw81_64/lib/cmake`.

```
> cmake -S . -B build -G "MinGW Makefiles"
> cmake --build build
```
