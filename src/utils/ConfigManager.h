#ifndef ConfigManager_H
#define ConfigManager_H

#include <QCoreApplication>
#include <QSettings>

class ConfigManager {
public:

	static ConfigManager& GetInstance();

	QVariant ReadValue(const QString& key, const QVariant& defaultValue = QVariant());
	void WriteValue(const QString& key, const QVariant& value);

	template<typename T>
	void WriteList(const QString& key, const QList<T>& sizes);

	template<typename T>
	QList<T>ReadList(const QString& key, const QList<T>& defaultValue);

private:

	ConfigManager();

	ConfigManager(const ConfigManager&) = delete;
	ConfigManager& operator=(const ConfigManager&) = delete;

private:

	QSettings m_settings;
};

template<typename T>
void ConfigManager::WriteList(const QString& key, const QList<T>& sizes)
{
	QVariantList variantList;

	for (const T& size : sizes) {
		variantList.append(QVariant::fromValue(size));
	}
	WriteValue(key, QVariant::fromValue(variantList));
}

template<typename T>
QList<T>ConfigManager::ReadList(const QString& key, const QList<T>& defaultValue)
{
	QVariantList variantList = ReadValue(key, QVariant::fromValue(QVariantList())).value<QVariantList>();
	QList<T> sizes;

	for (const QVariant& sizeVariant : variantList) {
		sizes.append(sizeVariant.value<T>());
	}
	return sizes.isEmpty() ? defaultValue : sizes;
}

#endif // CONFIGHANDLER_H
