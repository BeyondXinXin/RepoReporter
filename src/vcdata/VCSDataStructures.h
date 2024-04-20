#ifndef VCSDataStructures_H
#define VCSDataStructures_H


#include <QDebug>
#include <QString>
#include <QDateTime>

enum class FileOperation {
	Add,
	Modify,
	Delete
};
Q_DECLARE_METATYPE(FileOperation)

inline QString FileOperationToString(FileOperation operation)
{
	switch (operation) {
	case FileOperation::Add: return QStringLiteral("A");

	case FileOperation::Modify: return QStringLiteral("M");

	case FileOperation::Delete: return QStringLiteral("D");

	default: return QStringLiteral("Err");
	}
}

inline uint qHash(FileOperation key, uint seed = 0) noexcept
{
	return ::qHash(static_cast<int>(key), seed);
}

struct VCLogEntry {
	QString version;
	QSet<FileOperation>operations;
	QString message;
	QString author;
	QDateTime date;

	friend QDebug operator<<(QDebug dbg, const VCLogEntry& entry)
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
};


#endif // ifndef VCSDataStructures_H
