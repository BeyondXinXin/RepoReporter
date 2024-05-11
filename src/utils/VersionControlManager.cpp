#include "VersionControlManager.h"

#include <QDebug>
#include <QProcess>
#include <QTimeZone>
#include <QDir>
#include <QRegularExpression>
#include <QTimer>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QUrl>
#include <QElapsedTimer>
#include <QApplication>
#include <regex>

#include "FileUtil.h"

RepoType VersionControlManager::CurrentRepoType = RepoType::Git;

QString VersionControlManager::TortoiseGitPath = "C:\\Program Files\\TortoiseGit\\bin\\TortoiseGitProc.exe";
QString VersionControlManager::TortoiseSvnPath = "C:\\Program Files\\TortoiseSVN\\bin\\TortoiseProc.exe";
QString VersionControlManager::GitPath = "C:\\Program Files\\Git\\cmd\\git.exe";
QString VersionControlManager::SvnPath = "C:\\Program Files\\TortoiseSVN\\bin\\svn.exe";

bool VersionControlManager::VersionControlManager::CheckAndSetQuotepath()
{
	QStringList args;
	args << "config" << "--global" << "core.quotepath";
	QString outputStr;

	bool res = DoProcess(GitPath, args, "", outputStr);
	if (res) {
		if (outputStr.trimmed().toLower() == "true") {
			args << "false";
			res = DoProcess(GitPath, QStringList() << args, "", false);
		}
	}
	return res;
}

void VersionControlManager::PullRepository(const QString& repoPath)
{
	if (RepoType::Git == CurrentRepoType) {
		DoProcess(TortoiseGitPath, QStringList() << "/command:pull", repoPath);
	} else if (RepoType::Svn == CurrentRepoType) {
		DoProcess(TortoiseSvnPath, QStringList() << "/command:update" << "/path:''", repoPath);
	}
}

void VersionControlManager::SyncRepository(const QString& repoPath)
{
	if (RepoType::Git == CurrentRepoType) {
		DoProcess(TortoiseGitPath, QStringList() << "/command:sync", repoPath);
	}
}

void VersionControlManager::CheckRepository(const QString& repoPath)
{
	if (RepoType::Git == CurrentRepoType) {
		DoProcess(TortoiseGitPath, QStringList() << "/command:diff", repoPath);
	} else if (RepoType::Svn == CurrentRepoType) {
		DoProcess(TortoiseSvnPath, QStringList() << "/command:repostatus" << "/path:.", repoPath);
	}
}

QList<VCLogEntry>VersionControlManager::FetchLog(
	const QString& repoPath,
	QString& curVersion,
	QHash<QString, QMap<QString, VCFileEntry> >& fileMaps,
	bool allBranch)
{
	QList<VCLogEntry> logEntries;
	curVersion.clear();

	if (RepoType::Git == CurrentRepoType) {
		QStringList args;

		args << "log" << "--name-status" << "--pretty=format:^^^^%h|%B|%an|%ad|" << "--date=format-local:%c";
		if (allBranch) {
			args << "--all";
		}
		QString outputStr;
		if (!DoProcess(GitPath, args, repoPath, outputStr)) {
			return logEntries;
		}
		QStringList    lines = outputStr.split("^^^^");
		QList<QString> versions;
		QHash<QString, VCLogEntry> logEntrieHash;
		versions = AnalysisGitLogToLogEntry(lines, logEntrieHash, fileMaps);

		args.clear();
		args << "log" << "--numstat" << "--pretty=format:^^^^%h|%B|%an|%ad|";
		if (allBranch) {
			args << "--all";
		}
		outputStr;
		if (DoProcess(GitPath, args, repoPath, outputStr)) {
			lines = outputStr.split("^^^^");
			AnalysisGitChangesToFileEntry(lines, logEntrieHash, fileMaps);
		}

		args.clear();
		args << "log" << "-1" << "--pretty=format:%H";
		if (DoProcess(GitPath, args, repoPath, outputStr)) {
			curVersion = outputStr.split("\n").first();
		}

		foreach(auto version, versions)
		{
			logEntries << logEntrieHash.value(version);
		}
	} else if (RepoType::Svn == CurrentRepoType) {
		QStringList args;
		args << "info";
		QHash<QString, QString> svnInfo;
		QString outputStr;

		if (DoProcess(SvnPath, args, repoPath, outputStr)) {
			QStringList lines = outputStr.split("\r\n", QString::SkipEmptyParts);
			for (QString line : lines) {
				if (line.startsWith("Relative URL: ^")) {
					svnInfo["RelativeURL"] = line.remove("Relative URL: ^") + "/";
				} else if (line.startsWith("Last Changed Rev:")) {
					svnInfo["LastChangedRev"] = line.section(' ', -1);
				} else if (line.startsWith("URL:")) {
					svnInfo["URL"] = line.section(' ', -1);
				}
			}
		}

		args.clear();
		args << "log" << "-l" << "100" << "-v" << svnInfo["URL"];
		DoProcessLocal8Bit(SvnPath, args, repoPath, outputStr);
		QStringList lines = outputStr.split(
			"------------------------------------------------------------------------\r\n",
			QString::SkipEmptyParts);


		logEntries = AnalysisSvnLogToLogEntry(lines, svnInfo["RelativeURL"], fileMaps);
		curVersion = svnInfo["LastChangedRev"];
	}

	return logEntries;
}

