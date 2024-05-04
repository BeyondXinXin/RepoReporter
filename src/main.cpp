#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QThread>

#include "window/MainWindow.h"
#include "utils/LogManager.h"
#include "utils/CommandLineManager.h"

int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
	QCoreApplication::setOrganizationName("BeyondXin");
	QCoreApplication::setApplicationName("RepoReporter");
	QThread::currentThread()->setObjectName("Main");

	QApplication app(argc, argv);

	CommandLineManager::Initial(app);
	QApplication::setQuitOnLastWindowClosed(CommandLineManager::option.debug);

	LogManager logMar;
	Q_UNUSED(logMar)

	MainWindow w;
	w.show();

	int res = app.exec();

	return res;
}
