#include "AnnotationDesigner.h"
#include "qevent.h"
#include "QGraphicsItem"
#include "QMessageBox"
#include "QFileDialog"
// AnnotationDesigner::AnnotationDesigner(QWidget *parent)
// 	: QMainWindow(parent)
// {
// 	//ui.setupUi(this);
// }

AnnotationDesigner::AnnotationDesigner(QWidget *parent /*= Q_NULLPTR*/)
{
	//super(MainWindow, self).__init__()

	setWindowIcon(QIcon("ico/app.ico"));

	my_zoomValues = { 10, 20, 50, 100, 120, 150, 200, 300, 400 };

	//default values
	my_userZoomIndex = my_zoomValues.indexOf(100);
	my_userColor = QColor(255, 0, 0, 255);
	my_userFont = QFont();
	my_mainWindowRect = QRect(300, 300, 400, 600);
	my_colorDialogPosition = QPoint(300, 300);
	my_lastSaveFolder = "./";
	my_lastSaveExt = ".png";

	my_cmdStack = new QUndoStack();

	my_backgroundImageItem = nullptr;

	//my_lastViewMouseRightClickPos = nullptr;

	for (int i = 1; i < 6; ++i)
		my_userPredefinedAnnotations.append(QString("defaultText%1").arg(i));



	iniActions();
	initUI();
	readSettings();

	createDefaultBackgroundImage();
}

QMenu* AnnotationDesigner::getRecentImagesMenu()
{
// 	if (my_userRecentImages.count() == 0)
// 		return nullptr;

	QMenu* menu = new QMenu();

	for (auto imagePath : my_userRecentImages)
	{
		QAction* action = new QAction(imagePath, menu);
		connect(action, &QAction::triggered, this, [&]() {onLoadRecentImage(imagePath); });
		menu->addAction(action);
	}

	return menu;
}

QMenu* AnnotationDesigner::getPredefinedAnnotationsMenu()
{
	QMenu* menu = new QMenu();
	int index = 1;

	for (auto annotation : my_userPredefinedAnnotations)
	{
		QAction* action = new QAction(annotation, menu);
		if (index < 10)
		{
			action->setShortcut(QString("Ctrl+%1").arg(index));
			connect(action, &QAction::triggered, this, [&]() { onPredefinedAnnotationMenuItem(annotation); });
			menu->addAction(action);
		}
		index += 1;
	}

	return menu;
}

void AnnotationDesigner::updateColorAndFontWithSelectionValues()
{
	//#update UI based on the first item selected
	QList<QGraphicsItem *> selectedItems = my_scene->selectedItems();
	const int numItem = selectedItems.count();

	if (numItem == 1)
	{
		QGraphicsItem * firstItem = selectedItems[0];
		if (QGraphicsTextItem* firstTextItem = dynamic_cast<QGraphicsTextItem*>(firstItem))
		{
			QFont my_userFont = firstTextItem->font();
			QColor my_userColor = firstTextItem->defaultTextColor();

			updateColorDialogAndColorButton();
			updateFontFamilyComboBoxAndSizeComboBox();
		}
	}
}

