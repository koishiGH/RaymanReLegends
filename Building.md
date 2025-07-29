# RaymanReLegends Build Guide

Simple little build guide for RaymanReLegends.

## Windows

* Install [Visual Studio 2022](https://visualstudio.microsoft.com/vs/) with "Desktop Development with C++"

* Open platforms/VisualC/RaymanReLegends.sln

* Press CTRL+B and wait.

* You should be done, open the Debug or Release (depending on which one you chose) and run "Rayman Legends.exe"

## Linux

It's as simple as running 3 commands!

```bash
cd platforms/Linux
make install-deps # or make install-deps-arch if you're on arch linux
make
```