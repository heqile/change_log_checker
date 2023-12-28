## How to compile
### Create build dir
```bash
$ mkdir build
```
### Debug
#### Build
```bash
$ cmake -S . -B build 
$ cd build
$ cmake --build .
```
#### Run Test After Build
```bash
$ ctest .
```
### Release
```bash
$ cmake -DCMAKE_BUILD_TYPE=Release -DPACKAGE_TESTS=OFF -S . -B build
$ cd build
$ cmake --build .
```