#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtWidgets/QUndoStack>
#include <QtWidgets/QAction>
#include <QtWidgets/QFontComboBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QGraphicsView>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QDockWidget>

#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QMimeData>
#include "QGraphicsItem"
#include "QPixmap"
#include "qnamespace.h"
#include "QAbstractItemModel"
#include "QAbstractItemView"
#include "QApplication"
#include "QDialog"
#include "QString"
#include "optional"

//#include <optional>
//#include "ui_AnnotationDesigner.h"


/*
class TextItem(QGraphicsTextItem) 
{

	void __init__(self, parent = nullptr, scene = nullptr) 
{
	super(TextItem, self).__init__(parent, scene)
	//#my_setFocusProxy(QTextEdit())

	my_setFlag(QGraphicsItem.ItemIsMovable)
	my_setFlag(QGraphicsItem.ItemIsSelectable)

	void focusOutEvent(self, event) 
{
	cursor = my_textCursor()
	cursor.movePosition(QTextCursor.End)
	my_setTextCursor(cursor)
	my_setTextInteractionFlags(Qt::NoTextInteraction)
	super(TextItem, self).focusOutEvent(event)

	#void keyReleaseEvent(self, event) 
{
	#    print "keyReleaseEvent"
	#    goEditable = QEvent(QEvent.Type(eventId))
	#    QCoreApplication.instance().postEvent(self, goEditable)
	#    super(TextItem, self).keyReleaseEvent(event)

	void mouseDoubleClickEvent(self, event) 
{
	goEditable = QEvent(QEvent.Type(beginEditableEvent))
	QCoreApplication.instance().postEvent(self, goEditable)
	super(TextItem, self).mouseDoubleClickEvent(event)


	void event(self, ev) 
{
	if ev.type() == beginEditableEvent and my_textInteractionFlags() == Qt::NoTextInteraction 
{
		#print "goeditable"
		my_setTextInteractionFlags(Qt::TextEditorInteraction)
		#cursor = my_textCursor();
#cursor.movePosition(QTextCursor.End);
//#my_setTextCursor(cursor);
my_setFocus();
return true

return super(TextItem, self).event(ev)


class AddTextItemCmd(QUndoCommand) 
{

	void __init__(self, scene) 
{
	super(AddTextItemCmd, self).__init__(scene)








	class GraphicsView public QGraphicsView
{

	imageDropCallback = nullptr
	textDropCallback = nullptr
	imageWheelZoomCallback = nullptr
	mouseRightClickCallback = nullptr

	void __init__(self, scene) 
{
	super(GraphicsView, self).__init__(scene)

	my_setAcceptDrops(true)


	void mouseReleaseEvent(self, event) 
{
	if event.button() == Qt::RightButton 
{
		if my_mouseRightClickCallback 
{
			my_mouseRightClickCallback(event)
			super(GraphicsView, self).mouseReleaseEvent(event)

			void wheelEvent(self, event) 
{
			"""
			
{ param event 
{ QWheelEvent
			
{ return 
{ nullptr
			"""
			key_mod = event.modifiers()
			if key_mod & Qt::CTRL 
{
				if my_imageWheelZoomCallback 
{
					my_imageWheelZoomCallback(event.delta())
					event.accept()

					super(GraphicsView, self).wheelEvent(event)

					void dragEnterEvent(self, e) 
{
					#print e.source()
					if e.mimeData().hasUrls() or e.mimeData().hasText() 
{
						e.accept()
					else
{
e.ignore()

void dragMoveEvent(self, e) 
{
	if e.mimeData().hasUrls() or e.mimeData().hasText() 
{
		e.setDropAction(Qt::CopyAction)
		e.accept()
	else 
{
		e.ignore()

		void dropEvent(self, e) 
{
		if e.mimeData().hasUrls() 
{
			e.setDropAction(Qt::CopyAction)
			e.accept()

			firstUrl = e.mimeData().urls()[0]
			firstLocalFilePath = str(firstUrl.toLocalFile())

			if my_imageDropCallback
{
my_imageDropCallback(firstLocalFilePath)

elif e.mimeData().hasText() 
{
	e.setDropAction(Qt::CopyAction)
	e.accept()

	text = e.mimeData().text()

	if my_textDropCallback
{
my_textDropCallback(text, e.pos())


	else
{
e.ignore()


*/


