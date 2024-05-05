﻿#include "VersionControlManager.h"

#include <QDebug>
#include <QProcess>
#include <QTimeZone>
#include <QDir>
#include <QRegularExpression>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>

#include "FileUtil.h"

QList<VCLogEntry>VersionControlManager::FetchLog(
	const QString& repoPath, QString& curVersion)
{
	QList<VCLogEntry> logEntries;

	QProcess process;
	process.setProgram("git");
	QStringList args;

	args << "log" /*<< "--all"*/
	     << "--name-status"
	     << "--pretty=format:%h|%B|%an|%ad|"
	     << "--date=format-local:%c";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();

	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << u8"运行 git log" << args << u8"命令时出错。";
		return logEntries;
	}

	QString dataFormat = "ddd MMM d HH:mm:ss yyyy";
	QString outputStr(process.readAllStandardOutput());
	QStringList lines = outputStr.split("\n\n");

	foreach(const QString& line, lines)
	{
		QStringList parts = line.split("|");
		if (parts.size() != 5) {
			continue;
		}

		VCLogEntry entry;
		entry.version = parts[0];
		entry.message = parts[1];
		if (!entry.message.isEmpty() &&
		    (entry.message.endsWith('\n') || entry.message.endsWith('\r'))) {
			entry.message.chop(1);
		}
		entry.author = parts[2];
		entry.date = QLocale::c().toDateTime(
			parts[3].replace(QRegularExpression("\\s+"), " "), dataFormat);
		QStringList fileList = parts[4].split("\n");
		foreach(QString file, fileList)
		{
			if (file.trimmed().startsWith('M')) {
				entry.operations << FileOperation::Modify;
			} else if (file.trimmed().startsWith('A')) {
				entry.operations << FileOperation::Add;
			} else if (file.trimmed().startsWith('D')) {
				entry.operations << FileOperation::Delete;
			} else if (file.trimmed().startsWith('R')) {
				entry.operations << FileOperation::Rename;
			}
		}

		logEntries.append(entry);
	}


	args.clear();
	args << "log" << "-1" << "--pretty=format:%H";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();

	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << u8"运行"
		           " 'git log -1 --pretty=format:%H' "
		           "命令时出错。";
		curVersion = "";
		return logEntries;
	} else {
		QByteArray  output = process.readAllStandardOutput();
		QString     outputStr(output);
		QStringList lines = outputStr.split("\n");
		curVersion = lines.first();
	}

	return logEntries;
}

#include <regex>
QList<VCFileEntry>VersionControlManager::GetChangesForVersion(
	const QString& repoPath, const QList<QString>& versions)
{
	QList<VCFileEntry> fileEntries;

	QProcess process;

	process.setProgram("git");
	QStringList args;

	args << "show" << "--pretty=format:%n" << "--numstat";
	foreach(auto version, versions)
	{
		args << version;
	}
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();

	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << u8"运行 git show 命令时出错。";
		return fileEntries;
	}

	QByteArray  output = process.readAllStandardOutput();
	QString     outputStr(output);
	QStringList lines = outputStr.split("\n");
	QMap<QString, VCFileEntry> fileMap;

	foreach(QString line, lines)
	{
		if (line.isEmpty()) {
			continue;
		}
		QStringList parts = line.remove(" ").split(QRegExp("\\s+"));
		if (parts.size() < 3) {
			continue;
		}
		QString filePath = parts[2];
		int     startIndex = filePath.indexOf('{');
		if (startIndex != -1) {
			int endIndex = filePath.indexOf('}', startIndex);
			if (endIndex != -1) {
				QString pattern = filePath.mid(startIndex, endIndex - startIndex + 1);
				QStringList parts = pattern.mid(1, pattern.length() - 2).split("=>");
				if (parts.size() == 2) {
					filePath.replace(pattern, parts[1]);
				}
			}
		}
		if (filePath.contains("{")) {}
		VCFileEntry& entry = fileMap[filePath];

		entry.addNum += parts[0].toInt();
		entry.deleteNum += parts[1].toInt();
		entry.filePath = filePath;

		if ((entry.addNum > 0) && (entry.deleteNum > 0)) {
			entry.operation = FileOperation::Modify;
		} else if (entry.addNum > 0) {
			entry.operation = FileOperation::Add;
		} else if (entry.deleteNum > 0) {
			entry.operation = FileOperation::Delete;
		}
		QFileInfo fileInfo(filePath);

		entry.extensionName = fileInfo.suffix();
	}

	return fileMap.values();
}

void VersionControlManager::ShowCompare(
	const QString& repoPath, const QString& file,
	const QString& startrev, const QString& endrev)
{
	QProcess process;
	process.setProgram("TortoiseGitProc.exe");
	QStringList args;
	args << "/command:diff";
	args << QString("/path:%1").arg(file);
	args << QString("/startrev:%1~1").arg(startrev);
	args << QString("/endrev:%1").arg(endrev);
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.startDetached();
}

void VersionControlManager::ShowLog(const QString& repoPath, const QString& file)
{
	QProcess process;
	process.setProgram("TortoiseGitProc.exe");
	QStringList args;
	args << "/command:log";
	if (!file.isEmpty()) {
		args << QString("/path:%1").arg(file);
	}
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.startDetached();
}

