#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>

#include "MainWindow.h"

int main(int argc, char* argv[])
{
	QApplication app(argc, argv);

	QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);

	MainWindow w;
	QDesktopWidget* desktop = QApplication::desktop();
	QRect screenRect = desktop->screenGeometry();
	QSize screenSize = screenRect.size();
	QSize windowSize = screenSize / 2;

	w.resize(windowSize);
	w.move(screenRect.center() - w.rect().center());
	w.show();

	return app.exec();
}


