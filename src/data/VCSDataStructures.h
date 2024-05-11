#ifndef VCSDataStructures_H
#define VCSDataStructures_H


#include <QDebug>
#include <QString>
#include <QDateTime>
#include <QColor>

static const QColor AddColor(128, 0, 128);   // 紫色
static const QColor ModifyColor(0, 50, 177); // 深蓝色
static const QColor DeleteColor(100, 0, 58); // 暗红色
static const QColor RenameColor(0, 0, 255);  // 蓝色

static const QColor RepoUnsubmittedColor(150, 0, 85);


enum class RepoState {
	Normal      = 0,
	Unsubmitted = 1,
};
Q_DECLARE_METATYPE(RepoState)


enum class RepoType {
	Unknown = 0,
	Git     = 1,
	Svn     = 2,
};
Q_DECLARE_METATYPE(RepoType)


enum class FileOperation {
	Add,
	Modify,
	Delete,
	Rename
};
Q_DECLARE_METATYPE(FileOperation)

struct VCFileEntry {
	QString filePath;
	QString extensionName;
	int addNum = 0;
	int deleteNum = 0;
	FileOperation operation;

	friend QDebug operator<<(QDebug dbg, const VCFileEntry& entry);
};

struct VCLogEntry {
	QString version;
	QSet<FileOperation>operations;
	QString message;
	QString author;
	QDateTime date;
	friend QDebug operator<<(QDebug dbg, const VCLogEntry& entry);
};

struct VCRepoEntry {
	QString name;
	QString path;

	RepoType type = RepoType::Unknown;
	RepoState state = RepoState::Normal;

	VCRepoEntry(const QString& newName, const QString& newPath = "");
	VCRepoEntry(const VCRepoEntry& other);

	friend QDebug operator<<(QDebug dbg, const VCRepoEntry& path);
};

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
	    << "Date:" << entry.date.toString("yyyy/M/d hh:mm:ss");
	return dbg;
}

inline VCRepoEntry::VCRepoEntry(const QString& newName, const QString& newPath)
	: name(newName), path(newPath)
{}

inline VCRepoEntry::VCRepoEntry(const VCRepoEntry& other)
	: name(other.name), path(other.path), type(other.type), state(other.state)
{}

inline QDebug operator<<(QDebug dbg, const VCRepoEntry& path)
{
	dbg.nospace()
	        << "VCProjectPath("
	        << "name:" << path.name
	        << ", path:" << path.path
	        << ", type:" << static_cast<int>(path.type)
	        << ", state:" << static_cast<int>(path.state)
	        << ")";
	return dbg.space();
}

#endif // ifn
