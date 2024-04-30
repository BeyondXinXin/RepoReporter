#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <QObject>
#include <QFile>
#include <QDataStream>
#include <QPointer>

class LogManager : public QObject {
	Q_OBJECT

public:

	explicit LogManager(QObject* parent = nullptr);
	virtual ~LogManager() override;

private:

	void Initial();
	static void LogMessageOutput(QtMsgType type,
	                             const QMessageLogContext& context,
	                             const QString& msg);

private:

	static QString m_LogFilePath;
	bool m_WriteFile { true };
};

#endif // LOGMANAGER_H
