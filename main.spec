# -*- mode: python -*-

block_cipher = None


a = Analysis(['main.py'],
             pathex=['.'],
             hiddenimports=['PySide.QtCore','PySide.QtGui'],
			 datas= [ ('ico', 'ico' ) ],
             hookspath=None,
             runtime_hooks=None,
			 excludes=['PyQt4'],
             cipher=block_cipher)
			 
pyz = PYZ(a.pure, cipher=block_cipher)
exe = EXE(pyz,
          a.scripts,
          a.binaries,
          a.zipfiles,
          a.datas,
          name='AnnotationDesigner',
          version='1',
          debug=False,
		  icon='ico/app.ico',
          strip=None,
          upx=False,
          console=False )