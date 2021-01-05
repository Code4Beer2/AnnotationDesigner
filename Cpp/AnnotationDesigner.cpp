#include "AnnotationDesigner.h"

#include "qevent.h"
#include <QtWidgets/QGraphicsItem>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QFileDialog>
#include "QMimeData"
#include "QVariant"
#include <QtGui/QTextCursor>


AnnotationDesigner::AnnotationDesigner(QWidget* aParent)
: Super(aParent)
, my_backgroundImageItem(nullptr)
{
	setWindowIcon(QIcon(":/ico/app.ico"));

	//default values
	my_userZoomIndex = ourZoomValues.indexOf(100);
	my_userColor = QColor(255, 0, 0, 255);
	my_userFont = QFont();
	my_mainWindowRect = QRect(300, 300, 400, 600);
	my_colorDialogPosition = QPoint(300, 300);
	my_lastSaveFolder = "./";
	my_lastSaveExt = ".png";

	//my_cmdStack = new QUndoStack();
	my_backgroundImageItem = nullptr;

	for (int i = 1; i < 6; ++i)
		my_userPredefinedAnnotations.append(QString("defaultText%1").arg(i));

	initActions();
	initUI();
	readSettings();

	createDefaultBackgroundImage();
}

QMenu* AnnotationDesigner::getRecentImagesMenu()
{
	QMenu* menu = new QMenu();

	for (const QString& imagePath : my_userRecentImagesPaths)
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

	if (numItem > 0)
	{
		QGraphicsItem* firstItem = selectedItems[0];
		if (TextItem* firstTextItem = dynamic_cast<TextItem*>(firstItem))
		{
			my_userFont = firstTextItem->font();
			my_userColor = firstTextItem->defaultTextColor();

			updateColorDialogAndColorButton();
			updateFontFamilyComboBoxAndSizeComboBox();
		}
	}
}