class ListWidget : public QListWidget
{
	typedef QListWidget Super;
public:
	ListWidget(QWidget *parent = Q_NULLPTR)
	{
		setDragDropMode(QAbstractItemView::DragOnly);
	}

	QMimeData *mimeData(const QList<QListWidgetItem*> items) const override
	{
		QMimeData* data = Super::mimeData(items);

		QListWidgetItem* item = items[0];
		QString itemText = item->text();

		data->setData(qApp->applicationName(), itemText.toLocal8Bit());
		data->setText(itemText);

		return data;
	}
};

class AnnotationDesigner : public QMainWindow
{
	typedef QMainWindow Super;

	Q_OBJECT


private:
	QVector<int> my_zoomValues;
	QColorDialog* my_colorDialog;
	int my_userZoomIndex;
	QColor my_userColor;
	QFont my_userFont;
	QRect my_mainWindowRect;
	std::optional<QPoint> my_colorDialogPosition;
	QString my_lastSaveFolder;
	QString my_lastSaveExt;
	QUndoStack* my_cmdStack;
	QGraphicsPixmapItem* my_backgroundImageItem;
	QPoint my_lastViewMouseRightClickPos;
	QVector<QString> my_userPredefinedAnnotations;
	QVector<QString> my_userRecentImages;


	
	QAction* zoomInAction;
	QAction* zoomOutAction;
	QAction* my_showAboutAction;
	QAction* my_selectAllItemsAction;
	QAction* my_addAnnotationItemAction;

	QAction* my_loadBackgroundImageAction;
	QAction* my_clearAllAnnotationsItemAction;
	QAction* my_exitAction;
	QAction* my_saveAction;
	QAction* my_recentImagesAction;

	QGraphicsScene* my_scene;
	QGraphicsView* my_view;

	QPushButton* my_colorButton;
	QFontComboBox* my_fontFamilyComboBox;
	QComboBox* my_fontSizeComboBox;
	QComboBox* my_zoomComboBox;

	ListWidget* my_annotationsList;

public:
	AnnotationDesigner(QWidget *parent = Q_NULLPTR)
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

		for (int i=1; i<6; ++i)
			my_userPredefinedAnnotations.append(QString("defaultText%d").arg(i));



		iniActions();
		initUI();
		readSettings();

