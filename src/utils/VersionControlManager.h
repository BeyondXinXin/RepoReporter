#ifndef VERSIONCONTROLMANAGER_H
#define VERSIONCONTROLMANAGER_H

#include <QObject>

#include "data/VCSDataStructures.h"

class VersionControlManager {
public:

	static QList<VCLogEntry>FetchLog(const QString& repoPath, QString& curVersion);
	static QList<VCFileEntry>GetChangesForVersion(
		const QString& repoPath, const QList<QString>& versions);
};

#endif // VERSIONCONTROLMANAGER_H
