#include "AnnotationDesigner.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication app(argc, argv);
	app.setOrganizationName("Laurent");
	app.setApplicationName("AnnotationDesigner");
	app.setApplicationVersion("1.12");

	AnnotationDesigner mainWindow;
	mainWindow.show();

	return app.exec();
}