void VersionControlManager::OpenFile(
	const QString& repoPath, const QString& file, const QString& revision)
{
	QString   configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
	QFileInfo outputInfo = QFileInfo(file);
	QString   outputPath = QString("%1/.tmp/%2-%3.%4")
	                       .arg(configDir)
	                       .arg(outputInfo.completeBaseName())
	                       .arg(revision)
	                       .arg(outputInfo.completeSuffix());

	FileUtil::DirMake(outputPath);

	QProcess process;
	process.setProgram("git");
	QStringList args;
	args << "show" << QString("%1:%2").arg(revision).arg(file);
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.setStandardOutputFile(outputPath);
	process.start();
	process.waitForFinished(3000);

	QUrl fileUrl = QUrl::fromLocalFile(outputPath);
	QDesktopServices::openUrl(fileUrl);
}

void VersionControlManager::CompareFile(
	const QString& repoPath,
	const QString& file, const QString& revision,
	const QString& markFile, const QString& markRrevision)
{
	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	QStringList outFileList;
	QStringList revisionList{ revision, markRrevision };
	QStringList fileList{ file, markFile };

	for (int i = 0; i < revisionList.size(); i++) {
		QFileInfo fileInfo = QFileInfo(fileList.at(i));
		QString   outputPath = QString("%1/.tmp/%2-%3.%4")
		                       .arg(configDir)
		                       .arg(fileInfo.completeBaseName())
		                       .arg(revisionList.at(i))
		                       .arg(fileInfo.completeSuffix());

		QProcess process;
		process.setProgram("git");
		QStringList args;
		args << "show" << QString("%1:%2").arg(revisionList.at(i)).arg(fileList.at(i));
		process.setArguments(args);
		process.setWorkingDirectory(repoPath);
		process.setStandardOutputFile(outputPath);
		process.start();
		process.waitForFinished(3000);

		outFileList << outputPath;
	}

	QProcess process;
	process.setProgram("TortoiseGitProc.exe");
	QStringList args;
	args << "/command:diff";
	args << QString("/path:%1").arg(outFileList.at(0));
	args << QString("/path2:%1").arg(outFileList.at(1));
	process.setArguments(args);
	process.setWorkingDirectory(FileUtil::GetDirectoryFromPath(outFileList.at(0)));
	process.startDetached();
}

void VersionControlManager::OpenFileDirectory(const QString& repoPath, const QString& file)
{
	QFileInfo fileInfo(repoPath + "/" + file);
	QString   directory = fileInfo.absolutePath();
	FileUtil::OpenDirectoryInExplorer(directory);
}

void VersionControlManager::ExportFile(
	const QString& repoPath, const QStringList& files,
	const QString& revision, const QString& targetPath)
{
	QString lastFolderName = QDir(repoPath).dirName();
	QString outputPath = QString("%1/%2/").arg(targetPath).arg(lastFolderName);

	foreach(const QString& file, files)
	{
		QString outputFilePath = outputPath + file;
		FileUtil::DirMake(outputFilePath);

		QProcess process;
		process.setProgram("git");
		QStringList args;
		args << "show" << QString("%1:%2").arg(revision).arg(file);
		process.setArguments(args);
		process.setWorkingDirectory(repoPath);
		process.setStandardOutputFile(outputFilePath);
		process.start();
		process.waitForFinished(3000);
	}
}

QString VersionControlManager::GetCurrentBranch(const QString& repoPath)
{
	QProcess process;
	process.setProgram("git");
	QStringList args;
	args << "rev-parse" << "--abbrev-ref" << "HEAD";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();

	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << u8"获取当前分支名称时出错。";
		return "";
	}

	QString branch = process.readAllStandardOutput().trimmed();
	return branch;
}

QStringList VersionControlManager::GetAllBranches(const QString& repoPath)
{
	QProcess process;
	process.setProgram("git");
	QStringList args;
	args << "branch" << "-a";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();

	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << u8"获取所有分支时出错。";
		return QStringList();
	}

	QString output = process.readAllStandardOutput();
	QStringList branches = output.split(QRegExp("[\\r\\n]+"), QString::SkipEmptyParts);

	for (int i = 0; i < branches.size(); ++i) {
		QString branch = branches.at(i).trimmed();

		if (branch.startsWith('*')) {
			branches[i] = branch.mid(2);
		}

		if (branch.startsWith("remotes/")) {
			branches[i] = branch.mid(8);
		}
	}

	return branches;
}

bool VersionControlManager::CheckUncommittedChanges(const QString& repoPath)
{
	if (repoPath.isEmpty()) {
		return false;
	}

	QProcess process;
	process.setProgram("git");
	QStringList args;
	args << "diff" << "--quiet";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.start();
	process.waitForFinished();
	bool res = process.exitCode() != 0;

	return res;
}

void VersionControlManager::RepoPull(const QString& repoPath)
{
	QProcess process;
	process.setProgram("TortoiseGitProc.exe");
	QStringList args;
	args << "/command:pull";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.startDetached();
}

void VersionControlManager::RepoSync(const QString& repoPath)
{
	QProcess process;
	process.setProgram("TortoiseGitProc.exe");
	QStringList args;
	args << "/command:sync";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.startDetached();
}

void VersionControlManager::RepoCheck(const QString& repoPath)
{
	QProcess process;
	process.setProgram("TortoiseGitProc.exe");
	QStringList args;
	args << "/command:diff";
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.startDetached();
}
