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
#include <QtWidgets/QStatusBar>
#include <QFileDialog>
#include <QImageWriter>
#include <QImageReader>

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
#include "QPalette"
#include "QFont"
#include "QColor"
#include "QRect"
#include "QGraphicsScene"
#include "QComboBox"

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
	int my_userZoomIndex = 0;
	QColor my_userColor;
	QFont my_userFont;
	QRect my_mainWindowRect;
	std::optional<QPoint> my_colorDialogPosition;
	QString my_lastSaveFolder;
	QString my_lastSaveExt;
	QUndoStack* my_cmdStack = nullptr;
	QGraphicsPixmapItem* my_backgroundImageItem;
	QPoint my_lastViewMouseRightClickPos;
	QVector<QString> my_userPredefinedAnnotations;
	QVector<QString> my_userRecentImages;


	
	QAction* zoomInAction = nullptr;
	QAction* zoomOutAction = nullptr;
	QAction* my_showAboutAction = nullptr;
	QAction* my_selectAllItemsAction = nullptr;
	QAction* my_addAnnotationItemAction = nullptr;

	QAction* my_loadBackgroundImageAction = nullptr;
	QAction* my_clearAllAnnotationsItemAction = nullptr;
	QAction* my_exitAction = nullptr;
	QAction* my_saveAction = nullptr;
	QAction* my_recentImagesAction = nullptr;

	QGraphicsScene* my_scene = nullptr;
	QGraphicsView* my_view = nullptr;

	QPushButton* my_colorButton = nullptr;
	QFontComboBox* my_fontFamilyComboBox = nullptr;
	QComboBox* my_fontSizeComboBox = nullptr;
	QComboBox* my_zoomComboBox = nullptr;

	ListWidget* my_annotationsList = nullptr;

public:
	AnnotationDesigner(QWidget *parent = Q_NULLPTR);

	QMenu* getRecentImagesMenu();
	QMenu* getPredefinedAnnotationsMenu();

	void updateColorAndFontWithSelectionValues();

	QSettings getSettings();
	QString  getUniqueNewAnnotationText(const QString& text);
	static QString getImageFormatWildcards(const QList<QByteArray>& imageFormats, bool splittedValue);
	static QString getReadImageFormatWildcards();
	static QString getSaveImageFormatWildcards();
	void addAnnotationTextItem(const QString& text, const QPoint& viewPos);
	void addBackgroundImageItem(const QPixmap& pixmap);
	void closeEvent(QCloseEvent* anEvent) override;
	void createDefaultBackgroundImage();
	void iniActions();
	void initAnnotationsDock();
	void initMenuBar();
	void initSceneAndView();
	void initToolBar();
	void initUI();
	void loadBackgroundImage(const QString& path);
	void onAboutAction();
	void onAddNewTextButton();
	void onAddTextItemAction();
	void onClearAllAnnotationsItemsAction();
	void onColorDialogChanged();
	void onColorDialogFinished();
	void onDeleteSelectionAction();
	void onExitAction();
	void onFontComboBoxChanged();
	void onFontSizeComboBoxChanged();
	void onLoadBackgroundImageAction();
	void onLoadRecentImage(const QString& imagePath);
	void onPredefinedAnnotationMenuItem(const QString& annotation);
	void onSaveImageAction();
	void onSceneSelectionChanged();
	void onSelectAllItemAction();
	void onShowColorDialog();
	void onTextListItemChanged(QListWidgetItem* anItem);
	void onTextListWidgetAddAction();
	void onTextListWidgetDeleteSelectionAction();
	void onViewImageDrop(QString imagePath);
	void onViewImageScrollZoom(int aWheelData);
	void onViewMouseRightClick(const QMouseEvent& anEvent);
	void onViewTextDrop(QString aText, QPoint aViewPos);
	void onZoomComboBoxChanged();
	void onZoomInAction();
	void onZoomOutAction();
	void readSettings();
	void saveSettings();
	void updateAddAnnotationItemActionSubMenu();
	void updateAnnotationsTextList();
	void updateColorDialogAndColorButton();
	void updateFontFamilyComboBoxAndSizeComboBox();
	void updateRecentBackgroundImagesSubMenu();
	void updateSelectionColorAndFont();
	void updateViewScale();
	void updateZoomComboBox();
	void zoomIn();
	void zoomOut();
};