		createDefaultBackgroundImage();
	}




	void iniActions()
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


	void initUI()
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
		QString title = QString::asprintf("%s v%s", qApp->applicationName(), qApp->applicationVersion());
		setWindowTitle(title);
	}

	void initSceneAndView()
	{
		my_scene = new QGraphicsScene();
		connect(my_scene, &QGraphicsScene::selectionChanged, this, &AnnotationDesigner::onSceneSelectionChanged);

		my_view = new QGraphicsView();//GraphicsView(my_scene);
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

	void initMenuBar()
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

	void initToolBar() 
	{
		my_fontFamilyComboBox = new QFontComboBox();
		my_fontFamilyComboBox->setCurrentFont(my_userFont);
		connect(my_fontFamilyComboBox, &QFontComboBox::currentFontChanged, this, &AnnotationDesigner::onFontComboBoxChanged);

		my_fontSizeComboBox = new QComboBox();
		for (int i=0; i<200; ++i)
			my_fontSizeComboBox->addItem(QString::asprintf("%d", i));
		
		connect(my_fontSizeComboBox, &QComboBox::activated, this, &AnnotationDesigner::onFontSizeComboBoxChanged);

		my_colorButton = new QPushButton();
		connect(my_colorButton, &QPushButton::clicked, this, &AnnotationDesigner::onShowColorDialog);
		updateColorDialogAndColorButton();

		my_zoomComboBox = new QComboBox();
		for (int zommValue : my_zoomValues)
			my_zoomComboBox->addItem(QString::sprintf("%d%", zommValue));
		
		connect(my_zoomComboBox, &QComboBox::activated, this, &AnnotationDesigner::onZoomComboBoxChanged);
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

	void initAnnotationsDock()
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
		connect(addNewTextButton, &QAction::clicked, this, &AnnotationDesigner::onAddNewTextButton);
		containerLayout->addWidget(addNewTextButton);

		my_annotationsList = new ListWidget(container);

		QAction* deleteSelectionAction = new QAction("Delete", my_annotationsList);
		deleteSelectionAction->setShortcut(QKeySequence::Delete);
		deleteSelectionAction->setIcon(QIcon(("ico/delete.png")));
		deleteSelectionAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		connect(deleteSelectionAction, &QAction::triggered, this, &AnnotationDesigner::onTextListWidgetDeleteSelectionAction);
		my_annotationsList->addAction(deleteSelectionAction);

		QAction* addTextAction = new QAction("Add", my_annotationsList);
		addTextAction->setShortcut("+");
		addTextAction->setIcon(QIcon(("ico/add.png")));
		addTextAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
		connect(addTextAction, &QAction::triggered, this, &AnnotationDesigner::onTextListWidgetAddAction);
		my_annotationsList->addAction(addTextAction);

		my_annotationsList->setSelectionMode(QAbstractItemView::ExtendedSelection);
		my_annotationsList->setContextMenuPolicy(Qt::ActionsContextMenu);
		containerLayout->addWidget(my_annotationsList);
		connect(my_annotationsList, &QAction::itemChanged, this, &AnnotationDesigner::onTextListItemChanged);

		annotationsDock->setWidget(container);
		addDockWidget(Qt::LeftDockWidgetArea, annotationsDock);

		updateAnnotationsTextList();
	}



	void onZoomInAction()
	{
		zoomIn();
	}

	void onZoomOutAction()
	{
		zoomOut();
	}

	void onAboutAction()
	{
// 		//Popup a box with about message.//
// 		QString aboutTitle = "About %s" % (my_windowTitle());
// 		QString aboutText = "Version "%s"\nMade by "%s"\nplatform "%s"\npython "%s"\npyside "%s"\nQt "%s"" % (qApp.applicationVersion(),
// 			qApp.organizationName(),
// 			platform.system(),
// 			platform.python_version(),
// 			PySide.__version_info__,
// 			__version_info__);
// 
// 		aboutText += "\t\t\t\t\t\t\t"; // stupid padding to force QMessageBox to get bigger width
// 		QMessageBox.about(self, aboutTitle, aboutText);
	}


	void zoomIn()
	{
		my_userZoomIndex = min(my_userZoomIndex + 1, len(my_zoomValues) - 1);
		updateViewScale();
		updateZoomComboBox();
	}

	void zoomOut()
	{
		my_userZoomIndex = max(my_userZoomIndex - 1, 0);
		updateViewScale();
		updateZoomComboBox();
	}

	void onViewMouseRightClick(const QMouseEvent& anEvent)
	{
		my_lastViewMouseRightClickPos = event.pos();
	}

	void onViewImageScrollZoom(int wheelDelta)
	{
		if wheelDelta > 0
		{
			zoomIn();
		}
		else
		{
			zoomOut();
		}
	}

	void onViewTextDrop(QString text, QPoint viewPos)
	{
		addAnnotationTextItem(text, viewPos);
	}

	void onViewImageDrop(QString imagePath)
	{
		loadBackgroundImage(imagePath);
	}

	void onTextListWidgetAddAction()
	{
		QString newText = getUniqueNewAnnotationText("new");
		my_userPredefinedAnnotations.append(newText);
		updateAnnotationsTextList();
		updateAddAnnotationItemActionSubMenu();
	}

	void onTextListWidgetDeleteSelectionAction()
	{
		for (QListWidgetItem* selectedItem : my_annotationsList->selectedItems())
		{
			my_userPredefinedAnnotations.remove(selectedItem->text());
			updateAnnotationsTextList();
			updateAddAnnotationItemActionSubMenu();
		}
	}

	void getUniqueNewAnnotationText(const QString& text)
	{
		QString newText = text;
		int counter = 1;
		while (newText in my_userPredefinedAnnotations)
		{
			newText = "%s%d" % (text, counter);
			counter += 1;
		}
		return newText;
	}



	void onAddNewTextButton()
	{
		newText = getUniqueNewAnnotationText("new");
		my_userPredefinedAnnotations.append(newText);
		updateAnnotationsTextList();
		updateAddAnnotationItemActionSubMenu();
	}

	void onTextListItemChanged(QGraphicsTextItem* item)
	{
		modelIndex = my_annotationsList->indexFromItem(item);
		index = modelIndex.row();
		newText = item.text();
		if (newText not in my_userPredefinedAnnotations)
		{
			my_userPredefinedAnnotations[index] = newText;
			updateAnnotationsTextList();
			updateAddAnnotationItemActionSubMenu();
		}
	}

	void updateAnnotationsTextList()
	{
		my_annotationsList->clear();
		for (text_choice in my_userPredefinedAnnotations)
		{
			item = QListWidgetItem(text_choice);
			item.setFlags(item.flags() | Qt::ItemIsEditable);
			my_annotationsList->addItem(item);
		}
	}


	void addBackgroundImageItem(const QPixmap& pixmap)
	{
		//assert my_backgroundImageItem is nullptr
		my_backgroundImageItem = new QGraphicsPixmapItem(pixmap);
		my_backgroundImageItem->setZValue(-100) // image should not be on top of other items;
		my_scene->addItem(my_backgroundImageItem);
		my_scene->setSceneRect(pixmap.rect());
	}


	void createDefaultBackgroundImage()
	{
		//assert my_backgroundImageItem is nullptr
		QPixmap pixmap = QPixmap(800, 600);
		QPainter painter = QPainter();

		painter.begin(pixmap);
		painter.setBrush(Qt::gray);
		painter.setPen(Qt::black);
		painter.fillRect(pixmap.rect(), Qt::gray);
		painter.drawText(pixmap.rect(), Qt::AlignCenter, "<No Image Loaded>");
		painter.end();

		addBackgroundImageItem(pixmap);
	}


	void updateRecentBackgroundImagesSubMenu()
	{
		QMenu* menu = getRecentImagesMenu();
		my_recentImagesAction->setMenu(menu);
		my_recentImagesAction->setEnabled(menu ? true : false);
	}

	void loadBackgroundImage(const QString& path)
	{
		if (my_backgroundImageItem)
		{
			my_scene->removeItem(my_backgroundImageItem);
			my_backgroundImageItem = nullptr;

			QImage image = QImage(path);
			QPixmap pixmap = QPixmap(image);

			addBackgroundImageItem(pixmap);

			if (!my_userRecentImages.contains(path))
			{
				my_userRecentImages.insert(0, path);
				if (len(my_userRecentImages) > 10)
				{
					my_userRecentImages.pop();

					updateRecentBackgroundImagesSubMenu()
					statusBar()->showMessage("image %s loaded %d*%d" % (path, image.size().width(), image.size().height()))
				}
			}
		}
	}

	void onSelectAllItemAction()
	{
		my_scene->clearSelection();
		QList<QGraphicsItem *> allItems = my_scene->items();
		QVector<QGraphicsItem*> selectItems;
		for (QGraphicsItem* item : allItems)
		{
			if (QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(item))
			{
				selectItems.append(item);
				textItem.setSelected(true);
			}
		}
	}

	void onShowColorDialog()
	{
		my_colorDialog->setCurrentColor(my_userColor);
		if (my_colorDialogPosition)
		{
			my_colorDialog->move(my_colorDialogPosition);
			my_colorDialog->raise();
			my_colorDialog->open();

		}
	}

	void onAddTextItemAction()
	{
		//assert my_lastViewMouseRightClickPos
		addAnnotationTextItem("default text", my_lastViewMouseRightClickPos);
	}

	void onPredefinedAnnotationMenuItem(const QString& annotation)
	{
		//my_lastViewMouseRightClickPos
		addAnnotationTextItem(annotation, my_lastViewMouseRightClickPos);
	}

	
	void updateAddAnnotationItemActionSubMenu()
	{
		QMenu* menu = getPredefinedAnnotationsMenu();
		my_addAnnotationItemAction->setMenu(menu);
		my_addAnnotationItemAction->setEnabled(menu != nullptr);
	}

	void onDeleteSelectionAction()
	{
		QList<QGraphicsItem *> selectedItems = my_scene->selectedItems();
		for (QGraphicsItem* selectedItem : selectedItems)
		{
			my_scene->removeItem(selectedItem);
		}
	}
			
	void addAnnotationTextItem(const QString& text, const QPoint& viewPos)
	{
		QPoint scenePos = my_view->mapToScene(viewPos);

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

	void onSceneSelectionChanged()
	{
		updateColorAndFontWithSelectionValues()
	}

	void onLoadRecentImage(const QString& imagePath)
	{
		my_loadBackgroundImage(imagePath);
	}


											
	void onLoadBackgroundImageAction()
	{
		openFilename, ext = QFileDialog.getOpenFileName(self, "Load image", "", getReadImageFormatWildcards(), "*.jpg")
			if not openFilename
			{ 
				//#user canceled
					return

					my_loadBackgroundImage(openFilename)
			}
	}




					QMenu* getRecentImagesMenu()
					{
						if len(my_userRecentImages) == 0
						{
							return nullptr

								menu = QMenu()

								for imagePath in my_userRecentImages
								{
												action = new QAction(imagePath, menu)
												Action->triggered.connect(lambda arg1 = imagePath
								{ onLoadRecentImage(arg1))
												menu.addAction(action)
												return menu
												}
								}
						}
					}





					void getPredefinedAnnotationsMenu()
					{
						if len(my_userPredefinedAnnotations) == 0
						{
							return nullptr

								menu = QMenu()
								index = 1
								for annotation in my_userPredefinedAnnotations
								{
														action = new QAction(annotation, menu)
														if index < 10
														{
															Action->setShortcut("Ctrl+%d"% index)
																Action->triggered.connect(lambda arg1 = annotation
																	{ onPredefinedAnnotationMenuItem(arg1))
																								menu.addAction(action)
																								index += 1
																								return menu


														}
								}
						}
					}



					void updateColorAndFontWithSelectionValues()
					{
						//#update UI based on the first item selected
							selectedItems = my_scene->selectedItems()
							numItem = len(selectedItems)
							if numItem == 1
							{
								firstItem = selectedItems[0]
									if isinstance(firstItem, QGraphicsTextItem)
									{
										my_userFont = firstItem.font()
											my_userColor = firstItem.defaultTextColor()

											updateColorDialogAndColorButton()
											updateFontFamilyComboBoxAndSizeComboBox()
									}
							}
					}



	static void getImageFormatWildcards(const QString& imageFormats, bool splittedValue)
	{
		//#"Images (*.png *.xpm *.jpg);;Text files (*.txt);;XML files (*.xml)"

		if (splittedValue)
		{
			formatsWildcards = "";
			for (imageFormat in imageFormats)
			{
				formatsWildcards += ("*.%s;;") % imageFormat;
			}
		}
		else
		{
			formatsWildcards = "Images (";
			for (imageFormat in imageFormats)
			{
				formatsWildcards += ("*.%s ") % imageFormat;
				formatsWildcards += ");;";
			}
		}

		return formatsWildcards
	}




	static void getSaveImageFormatWildcards()
	{
		formats = QImageWriter.supportedImageFormats();
		return MainWindow.getImageFormatWildcards(formats, true);
	}


	static void getReadImageFormatWildcards()
	{
		formats = QImageReader.supportedImageFormats();
		return MainWindow.getImageFormatWildcards(formats, false);
	}

	void onExitAction()
	{
		Close();
	}

	void onSaveImageAction()
	{
		if (not my_backgroundImageItem)
		{
			my_statusBar().showMessage("no background image");
			return;
		}

		saveFilename, ext = QFileDialog.getSaveFileName(self, "Save file", my_lastSaveFolder, MainWindow.getSaveImageFormatWildcards(), "*.jpg");
			if (not saveFilename) //# user canceled dialog
				return;


		my_lastSaveFolder = saveFilename;

		saveRect = my_backgroundImageItem.sceneBoundingRect();

		savedPixmap = QPixmap(saveRect.width(), saveRect.height());
		savedPixmapPainter = QPainter(savedPixmap);
		my_scene->clearSelection() //#clear selection so we don"t render selection boxes
		my_scene->render(savedPixmapPainter, saveRect, saveRect);
		cleanedExt = ext;
		savedPixmap.save(saveFilename, cleanedExt);
		savedPixmapPainter.end();

		statusbas().showMessage("image saved to "%s"" % saveFilename);
	}

	void onClearAllAnnotationsItemsAction()
	{
		items = my_scene->items();
		for (item in items)
		{
			if (item is not my_backgroundImageItem)
			{
				my_scene->removeItem(item);
			}
		}
	}

	void closeEvent(const QCloseEvent& event)
	{
		my_scene->selectionChanged.disconnect();
		my_view->setScene(nullptr);
		my_scene = nullptr;
		my_view = nullptr;
		my_colorDialog->close();
		saveSettings();
			

		Super::closeEvent(event);
	}

	void onColorDialogFinished()
	{
		my_colorDialogPosition = my_colorDialog->pos()
	}

	void onColorDialogChanged()
	{
		my_userColor = my_colorDialog->currentColor();
		updateColorDialogAndColorButton();
		updateSelectionColorAndFont();
	}



	void updateSelectionColorAndFont()
	{
		for (auto item : my_scene->selectedItems())
		{
			if (QGraphicsTextItem* textItem = dynamic_cast<QGraphicsTextItem*>(item))
			{
				item.setDefaultTextColor(my_userColor);
				item.setFont(my_userFont);
			}
		}
	}

	void onFontComboBoxChanged()
	{
		my_userFont.setFamily(my_fontFamilyComboBox->currentFont().family());
		updateSelectionColorAndFont();
	}


	void updateViewScale()
	{
		float zoomPercent = my_zoomValues[my_userZoomIndex];
		float zoomNormalized = zoomPercent / 100.0;
		transform = QTransform();
		transform.scale(zoomNormalized, zoomNormalized);
		my_view->setTransform(transform);
	}

	void onZoomComboBoxChanged()
	{
		my_userZoomIndex = my_zoomComboBox.currentIndex();
		updateViewScale();
	}

	void onFontSizeComboBoxChanged()
	{
		pointSize = int(my_fontSizeComboBox->currentText());
		my_userFont.setPointSize(pointSize);
		updateSelectionColorAndFont();
	}

	QSettings getSettings()
	{
		return QSettings(qApp.organizationName(), qApp.applicationName());
	}

	void saveSettings()
	{
		QSettings settings = getSettings();
		settings.setValue("geometry", saveGeometry());
		settings.setValue("windowState", saveState());
		settings.setValue("userColor", my_userColor);
		settings.setValue("userFont", my_userFont.toString());
		settings.setValue("colorDialogPosition", my_colorDialogPosition);
		settings.setValue("lastSaveFolder", my_lastSaveFolder);
		settings.setValue("userPredefinedAnnotations", my_userPredefinedAnnotations);
		settings.setValue("userRecentImages", my_userRecentImages);
	}

	void readSettings()
	{
		QSettings settings = getSettings();
		restoreGeometry(settings.value("geometry"));
		restoreState(settings.value("windowState"));
		my_userColor = settings.value("userColor", my_userColor);
		QVariant fontString = settings.value("userFont", "");
		if (fontString.str)
		{
			my_userFont.fromString(fontString);
			my_colorDialogPosition = settings.value("colorDialogPosition", my_colorDialogPosition);
			my_lastSaveFolder = settings.value("lastSaveFolder", my_lastSaveFolder);

			my_userPredefinedAnnotations = settings.value("userPredefinedAnnotations", my_userPredefinedAnnotations);

			my_userRecentImages = settings.value("userRecentImages", my_userRecentImages);

			updateColorDialogAndColorButton();
			updateFontFamilyComboBoxAndSizeComboBox();
			updateAnnotationsTextList();
			updateAddAnnotationItemActionSubMenu();
			updateRecentBackgroundImagesSubMenu();
		}
	}

	void updateFontFamilyComboBoxAndSizeComboBox()
	{
		my_fontFamilyComboBox->setCurrentFont(my_userFont);

		int index = my_userFont.pointSize() - 1;
		my_fontSizeComboBox->setCurrentIndex(index);
	}

	void updateColorDialogAndColorButton()
	{
		my_colorButton->setFlat(true); //# color palette doesnt work if not flat
		QPalette palette = my_colorButton->palette();
		QPalette::ColorRole role = my_colorButton->backgroundRole();
		palette.setColor(role, my_userColor);
		my_colorButton->setPalette(palette);
		my_colorButton->setAutoFillBackground(true);

		my_colorDialog->setCurrentColor(my_userColor);
	}

	void updateZoomComboBox()
	{
		my_zoomComboBox.setCurrentIndex(my_userZoomIndex);
	}
};
