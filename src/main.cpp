#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QThread>

#include "window/MainWindow.h"
#include "utils/LogManager.h"
#include "utils/CommandLineManager.h"
#include "utils/ConfigManager.h"

int main(int argc, char* argv[])
{
	QCoreApplication::setOrganizationName("BeyondXin");
	QCoreApplication::setApplicationName("RepoReporter");
	QThread::currentThread()->setObjectName("Main");

	bool state;
	state = ConfigManager::GetInstance().ReadValue("DisableHighDpiScaling", false).toBool();
	QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling, state);
	state = ConfigManager::GetInstance().ReadValue("UseHighDpiPixmaps", true).toBool();
	QApplication::setAttribute(Qt::AA_UseHighDpiPixmaps,    state);
	state = ConfigManager::GetInstance().ReadValue("EnableHighDpiScaling", true).toBool();
	QApplication::setAttribute(Qt::AA_EnableHighDpiScaling, state);

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