void AnnotationDesigner::iniActions()
{
	//(\w + )->(\w + )\.connect\((\w + )\);
	//connect($1, &QAction::$2, this, &AnnotationDesigner::$3);

	setAcceptDrops(true);

	zoomInAction = new QAction(QString("ZoomIn"), this);
	zoomInAction->setStatusTip("Zoom in");
	zoomInAction->setShortcut(QKeySequence::ZoomIn);
	zoomInAction->setIcon(QIcon(("ico/zoomIn.png")));
	connect(zoomInAction, &QAction::triggered, this, &AnnotationDesigner::onZoomInAction);

	zoomOutAction = new QAction("ZoomOut", this);
	zoomOutAction->setStatusTip("Zoom out");
	zoomOutAction->setShortcut(QKeySequence::ZoomOut);
	zoomOutAction->setIcon(QIcon(("ico/zoomOut.png")));
	connect(zoomOutAction, &QAction::triggered, this, &AnnotationDesigner::onZoomOutAction);

	my_showAboutAction = new QAction("About", this);
	my_showAboutAction->setMenuRole(QAction::AboutRole);
	my_showAboutAction->setStatusTip("About the app");
	my_showAboutAction->setIcon(QIcon(("ico/about.png")));
	connect(my_showAboutAction, &QAction::triggered, this, &AnnotationDesigner::onAboutAction);

	my_selectAllItemsAction = new QAction("Select all", this);
	my_selectAllItemsAction->setShortcut(QKeySequence::SelectAll);
	my_selectAllItemsAction->setStatusTip("Select all annoations");
	my_selectAllItemsAction->setIcon(QIcon(("ico/selectAll.png")));
	connect(my_selectAllItemsAction, &QAction::triggered, this, &AnnotationDesigner::onSelectAllItemAction);

	my_addAnnotationItemAction = new QAction("Add annotation", this);
	////#my_addTextItemAction->setShortcut("Ctrl+L");
	my_addAnnotationItemAction->setIcon(QIcon(("ico/add.png")));
	my_addAnnotationItemAction->setStatusTip("Add annotation at mouse position");
	connect(my_addAnnotationItemAction, &QAction::triggered, this, &AnnotationDesigner::onAddTextItemAction);

	my_loadBackgroundImageAction = new QAction("Load background image", this);
	my_loadBackgroundImageAction->setShortcut(QKeySequence("Ctrl+L"));
	my_loadBackgroundImageAction->setStatusTip("Load background image");
	my_loadBackgroundImageAction->setIcon(QIcon(("ico/open.png")));
	connect(my_loadBackgroundImageAction, &QAction::triggered, this, &AnnotationDesigner::onLoadBackgroundImageAction);

	my_clearAllAnnotationsItemAction = new QAction("Clear all", this);
	my_clearAllAnnotationsItemAction->setShortcut(QKeySequence("Ctrl+L"));
	my_clearAllAnnotationsItemAction->setIcon(QIcon(("ico/clear.png")));
	my_clearAllAnnotationsItemAction->setStatusTip("Exit application");
	connect(my_clearAllAnnotationsItemAction, &QAction::triggered, this, &AnnotationDesigner::onClearAllAnnotationsItemsAction);


	my_exitAction = new QAction("Exit", this);
	my_exitAction->setShortcut(QKeySequence::Quit);
	my_exitAction->setStatusTip("Exit application");
	my_exitAction->setIcon(QIcon(("ico/quit.png")));
	connect(my_exitAction, &QAction::triggered, this, &AnnotationDesigner::onExitAction);

	my_saveAction = new QAction("Save", this);
	my_saveAction->setShortcut(QKeySequence::Save);
	my_saveAction->setStatusTip("Save picture");
	my_saveAction->setIcon(QIcon(("ico/save.png")));
	connect(my_saveAction, &QAction::triggered, this, &AnnotationDesigner::onSaveImageAction);

	my_recentImagesAction = new QAction("Recent Images", this);
	////#my_recentImagesAction->setShortcut(QKeySequence::Save);
	my_recentImagesAction->setStatusTip("Recent images which have been loaded");
	////#my_recentImagesAction->setIcon(QIcon(("ico/save.png")))
	////#connect(my_recentImagesAction, &QAction::triggered, this, &AnnotationDesigner::onSaveImageAction)
}

void AnnotationDesigner::initUI()
{
	my_colorDialog = new QColorDialog();
	connect(my_colorDialog, &QColorDialog::finished, this, &AnnotationDesigner::onColorDialogFinished);
	connect(my_colorDialog, &QColorDialog::currentColorChanged, this, &AnnotationDesigner::onColorDialogChanged);

	statusBar();

	initSceneAndView();
	initAnnotationsDock();
	initToolBar();
	initMenuBar();

	setGeometry(300, 300, 640, 480);
	QString title = QString("%1 v%2").arg(qApp->applicationName(), qApp->applicationVersion());
	setWindowTitle(title);
}

