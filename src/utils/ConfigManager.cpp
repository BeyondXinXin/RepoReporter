#include "ConfigManager.h"

#include <QApplication>
#include <QDebug>
#include <QStandardPaths>
#include <QDir>

ConfigManager& ConfigManager::GetInstance()
{
	static ConfigManager instance;

	return instance;
}

QVariant ConfigManager::ReadValue(const QString& key, const QVariant& defaultValue)
{
	return m_settings.value(key, defaultValue);
}

void ConfigManager::WriteValue(const QString& key, const QVariant& value)
{
	m_settings.setValue(key, value);
	m_settings.sync();
}

ConfigManager::ConfigManager()
	: m_settings(QApplication::organizationName(), QApplication::applicationName())
{
	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	QString tmpFilePath = configDir + "/.tmp/";
	QDir    dir(tmpFilePath);
	if (!dir.exists()) {
		if (!dir.mkpath(tmpFilePath)) {
			qInfo() << u8"无法创建缓存目录。";
		}
	}
}
