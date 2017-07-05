import sys
from PySide import QtGui
from PySide import QtCore

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
            print 'goeditable'
            self.setTextInteractionFlags(QtCore.Qt.TextEditorInteraction)
            #cursor = self.textCursor();
            #cursor.movePosition(QtGui.QTextCursor.End);
            #self.setTextCursor(cursor);
            self.setFocus();
            return True

        return super(TextItem, self).event(ev)


class GraphicView(QtGui.QGraphicsView):

    def __init__(self, scene):
        super(GraphicView, self).__init__(scene)

        self.contextMenuEventCallback = None

    def contextMenuEvent(self, event):
        self.contextMenuEventCallback(event) # redirect event to callback


class AddTextItemCmd(QtGui.QUndoCommand):

    def __init__(self, scene):
        super(GraphicView, self).__init__(scene)


class ListWidget(QtGui.QListWidget):
    def __init__(self, parent):
        super(ListWidget, self).__init__(parent)


    #def

class MainWindow(QtGui.QMainWindow):
    
    def __init__(self):
        super(MainWindow, self).__init__()

        #default values
        self.userZoom = 100
        self.userColor = QtGui.QColor(255, 0, 0, 255)
        self.userFont = QtGui.QFont()
        self.mainWindowRect = QtCore.QRect(300, 300, 400, 600)
        self.colorDialogPosition = QtCore.QPoint(300, 300)
        self.lastSaveFolder = './'
        self.lastSaveExt = '.png'

        self.cmdStack = QtGui.QUndoStack()

        self.backgroundImageItem = None

        self.userAnnotations = list()
        for i in range(1,6):
            self.userAnnotations.append('defaultText%d' % i)

        self.iniActions()
        self.initUI()
        self.readSettings()

    def iniActions(self):
        self.selectAllItemsAction = QtGui.QAction('Select all items', self)
        self.selectAllItemsAction.setShortcut('Ctrl+A')
        self.selectAllItemsAction.setStatusTip('Select all items')
        self.selectAllItemsAction.triggered.connect(self.onSelectAllItemAction)

        self.addTextItemAction = QtGui.QAction('Add text item', self)
        #self.addTextItemAction.setShortcut('Ctrl+L')
        self.addTextItemAction.setStatusTip('Add text item at mouse position')
        self.addTextItemAction.triggered.connect(self.onAddTextItemAction)

        self.loadBackgroundImageAction = QtGui.QAction('Load background image', self)
        self.loadBackgroundImageAction.setShortcut('Ctrl+L')
        self.loadBackgroundImageAction.setStatusTip('Load background image')
        self.loadBackgroundImageAction.triggered.connect(self.onLoadBackgroundImageAction)

        self.clearImageAction = QtGui.QAction('Clear', self)
        self.clearImageAction.setShortcut('Ctrl+L')
        self.clearImageAction.setStatusTip('Exit application')
        self.clearImageAction.triggered.connect(self.onClearImageAction)

        self.exitAction = QtGui.QAction('Exit', self)
        self.exitAction.setShortcut('Ctrl+Q')
        self.exitAction.setStatusTip('Exit application')
        self.exitAction.triggered.connect(self.onExitAction)

        self.saveAction = QtGui.QAction('Save', self)
        self.saveAction.setShortcut('Ctrl+S')
        self.saveAction.setStatusTip('Save picture')
        self.saveAction.triggered.connect(self.onSaveImageAction)

        self.fitToContentAction = QtGui.QAction('Fit', self)
        self.fitToContentAction.setShortcut('Ctrl+F')
        self.fitToContentAction.setStatusTip('Fit view to content')
        self.fitToContentAction.triggered.connect(self.fitToContent)

        self.deleteSelectionAction = QtGui.QAction('Delete', self)
        self.deleteSelectionAction.setStatusTip('Delete selection')
        self.deleteSelectionAction.setShortcut(QtGui.QKeySequence.Delete)
        self.deleteSelectionAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
        self.deleteSelectionAction.triggered.connect(self.onDeleteSelectionAction)

        
    def initUI(self):
        self.colorDialog = QtGui.QColorDialog()
        self.colorDialog.finished.connect(self.onColorDialogFinished)
        self.colorDialog.currentColorChanged.connect(self.onColorDialogChanged)

        self.statusBar()

        self.initSceneAndView()
        self.initAnnotationsDock()
        self.iniToolBar()
        self.initMenuBar()

        self.setGeometry(300, 300, 640, 480)
        self.setWindowTitle('WinFlo32.exe')

    def initSceneAndView(self):
        self.scene = QtGui.QGraphicsScene()
        self.scene.selectionChanged.connect(self.onSceneSelectionChanged)

        self.view = GraphicView(self.scene)
        #self.view.setRubberBandSelectionMode()
        self.view.setInteractive(True)
        #self.view.setTransformationAnchor(QtGui.QGraphicsView.AnchorUnderMouse)
        self.view.setRubberBandSelectionMode(QtCore.Qt.IntersectsItemShape)
        self.view.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
        #self.view.setStyleSheet('QGraphicsView { background-color: rgb(96.5%, 96.5%, 96.5%); }')
        #self._noDrag = QGraphicsView.RubberBandDrag
        #self._yesDrag = QGraphicsView.ScrollHandDrag

        self.view.contextMenuEventCallback = self.onGraphicViewGetContextualMenu
        self.setCentralWidget(self.view)

    def initMenuBar(self):
        menubar = self.menuBar()
        fileMenu = menubar.addMenu('&File')
        fileMenu.addAction(self.loadBackgroundImageAction)
        fileMenu.addAction(self.saveAction)
        fileMenu.addAction(self.exitAction)

    def iniToolBar(self):
        self.fontFamilyComboBox = QtGui.QFontComboBox()
        self.fontFamilyComboBox.setCurrentFont(self.userFont)
        self.fontFamilyComboBox.currentFontChanged.connect(self.onFontComboBoxchanged)

        self.fontSizeComboBox = QtGui.QComboBox()
        for size in range(1, 200):
            self.fontSizeComboBox.addItem(str(size))
        self.fontSizeComboBox.activated.connect(self.onFontSizeComboBoxChanged)

        self.colorButton = QtGui.QPushButton()
        self.colorButton.clicked.connect(self.onShowColorDialog)
        self.updateColorDialogAndColorButton()

        self.zoomComboBox = QtGui.QComboBox()
        self.zoomValues = [10, 20, 50, 100, 120, 150, 200]
        for zoomValue in self.zoomValues:
            self.zoomComboBox.addItem('%d%%' % zoomValue)
        self.zoomComboBox.activated.connect(self.onZoomComboBoxChanged)
        self.updateZoomComboBox()

        toolbar = self.addToolBar('Toolbar')
        toolbar.setObjectName('toolbar')
        toolbar.addAction(self.loadBackgroundImageAction)
        toolbar.addAction(self.exitAction)
        toolbar.addAction(self.saveAction)
        toolbar.addAction(self.clearImageAction)
        toolbar.addAction(self.selectAllItemsAction)
        toolbar.addAction(self.deleteSelectionAction)
        toolbar.addWidget(self.fontFamilyComboBox)
        toolbar.addWidget(self.fontSizeComboBox)
        toolbar.addWidget(self.colorButton)
        toolbar.addAction(self.fitToContentAction)
        toolbar.addWidget(self.zoomComboBox)

    def initAnnotationsDock(self):
        annotationsDock = QtGui.QDockWidget('Annotations')
        annotationsDock.setObjectName('annotationsDock')
        annotationsDock.setFeatures(0)
        container = QtGui.QWidget()
        containerLayout = QtGui.QVBoxLayout()
        container.setLayout(containerLayout);

        addNewTextButton = QtGui.QPushButton(container)
        addNewTextButton.setText('add')
        addNewTextButton.clicked.connect(self.onAddNewTextButton)
        containerLayout.addWidget(addNewTextButton)

        self.annotationsList = QtGui.QListWidget(container)

        deleteSelectionAction = QtGui.QAction('delete', self.annotationsList)
        deleteSelectionAction.setShortcut(QtGui.QKeySequence.Delete)
        deleteSelectionAction.setShortcutContext(QtCore.Qt.WidgetWithChildrenShortcut)
        deleteSelectionAction.triggered.connect(self.onTextListWidgetDeleteSelectionAction)
        self.annotationsList.addAction(deleteSelectionAction)

        addTextAction = QtGui.QAction('add', self.annotationsList)
        addTextAction.setShortcut('+')
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

    def onTextListWidgetAddAction(self):
        newText = self.getUniqueNewAnnotationText('new')
        self.userAnnotations.append(newText)
        self.updateAnnotationsTextList()

    def onTextListWidgetDeleteSelectionAction(self):
        print 'onTextListWidgetDeleteSelectionAction'
        for selectedItem in self.annotationsList.selectedItems():
            self.userAnnotations.remove(selectedItem.text())
        self.updateAnnotationsTextList()

    def getUniqueNewAnnotationText(self, text):
        newText = text
        counter = 1
        while newText in self.userAnnotations:
            newText = '%s%d' % (text, counter)
            counter += 1

        return newText

    def onAddNewTextButton(self):
        newText = self.getUniqueNewAnnotationText('new')
        self.userAnnotations.append(newText)
        self.updateAnnotationsTextList()

    def onTextListItemChanged(self, item):
        modelIndex = self.annotationsList.indexFromItem(item)
        index = modelIndex.row()
        newText = item.text()
        if newText not in self.userAnnotations:
            self.userAnnotations[index] = newText
        self.updateAnnotationsTextList()

    def updateAnnotationsTextList(self):
        self.annotationsList.clear()
        for text_choice in self.userAnnotations:
            item = QtGui.QListWidgetItem(text_choice)
            item.setFlags (item.flags() | QtCore.Qt.ItemIsEditable)
            self.annotationsList.addItem(item)

    def onLoadBackgroundImageAction(self):
        if(self.backgroundImageItem):
            self.scene.removeItem(self.backgroundImageItem)

        openFilename, ext = QtGui.QFileDialog.getOpenFileName(self, 'Load image', '', MainWindow.getReadImageFormatWildcards(), '*.png')
        image =  QtGui.QImage(openFilename)
        pixmap = QtGui.QPixmap(image)

        self.backgroundImageItem = QtGui.QGraphicsPixmapItem(pixmap)
        self.backgroundImageItem.setZValue(-100)
        self.scene.addItem(self.backgroundImageItem)


    def addAnnotationTextItem(self, text):
        mousePos = self.view.mapFromGlobal(QtGui.QCursor.pos())
        scenePos = self.view.mapToScene(mousePos)

        textItem = TextItem()
        textItem.setPlainText(text)
        textItem.setFont(self.userFont)
        textItem.setDefaultTextColor(self.userColor)
        textItem.setFlag(QtGui.QGraphicsTextItem.ItemIsMovable)
        textItem.setFlag(QtGui.QGraphicsTextItem.ItemIsSelectable)
        textItem.setPos(scenePos)

        self.scene.addItem(textItem)

    def onAddTextItemAction(self):
        self.addAnnotationTextItem('default text')

    def onPredifinedAnnotationMenuItem(self, annotation):
        print annotation
        self.addAnnotationTextItem(annotation)

    def getPredifinedAnnotationsMenu(self):
        menu = QtGui.QMenu('SUB MENU')
        for annotation in self.userAnnotations:
            action = QtGui.QAction(annotation, menu)
            action.triggered.connect(lambda arg=annotation: self.onPredifinedAnnotationMenuItem(arg))
            menu.addAction(action)
        return menu

    def onGraphicViewGetContextualMenu(self, event):
        subMenu = self.getPredifinedAnnotationsMenu()
        self.addTextItemAction.setMenu(subMenu)

        menu = QtGui.QMenu(self)
        menu.addAction(self.loadBackgroundImageAction)
        menu.addAction(self.addTextItemAction)
        menu.exec_(event.globalPos())


    def onDeleteSelectionAction(self):
        selectedItems = self.scene.selectedItems()
        for selectedItem in selectedItems:
            self.scene.removeItem(selectedItem)

    def onSceneSelectionChanged(self):
        self.updateUIWithSelectionValues()


    def updateUIWithSelectionValues(self):
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


    def getContentRect(self):
        totalRect = None
        items = None

        if(self.scene.selectedItems()):
            items = self.scene.selectedItems()
        else:
            items = self.scene.items()

        for item in items:
            itemRect = item.boundingRect()
            if totalRect == None:
                totalRect = itemRect
            else:
                totalRect = itemRect.united(totalRect)

    def fitToContent(self):
        contentRect = self.getContentRect()
        if(contentRect):
            self.view.fitInView(contentRect)



    @staticmethod
    def getImageFormatWildcards(formats):
        #'Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)'
        formatsWildcards = 'Images ('
        for format in formats:
            formatsWildcards += ('*.%s ') % format

        formatsWildcards +=');;'

        return formatsWildcards

    @staticmethod
    def getSaveImageFormatWildcards():
        formats = QtGui.QImageWriter.supportedImageFormats()
        return MainWindow.getImageFormatWildcards(formats)

    @staticmethod
    def getReadImageFormatWildcards():
        formats = QtGui.QImageReader.supportedImageFormats()
        return MainWindow.getImageFormatWildcards(formats)

    def onExitAction(self):
        self.close()


    def onSaveImageAction(self):
        imageFormats = QtGui.QImageWriter.supportedImageFormats()
        formatsWildcards = ''
        for imageFormat in imageFormats:
            formatsWildcards += '*.%s;;' % imageFormat


        saveFilename, ext = QtGui.QFileDialog.getSaveFileName(self, 'Save file', self.lastSaveFolder, MainWindow.getSaveImageFormatWildcards(), '*.png')
        self.lastSaveFolder = saveFilename
        print saveFilename
        #items = self.scene.get_target_items()
        totalRect = QtCore.QRect(0, 0, 800, 400)

        #QtGui.QGraphicsPixmapItem.()
        if self.backgroundImageItem:
            totalRect = self.backgroundImageItem.sceneBoundingRect()

        savedPixmap = QtGui.QPixmap(totalRect.width(), totalRect.height())
        savedPixmapPainter = QtGui.QPainter(savedPixmap)
        self.scene.clearSelection() #clear selection so we don't render selection boxes
        self.scene.render(savedPixmapPainter, totalRect, totalRect)
        cleanedExt = ext
        savedPixmap.save(saveFilename, cleanedExt)
        savedPixmapPainter.end()

    def onClearImageAction(self):
        print 'clear'

    def closeEvent(self, event):
        self.scene.selectionChanged.disconnect()
        self.view.setScene(None)
        self.scene = None
        self.view = None
        self.colorDialog.close()
        self.saveSettings()
        QtGui.QMainWindow.closeEvent(self, event)


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

    def onFontComboBoxchanged(self):
        self.userFont.setFamily(self.fontFamilyComboBox.currentFont().family())
        self.updateSelectionColorAndFont()

    def onZoomComboBoxChanged(self):
        zoomPercent = self.zoomValues[self.zoomComboBox.currentIndex()]
        zoomNormalized = zoomPercent / 100.0
        transform = QtGui.QTransform()
        transform.scale(zoomNormalized, zoomNormalized)
        self.view.setTransform(transform)

    def onFontSizeComboBoxChanged(self):
        pointSize = int(self.fontSizeComboBox.currentText())
        self.userFont.setPointSize(pointSize)
        self.updateSelectionColorAndFont()

    @staticmethod
    def getSettings():
        return QtCore.QSettings('FlorentinCorp', 'WinFlo32')

    def saveSettings(self):
        settings = MainWindow.getSettings()
        settings.setValue('geometry', self.saveGeometry())
        settings.setValue('windowState', self.saveState())
        settings.setValue('color', self.userColor)
        settings.setValue('font', self.userFont.toString())
        settings.setValue('colorDialogPosition', self.colorDialogPosition)
        settings.setValue('lastSaveFolder', self.lastSaveFolder)
        settings.setValue('userAnnotations', self.userAnnotations)


    def readSettings(self):
        settings = MainWindow.getSettings()
        self.restoreGeometry(settings.value('geometry'))
        self.restoreState(settings.value('windowState'))
        self.userColor = settings.value('color', self.userColor)
        fontString = settings.value('font', '')
        if(isinstance(fontString, basestring)):
            self.userFont.fromString(fontString)
        self.colorDialogPosition = settings.value('colorDialogPosition', self.colorDialogPosition)
        self.lastSaveFolder = settings.value('lastSaveFolder', self.lastSaveFolder)

        self.userAnnotations = settings.value('userAnnotations', self.userAnnotations)

        self.updateColorDialogAndColorButton()
        self.updateFontFamilyComboBoxAndSizeComboBox()
        self.updateAnnotationsTextList()

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
        index = self.zoomValues.index(self.userZoom)
        self.zoomComboBox.setCurrentIndex(index)

def main():
    
    app = QtGui.QApplication(sys.argv)
    app.setApplicationName('winFlo32')
    app.setApplicationName('winFlo32')
    mainWindow = MainWindow()
    mainWindow.show()
    sys.exit(app.exec_())


if __name__ == '__main__':
        main()



