#ifndef VERSIONCONTROLMANAGER_H
#define VERSIONCONTROLMANAGER_H

#include <QObject>

#include "vcdata/VCSDataStructures.h"

class VersionControlManager {
public:

	static QList<VCLogEntry>FetchLog(const QString& repoPath);
	static QString GetChangesForVersion(const QString& repoPath, int version);
};

#endif // VERSIONCONTROLMANAGER_H
