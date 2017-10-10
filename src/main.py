import os
import sys
import platform
import PySide
from PySide import QtGui
from PySide import QtCore


# get resource path
def getResPath(relative):
    if hasattr(sys, "_MEIPASS"): # if frozen with pyApplication
        return os.path.join(sys._MEIPASS, relative)
    return os.path.join(relative)

beginEditableEvent = QtCore.QEvent.registerEventType()

class TextItem(QtGui.QGraphicsTextItem):

    def __init__(self, parent=None, scene=None):
        super(TextItem, self).__init__(parent, scene)
        #self.setFocusProxy(QtGui.QTextEdit())

        self.setFlag(QtGui.QGraphicsItem.ItemIsMovable)
        self.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)

    def focusOutEvent(self, event):
        cursor = self.textCursor()
        cursor.movePosition(QtGui.QTextCursor.End)
        self.setTextCursor(cursor)
        self.setTextInteractionFlags(QtCore.Qt.NoTextInteraction)
        super(TextItem, self).focusOutEvent(event)

    #def keyReleaseEvent(self, event):
    #    print 'keyReleaseEvent'
    #    goEditable = QtCore.QEvent(QtCore.QEvent.Type(eventId))
    #    QtCore.QCoreApplication.instance().postEvent(self, goEditable)
    #    super(TextItem, self).keyReleaseEvent(event)

    def mouseDoubleClickEvent(self, event):
        goEditable = QtCore.QEvent(QtCore.QEvent.Type(beginEditableEvent))
        QtCore.QCoreApplication.instance().postEvent(self, goEditable)
        super(TextItem, self).mouseDoubleClickEvent(event)


    def event(self, ev):
        if ev.type() == beginEditableEvent and self.textInteractionFlags() == QtCore.Qt.NoTextInteraction:
            #print 'goeditable'
            self.setTextInteractionFlags(QtCore.Qt.TextEditorInteraction)
            #cursor = self.textCursor();
            #cursor.movePosition(QtGui.QTextCursor.End);
            #self.setTextCursor(cursor);
            self.setFocus();
            return True

        return super(TextItem, self).event(ev)


class AddTextItemCmd(QtGui.QUndoCommand):

    def __init__(self, scene):
        super(AddTextItemCmd, self).__init__(scene)


class ListWidget(QtGui.QListWidget):
    def __init__(self, parent):
        super(ListWidget, self).__init__(parent)

        self.setDragDropMode(QtGui.QAbstractItemView.DragOnly)

    def mimeData(self, items):
        mimeData = super(ListWidget, self).mimeData(items) #crash if not called ..weird

        item = items[0]
        itemText = item.text()

        mimeData.setData(QtGui.qApp.applicationName(), itemText)
        mimeData.setText(itemText)

        return  mimeData

class GraphicsView(QtGui.QGraphicsView):

    imageDropCallback = None
    textDropCallback = None
    imageWheelZoomCallback = None
    mouseRightClickCallback = None

    def __init__(self, scene):
        super(GraphicsView, self).__init__(scene)

        self.setAcceptDrops(True)


    def mouseReleaseEvent(self, event):
        if event.button() == QtCore.Qt.RightButton:
            if self.mouseRightClickCallback:
                self.mouseRightClickCallback(event)
        super(GraphicsView, self).mouseReleaseEvent(event)

    def wheelEvent(self, event):
        """
        :param event: QtGui.QWheelEvent
        :return: None
        """
        key_mod = event.modifiers()
        if key_mod & QtCore.Qt.CTRL:
            if self.imageWheelZoomCallback:
                self.imageWheelZoomCallback(event.delta())
            event.accept()

        super(GraphicsView, self).wheelEvent(event)

    def dragEnterEvent(self, e):
        #print e.source()
        if e.mimeData().hasUrls() or e.mimeData().hasText():
            e.accept()
        else:
            e.ignore()

    def dragMoveEvent(self, e):
        if e.mimeData().hasUrls() or e.mimeData().hasText():
            e.setDropAction(QtCore.Qt.CopyAction)
            e.accept()
        else:
            e.ignore()

    def dropEvent(self, e):
        if e.mimeData().hasUrls():
            e.setDropAction(QtCore.Qt.CopyAction)
            e.accept()

            firstUrl =  e.mimeData().urls()[0]
            firstLocalFilePath = str(firstUrl.toLocalFile())

            if self.imageDropCallback:
                self.imageDropCallback(firstLocalFilePath)

        elif e.mimeData().hasText():
            e.setDropAction(QtCore.Qt.CopyAction)
            e.accept()

            text =  e.mimeData().text()

            if self.textDropCallback:
                self.textDropCallback(text, e.pos())


        else:
            e.ignore()