void AnnotationDesigner::initSceneAndView()
{
	my_scene = new QGraphicsScene();
	connect(my_scene, &QGraphicsScene::selectionChanged, this, &AnnotationDesigner::onSceneSelectionChanged);

	my_view = new QGraphicsView();//GraphicsView(my_scene);
	my_view->setScene(my_scene);
// 		my_view->imageDropCallback = onViewImageDrop;
// 		my_view->textDropCallback = onViewTextDrop;
// 
// 		my_view->imageWheelZoomCallback = onViewImageScrollZoom;
// 		my_view->mouseRightClickCallback = onViewMouseRightClick;
		//#my_view->setRubberBandSelectionMode();
	my_view->setInteractive(true);
	//#my_view->setTransformationAnchor(QGraphicsView.AnchorUnderMouse);
	my_view->setRubberBandSelectionMode(Qt::IntersectsItemShape);
	my_view->setDragMode(QGraphicsView::RubberBandDrag);
	my_view->setContextMenuPolicy(Qt::ActionsContextMenu);
	//#my_view->setStyleSheet("QGraphicsView { background-color: rgb(96.5%, 96.5%, 96.5%); }");
	//#my__noDrag = QGraphicsView.RubberBandDrag;
	//#my__yesDrag = QGraphicsView.ScrollHandDrag;

	QAction* deleteSelectionAction = new QAction("Delete", my_view);
	deleteSelectionAction->setStatusTip("Delete selection");
	deleteSelectionAction->setShortcut(QKeySequence::Delete);
	deleteSelectionAction->setIcon(QIcon(("ico/delete.png")));
	deleteSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(deleteSelectionAction, &QAction::triggered, this, &AnnotationDesigner::onDeleteSelectionAction);

	my_view->addAction(deleteSelectionAction);
	my_view->addAction(my_loadBackgroundImageAction);
	my_view->addAction(my_addAnnotationItemAction);
	my_view->addAction(my_saveAction);
	my_view->addAction(my_recentImagesAction);

	setCentralWidget(my_view);
}

void AnnotationDesigner::initMenuBar()
{
	QMenuBar* mainMenuBar = menuBar();
	QMenu* fileMenu = mainMenuBar->addMenu("&File");
	fileMenu->addAction(my_loadBackgroundImageAction);
	fileMenu->addAction(my_saveAction);
	fileMenu->addSeparator();
	fileMenu->addAction(my_recentImagesAction);
	fileMenu->addSeparator();
	fileMenu->addAction(my_exitAction);

	QMenu* helpMenu = mainMenuBar->addMenu("&Help");
	helpMenu->addAction(my_showAboutAction);
}

void AnnotationDesigner::initToolBar()
{
	my_fontFamilyComboBox = new QFontComboBox();
	my_fontFamilyComboBox->setCurrentFont(my_userFont);
	connect(my_fontFamilyComboBox, &QFontComboBox::currentFontChanged, this, &AnnotationDesigner::onFontComboBoxChanged);

	my_fontSizeComboBox = new QComboBox();
	for (int i = 0; i < 200; ++i)
		my_fontSizeComboBox->addItem(QString("%1").arg(i));

	connect(my_fontSizeComboBox, qOverload<int>(&QComboBox::activated), this, &AnnotationDesigner::onFontSizeComboBoxChanged);

	my_colorButton = new QPushButton();
	connect(my_colorButton, &QPushButton::clicked, this, &AnnotationDesigner::onShowColorDialog);
	updateColorDialogAndColorButton();

	my_zoomComboBox = new QComboBox();
	for (int zommValue : my_zoomValues)
		my_zoomComboBox->addItem(QString("%1%").arg(zommValue));

	connect(my_zoomComboBox, qOverload<int>(&QComboBox::activated), this, &AnnotationDesigner::onZoomComboBoxChanged);
	updateZoomComboBox();

	QToolBar* toolbar = addToolBar("Toolbar");
	toolbar->setObjectName("toolbar");
	toolbar->addAction(my_loadBackgroundImageAction);
	toolbar->addAction(my_exitAction);
	toolbar->addAction(my_saveAction);
	toolbar->addAction(my_clearAllAnnotationsItemAction);
	toolbar->addAction(my_selectAllItemsAction);
	//#toolbar->addAction(my_deleteSelectionAction);
	toolbar->addSeparator();
	toolbar->addWidget(my_fontFamilyComboBox);
	toolbar->addWidget(my_fontSizeComboBox);
	toolbar->addWidget(my_colorButton);
	toolbar->addWidget(my_zoomComboBox);
	toolbar->addAction(zoomInAction);
	toolbar->addAction(zoomOutAction);
}

