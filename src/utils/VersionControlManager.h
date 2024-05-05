#ifndef VERSIONCONTROLMANAGER_H
#define VERSIONCONTROLMANAGER_H

#include <QObject>

#include "data/VCSDataStructures.h"

class VersionControlManager {
public:

	static bool VersionControlManager::CheckAndSetQuotepath();

	static QList<VCLogEntry>FetchLog(
		const QString& repoPath, QString& curVersion, bool allBranch = false);

	static QList<VCFileEntry>GetChangesForVersion(
		const QString& repoPath, const QList<QString>& versions);

	static void ShowCompare(
		const QString& repoPath, const QString& file,
		const QString& startrev, const QString& endrev);

	static void ShowLog(const QString& repoPath, const QString& file);

	static void OpenFile(
		const QString& repoPath, const QString& file, const QString& revision);

	static void CompareFile(
		const QString& repoPath,
		const QString& file, const QString& revision,
		const QString& markFile, const QString& markRrevision);

	static void OpenFileDirectory(
		const QString& repoPath, const QString& file);

	static void ExportFile(
		const QString& repoPath, const QStringList& files,
		const QString& revision, const QString& targetPath);

	static QString GetCurrentBranch(const QString& repoPath);

	static QStringList GetAllBranches(const QString& repoPath);

	static bool CheckUncommittedChanges(const QString& repoPath);

	static void RepoPull(const QString& repoPath);

	static void RepoSync(const QString& repoPath);

	static void RepoCheck(const QString& repoPath);
};

#endif // VERSIONCONTROLMANAGER_H
