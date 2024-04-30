#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>
#include <QThread>

#include "MainWindow.h"
#include "ConfigManager.h"
#include "LogManager.h"

int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
	QCoreApplication::setOrganizationName("BeyondXin");
	QCoreApplication::setApplicationName("RepoReporter");

	QApplication app(argc, argv);

	QThread::currentThread()->setObjectName("Main");

	LogManager logMar;

	Q_UNUSED(logMar)

	QSize lastSize = ConfigManager::GetInstance().ReadValue("LastWindowSize").toSize();
	QPoint lastPosition = ConfigManager::GetInstance().ReadValue("LastWindowPosition").toPoint();

	MainWindow w;

	if (lastSize.isValid()) {
		w.resize(lastSize);
	} else {
		QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
		QRect    screenRect = screen->geometry();
		QSize    screenSize = screenRect.size();
		QSize    windowSize = screenSize / 2;

		w.resize(windowSize);
	}

	if (!lastPosition.isNull()) {
		w.move(lastPosition);
	} else {
		QScreen* screen = QGuiApplication::screenAt(QCursor::pos());
		QRect    screenRect = screen->geometry();

		w.move(screenRect.center() - w.rect().center());
	}
	w.show();

	qInfo() << u8"软件启动。";

	int res = app.exec();

	qInfo() << u8"软件关闭。";

	ConfigManager::GetInstance().WriteValue("LastWindowSize", w.size());
	ConfigManager::GetInstance().WriteValue("LastWindowPosition", w.pos());

	return res;
}
