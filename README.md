# TerraMater

## Installation guide:

1. Install the vulkan sdk.
    - For Linux follow these instructions: https://vulkan.lunarg.com/doc/view/1.1.101.0/linux/getting_started.html
    - On Windows install the sdk in a folder cmake will find, for example the standard directory.
2. Install packages that are needed, on Linux this is the `xorg-dev` package, which is used by GLFW.
3. Clone the repository with `--recursive`.
4. Inside the repository directory create a new folder and change into the folder :
```
mkdir build
cd build/
```
5. Run CMake and call make as follows:
```
cmake ..
make
```
6. Change back to the root folder of the repository and start the programm:
```
cd ..
./build/TerraMater 
```



**How to configure PhysX on Windows in Visual Studio**

* copy the dll files from lib folder into build/debug (screenshot 1-3)
* go to Visual Studio open rightclick on "TerraMater" > Properities > C/C++  > Code Generation 
         under Runtime Library select "Multi-threaded Debug (/MTd)"
* Next is to right click on "glfw" > Properities > C/C++ > Code Generation 
         under Runtime Library select "Multi-threaded Debug (/MTd)"
* Next is to right click on "PhysX" > Properities > C/C++ > Code Generation
        under Runtime Library select "Multi-threaded Debug (/MTd)"
* https://pages.cpsc.ucalgary.ca/~mmactavi/585/PhysXSetup.pdf