void AnnotationDesigner::initAnnotationsDock()
{
	QDockWidget* annotationsDock = new QDockWidget("Annotations");
	annotationsDock->setObjectName("annotationsDock");
	annotationsDock->setFeatures(0);
	QWidget* container = new QWidget();
	QVBoxLayout* containerLayout = new QVBoxLayout();
	container->setLayout(containerLayout);;

	QPushButton* addNewTextButton = new QPushButton(container);
	addNewTextButton->setText("Add");
	addNewTextButton->setIcon(QIcon(("ico/add.png")));
	connect(addNewTextButton, &QPushButton::clicked, this, &AnnotationDesigner::onAddNewTextButton);
	containerLayout->addWidget(addNewTextButton);

	my_annotationsList = new ListWidget(container);

	QAction* deleteSelectionAction = new QAction("Delete", my_annotationsList);
	deleteSelectionAction->setShortcut(QKeySequence::Delete);
	deleteSelectionAction->setIcon(QIcon(("ico/delete.png")));
	deleteSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(deleteSelectionAction, &QAction::triggered, this, &AnnotationDesigner::onTextListWidgetDeleteSelectionAction);
	my_annotationsList->addAction(deleteSelectionAction);

	QAction* addTextAction = new QAction("Add", my_annotationsList);
	addTextAction->setShortcut(QKeySequence("+"));
	addTextAction->setIcon(QIcon(("ico/add.png")));
	addTextAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(addTextAction, &QAction::triggered, this, &AnnotationDesigner::onTextListWidgetAddAction);
	my_annotationsList->addAction(addTextAction);

	my_annotationsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
	my_annotationsList->setContextMenuPolicy(Qt::ActionsContextMenu);
	containerLayout->addWidget(my_annotationsList);
	connect(my_annotationsList, &QListWidget::itemChanged, this, &AnnotationDesigner::onTextListItemChanged);

	annotationsDock->setWidget(container);
	addDockWidget(Qt::LeftDockWidgetArea, annotationsDock);

	updateAnnotationsTextList();
}

void AnnotationDesigner::onZoomInAction()
{
	zoomIn();
}

void AnnotationDesigner::onZoomOutAction()
{
	zoomOut();
}


// doesnt work with visual ...
// static const char* getCppVersion()
// {
// 	switch (__cplusplus)
// 	{
// 	case 201703L:	return "C++17\n";
// 	case 201402L:	return "C++14\n";
// 	case 201103L:	return "C++11\n";
// 	case 199711L:	return "C++98\n";
// 	default:		return "pre-standard C++\n";
// 	}
// }

void AnnotationDesigner::onAboutAction()
{
	//Popup a box with about message.//
	QString aboutTitle = QString("About %1").arg(qApp->applicationName());
	QString aboutText = QString("Version %1\nMade by %2\nplatform\nQt %3")
		.arg(qApp->applicationVersion(),qApp->organizationName(), qVersion());

	aboutText += "\t\t\t\t\t\t\t"; // stupid padding to force QMessageBox to get bigger width
	QMessageBox::about(this, aboutTitle, aboutText);
}

