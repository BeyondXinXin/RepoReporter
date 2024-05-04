﻿#ifndef VERSIONCONTROLMANAGER_H
#define VERSIONCONTROLMANAGER_H

#include <QObject>

#include "data/VCSDataStructures.h"

class VersionControlManager {
public:

	static QList<VCLogEntry>FetchLog(
		const QString& repoPath, QString& curVersion);

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
};

#endif // VERSIONCONTROLMANAGER_H