QList<VCFileEntry>VersionControlManager::GetChangesForVersion(
	const QString& repoPath, const QList<QString>& versions)
{
	QList<VCFileEntry> fileEntries;

	if (RepoType::Svn == CurrentRepoType) {
		QStringList args;
		args << "log" << "-v" << "-r";
		if (1 == versions.size()) {
			args << versions.first();
		} else {
			args << QString("%1:%2").arg(versions.last()).arg(versions.first());
		}
		QString outputStr;
		DoProcessLocal8Bit(SvnPath, args, repoPath, outputStr);
		QStringList lines = outputStr.split(
			"------------------------------------------------------------------------\r\n",
			QString::SkipEmptyParts);
		args.clear();
		args << "info";
		QString relativeUrl;
		if (DoProcess(SvnPath, args, repoPath, outputStr)) {
			QStringList lines = outputStr.split("\r\n", QString::SkipEmptyParts);
			for (QString line : lines) {
				if (line.startsWith("Relative URL: ^")) {
					relativeUrl = line.remove("Relative URL: ^") + "/";
					break;
				}
			}
		}
		fileEntries = AnalysisSvnChangesToFileEntry(lines, relativeUrl);
	}


	return fileEntries;
}

void VersionControlManager::ShowCompare(
	const QString& repoPath, const QString& file,
	const QString& startrev, const QString& endrev)
{
	if (RepoType::Git == CurrentRepoType) {
		QStringList args;
		args << "/command:diff"
		     << QString("/path:%1").arg(file)
		     << QString("/startrev:%1~1").arg(startrev)
		     << QString("/endrev:%1").arg(endrev);
		QString res;
		DoProcess(TortoiseGitPath, args, repoPath, res);
	} else if (RepoType::Svn == CurrentRepoType) {
		QStringList args;
		args << "/command:diff"
		     << QString("/path:%1").arg(file)
		     << QString("/startrev:%1").arg(startrev.toInt() - 1)
		     << QString("/endrev:%1").arg(endrev);
		DoProcess(TortoiseSvnPath, args, repoPath);
	}
}

void VersionControlManager::ShowLog(
	const QString& repoPath, const QString& file)
{
	if (RepoType::Git == CurrentRepoType) {
		QStringList args;
		args << "/command:log";
		if (!file.isEmpty()) {
			args << QString("/path:%1").arg(file);
		}
		DoProcess(TortoiseGitPath, args, repoPath);
	} else if (RepoType::Svn == CurrentRepoType) {
		QStringList args;
		args << "/command:log";
		if (!file.isEmpty()) {
			args << QString("/path:%1").arg(file);
		}
		DoProcess(TortoiseSvnPath, args, repoPath);
	}
}

