#include <QApplication>
#include <QDebug>
#include <QDesktopWidget>
#include <QScreen>

#include "MainWindow.h"
#include "ConfigManager.h"

int main(int argc, char* argv[])
{
	QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling);
	QCoreApplication::setOrganizationName("BeyondXin");
	QCoreApplication::setApplicationName("RepoReporter");

	QApplication app(argc, argv);

	QSize  lastSize = ConfigManager::GetInstance().ReadValue("LastWindowSize").toSize();
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

	int res = app.exec();

	ConfigManager::GetInstance().WriteValue("LastWindowSize", w.size());
	ConfigManager::GetInstance().WriteValue("LastWindowPosition", w.pos());

	return res;
}