#icon found at http://docs.wxwidgets.org/trunk/page_stockitems.html
class MainWindow(QtGui.QMainWindow):
    
    def __init__(self):
        super(MainWindow, self).__init__()

        self.setWindowIcon(QtGui.QIcon(getResPath('ico/app.ico')))

        self.zoomValues = [10, 20, 50, 100, 120, 150, 200, 300, 400]

        #default values
        self.userZoomIndex = self.zoomValues.index(100)
        self.userColor = QtGui.QColor(255, 0, 0, 255)
        self.userFont = QtGui.QFont()
        self.mainWindowRect = QtCore.QRect(300, 300, 400, 600)
        self.colorDialogPosition = QtCore.QPoint(300, 300)
        self.lastSaveFolder = './'
        self.lastSaveExt = '.png'

        self.cmdStack = QtGui.QUndoStack()

        self.backgroundImageItem = None

        self.lastViewMouseRightClickPos = None

        self.userPredefinedAnnotations = list()
        for i in range(1,6):
            self.userPredefinedAnnotations.append('defaultText%d' % i)

        self.userRecentImages = list()

        self.iniActions()
        self.initUI()
        self.readSettings()

        self.createDefaultBackgroundImage()

    def iniActions(self):

        self.setAcceptDrops(True)

        self.zoomInAction = QtGui.QAction('ZoomIn', self)
        self.zoomInAction.setStatusTip('Zoom in')
        self.zoomInAction.setShortcut(QtGui.QKeySequence.ZoomIn)
        self.zoomInAction.setIcon(QtGui.QIcon(getResPath('ico/zoomIn.png')))
        self.zoomInAction.triggered.connect(self.onZoomInAction)
        
        self.zoomOutAction = QtGui.QAction('ZoomOut', self)
        self.zoomOutAction.setStatusTip('Zoom out')
        self.zoomOutAction.setShortcut(QtGui.QKeySequence.ZoomOut)
        self.zoomOutAction.setIcon(QtGui.QIcon(getResPath('ico/zoomOut.png')))
        self.zoomOutAction.triggered.connect(self.onZoomOutAction)

        self.showAboutAction = QtGui.QAction('About', self)
        self.showAboutAction.setMenuRole(QtGui.QAction.AboutRole)
        self.showAboutAction.setStatusTip('About the app')
        self.showAboutAction.setIcon(QtGui.QIcon(getResPath('ico/about.png')))
        self.showAboutAction.triggered.connect(self.onAboutAction)

        self.selectAllItemsAction = QtGui.QAction('Select all', self)
        self.selectAllItemsAction.setShortcut(QtGui.QKeySequence.SelectAll)
        self.selectAllItemsAction.setStatusTip('Select all annoations')
        self.selectAllItemsAction.setIcon(QtGui.QIcon(getResPath('ico/selectAll.png')))
        self.selectAllItemsAction.triggered.connect(self.onSelectAllItemAction)

        self.addAnnotationItemAction = QtGui.QAction('Add annotation', self)
        #self.addTextItemAction.setShortcut('Ctrl+L')
        self.addAnnotationItemAction.setIcon(QtGui.QIcon(getResPath('ico/add.png')))
        self.addAnnotationItemAction.setStatusTip('Add annotation at mouse position')
        self.addAnnotationItemAction.triggered.connect(self.onAddTextItemAction)

        self.loadBackgroundImageAction = QtGui.QAction('Load background image', self)
        self.loadBackgroundImageAction.setShortcut('Ctrl+L')
        self.loadBackgroundImageAction.setStatusTip('Load background image')
        self.loadBackgroundImageAction.setIcon(QtGui.QIcon(getResPath('ico/open.png')))
        self.loadBackgroundImageAction.triggered.connect(self.onLoadBackgroundImageAction)

        self.clearAllAnnotationsItemAction = QtGui.QAction('Clear all', self)
        self.clearAllAnnotationsItemAction.setShortcut('Ctrl+L')
        self.clearAllAnnotationsItemAction.setIcon(QtGui.QIcon(getResPath('ico/clear.png')))
        self.clearAllAnnotationsItemAction.setStatusTip('Exit application')
        self.clearAllAnnotationsItemAction.triggered.connect(self.onClearAllAnnotationsItemsAction)


        self.exitAction = QtGui.QAction('Exit', self)
        self.exitAction.setShortcut(QtGui.QKeySequence.Quit)
        self.exitAction.setStatusTip('Exit application')
        self.exitAction.setIcon(QtGui.QIcon(getResPath('ico/quit.png')))
        self.exitAction.triggered.connect(self.onExitAction)

        self.saveAction = QtGui.QAction('Save', self)
        self.saveAction.setShortcut(QtGui.QKeySequence.Save)
        self.saveAction.setStatusTip('Save picture')
        self.saveAction.setIcon(QtGui.QIcon(getResPath('ico/save.png')))
        self.saveAction.triggered.connect(self.onSaveImageAction)

        self.recentImagesAction = QtGui.QAction('Recent Images', self)
        #self.recentImagesAction.setShortcut(QtGui.QKeySequence.Save)
        self.recentImagesAction.setStatusTip('Recent images which have been loaded')
        #self.recentImagesAction.setIcon(QtGui.QIcon(getResPath('ico/save.png')))
        #self.recentImagesAction.triggered.connect(self.onSaveImageAction)

        
    def initUI(self):
        self.colorDialog = QtGui.QColorDialog()
        self.colorDialog.finished.connect(self.onColorDialogFinished)
        self.colorDialog.currentColorChanged.connect(self.onColorDialogChanged)

        self.statusBar()

        self.initSceneAndView()
        self.initAnnotationsDock()
        self.initToolBar()
        self.initMenuBar()

        self.setGeometry(300, 300, 640, 480)
        title = '%s v%s' % (QtGui.qApp.applicationName(), QtGui.qApp.applicationVersion())
        self.setWindowTitle(title)

    def initSceneAndView(self):
        self.scene = QtGui.QGraphicsScene()
        self.scene.selectionChanged.connect(self.onSceneSelectionChanged)

        self.view = GraphicsView(self.scene)
        self.view.imageDropCallback = self.onViewImageDrop
        self.view.textDropCallback = self.onViewTextDrop

        self.view.imageWheelZoomCallback = self.onViewImageScrollZoom
        self.view.mouseRightClickCallback = self.onViewMouseRightClick
        #self.view.setRubberBandSelectionMode()
        self.view.setInteractive(True)
        #self.view.setTransformationAnchor(QtGui.QGraphicsView.AnchorUnderMouse)
        self.view.setRubberBandSelectionMode(QtCore.Qt.IntersectsItemShape)
        self.view.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
        self.view.setContextMenuPolicy(QtCore.Qt.ActionsContextMenu)
        #self.view.setStyleSheet('QGraphicsView { background-color: rgb(96.5%, 96.5%, 96.5%); }')
        #self._noDrag = QGraphicsView.RubberBandDrag
        #self._yesDrag = QGraphicsView.ScrollHandDrag

        deleteSelectionAction = QtGui.QAction('Delete', self.view)
        deleteSelectionAction.setStatusTip('Delete selection')
        deleteSelectionAction.setShortcut(QtGui.QKeySequence.Delete)
        deleteSelectionAction.setIcon(QtGui.QIcon(getResPath('ico/delete.png')))
        deleteSelectionAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
        deleteSelectionAction.triggered.connect(self.onDeleteSelectionAction)

        self.view.addAction(deleteSelectionAction)
        self.view.addAction(self.loadBackgroundImageAction)
        self.view.addAction(self.addAnnotationItemAction)
        self.view.addAction(self.saveAction)
        self.view.addAction(self.recentImagesAction)

        self.setCentralWidget(self.view)

    def initMenuBar(self):
        mainMenuBar = self.menuBar()
        fileMenu = mainMenuBar.addMenu('&File')
        fileMenu.addAction(self.loadBackgroundImageAction)
        fileMenu.addAction(self.saveAction)
        fileMenu.addSeparator()
        fileMenu.addAction(self.recentImagesAction)
        fileMenu.addSeparator()
        fileMenu.addAction(self.exitAction)

        helpMenu = mainMenuBar.addMenu('&Help')
        helpMenu.addAction(self.showAboutAction)

    def initToolBar(self):
        self.fontFamilyComboBox = QtGui.QFontComboBox()
        self.fontFamilyComboBox.setCurrentFont(self.userFont)
        self.fontFamilyComboBox.currentFontChanged.connect(self.onFontComboBoxChanged)

        self.fontSizeComboBox = QtGui.QComboBox()
        for size in range(1, 200):
            self.fontSizeComboBox.addItem(str(size))
        self.fontSizeComboBox.activated.connect(self.onFontSizeComboBoxChanged)

        self.colorButton = QtGui.QPushButton()
        self.colorButton.clicked.connect(self.onShowColorDialog)
        self.updateColorDialogAndColorButton()

        self.zoomComboBox = QtGui.QComboBox()
        for zoomValue in self.zoomValues:
            self.zoomComboBox.addItem('%d%%' % zoomValue)
        self.zoomComboBox.activated.connect(self.onZoomComboBoxChanged)
        self.updateZoomComboBox()

        toolbar = self.addToolBar('Toolbar')
        toolbar.setObjectName('toolbar')
        toolbar.addAction(self.loadBackgroundImageAction)
        toolbar.addAction(self.exitAction)
        toolbar.addAction(self.saveAction)
        toolbar.addAction(self.clearAllAnnotationsItemAction)
        toolbar.addAction(self.selectAllItemsAction)
        #toolbar.addAction(self.deleteSelectionAction)
        toolbar.addSeparator()
        toolbar.addWidget(self.fontFamilyComboBox)
        toolbar.addWidget(self.fontSizeComboBox)
        toolbar.addWidget(self.colorButton)
        toolbar.addWidget(self.zoomComboBox)
        toolbar.addAction(self.zoomInAction)
        toolbar.addAction(self.zoomOutAction)

    def initAnnotationsDock(self):
        annotationsDock = QtGui.QDockWidget('Annotations')
        annotationsDock.setObjectName('annotationsDock')
        annotationsDock.setFeatures(0)
        container = QtGui.QWidget()
        containerLayout = QtGui.QVBoxLayout()
        container.setLayout(containerLayout);

        addNewTextButton = QtGui.QPushButton(container)
        addNewTextButton.setText('Add')
        addNewTextButton.setIcon(QtGui.QIcon(getResPath('ico/add.png')))
        addNewTextButton.clicked.connect(self.onAddNewTextButton)
        containerLayout.addWidget(addNewTextButton)

        self.annotationsList = ListWidget(container)

        deleteSelectionAction = QtGui.QAction('Delete', self.annotationsList)
        deleteSelectionAction.setShortcut(QtGui.QKeySequence.Delete)
        deleteSelectionAction.setIcon(QtGui.QIcon(getResPath('ico/delete.png')))
        deleteSelectionAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
        deleteSelectionAction.triggered.connect(self.onTextListWidgetDeleteSelectionAction)
        self.annotationsList.addAction(deleteSelectionAction)

        addTextAction = QtGui.QAction('Add', self.annotationsList)
        addTextAction.setShortcut('+')
        addTextAction.setIcon(QtGui.QIcon(getResPath('ico/add.png')))
        addTextAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
        addTextAction.triggered.connect(self.onTextListWidgetAddAction)
        self.annotationsList.addAction(addTextAction)

        self.annotationsList.setSelectionMode(QtGui.QAbstractItemView.ExtendedSelection)
        self.annotationsList.setContextMenuPolicy(QtCore.Qt.ActionsContextMenu)
        containerLayout.addWidget(self.annotationsList)
        self.annotationsList.itemChanged.connect(self.onTextListItemChanged)

        annotationsDock.setWidget(container)
        self.addDockWidget(QtCore.Qt.LeftDockWidgetArea, annotationsDock)

        self.updateAnnotationsTextList()



    def onZoomInAction(self):
        self.zoomIn()

    def onZoomOutAction(self):
        self.zoomOut()

    def onAboutAction(self):
        '''Popup a box with about message.'''
        aboutTitle = 'About %s' % (self.windowTitle())
        aboutText = 'Version "%s"\nMade by "%s"\nplatform "%s"\npython "%s"\npyside "%s"\nQt "%s"' % (QtGui.qApp.applicationVersion(),
                                                                               QtGui.qApp.organizationName(),
                                                                               platform.system(),
                                                                               platform.python_version(),
                                                                               PySide.__version_info__,
                                                                               QtCore.__version_info__)

        aboutText += '\t\t\t\t\t\t\t' # stupid padding to force QMessageBox to get bigger width
        QtGui.QMessageBox.about(self, aboutTitle, aboutText)


    def zoomIn(self):
        self.userZoomIndex = min(self.userZoomIndex + 1, len(self.zoomValues)-1)
        self.updateViewScale()
        self.updateZoomComboBox()

    def zoomOut(self):
        self.userZoomIndex = max(self.userZoomIndex - 1, 0)
        self.updateViewScale()
        self.updateZoomComboBox()

    def onViewMouseRightClick(self, event):
        self.lastViewMouseRightClickPos = event.pos()

    def onViewImageScrollZoom(self, wheelDelta):
        if wheelDelta>0:
            self.zoomIn()
        else:
            self.zoomOut()

    def onViewTextDrop(self, text, viewPos):
        self.addAnnotationTextItem(text, viewPos)

    def onViewImageDrop(self, imagePath):
        self.loadBackgroundImage(imagePath)

    def onTextListWidgetAddAction(self):
        newText = self.getUniqueNewAnnotationText('new')
        self.userPredefinedAnnotations.append(newText)
        self.updateAnnotationsTextList()
        self.updateAddAnnotationItemActionSubMenu()

    def onTextListWidgetDeleteSelectionAction(self):
        for selectedItem in self.annotationsList.selectedItems():
            self.userPredefinedAnnotations.remove(selectedItem.text())
        self.updateAnnotationsTextList()
        self.updateAddAnnotationItemActionSubMenu()

    def getUniqueNewAnnotationText(self, text):
        newText = text
        counter = 1
        while newText in self.userPredefinedAnnotations:
            newText = '%s%d' % (text, counter)
            counter += 1

        return newText

    def onAddNewTextButton(self):
        newText = self.getUniqueNewAnnotationText('new')
        self.userPredefinedAnnotations.append(newText)
        self.updateAnnotationsTextList()
        self.updateAddAnnotationItemActionSubMenu()

    def onTextListItemChanged(self, item):
        modelIndex = self.annotationsList.indexFromItem(item)
        index = modelIndex.row()
        newText = item.text()
        if newText not in self.userPredefinedAnnotations:
            self.userPredefinedAnnotations[index] = newText
        self.updateAnnotationsTextList()
        self.updateAddAnnotationItemActionSubMenu()

    def updateAnnotationsTextList(self):
        self.annotationsList.clear()
        for text_choice in self.userPredefinedAnnotations:
            item = QtGui.QListWidgetItem(text_choice)
            item.setFlags (item.flags() | QtCore.Qt.ItemIsEditable)
            self.annotationsList.addItem(item)


    def addBackgroundImageItem(self, pixmap):
        assert self.backgroundImageItem is None
        self.backgroundImageItem = QtGui.QGraphicsPixmapItem(pixmap)
        self.backgroundImageItem.setZValue(-100) # image should not be on top of other items
        self.scene.addItem(self.backgroundImageItem)
        self.scene.setSceneRect(pixmap.rect())


    def createDefaultBackgroundImage(self):
        assert self.backgroundImageItem is None
        pixmap = QtGui.QPixmap(800, 600)
        painter = QtGui.QPainter()

        painter.begin(pixmap)
        painter.setBrush(QtCore.Qt.gray)
        painter.setPen(QtCore.Qt.black)
        painter.fillRect(pixmap.rect(), QtCore.Qt.gray)
        painter.drawText(pixmap.rect(), QtCore.Qt.AlignCenter, '<No Image Loaded>'),
        painter.end()

        self.addBackgroundImageItem(pixmap)

    def loadBackgroundImage(self, path):
        if self.backgroundImageItem:
            self.scene.removeItem(self.backgroundImageItem)
            self.backgroundImageItem = None

        image =  QtGui.QImage(path)
        pixmap = QtGui.QPixmap(image)

        self.addBackgroundImageItem(pixmap)

        if(path not in self.userRecentImages):
            self.userRecentImages.insert(0, path);
            if(len(self.userRecentImages)>10):
                self.userRecentImages.pop()

        self.updateRecentBackgroundImagesSubMenu()

        self.statusBar().showMessage('image %s loaded %d*%d' % (path, image.size().width(), image.size().height()))


    def onLoadBackgroundImageAction(self):
        openFilename, ext = QtGui.QFileDialog.getOpenFileName(self, 'Load image', '', MainWindow.getReadImageFormatWildcards(), '*.jpg')
        if not openFilename: #user canceled
            return

        self.loadBackgroundImage(openFilename)

    def onLoadRecentImage(self, imagePath):
        self.loadBackgroundImage(imagePath)

    def updateRecentBackgroundImagesSubMenu(self):
        menu = self.getRecentImagesMenu()
        self.recentImagesAction.setMenu(menu)
        self.recentImagesAction.setEnabled(True if menu else False)

    def getRecentImagesMenu(self):
        if len(self.userRecentImages) == 0:
            return None

        menu = QtGui.QMenu()

        for imagePath in self.userRecentImages:
            action = QtGui.QAction(imagePath, menu)
            action.triggered.connect(lambda arg1=imagePath: self.onLoadRecentImage(arg1))
            menu.addAction(action)
        return menu

    def addAnnotationTextItem(self, text, viewPos):

        scenePos = self.view.mapToScene(viewPos)

        textItem = TextItem()
        textItem.setPlainText(text)
        textItem.setFont(self.userFont)
        textItem.setDefaultTextColor(self.userColor)
        textItem.setFlag(QtGui.QGraphicsTextItem.ItemIsMovable)
        textItem.setFlag(QtGui.QGraphicsTextItem.ItemIsSelectable)
        centeredPos = QtCore.QPointF(scenePos.x() - textItem.boundingRect().width()/2, scenePos.y() - textItem.boundingRect().height()/2)
        textItem.setPos(centeredPos)

        self.scene.addItem(textItem)

    def onAddTextItemAction(self):
        assert self.lastViewMouseRightClickPos
        self.addAnnotationTextItem('default text', self.lastViewMouseRightClickPos)

    def onPredefinedAnnotationMenuItem(self, annotation):
        self.lastViewMouseRightClickPos
        self.addAnnotationTextItem(annotation, self.lastViewMouseRightClickPos)

    def updateAddAnnotationItemActionSubMenu(self):
        menu = self.getPredefinedAnnotationsMenu()
        self.addAnnotationItemAction.setMenu(menu)
        self.addAnnotationItemAction.setEnabled(True if menu else False)

    def getPredefinedAnnotationsMenu(self):
        if len(self.userPredefinedAnnotations) == 0:
            return None

        menu = QtGui.QMenu()
        index = 1
        for annotation in self.userPredefinedAnnotations:
            action = QtGui.QAction(annotation, menu)
            if index < 10:
                action.setShortcut('Ctrl+%d'% index)
            action.triggered.connect(lambda arg1=annotation: self.onPredefinedAnnotationMenuItem(arg1))
            menu.addAction(action)
            index += 1
        return menu

    def onDeleteSelectionAction(self):
        selectedItems = self.scene.selectedItems()
        for selectedItem in selectedItems:
            self.scene.removeItem(selectedItem)

    def onSceneSelectionChanged(self):
        self.updateColorAndFontWithSelectionValues()


    def updateColorAndFontWithSelectionValues(self):
        #update UI based on the first item selected
        selectedItems = self.scene.selectedItems()
        numItem = len(selectedItems)
        if numItem == 1:
            firstItem = selectedItems[0]
            if isinstance(firstItem, QtGui.QGraphicsTextItem):
                self.userFont = firstItem.font()
                self.userColor = firstItem.defaultTextColor()

                self.updateColorDialogAndColorButton()
                self.updateFontFamilyComboBoxAndSizeComboBox()

    def onSelectAllItemAction(self):
        self.scene.clearSelection()

        allItems = self.scene.items()
        selectItems = list()
        for item in allItems:
            if isinstance(item, QtGui.QGraphicsTextItem):
                selectItems.append(item)
                item.setSelected(True)

    def onShowColorDialog(self):
        self.colorDialog.setCurrentColor(self.userColor)
        if self.colorDialogPosition:
            self.colorDialog.move(self.colorDialogPosition)
        self.colorDialog.raise_()
        self.colorDialog.open()


    @staticmethod
    def getImageFormatWildcards(imageFormats, splittedValue):
        #'Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)'

        if splittedValue:
            formatsWildcards = ''
            for imageFormat in imageFormats:
                formatsWildcards += ('*.%s;;') % imageFormat
        else:
            formatsWildcards = 'Images ('
            for imageFormat in imageFormats:
                formatsWildcards += ('*.%s ') % imageFormat

            formatsWildcards +=');;'

        return formatsWildcards



    @staticmethod
    def getSaveImageFormatWildcards():
        formats = QtGui.QImageWriter.supportedImageFormats()
        return MainWindow.getImageFormatWildcards(formats, True)

    @staticmethod
    def getReadImageFormatWildcards():
        formats = QtGui.QImageReader.supportedImageFormats()
        return MainWindow.getImageFormatWildcards(formats, False)

    def onExitAction(self):
        self.close()


    def onSaveImageAction(self):
        if not self.backgroundImageItem:
            self.statusBar().showMessage('no background image')
            return

        saveFilename, ext = QtGui.QFileDialog.getSaveFileName(self, 'Save file', self.lastSaveFolder, MainWindow.getSaveImageFormatWildcards(), '*.jpg')
        if not saveFilename: # user canceled dialog
            return


        self.lastSaveFolder = saveFilename

        saveRect = self.backgroundImageItem.sceneBoundingRect()

        savedPixmap = QtGui.QPixmap(saveRect.width(), saveRect.height())
        savedPixmapPainter = QtGui.QPainter(savedPixmap)
        self.scene.clearSelection() #clear selection so we don't render selection boxes
        self.scene.render(savedPixmapPainter, saveRect, saveRect)
        cleanedExt = ext
        savedPixmap.save(saveFilename, cleanedExt)
        savedPixmapPainter.end()

        self.statusBar().showMessage('image saved to "%s"' % saveFilename)

    def onClearAllAnnotationsItemsAction(self):
        items = self.scene.items()
        for item in items:
            if(item is not self.backgroundImageItem):
                self.scene.removeItem(item)

    def closeEvent(self, event):
        self.scene.selectionChanged.disconnect()
        self.view.setScene(None)
        self.scene = None
        self.view = None
        self.colorDialog.close()
        self.saveSettings()

        super(MainWindow,self).closeEvent(event)

    def onColorDialogFinished(self):
        self.colorDialogPosition = self.colorDialog.pos()

    def onColorDialogChanged(self):
        self.userColor = self.colorDialog.currentColor()
        self.updateColorDialogAndColorButton()
        self.updateSelectionColorAndFont()


    def updateSelectionColorAndFont(self):
         for item in self.scene.selectedItems():
            if isinstance(item, QtGui.QGraphicsTextItem):
                item.setDefaultTextColor(self.userColor)
                item.setFont(self.userFont)

    def onFontComboBoxChanged(self):
        self.userFont.setFamily(self.fontFamilyComboBox.currentFont().family())
        self.updateSelectionColorAndFont()


    def updateViewScale(self):
        zoomPercent = self.zoomValues[self.userZoomIndex]
        zoomNormalized = zoomPercent / 100.0
        transform = QtGui.QTransform()
        transform.scale(zoomNormalized, zoomNormalized)
        self.view.setTransform(transform)

    def onZoomComboBoxChanged(self):
        self.userZoomIndex = self.zoomComboBox.currentIndex()
        self.updateViewScale()

    def onFontSizeComboBoxChanged(self):
        pointSize = int(self.fontSizeComboBox.currentText())
        self.userFont.setPointSize(pointSize)
        self.updateSelectionColorAndFont()

    @staticmethod
    def getSettings():
        return QtCore.QSettings(QtGui.qApp.organizationName(), QtGui.qApp.applicationName())

    def saveSettings(self):
        settings = MainWindow.getSettings()
        settings.setValue('geometry', self.saveGeometry())
        settings.setValue('windowState', self.saveState())
        settings.setValue('userColor', self.userColor)
        settings.setValue('userFont', self.userFont.toString())
        settings.setValue('colorDialogPosition', self.colorDialogPosition)
        settings.setValue('lastSaveFolder', self.lastSaveFolder)
        settings.setValue('userPredefinedAnnotations', self.userPredefinedAnnotations)
        settings.setValue('userRecentImages', self.userRecentImages)

    def readSettings(self):
        settings = MainWindow.getSettings()
        self.restoreGeometry(settings.value('geometry'))
        self.restoreState(settings.value('windowState'))
        self.userColor = settings.value('userColor', self.userColor)
        fontString = settings.value('userFont', '')
        if(isinstance(fontString, basestring)):
            self.userFont.fromString(fontString)
        self.colorDialogPosition = settings.value('colorDialogPosition', self.colorDialogPosition)
        self.lastSaveFolder = settings.value('lastSaveFolder', self.lastSaveFolder)

        self.userPredefinedAnnotations = settings.value('userPredefinedAnnotations', self.userPredefinedAnnotations)

        self.userRecentImages = settings.value('userRecentImages', self.userRecentImages)

        self.updateColorDialogAndColorButton()
        self.updateFontFamilyComboBoxAndSizeComboBox()
        self.updateAnnotationsTextList()
        self.updateAddAnnotationItemActionSubMenu()
        self.updateRecentBackgroundImagesSubMenu()

    def updateFontFamilyComboBoxAndSizeComboBox(self):
        self.fontFamilyComboBox.setCurrentFont(self.userFont)

        index = self.userFont.pointSize() - 1
        self.fontSizeComboBox.setCurrentIndex(index)

    def updateColorDialogAndColorButton(self):
        self.colorButton.setFlat(True) # color palette doesnt work if not flat
        palette = self.colorButton.palette()
        role = self.colorButton.backgroundRole()
        palette.setColor(role, self.userColor)
        self.colorButton.setPalette(palette)
        self.colorButton.setAutoFillBackground(True)

        self.colorDialog.setCurrentColor(self.userColor)

    def updateZoomComboBox(self):
        self.zoomComboBox.setCurrentIndex(self.userZoomIndex)

def main():
    # when executing from network
    QtCore.QCoreApplication.addLibraryPath(os.path.join(os.path.dirname(QtCore.__file__), "plugins"))
    QtGui.QImageReader.supportedImageFormats()

    app = QtGui.QApplication(sys.argv)
    app.setOrganizationName('Laurent')
    app.setApplicationName('AnnotationDesigner')
    app.setApplicationVersion('1.11')
    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
        main()



