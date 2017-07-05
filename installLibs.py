import urllib
import os


if not os.path.isdir('tmp'):
    os.mkdir('tmp')
urllib.urlretrieve ("https://bootstrap.pypa.io/get-pip.py", "tmp/get-pip.py")

os.system("python tmp/get-pip.py")
os.system("python -m pip install PySid --upgrade")
os.system("python -m pip install cx_Freeze --upgrade")



