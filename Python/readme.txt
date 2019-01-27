how to build 

1)install python 2.7 => python installer
https://www.python.org/download/releases/2.7/
2)add the python dir to the env var PATH (it expect python to be in C:\python27 => bat file
./setupPython/addPythonDirToEnvPath.bat
3)download libs for python (pyside, pysintaller ...) => bat file
./setupPython/configPython.bat
4)build the exe => bat file
./build.bat

the build result should be now in the new created dir "dist"


for developing recommended IDE :
https://www.jetbrains.com/pycharm/download/#section=windows

