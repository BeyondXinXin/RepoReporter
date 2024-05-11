#include "LogManager.h"

#include <QReadWriteLock>
#include <QTextStream>
#include <QDateTime>
#include <QThread>
#include <QStandardPaths>
#include <QDir>
#include <QDebug>
#include <QIODevice>

#include "CommandLineManager.h"
#include "FileUtil.h"

QString LogManager::m_LogFilePath = "";

LogManager::LogManager(QObject* parent) : QObject(parent)
{
	this->Initial();
}

LogManager::~LogManager()
{
	qInstallMessageHandler(nullptr);
}

void LogManager::Initial()
{
	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	m_LogFilePath = configDir + "/log/";
	QDir dir(m_LogFilePath);
	if (!dir.exists()) {
		if (!dir.mkpath(m_LogFilePath)) {
			qInfo() << u8"无法创建日志目录。";
			m_LogFilePath = "";
			m_WriteFile = false;
		}
	}
	qDebug() << m_LogFilePath;
	FileUtil::LimtFilesNumber(m_LogFilePath);
	qInstallMessageHandler(LogMessageOutput);
}

void LogManager::LogMessageOutput(QtMsgType type,
                                  const QMessageLogContext& context,
                                  const QString& msg)
{
	static QReadWriteLock lock;
	QWriteLocker locker(&lock);
	static QTextStream cout(stdout, QIODevice::WriteOnly);
	QString text;

	switch (type) {
	case QtDebugMsg: {
		if (CommandLineManager::option.debug) {
			cout << msg << endl;
		}
		return;
	}

	case QtInfoMsg: {
		text = QString("INFO:");
		break;
	}

	case QtWarningMsg: {
		text = QString("WARNING:");
		break;
	}

	case QtCriticalMsg: {
		text = QString("CRITICAL:");
		break;
	}

	case QtFatalMsg: {
		text = QString("FATAL:");
		break;
	}
	}
	QString currentDateTime = QDateTime::currentDateTime()
	                          .toString("hh:mm:ss");
	QString currentThreadName = QThread::currentThread()->objectName();
	QString message = QString("%1 [%2] %3 %4 %5 ")
	                  .arg(currentDateTime)
	                  .arg(currentThreadName)
	                  .arg(text)
	                  .arg(msg)
	                  .arg(QString(context.function).remove(QRegExp("\\((.*)\\)")));

	cout << message << endl;
	cout.flush();


	// 写日志
	static QString logFileName
	        = QString("%1%2.log")
	          .arg(m_LogFilePath)
	          .arg(QDateTime::currentDateTime().toString("log_yyyy_MM_dd_hh_mm_ss"));
	QFile file(logFileName);

	file.open(QIODevice::WriteOnly | QIODevice::Append);
	QTextStream stream(&file);

	stream.setCodec("UTF-8");
	stream << message << "\r\n";
	file.flush();
	file.close();
}
