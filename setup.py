from cx_Freeze import setup, Executable

# dependencies
build_exe_options = {
    "packages": ["os", "sys", "glob", "re", "atexit", "PySide.QtCore", "PySide.QtGui", "PySide.QtXml"],
    #"include_files": [("main.py")], # this isn't necessary after all
    "excludes": ["Tkinter", "Tkconstants", "tcl"],
    "build_exe": "build"#,
    #"icon": "./example/Resources/Icons/monitor.ico"
}

executable = [
                Executable("main.py",
                base="Win32GUI",
                targetName="WinFlo32.exe")
                #targetDir="build",
                #copyDependentFiles=True)
]

setup(
    name="WinFlo32",
    version="0.1",
    description="WinFlo32", # Using the word "test" makes the exe to invoke the UAC in win7. WTH?
    author="Laurent",
    options={"build_exe": build_exe_options},
    executables=executable,
    requires=['PySide', 'cx_Freeze', 'simplejson']
)