void AnnotationDesigner::zoomIn()
{
	my_userZoomIndex = qMin(my_userZoomIndex + 1, (my_zoomValues.length()) - 1);
	updateViewScale();
	updateZoomComboBox();
}

void AnnotationDesigner::zoomOut()
{
	my_userZoomIndex = qMax(my_userZoomIndex - 1, 0);
	updateViewScale();
	updateZoomComboBox();
}

void AnnotationDesigner::onViewMouseRightClick(const QMouseEvent& anEvent)
{
	my_lastViewMouseRightClickPos = anEvent.pos();
}

void AnnotationDesigner::onViewImageScrollZoom(int aWheelData)
{
	if (aWheelData > 0)
	{
		zoomIn();
	}
	else
	{
		zoomOut();
	}
}

void AnnotationDesigner::onViewTextDrop(QString aText, QPoint aViewPos)
{
	addAnnotationTextItem(aText, aViewPos);
}

void AnnotationDesigner::onViewImageDrop(QString imagePath)
{
	loadBackgroundImage(imagePath);
}

void AnnotationDesigner::onTextListWidgetAddAction()
{
	QString newText = getUniqueNewAnnotationText("new");
	my_userPredefinedAnnotations.append(newText);
	updateAnnotationsTextList();
	updateAddAnnotationItemActionSubMenu();
}

void AnnotationDesigner::onTextListWidgetDeleteSelectionAction()
{
	for (QListWidgetItem* selectedItem : my_annotationsList->selectedItems())
	{
		my_userPredefinedAnnotations.removeOne(selectedItem->text());
		updateAnnotationsTextList();
		updateAddAnnotationItemActionSubMenu();
	}
}

QString AnnotationDesigner::getUniqueNewAnnotationText(const QString& text)
{
	QString newText = text;
	int counter = 1;
	for (auto newText : my_userPredefinedAnnotations)
	{
		newText.sprintf("%s%d", text, counter);
		counter += 1;
	}
	return newText;
}

void AnnotationDesigner::onAddNewTextButton()
{
	QString newText = getUniqueNewAnnotationText("new");
	my_userPredefinedAnnotations.append(newText);
	updateAnnotationsTextList();
	updateAddAnnotationItemActionSubMenu();
}

void AnnotationDesigner::onTextListItemChanged(QListWidgetItem* anItem)
{
	int index = my_annotationsList->row(anItem);
	QString newText = anItem->text();
	if (!my_userPredefinedAnnotations.contains(newText))
	{
		my_userPredefinedAnnotations[index] = newText;
		updateAnnotationsTextList();
		updateAddAnnotationItemActionSubMenu();
	}
}

void AnnotationDesigner::updateAnnotationsTextList()
{
	my_annotationsList->clear();
	for (auto text_choice : my_userPredefinedAnnotations)
	{
		QListWidgetItem* item = new QListWidgetItem(text_choice);
		item->setFlags(item->flags() | Qt::ItemIsEditable);
		my_annotationsList->addItem(item);
	}
}

void AnnotationDesigner::addBackgroundImageItem(const QPixmap& pixmap)
{
	//assert my_backgroundImageItem is nullptr
	my_backgroundImageItem = new QGraphicsPixmapItem(pixmap);
	my_backgroundImageItem->setZValue(-100); // image should not be on top of other items;
	my_scene->addItem(my_backgroundImageItem);
	my_scene->setSceneRect(pixmap.rect());
}

void AnnotationDesigner::createDefaultBackgroundImage()
{
	//assert my_backgroundImageItem is nullptr
	QPixmap pixmap = QPixmap(800, 600);
	QPainter painter = QPainter();

	painter.begin(&pixmap);
	painter.setBrush(Qt::gray);
	painter.setPen(Qt::black);
	painter.fillRect(pixmap.rect(), Qt::gray);
	painter.drawText(pixmap.rect(), Qt::AlignCenter, "<No Image Loaded>");
	painter.end();

	addBackgroundImageItem(pixmap);
}

