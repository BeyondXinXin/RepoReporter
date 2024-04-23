#ifndef VERSIONCONTROLMANAGER_H
#define VERSIONCONTROLMANAGER_H

#include <QObject>

#include "vcdata/VCSDataStructures.h"

class VersionControlManager {
public:

	static QList<VCLogEntry> FetchLog(const QString& repoPath);
	static QList<VCFileEntry> GetChangesForVersion(const QString& repoPath, const QList<QString>& versions);
};

#endif // VERSIONCONTROLMANAGER_H
