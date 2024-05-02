#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QThread>

#include "window/MainWindow.h"
#include "utils/LogManager.h"

int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
	QCoreApplication::setOrganizationName("BeyondXin");
	QCoreApplication::setApplicationName("RepoReporter");
	QApplication::setQuitOnLastWindowClosed(false);
	QThread::currentThread()->setObjectName("Main");

	QApplication app(argc, argv);

	LogManager logMar;
	Q_UNUSED(logMar)

	MainWindow w;
	w.show();

	int res = app.exec();

	return res;
}