void AnnotationDesigner::updateRecentBackgroundImagesSubMenu()
{
	QMenu* menu = getRecentImagesMenu();
	my_recentImagesAction->setMenu(menu);
	my_recentImagesAction->setEnabled(menu ? true : false);
}

void AnnotationDesigner::loadBackgroundImage(const QString& path)
{
	if (my_backgroundImageItem)
	{
		my_scene->removeItem(my_backgroundImageItem);
		my_backgroundImageItem = nullptr;
	}

		//QImage image = QImage(path);
	QPixmap pixmap = QPixmap(path);

	addBackgroundImageItem(pixmap);

	if (!my_userRecentImages.contains(path))
	{
		my_userRecentImages.insert(0, path);
		if ((my_userRecentImages.length()) > 10)
		{
			my_userRecentImages.pop_back();

			updateRecentBackgroundImagesSubMenu();
			statusBar()->showMessage(QString("image %1 loaded %2*%3").arg(path, pixmap.size().width(), pixmap.size().height()));
		}
	}
}

void AnnotationDesigner::onSelectAllItemAction()
{
	my_scene->clearSelection();
	QList<QGraphicsItem *> allItems = my_scene->items();
	QVector<QGraphicsItem*> selectItems;
	for (QGraphicsItem* item : allItems)
	{
		if (QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(item))
		{
			selectItems.append(item);
			textItem->setSelected(true);
		}
	}
}

void AnnotationDesigner::onShowColorDialog()
{
	my_colorDialog->setCurrentColor(my_userColor);
	if (my_colorDialogPosition)
	{
		my_colorDialog->move(my_colorDialogPosition.value());
		my_colorDialog->raise();
		my_colorDialog->open();

	}
}

void AnnotationDesigner::onAddTextItemAction()
{
	//assert my_lastViewMouseRightClickPos
	addAnnotationTextItem("default text", my_lastViewMouseRightClickPos);
}

void AnnotationDesigner::onPredefinedAnnotationMenuItem(const QString& annotation)
{
	//my_lastViewMouseRightClickPos
	addAnnotationTextItem(annotation, my_lastViewMouseRightClickPos);
}

void AnnotationDesigner::updateAddAnnotationItemActionSubMenu()
{
	QMenu* menu = getPredefinedAnnotationsMenu();
	my_addAnnotationItemAction->setMenu(menu);
	my_addAnnotationItemAction->setEnabled(menu != nullptr);
}

void AnnotationDesigner::onDeleteSelectionAction()
{
	QList<QGraphicsItem *> selectedItems = my_scene->selectedItems();
	for (QGraphicsItem* selectedItem : selectedItems)
	{
		my_scene->removeItem(selectedItem);
	}
}

void AnnotationDesigner::addAnnotationTextItem(const QString& text, const QPoint& viewPos)
{
	QPointF scenePos = my_view->mapToScene(viewPos);

	QGraphicsTextItem* textItem = new QGraphicsTextItem();// TextItem(); TODO
	textItem->setPlainText(text);
	textItem->setFont(my_userFont);
	textItem->setDefaultTextColor(my_userColor);
	textItem->setFlag(QGraphicsItem::ItemIsMovable);
	textItem->setFlag(QGraphicsItem::ItemIsSelectable);
	QPointF centeredPos = QPointF(scenePos.x() - textItem->boundingRect().width() / 2, scenePos.y() - textItem->boundingRect().height() / 2);
	textItem->setPos(centeredPos);

	my_scene->addItem(textItem);
}

void AnnotationDesigner::onSceneSelectionChanged()
{
	updateColorAndFontWithSelectionValues();
}

void AnnotationDesigner::onLoadRecentImage(const QString& imagePath)
{
	loadBackgroundImage(imagePath);
}

