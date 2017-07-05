import urllib
import os
import subprocess
import sys

pythonPath  = os.path.split(sys.executable)[0]
print pythonPath

#if sys.path
#sys.path.append(pythonPath)

if not os.path.isdir('tmp'):
    os.mkdir('tmp')
urllib.urlretrieve ("https://bootstrap.pypa.io/get-pip.py", "tmp/get-pip.py")

os.system("python tmp/get-pip.py")
os.system("python -m pip install PySid --upgrade")
os.system("python -m pip install cx_Freeze --upgrade")