void VersionControlManager::OpenFile(
	const QString& repoPath, const QString& file,
	const QString& revision)
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
	QStringList args;
	if (RepoType::Git == CurrentRepoType) {
		process.setProgram(GitPath);
		args << "show" << QString("%1:%2").arg(revision).arg(file);
	} else if (RepoType::Svn == CurrentRepoType) {
		process.setProgram(SvnPath);
		args << "cat" << "-r" << revision << file;
	}
	process.setArguments(args);
	process.setWorkingDirectory(repoPath);
	process.setStandardOutputFile(outputPath);
	process.start();
	process.waitForFinished(3000);
	QUrl fileUrl = QUrl::fromLocalFile(outputPath);
	QDesktopServices::openUrl(fileUrl);
}

void VersionControlManager::CompareFiles(
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
		QStringList args;
		if (RepoType::Git == CurrentRepoType) {
			process.setProgram(GitPath);
			args << "show" << QString("%1:%2").arg(revisionList.at(i)).arg(fileList.at(i));
		} else if (RepoType::Svn == CurrentRepoType) {
			process.setProgram(SvnPath);
			args << "cat" << "-r" << revisionList.at(i) << fileList.at(i);
		}
		process.setArguments(args);
		process.setWorkingDirectory(repoPath);
		process.setStandardOutputFile(outputPath);
		process.start();
		process.waitForFinished(3000);

		outFileList << outputPath;
	}

	QProcess process;
	process.setProgram(TortoiseGitPath);
	QStringList args;
	args << "/command:diff";
	args << QString("/path:%1").arg(outFileList.at(0));
	args << QString("/path2:%1").arg(outFileList.at(1));
	process.setArguments(args);
	process.setWorkingDirectory(FileUtil::GetDirectoryFromPath(outFileList.at(0)));
	process.startDetached();
}

void VersionControlManager::OpenFileDirectory(
	const QString& repoPath, const QString& file)
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
		QStringList args;
		if (RepoType::Git == CurrentRepoType) {
			process.setProgram(GitPath);
			args << "show" << QString("%1:%2").arg(revision).arg(file);
		} else if (RepoType::Svn == CurrentRepoType) {
			process.setProgram(SvnPath);
			args << "cat" << "-r" << revision << file;
		}
		process.setArguments(args);
		process.setWorkingDirectory(repoPath);
		process.setStandardOutputFile(outputFilePath);
		process.start();
		process.waitForFinished(3000);
	}
}

QString VersionControlManager::GetCurrentBranch(
	const QString& repoPath)
{
	if (RepoType::Git == CurrentRepoType) {
		QStringList args;
		args << "rev-parse" << "--abbrev-ref" << "HEAD";
		QString res;
		DoProcess(GitPath, args, repoPath, res);
		return res.simplified();
	}
	return "";
}

bool VersionControlManager::CheckUncommittedChanges(
	const QString& repoPath, const RepoType& type)
{
	if (RepoType::Git == type) {
		if (repoPath.isEmpty()) {
			return false;
		}
		QStringList args;
		args << "diff" << "--quiet";
		int exitCode;
		DoProcess(GitPath, args, repoPath, exitCode);
		return exitCode != 0;
	} else if (RepoType::Svn == type) {
		if (repoPath.isEmpty()) {
			return false;
		}
		QStringList args;
		args << "status";
		QString output;
		DoProcess(SvnPath, args, repoPath, output);
		return !output.isEmpty();
	}
	return false;
}

bool VersionControlManager::DoProcess(
	const QString& program, const QStringList& args,
	const QString& path, const bool& detached)
{
	QProcess process;
	process.setProgram(program);
	process.setArguments(args);
	process.setWorkingDirectory(path);
	if (detached) {
		process.startDetached();
		return true;
	}
	process.start();
	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << "Failed to run:" << program << args;
		return false;
	}
	return true;
}

bool VersionControlManager::DoProcess(
	const QString& program, const QStringList& args,
	const QString& path, QString& res)
{
	QProcess process;
	process.setProgram(program);
	process.setArguments(args);
	process.setWorkingDirectory(path);
	process.start();
	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << "Failed to run:" << program << args;
		return false;
	}
	QByteArray output = process.readAllStandardOutput();
	res = QString::fromUtf8(output);
	return true;
}