void AnnotationDesigner::onLoadBackgroundImageAction()
{
	QString ext = "*.jpg";
	QString fileName = QFileDialog::getOpenFileName(this, "Load image", "", getReadImageFormatWildcards(), &ext);
	if(!fileName.isEmpty())
	{
		loadBackgroundImage(fileName);
	}
}

QString AnnotationDesigner::getImageFormatWildcards(const QList<QByteArray>& imageFormats, bool splittedValue)
{
	//#"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"

	QString formatsWildcards;

	if (splittedValue)
	{
		formatsWildcards = "";
		for (auto imageFormat : imageFormats)
		{
			formatsWildcards += QString("*.%1;;").arg(QString(imageFormat));
		}
	}
	else
	{
		formatsWildcards = "Images (";
		for (auto imageFormat : imageFormats)
		{
			formatsWildcards += QString("*.%1 ").arg(QString(imageFormat));
		}
		formatsWildcards += ");;";
	}

	return formatsWildcards;
}

QString AnnotationDesigner::getSaveImageFormatWildcards()
{
	QList<QByteArray> formats = QImageWriter::supportedImageFormats();
	return getImageFormatWildcards(formats, true);
}

QString AnnotationDesigner::getReadImageFormatWildcards()
{
	QList<QByteArray> formats = QImageReader::supportedImageFormats();
	return getImageFormatWildcards(formats, false);
}

void AnnotationDesigner::onExitAction()
{
	close();
}

void AnnotationDesigner::onSaveImageAction()
{
	if (!my_backgroundImageItem)
	{
		statusBar()->showMessage("no background image");
		return;
	}

// 	static QString getSaveFileName(QWidget *parent = Q_NULLPTR,
// 		const QString &caption = QString(),
// 		const QString &dir = QString(),
// 		const QString &filter = QString(),
// 		QString *selectedFilter = Q_NULLPTR,
// 		Options options = Options());

	QString ext = "*.jpg";
	QString saveFilename = QFileDialog::getSaveFileName(this, 
		"Save file",
		my_lastSaveFolder,
		getSaveImageFormatWildcards(),
		&ext);

	if (saveFilename.isEmpty()) //# user canceled dialog
		return;


	my_lastSaveFolder = saveFilename;

	QRectF saveRect = my_backgroundImageItem->sceneBoundingRect();

	QPixmap savedPixmap = QPixmap(saveRect.width(), saveRect.height());
	QPainter savedPixmapPainter = QPainter(&savedPixmap);
	my_scene->clearSelection(); //#clear selection so we don"t render selection boxes
	my_scene->render(&savedPixmapPainter, saveRect, saveRect);
	//cleanedExt = ext;
	savedPixmap.save(saveFilename, ext.toLatin1());
	savedPixmapPainter.end();

	statusBar()->showMessage(QString("image saved to \"%1\"").arg(saveFilename));
}

void AnnotationDesigner::onClearAllAnnotationsItemsAction()
{
	QList<QGraphicsItem *> items = my_scene->items();
	for (auto item : items)
	{
		if (item != my_backgroundImageItem)
		{
			my_scene->removeItem(item);
		}
	}
}

void AnnotationDesigner::closeEvent(QCloseEvent* anEvent)
{
	disconnect(my_scene, &QGraphicsScene::selectionChanged, this, &AnnotationDesigner::onSceneSelectionChanged);
	my_view->setScene(nullptr);
	my_scene = nullptr;
	my_view = nullptr;
	my_colorDialog->close();
	saveSettings();


	Super::closeEvent(anEvent);
}

void AnnotationDesigner::onColorDialogFinished()
{
	my_colorDialogPosition = my_colorDialog->pos();
}

void AnnotationDesigner::onColorDialogChanged()
{
	my_userColor = my_colorDialog->currentColor();
	updateColorDialogAndColorButton();
	updateSelectionColorAndFont();
}

