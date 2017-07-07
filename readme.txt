how to build 

1)install python 2.7
https://www.python.org/download/releases/2.7/
2)add the python dir to the env var PATH, use the bat (it expect python to be in C:\python27
./setupPython/addPythonDirToEnvPath.bat
3)download libs for python (pyside, pysintaller ...), use the bat
./setupPython/configPython.bat
4)build the exe, use the bat
./build.bat

the build result should be now in the new created dir "dist"


for developing recommended IDE :
https://www.jetbrains.com/pycharm/download/#section=windows