bool VersionControlManager::DoProcessLocal8Bit(
	const QString& program, const QStringList& args,
	const QString& path, QString& res)
{
	QProcess process;
	process.setProgram(program);
	process.setArguments(args);
	process.setWorkingDirectory(path);
	process.start();
	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << "Failed to run:" << program << args;
		return false;
	}
	QByteArray output = process.readAllStandardOutput();
	res = QString::fromLocal8Bit(output);
	return true;
}

bool VersionControlManager::DoProcess(
	const QString& program, const QStringList& args,
	const QString& path, int& exitCode)
{
	QProcess process;
	process.setProgram(program);
	process.setArguments(args);
	process.setWorkingDirectory(path);
	process.start();
	if (!process.waitForStarted() || !process.waitForFinished()) {
		qInfo() << "Failed to run:" << program << args;
		return false;
	}
	exitCode = process.exitCode();
	return true;
}

QList<QString>VersionControlManager::AnalysisGitLogToLogEntry(
	const QStringList& lines,
	QHash<QString, VCLogEntry>& logEntries,
	QHash<QString, QMap<QString, VCFileEntry> >& fileMaps)
{
	QList<QString> versions;
	QString dataFormat = "ddd MMM d HH:mm:ss yyyy";

	foreach(const QString& line, lines)
	{
		QStringList parts = line.split("|");
		if (parts.size() != 5) {
			continue;
		}
		QString version = parts[0];
		versions << version;

		VCLogEntry& entry = logEntries[version];
		entry.version = version;
		entry.message = parts[1];
		if (!entry.message.isEmpty() &&
		    (entry.message.endsWith('\n') || entry.message.endsWith('\r'))) {
			entry.message.chop(1);
		}
		entry.author = parts[2];
		entry.date = QLocale::c().toDateTime(
			parts[3].replace(QRegularExpression("\\s+"), " "), dataFormat);
		QStringList fileList = parts[4].split("\n", QString::SkipEmptyParts);
		foreach(QString file, fileList)
		{
			VCFileEntry fileEntry;
			QStringList tmp = file.split("\t");
			QString     operation = tmp[0].trimmed();
			QString     filePath = tmp.at(1);
			if (operation.startsWith('M')) {
				fileEntry.operation = FileOperation::Modify;
				entry.operations << FileOperation::Modify;
			} else if (operation.startsWith('A')) {
				fileEntry.operation = FileOperation::Add;
				entry.operations << FileOperation::Add;
			} else if (operation.startsWith('D')) {
				fileEntry.operation = FileOperation::Delete;
				entry.operations << FileOperation::Delete;
			} else if (operation.startsWith('R')) {
				fileEntry.operation = FileOperation::Rename;
				entry.operations << FileOperation::Rename;
				filePath = tmp.at(2);
			}
			fileEntry.extensionName = QFileInfo(tmp.at(1)).suffix();
			fileEntry.filePath = filePath;
			fileMaps[version][filePath] = fileEntry;
		}
	}

	return versions;
}

void VersionControlManager::AnalysisGitChangesToFileEntry(
	const QStringList& lines,
	QHash<QString, VCLogEntry>& logEntries,
	QHash<QString, QMap<QString, VCFileEntry> >& fileMaps)
{
	foreach(QString line, lines)
	{
		QStringList parts = line.split("|");
		if (parts.size() != 5) {
			continue;
		}
		QString version = parts[0];
		VCLogEntry& entry = logEntries[version];
		QMap<QString, VCFileEntry>& fileMap = fileMaps[version];
		QStringList fileList = parts[4].split("\n", QString::SkipEmptyParts);
		foreach(QString file, fileList)
		{
			QStringList tmp = file.split("\t");
			QString     filePath = tmp.at(2);

			if (filePath.contains(" => ")) {
				QStringList completeList = filePath.split(" => ");
				QStringList beforeList = completeList[0].split("{");
				QStringList afterList = completeList[1].split("}");
				if (2 == afterList.size()) {
					filePath = beforeList.at(0);
				} else {
					filePath.clear();
				}

				if (2 == afterList.size()) {
					if (afterList.at(0).isEmpty()) {
						filePath += afterList[1].remove(0, 1);
					} else {
						filePath += afterList.at(0) + afterList.at(1);
					}
				} else {
					filePath += afterList.at(0);
				}
			}

			if (fileMap.contains(filePath)) {
				VCFileEntry& fileEntry = fileMap[filePath];
				fileEntry.addNum += tmp[0].toInt();
				fileEntry.deleteNum += tmp[1].toInt();
			} else {
				qInfo() << u8"文件名解析失败" << version << filePath;
			}
		}
	}
}