void AnnotationDesigner::initActions()
{
	setAcceptDrops(true);

	myZoomInAction = new QAction(QString("ZoomIn"), this);
	myZoomInAction->setStatusTip("Zoom in");
	myZoomInAction->setShortcut(QKeySequence::ZoomIn);
	myZoomInAction->setIcon(QIcon((":/ico/zoomIn.png")));
	connect(myZoomInAction, &QAction::triggered, this, &AnnotationDesigner::onZoomInAction);

	myZoomOutAction = new QAction("ZoomOut", this);
	myZoomOutAction->setStatusTip("Zoom out");
	myZoomOutAction->setShortcut(QKeySequence::ZoomOut);
	myZoomOutAction->setIcon(QIcon((":/ico/zoomOut.png")));
	connect(myZoomOutAction, &QAction::triggered, this, &AnnotationDesigner::onZoomOutAction);

	my_showAboutAction = new QAction("About", this);
	my_showAboutAction->setMenuRole(QAction::AboutRole);
	my_showAboutAction->setStatusTip("About the app");
	my_showAboutAction->setIcon(QIcon((":/ico/about.png")));
	connect(my_showAboutAction, &QAction::triggered, this, &AnnotationDesigner::onAboutAction);

	my_selectAllItemsAction = new QAction("Select all", this);
	my_selectAllItemsAction->setShortcut(QKeySequence::SelectAll);
	my_selectAllItemsAction->setStatusTip("Select all annoations");
	my_selectAllItemsAction->setIcon(QIcon((":/ico/selectAll.png")));
	connect(my_selectAllItemsAction, &QAction::triggered, this, &AnnotationDesigner::onSelectAllItemAction);

	my_addAnnotationItemAction = new QAction("Add annotation", this);
	////#my_addTextItemAction->setShortcut("Ctrl+L");
	my_addAnnotationItemAction->setIcon(QIcon((":/ico/add.png")));
	my_addAnnotationItemAction->setStatusTip("Add annotation at mouse position");
	connect(my_addAnnotationItemAction, &QAction::triggered, this, &AnnotationDesigner::onAddTextItemAction);

	my_loadBackgroundImageAction = new QAction("Load background image", this);
	my_loadBackgroundImageAction->setShortcut(QKeySequence("Ctrl+L"));
	my_loadBackgroundImageAction->setStatusTip("Load background image");
	my_loadBackgroundImageAction->setIcon(QIcon((":/ico/open.png")));
	connect(my_loadBackgroundImageAction, &QAction::triggered, this, &AnnotationDesigner::onLoadBackgroundImageAction);

	my_clearAllAnnotationsItemAction = new QAction("Clear all", this);
	my_clearAllAnnotationsItemAction->setShortcut(QKeySequence("Ctrl+C"));
	my_clearAllAnnotationsItemAction->setIcon(QIcon((":/ico/clear.png")));
	my_clearAllAnnotationsItemAction->setStatusTip("Exit application");
	connect(my_clearAllAnnotationsItemAction, &QAction::triggered, this, &AnnotationDesigner::onClearAllAnnotationsItemsAction);

	my_exitAction = new QAction("Exit", this);
	my_exitAction->setShortcut(QKeySequence::Quit);
	my_exitAction->setStatusTip("Exit application");
	my_exitAction->setIcon(QIcon((":/ico/quit.png")));
	connect(my_exitAction, &QAction::triggered, this, &AnnotationDesigner::onExitAction);

	my_saveAction = new QAction("Save", this);
	my_saveAction->setShortcut(QKeySequence::Save);
	my_saveAction->setStatusTip("Save picture");
	my_saveAction->setIcon(QIcon((":/ico/save.png")));
	connect(my_saveAction, &QAction::triggered, this, &AnnotationDesigner::onSaveImageAction);

	my_recentImagesAction = new QAction("Recent Images", this);
	////#my_recentImagesAction->setShortcut(QKeySequence::Save);
	my_recentImagesAction->setStatusTip("Recent images which have been loaded");
	////#my_recentImagesAction->setIcon(QIcon((":/ico/save.png")))
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

	my_view = new GraphicsView();
	my_view->setScene(my_scene);
	my_view->my_imageDropCallback = [&](const QString& anImagePath) { onViewImageDrop(anImagePath); };
	my_view->my_textDropCallback = [&](const QString& aText, const QPoint& aDropPos) { onViewTextDrop(aText, aDropPos); };
// 
 	my_view->my_imageWheelZoomCallback = [&](int aDelta) { onViewImageScrollZoom(aDelta); };
 	my_view->my_mouseRightClickCallback = [&](const QMouseEvent* aMouseEvent) { onViewMouseRightClick(aMouseEvent); };
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
	deleteSelectionAction->setIcon(QIcon((":/ico/delete.png")));
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

// 	QFrame* colorButtonFrame = new QFrame();
// 	colorButtonFrame->setFrameStyle(QFrame::StyledPanel);
// 	colorButtonFrame->setLineWidth(2);
// 	QHBoxLayout* colorButtonFrameLayout = new QHBoxLayout();
// 	colorButtonFrame->setLayout(colorButtonFrameLayout);


	my_colorButton = new QPushButton();
	//colorButtonFrameLayout->addWidget(my_colorButton);
	connect(my_colorButton, &QPushButton::clicked, this, &AnnotationDesigner::onShowColorDialog);
	updateColorDialogAndColorButton();

	my_zoomComboBox = new QComboBox();
	for (int zommValue : ourZoomValues)
		my_zoomComboBox->addItem(QString("%1%").arg(zommValue));

	connect(my_zoomComboBox, qOverload<int>(&QComboBox::activated), this, &AnnotationDesigner::onZoomComboBoxChanged);
	updateZoomComboBox();

	QToolBar* toolbar = addToolBar(tr("Toolbar"));
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
	toolbar->addAction(myZoomInAction);
	toolbar->addAction(myZoomOutAction);
}

