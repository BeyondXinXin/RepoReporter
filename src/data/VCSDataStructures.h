#ifndef VCSDataStructures_H
#define VCSDataStructures_H


#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QColor>

/*
 * FileOperation
 * VCLogEntry
 * VCProjectPath
 */

static const QColor AddColor(128, 0, 128);   // 紫色
static const QColor ModifyColor(0, 50, 177); // 深蓝色
static const QColor DeleteColor(100, 0, 58); // 暗红色
static const QColor RenameColor(0, 0, 255);  // 蓝色

// --------------------- RepoType ---------------------
enum class RepoType {
	Unknown = 0,
	Git     = 1,
	Svn     = 2,
};
Q_DECLARE_METATYPE(RepoType)

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

inline FileOperation StringToFileOperation(QString str)
{
	if (str == "M") {
		return FileOperation::Modify;
	} else if (str == "A") {
		return FileOperation::Add;
	} else if (str == "D") {
		return FileOperation::Delete;
	} else if (str == "R") {
		return FileOperation::Rename;
	} else {
		return FileOperation::Modify;
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
	RepoType type = RepoType::Unknown;

	VCProjectPath(const QString& newName, const QString& newPath = "");
	VCProjectPath(const VCProjectPath& other);
	friend QDebug operator<<(QDebug dbg, const VCProjectPath& path);
};

inline VCProjectPath::VCProjectPath(const QString& newName, const QString& newPath)
	: name(newName), path(newPath)
{}

inline VCProjectPath::VCProjectPath(const VCProjectPath& other)
	: name(other.name), path(other.path), type(other.type)
{}

inline QDebug operator<<(QDebug dbg, const VCProjectPath& path)
{
	dbg.nospace()
	        << "VCProjectPath("
	        << "name:" << path.name
	        << ", path:" << path.path
	        << ", type:" << static_cast<int>(path.type)
	        << ")";
	return dbg.space();
}

#endif // ifndef VCSDataStructures_H