void AnnotationDesigner::updateSelectionColorAndFont()
{
	for (auto item : my_scene->selectedItems())
	{
		if (QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(item))
		{
			textItem->setDefaultTextColor(my_userColor);
			textItem->setFont(my_userFont);
		}
	}
}

void AnnotationDesigner::onFontComboBoxChanged()
{
	my_userFont.setFamily(my_fontFamilyComboBox->currentFont().family());
	updateSelectionColorAndFont();
}

void AnnotationDesigner::updateViewScale()
{
	float zoomPercent = my_zoomValues[my_userZoomIndex];
	float zoomNormalized = zoomPercent / 100.0;
	QTransform transform;
	transform.scale(zoomNormalized, zoomNormalized);
	my_view->setTransform(transform);
}

void AnnotationDesigner::onZoomComboBoxChanged()
{
	my_userZoomIndex = my_zoomComboBox->currentIndex();
	updateViewScale();
}

void AnnotationDesigner::onFontSizeComboBoxChanged()
{
	const int pointSize = my_fontSizeComboBox->currentText().toInt();
	my_userFont.setPointSize(pointSize);
	updateSelectionColorAndFont();
}

QSettings AnnotationDesigner::getSettings()
{
	return QSettings(qApp->organizationName(), qApp->applicationName());
}

void AnnotationDesigner::saveSettings()
{
// 	QSettings settings = getSettings();
// 	settings.setValue("geometry", saveGeometry());
// 	settings.setValue("windowState", saveState());
// 	settings.setValue("userColor", my_userColor);
// 	settings.setValue("userFont", my_userFont.toString());
// 	if(my_colorDialogPosition.has_value())
// 		settings.setValue("colorDialogPosition", my_colorDialogPosition.value());
// 	settings.setValue("lastSaveFolder", my_lastSaveFolder);
// 	settings.setValue("userPredefinedAnnotations", my_userPredefinedAnnotations);
// 	settings.setValue("userRecentImages", my_userRecentImages);
}

void AnnotationDesigner::readSettings()
{
// 	QSettings settings = getSettings();
// 	restoreGeometry(settings.value("geometry"));
// 	restoreState(settings.value("windowState"));
// 	my_userColor = settings.value("userColor", my_userColor);
// 	QVariant fontString = settings.value("userFont", "");
// 	if (fontString.str)
// 	{
// 		my_userFont.fromString(fontString);
// 		my_colorDialogPosition = settings.value("colorDialogPosition", my_colorDialogPosition);
// 		my_lastSaveFolder = settings.value("lastSaveFolder", my_lastSaveFolder);
// 
// 		my_userPredefinedAnnotations = settings.value("userPredefinedAnnotations", my_userPredefinedAnnotations);
// 
// 		my_userRecentImages = settings.value("userRecentImages", my_userRecentImages);
// 
// 		updateColorDialogAndColorButton();
// 		updateFontFamilyComboBoxAndSizeComboBox();
// 		updateAnnotationsTextList();
// 		updateAddAnnotationItemActionSubMenu();
// 		updateRecentBackgroundImagesSubMenu();
// 	}
}

void AnnotationDesigner::updateFontFamilyComboBoxAndSizeComboBox()
{
	my_fontFamilyComboBox->setCurrentFont(my_userFont);

	int index = my_userFont.pointSize() - 1;
	my_fontSizeComboBox->setCurrentIndex(index);
}

void AnnotationDesigner::updateColorDialogAndColorButton()
{
	my_colorButton->setFlat(true); //# color palette doesnt work if not flat
	QPalette palette = my_colorButton->palette();
	QPalette::ColorRole role = my_colorButton->backgroundRole();
	palette.setColor(role, my_userColor);
	my_colorButton->setPalette(palette);
	my_colorButton->setAutoFillBackground(true);

	my_colorDialog->setCurrentColor(my_userColor);
}

void AnnotationDesigner::updateZoomComboBox()
{
	my_zoomComboBox->setCurrentIndex(my_userZoomIndex);
}
