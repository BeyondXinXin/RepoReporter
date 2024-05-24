#ifndef AUTORUNMANAGER_H
#define AUTORUNMANAGER_H

#include <QString>
#include <QSettings>
#include <QDir>
#include <QFile>
#include <QTextStream>
#include <QDebug>

class AutoRunManager {
public:

	static void SetAutoRun(const QString& appName, const QString& appPath)
	{
#if defined(Q_OS_WIN)
		QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		                   QSettings::NativeFormat);
		settings.setValue(appName, QDir::toNativeSeparators(appPath));
#elif defined(Q_OS_LINUX)
		QString autostartPath = QDir::homePath() + "/.config/autostart";
		QDir    dir;
		if (!dir.exists(autostartPath)) {
			dir.mkpath(autostartPath);
		}

		QFile file(autostartPath + "/" + appName + ".desktop");
		if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
			QTextStream out(&file);
			out << "[Desktop Entry]" << "\n";
			out << "Type=Application" << "\n";
			out << "Exec=" << appPath << "\n";
			out << "Hidden=false" << "\n";
			out << "NoDisplay=false" << "\n";
			out << "X-GNOME-Autostart-enabled=true" << "\n";
			out << "Name=" << appName << "\n";
			file.close();
		} else {
			qDebug() << "Failed to create .desktop file for auto start.";
		}
#else // if defined(Q_OS_WIN)
		qDebug() << "AutoRunManager::SetAutoRun is not implemented for this OS.";
#endif // if defined(Q_OS_WIN)
	}

	static void RemoveAutoRun(const QString& appName)
	{
#if defined(Q_OS_WIN)
		QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run",
		                   QSettings::NativeFormat);
		settings.remove(appName);
#elif defined(Q_OS_LINUX)
		QString autostartPath = QDir::homePath() + "/.config/autostart";
		QFile   file(autostartPath + "/" + appName + ".desktop");
		if (file.exists()) {
			if (file.remove()) {
				qDebug() << ".desktop file removed successfully.";
			} else {
				qDebug() << "Failed to remove .desktop file.";
			}
		}
#else // if defined(Q_OS_WIN)
		qDebug() << "AutoRunManager::RemoveAutoRun is not implemented for this OS.";
#endif // if defined(Q_OS_WIN)
	}
};

#endif // AUTORUNMANAGER_H
