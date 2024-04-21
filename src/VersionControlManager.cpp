#include "VersionControlManager.h"

#include <QDebug>
#include <QProcess>
#include <QTimeZone>
#include <QDir>


QList<VCLogEntry> VersionControlManager::FetchLog(const QString& repoPath)
{
	QList<VCLogEntry> logEntries;

	// Check if repoPath exists
	QDir repoDir(repoPath);
	if (!repoDir.exists()) {
		qInfo() << u8"仓库路径不存在:" << repoPath;
		return logEntries;
	}

	// Run git log command
	QProcess process;

	process.setProgram("git");
	QStringList args;

	args << "log" << "--name-status" << "--pretty=format:%h|%s|%an|%ad" << "--date=format-local:%c";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();

	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << u8"运行 git log 命令时出错。";
		return logEntries;
	}

	// Parse the output
	QByteArray  output = process.readAllStandardOutput();
	QString     outputStr(output);
	QStringList lines = outputStr.split("\n");

	foreach(const QString& line, lines)
	{
		QStringList parts = line.split("|");

		if (parts.size() == 4) {
			VCLogEntry entry;

			entry.version = parts[0];
			entry.message = parts[1];
			entry.author = parts[2];
			entry.date = QLocale::c().toDateTime(parts[3], "ddd MMM d HH:mm:ss yyyy");
			logEntries.append(entry);
		} else if (parts.size() > 0) {
			if (!logEntries.isEmpty()) {
				FileOperation operation = FileOperation::Modify;

				if (parts[0].trimmed().startsWith('A')) {
					operation = FileOperation::Add;
				} else if (parts[0].trimmed().startsWith('D')) {
					operation = FileOperation::Delete;
				}
				logEntries.last().operations << operation;
			}
		}
	}

	return logEntries;
}

QString VersionControlManager::GetChangesForVersion(const QString& repoPath, int version)
{
	// Run git show command for the given version
	QProcess process;

	process.setProgram("git");
	QStringList args;

	args << "show" << QString::number(version);
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();

	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << u8"运行 git show 命令时出错。";
		return QString();
	}

	// Return the output as QString
	QByteArray output = process.readAllStandardOutput();
	QString    outputStr(output);

	return outputStr;
}