void AnnotationDesigner::initAnnotationsDock()
{
	QDockWidget* annotationsDock = new QDockWidget(tr("Annotations"));
	annotationsDock->setObjectName("annotationsDock");
	annotationsDock->setFeatures(QDockWidget::DockWidgetFeatures());
	QWidget* container = new QWidget();
	QVBoxLayout* containerLayout = new QVBoxLayout();
	container->setLayout(containerLayout);;

	QPushButton* addNewTextButton = new QPushButton(container);
	addNewTextButton->setText(tr("Add"));
	addNewTextButton->setIcon(QIcon((":/ico/add.png")));
	connect(addNewTextButton, &QPushButton::clicked, this, &AnnotationDesigner::onAddNewTextButton);
	containerLayout->addWidget(addNewTextButton);

	my_annotationsList = new ListWidget(container);

	QAction* deleteSelectionAction = new QAction(tr("Delete"), my_annotationsList);
	deleteSelectionAction->setShortcut(QKeySequence::Delete);
	deleteSelectionAction->setIcon(QIcon((":/ico/delete.png")));
	deleteSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
	connect(deleteSelectionAction, &QAction::triggered, this, &AnnotationDesigner::onTextListWidgetDeleteSelectionAction);
	my_annotationsList->addAction(deleteSelectionAction);

	QAction* addTextAction = new QAction(tr("Add"), my_annotationsList);
	addTextAction->setShortcut(QKeySequence("+"));
	addTextAction->setIcon(QIcon((":/ico/add.png")));
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

void AnnotationDesigner::onAboutAction()
{
	//Popup a box with about message.//
	const QString aboutTitle = tr("About %1").arg(qApp->applicationName());
	
	QString aboutText = tr("Version %1\nMade by %2\nplatform\nQt %3")
		.arg(qApp->applicationVersion(), qApp->organizationName(), qVersion());

	aboutText += "\t\t\t\t\t\t\t"; // stupid padding to force QMessageBox to get bigger width
	QMessageBox::about(this, aboutTitle, aboutText);
}

void AnnotationDesigner::zoomIn()
{
	my_userZoomIndex = qMin(my_userZoomIndex + 1, (ourZoomValues.length()) - 1);
	updateViewScale();
	updateZoomComboBox();
}

void AnnotationDesigner::zoomOut()
{
	my_userZoomIndex = qMax(my_userZoomIndex - 1, 0);
	updateViewScale();
	updateZoomComboBox();
}

void AnnotationDesigner::onViewMouseRightClick(const QMouseEvent* anEvent)
{
	my_lastViewMouseRightClickPos = anEvent->pos();
}

void AnnotationDesigner::onViewImageScrollZoom(int aWheelData)
{
	if (aWheelData > 0)
		zoomIn();
	else if (aWheelData < 0)
		zoomOut();
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
	QString newText = getUniqueNewAnnotationText(tr("new"));
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

	while (my_userPredefinedAnnotations.contains(newText))
	{
		newText = QString("%1%2").arg(text).arg(counter);
		counter += 1;
	}

	return newText;
}

void AnnotationDesigner::onAddNewTextButton()
{
	QString newText = getUniqueNewAnnotationText(tr("new"));
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
	painter.drawText(pixmap.rect(), Qt::AlignCenter, tr("<No Image Loaded>"));
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

	if (!my_userRecentImagesPaths.contains(path))
	{
		my_userRecentImagesPaths.insert(0, path);
		if ((my_userRecentImagesPaths.length()) > 10)
		{
			my_userRecentImagesPaths.pop_back();

			updateRecentBackgroundImagesSubMenu();
			const QString msg = tr("image %1 loaded %2*%3").arg(path).arg(pixmap.size().width()).arg(pixmap.size().height());
			statusBar()->showMessage(msg);
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
		if (TextItem* textItem = dynamic_cast<TextItem*>(item))
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
	addAnnotationTextItem(tr("default text"), my_lastViewMouseRightClickPos);
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

	TextItem* textItem = new TextItem();
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
	QString fileName = QFileDialog::getOpenFileName(this, tr("Load image"), "", getReadImageFormatWildcards(), &ext);
	if(!fileName.isEmpty())
	{
		loadBackgroundImage(fileName);
	}
}

QString AnnotationDesigner::getImageFormatWildcards(const QList<QByteArray>& imageFormats, bool splittedValue)
{
	//"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"

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

const QVector<int> AnnotationDesigner::ourZoomValues = { 10, 20, 50, 100, 120, 150, 200, 300, 400 };

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
		statusBar()->showMessage(tr("no background image"));
		return;
	}

	QString ext = "*.jpg";
	QString saveFilename = QFileDialog::getSaveFileName(this, 
		tr("Save file"),
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

	const bool saved = savedPixmap.save(saveFilename);
	savedPixmapPainter.end();

	if(saved)
		statusBar()->showMessage(tr("image saved to \"%1\"").arg(saveFilename));
	else
		statusBar()->showMessage(tr("failed to save to \"%1\"").arg(saveFilename));
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
		if (TextItem* textItem = dynamic_cast<TextItem*>(item))
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
	const int zoomPercent = ourZoomValues[my_userZoomIndex];
	const float zoomNormalized =(float)zoomPercent / 100.f;

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
	QSettings settings = getSettings();

	settings.setValue("mainWindowGeometry", saveGeometry());
	settings.setValue("mainWindowState", saveState());
	
	settings.setValue("userColor", my_userColor.rgb());
	settings.setValue("userFont", my_userFont.toString());

	QVariant colorDialogPositionValue;
	if (my_colorDialogPosition.has_value())
		colorDialogPositionValue = my_colorDialogPosition.value();

	settings.setValue("colorDialogPosition", colorDialogPositionValue);
	settings.setValue("lastSaveFolder", my_lastSaveFolder);
	settings.setValue("userPredefinedAnnotations", my_userPredefinedAnnotations);
	settings.setValue("userRecentImages", my_userRecentImagesPaths);
}

void AnnotationDesigner::readSettings()
{
	QSettings settings = getSettings();

	const QVariant mainwindowGeometryValue = settings.value("mainWindowGeometry");
	if(mainwindowGeometryValue.type() == QVariant::ByteArray)
		restoreGeometry(mainwindowGeometryValue.toByteArray());

	const QVariant mainWindowStateValue = settings.value("mainWindowState");
	if (mainWindowStateValue.type() == QVariant::ByteArray)
		restoreState(mainWindowStateValue.toByteArray());

	const QVariant userColorValue = settings.value("userColor");
	if (userColorValue.type() == QVariant::Int)
		my_userColor = userColorValue.toInt();

	const QVariant userFontValue = settings.value("userFont");
	if (userFontValue.type() == QVariant::String)
		my_userFont.fromString(userFontValue.toString());

	my_colorDialogPosition = my_colorDialogPosition;
	QVariant colorDialogPositionValue = settings.value("colorDialogPosition");
	if (colorDialogPositionValue.type() == QVariant::Point)
		my_colorDialogPosition = colorDialogPositionValue.toPoint();
		
	const QVariant lastSaveFolderValue = settings.value("lastSaveFolder");
	if (lastSaveFolderValue.type() == QVariant::String)
		my_lastSaveFolder = lastSaveFolderValue.toString();

	const QVariant userPredefinedAnnotationsValue = settings.value("userPredefinedAnnotations");
	if (userPredefinedAnnotationsValue.type() == QVariant::StringList)
		my_userPredefinedAnnotations = userPredefinedAnnotationsValue.toStringList();

	const QVariant userRecentImagesValue = settings.value("userRecentImages");
	if (userRecentImagesValue.type() == QVariant::StringList)
		my_userRecentImagesPaths = userRecentImagesValue.toStringList();

	updateColorDialogAndColorButton();
	updateFontFamilyComboBoxAndSizeComboBox();
	updateAnnotationsTextList();
	updateAddAnnotationItemActionSubMenu();
	updateRecentBackgroundImagesSubMenu();
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

//////////////////////////////////////////////////////////////////////////
GraphicsView::GraphicsView()
{
	setAcceptDrops(true);
}

GraphicsView::~GraphicsView()
{

}

void GraphicsView::mouseReleaseEvent(QMouseEvent* aMouseEvent)
{
	Super::mouseReleaseEvent(aMouseEvent);

	if (aMouseEvent->button() == Qt::RightButton)
	{
		if (my_mouseRightClickCallback)
		{
			my_mouseRightClickCallback(aMouseEvent);
		}
	}
}

void GraphicsView::wheelEvent(QWheelEvent* aWheelEvent)
{
	const Qt::KeyboardModifiers keyModifiers = aWheelEvent->modifiers();
	if (keyModifiers & Qt::CTRL)
	{
		if (my_imageWheelZoomCallback)
		{
			const QPoint angleDelta = aWheelEvent->angleDelta();
			const int angleDeltaY = angleDelta.y();
		
			my_imageWheelZoomCallback(angleDeltaY);
			aWheelEvent->accept();
		}
	}

	Super::wheelEvent(aWheelEvent);
}

void GraphicsView::dragEnterEvent(QDragEnterEvent* aDragEnterEvent)
{
	const QMimeData* dragEnterEventMimeData = aDragEnterEvent->mimeData();

	if (dragEnterEventMimeData->hasUrls() || dragEnterEventMimeData->hasText())
		aDragEnterEvent->accept();
	else
		aDragEnterEvent->ignore();

//	Super::dragEnterEvent(aDragEnterEvent);
}

void GraphicsView::dragMoveEvent(QDragMoveEvent* aDragMoveEvent)
{
	const QMimeData* dragMoveEventMimeData = aDragMoveEvent->mimeData();

	if (dragMoveEventMimeData->hasUrls() || dragMoveEventMimeData->hasText())
	{
		aDragMoveEvent->setDropAction(Qt::CopyAction);
		aDragMoveEvent->accept();
	}
	else
	{
		aDragMoveEvent->ignore();
	}

//	Super::dragMoveEvent(aDragMoveEvent);
}

void GraphicsView::dropEvent(QDropEvent* aDropEvent)
{
	const QMimeData* dropEventMimeData = aDropEvent->mimeData();

	if (dropEventMimeData->hasUrls())
	{
		aDropEvent->setDropAction(Qt::CopyAction);
		aDropEvent->accept();

		const QUrl firstUrl = dropEventMimeData->urls()[0];
		const QString firstLocalFilePath = firstUrl.toLocalFile();

		if (my_imageDropCallback)
			my_imageDropCallback(firstLocalFilePath);
	}
	else if (dropEventMimeData->hasText())
	{
		aDropEvent->setDropAction(Qt::CopyAction);
		aDropEvent->accept();

		const QString text = dropEventMimeData->text();

		if (my_textDropCallback)
			my_textDropCallback(text, aDropEvent->pos());
	}
	else
	{
		aDropEvent->ignore();
	}
}

//////////////////////////////////////////////////////////////////////////

ListWidget::ListWidget(QWidget* aParent)
	: Super(aParent)
{
	setDragDropMode(QAbstractItemView::DragOnly);
}

QMimeData* ListWidget::mimeData(const QList<QListWidgetItem*>& items) const
{
	QMimeData* data = Super::mimeData(items);

	QListWidgetItem* item = items[0];
	const QString itemText = item->text();

	data->setData(qApp->applicationName(), itemText.toLocal8Bit());
	data->setText(itemText);

	return data;
}


//////////////////////////////////////////////////////////////////////////

const QEvent::Type TextItem::ourBeginEditableEventType = static_cast<QEvent::Type>(QEvent::registerEventType());

TextItem::TextItem(QGraphicsItem* aParent)
: Super(aParent)
{
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);
}

TextItem::~TextItem()
{
	QCoreApplication::instance()->removePostedEvents(this);
}

void TextItem::focusOutEvent(QFocusEvent* aFocusEvent)
{
	QTextCursor cursor = textCursor();
	cursor.movePosition(QTextCursor::End);
	setTextCursor(cursor);
	setTextInteractionFlags(Qt::NoTextInteraction);

	Super::focusOutEvent(aFocusEvent);
}

void TextItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aMouseEvent)
{
	QEvent* beginEditableEvent = new QEvent(ourBeginEditableEventType);
	QCoreApplication::instance()->postEvent(this, beginEditableEvent);

	Super::mouseDoubleClickEvent(aMouseEvent);
}

bool TextItem::event(QEvent* anEvent)
{
	if (anEvent->type() == ourBeginEditableEventType && textInteractionFlags() == Qt::NoTextInteraction)
	{
		setTextInteractionFlags(Qt::TextEditorInteraction);
		// 			#cursor = my_textCursor();
		// 			#cursor.movePosition(QTextCursor.End);
					//#my_setTextCursor(cursor);
		setFocus();

		return true;
	}

	return Super::event(anEvent);
}
