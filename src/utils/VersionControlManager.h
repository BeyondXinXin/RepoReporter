#ifndef VERSIONCONTROLMANAGER_H
#define VERSIONCONTROLMANAGER_H

#include <QObject>

#include "data/VCSDataStructures.h"

class VersionControlManager {
public:

	static bool CheckAndSetQuotepath();

	static void PullRepository(const QString& repoPath);

	static void SyncRepository(const QString& repoPath);

	static void CheckRepository(const QString& repoPath);

	static QList<VCLogEntry>FetchLog(
		const QString& repoPath,
		QString& curVersion,
		QHash<QString, QMap<QString, VCFileEntry> >& fileMaps,
		bool allBranch = false);

	static QList<VCFileEntry>GetChangesForVersion(
		const QString& repoPath, const QList<QString>& versions);

	static void ShowCompare(
		const QString& repoPath, const QString& file,
		const QString& startrev, const QString& endrev);

	static void ShowLog(
		const QString& repoPath, const QString& file);

	static void OpenFile(
		const QString& repoPath, const QString& file, const QString& revision);

	static void CompareFiles(
		const QString& repoPath,
		const QString& file, const QString& revision,
		const QString& markFile, const QString& markRrevision);

	static void OpenFileDirectory(
		const QString& repoPath, const QString& file);

	static void ExportFile(
		const QString& repoPath, const QStringList& files,
		const QString& revision, const QString& targetPath);

	static QString GetCurrentBranch(const QString& repoPath);

	static bool CheckUncommittedChanges(const QString& repoPath, const RepoType& type);

private:

	static bool DoProcess(const QString& program, const QStringList& args,
	                      const QString& path, const bool& detached = true);

	static bool DoProcess(const QString& program, const QStringList& args,
	                      const QString& path, QString& res);

	static bool DoProcessLocal8Bit(const QString& program, const QStringList& args,
	                               const QString& path, QString& res);

	static bool DoProcess(const QString& program, const QStringList& args,
	                      const QString& path, int& exitCode);

	static QList<QString>AnalysisGitLogToLogEntry(
		const QStringList& lines,
		QHash<QString, VCLogEntry>& logEntries,
		QHash<QString, QMap<QString, VCFileEntry> >& fileMaps);

	static void AnalysisGitChangesToFileEntry(
		const QStringList& lines,
		QHash<QString, VCLogEntry>& logEntries,
		QHash<QString, QMap<QString, VCFileEntry> >& fileMaps);

	static QList<VCLogEntry>AnalysisSvnLogToLogEntry(
		const QStringList& lines, const QString& relativeUrl,
		QHash<QString, QMap<QString, VCFileEntry> >& fileMaps);

	static QList<VCFileEntry>AnalysisSvnChangesToFileEntry(
		const QStringList& lines, const QString& relativeUrl);

public:

	static RepoType CurrentRepoType;

private:

	static QString TortoiseGitPath;
	static QString TortoiseSvnPath;
	static QString GitPath;
	static QString SvnPath;
};

#endif // VERSIONCONTROLMANAGER_H