QList<VCLogEntry>VersionControlManager::AnalysisSvnLogToLogEntry(
	const QStringList& lines,
	const QString& relativeUrl,
	QHash<QString, QMap<QString, VCFileEntry> >& fileMaps)
{
	QList<VCLogEntry> logEntries;
	QString dataFormat = "yyyy-MM-dd hh:mm:ss ";
	foreach(QString line, lines)
	{
		VCLogEntry  logEntry;
		QStringList fields = line.split(" | ");
		logEntry.version = fields[0].remove("r");
		logEntry.author = fields.at(1);
		logEntry.date = QLocale::c().toDateTime(fields[2].split("+").first(), dataFormat);

		QStringList strs = fields[3].split("\r\n\r\n");
		logEntry.message = strs.at(1);
		QStringList files = strs[0].split("Changed paths:\r\n")[1].split("\r\n");
		foreach(QString file, files)
		{
			QStringList  tmpList = file.trimmed().split(" ");
			QString      filePath = tmpList[1].remove(relativeUrl);
			QString      fileOperation = tmpList.at(0);
			VCFileEntry& fileEntry = fileMaps[logEntry.version][filePath];
			fileEntry.filePath = filePath;
			if (fileOperation.startsWith('M')) {
				fileEntry.operation = FileOperation::Modify;
				logEntry.operations << FileOperation::Modify;
			} else if (fileOperation.startsWith('A')) {
				fileEntry.operation = FileOperation::Add;
				logEntry.operations << FileOperation::Add;
			} else if (fileOperation.startsWith('D')) {
				fileEntry.operation = FileOperation::Delete;
				logEntry.operations << FileOperation::Delete;
			} else if (fileOperation.startsWith('R')) {
				fileEntry.operation = FileOperation::Rename;
				logEntry.operations << FileOperation::Rename;
			}
			QFileInfo fileInfo(filePath);
			fileEntry.extensionName = fileInfo.suffix();
		}
		logEntries << logEntry;
	}
	return logEntries;
}

QList<VCFileEntry>VersionControlManager::AnalysisSvnChangesToFileEntry(
	const QStringList& lines, const QString& relativeUrl)
{
	QMap<QString, VCFileEntry> fileMap;
	foreach(QString line, lines)
	{
		QStringList fields = line.split(" | ");
		QStringList strs = fields[3].split("\r\n\r\n");
		QStringList files = strs[0].split("Changed paths:\r\n")[1].split("\r\n");
		foreach(QString file, files)
		{
			QStringList  tmpList = file.trimmed().split(" ");
			QString      filePath = tmpList[1].remove(relativeUrl);
			QString      fileOperation = tmpList.at(0);
			VCFileEntry& entry = fileMap[filePath];
			entry.filePath = filePath;
			if (fileOperation.startsWith('M')) {
				entry.operation = FileOperation::Modify;
			} else if (fileOperation.startsWith('A')) {
				entry.operation = FileOperation::Add;
			} else if (fileOperation.startsWith('D')) {
				entry.operation = FileOperation::Delete;
			} else if (fileOperation.startsWith('R')) {
				entry.operation = FileOperation::Rename;
			}
			QFileInfo fileInfo(filePath);
			entry.extensionName = fileInfo.suffix();
		}
	}

	return fileMap.values();
}
