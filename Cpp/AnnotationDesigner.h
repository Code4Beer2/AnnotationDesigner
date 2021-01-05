#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QColorDialog>
#include <QtWidgets/QGraphicsPixmapItem>
#include <QtGui/QUndoStack>
#include <QtGui/QAction>
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
#include <QtWidgets/QFileDialog>
#include <QImageWriter>
#include <QImageReader>

#include <QtCore/QSettings>
#include <QtCore/QVector>
#include <QtCore/QMimeData>
#include <QtWidgets/QGraphicsItem>
#include "QPixmap"
#include "qnamespace.h"
#include "QAbstractItemModel"
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include "QString"
#include "optional"
#include "QPalette"
#include "QFont"
#include "QColor"
#include "QRect"
#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QComboBox>
#include <functional>


class TextItem : public QGraphicsTextItem
{
	typedef QGraphicsTextItem Super;

	Q_OBJECT
public:
	static const QEvent::Type ourBeginEditableEventType;

	TextItem(QGraphicsItem* aParent = nullptr);
	~TextItem();

	void focusOutEvent(QFocusEvent* aFocusEvent) override;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* aMouseEvent) override;
	bool event(QEvent* anEvent) override;
};

class GraphicsView : public QGraphicsView
{
	typedef QGraphicsView Super;

	Q_OBJECT
public:
	std::function<void(const QString& aImagePath)> my_imageDropCallback;
	std::function<void(const QString& aText, const QPoint& aPos) > my_textDropCallback;
	std::function<void(int aDelta)> my_imageWheelZoomCallback;
	std::function<void(const QMouseEvent* aMouseEvent)> my_mouseRightClickCallback;

	GraphicsView();
	~GraphicsView();

	void mouseReleaseEvent(QMouseEvent* aMouseEvent) override;
	void wheelEvent(QWheelEvent* aWheelEvent) override;
	void dragEnterEvent(QDragEnterEvent* aDragEnterEvent) override;
	void dragMoveEvent(QDragMoveEvent* aDragMoveEvent) override;
	void dropEvent(QDropEvent* aDropEvent) override;
};

class ListWidget : public QListWidget
{
	typedef QListWidget Super;

	Q_OBJECT
public:
	ListWidget(QWidget* aParent = nullptr);

	QMimeData* mimeData(const QList<QListWidgetItem*>& items) const override;
};

class AnnotationDesigner : public QMainWindow
{
	typedef QMainWindow Super;
	Q_OBJECT

public:

	AnnotationDesigner(QWidget* aParent = nullptr);

protected:
	static QString getImageFormatWildcards(const QList<QByteArray>& imageFormats, bool splittedValue);
	static QString getReadImageFormatWildcards();
	static QString getSaveImageFormatWildcards();

	static const QVector<int> ourZoomValues;

	QMenu* getRecentImagesMenu();
	QMenu* getPredefinedAnnotationsMenu();

	void updateColorAndFontWithSelectionValues();

	QSettings getSettings();
	QString  getUniqueNewAnnotationText(const QString& text);
	void addAnnotationTextItem(const QString& text, const QPoint& viewPos);
	void addBackgroundImageItem(const QPixmap& pixmap);
	void closeEvent(QCloseEvent* anEvent) override;
	void createDefaultBackgroundImage();
	void initActions();
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
	void onViewMouseRightClick(const QMouseEvent* anEvent);
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

private:
	QColorDialog* my_colorDialog;
	int my_userZoomIndex = 0;
	QColor my_userColor;
	QFont my_userFont;
	QRect my_mainWindowRect;
	std::optional<QPoint> my_colorDialogPosition;
	QString my_lastSaveFolder;
	QString my_lastSaveExt;
	QGraphicsPixmapItem* my_backgroundImageItem;
	QPoint my_lastViewMouseRightClickPos;
	QStringList my_userPredefinedAnnotations;
	QStringList my_userRecentImagesPaths;

	QAction* myZoomInAction = nullptr;
	QAction* myZoomOutAction = nullptr;
	QAction* my_showAboutAction = nullptr;
	QAction* my_selectAllItemsAction = nullptr;
	QAction* my_addAnnotationItemAction = nullptr;

	QAction* my_loadBackgroundImageAction = nullptr;
	QAction* my_clearAllAnnotationsItemAction = nullptr;
	QAction* my_exitAction = nullptr;
	QAction* my_saveAction = nullptr;
	QAction* my_recentImagesAction = nullptr;

	QGraphicsScene* my_scene = nullptr;
	GraphicsView* my_view = nullptr;

	QPushButton* my_colorButton = nullptr;
	QFontComboBox* my_fontFamilyComboBox = nullptr;
	QComboBox* my_fontSizeComboBox = nullptr;
	QComboBox* my_zoomComboBox = nullptr;

	ListWidget* my_annotationsList = nullptr;
};
