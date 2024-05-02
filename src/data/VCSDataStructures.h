#ifndef VCSDataStructures_H
#define VCSDataStructures_H


#include <QDebug>
#include <QString>
#include <QDateTime>

/*
 * FileOperation
 * VCLogEntry
 * VCProjectPath
 */


// --------------------- FileOperation ---------------------

enum class FileOperation {
	Add,
	Modify,
	Delete,
	Rename
};
Q_DECLARE_METATYPE(FileOperation)

inline QString FileOperationToString(FileOperation operation)
{
	switch (operation) {
	case FileOperation::Add: return QStringLiteral("A");

	case FileOperation::Modify: return QStringLiteral("M");

	case FileOperation::Delete: return QStringLiteral("D");

	case FileOperation::Rename: return QStringLiteral("R");

	default: return QStringLiteral("Err");
	}
}

inline uint qHash(FileOperation key, uint seed = 0) noexcept
{
	return ::qHash(static_cast<int>(key), seed);
}

// --------------------- VCFileEntry ---------------------

struct VCFileEntry {
	QString filePath;
	QString extensionName;
	int addNum;
	int deleteNum;
	FileOperation operation;

	friend QDebug operator<<(QDebug dbg, const VCFileEntry& entry);
};

inline QDebug operator<<(QDebug dbg, const VCFileEntry& entry)
{
	dbg << "VCFileEntry("
	    << "filePath:" << entry.filePath << ", "
	    << "extensionName:" << entry.extensionName << ", "
	    << "addNum:" << entry.addNum << ", "
	    << "deleteNum:" << entry.deleteNum << ", "
	    << "operation:" << FileOperationToString(entry.operation) << ")";
	return dbg;
}

// --------------------- VCLogEntry ---------------------

struct VCLogEntry {
	QString version;
	QSet<FileOperation>operations;
	QString message;
	QString author;
	QDateTime date;

	friend QDebug operator<<(QDebug dbg, const VCLogEntry& entry);
};

inline QDebug operator<<(QDebug dbg, const VCLogEntry& entry)
{
	dbg << "VCLogEntry(" << "Version:" << entry.version << ", "
	    << "Operations:[";

	for (const FileOperation& operation : entry.operations) {
		dbg << FileOperationToString(operation) << ", ";
	}

	dbg << "], "
	    << "Message:" << entry.message << ", "
	    << "Author:" << entry.author << ", "
	    << "Date:" << entry.date.toString("yyyy/M/d hh:mm:ss") << ")";
	return dbg;
}

// --------------------- VCProjectPath ---------------------

struct VCProjectPath {
	QString name;
	QString path;

	VCProjectPath(const QString& newName, const QString& newPath = "");
	VCProjectPath(const VCProjectPath& other);
	friend QDebug operator<<(QDebug dbg, const VCProjectPath& path);
};

inline VCProjectPath::VCProjectPath(const QString& newName, const QString& newPath)
	: name(newName), path(newPath)
{}

inline VCProjectPath::VCProjectPath(const VCProjectPath& other)
	: name(other.name), path(other.path)
{}

inline QDebug operator<<(QDebug dbg, const VCProjectPath& path)
{
	dbg.nospace() << "VCProjectPath(name:" << path.name << ", path:" << path.path << ")";
	return dbg.space();
}

#endif // ifndef VCSDataStructures_H
