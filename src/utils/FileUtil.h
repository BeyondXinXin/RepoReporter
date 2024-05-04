#ifndef FILEUTIL_H
#define FILEUTIL_H

#include <QFile>
#include <QCoreApplication>

class FileUtil : public QFile {
public:

	static bool FileCopy(const QString& src,
	                     const QString& dst,
	                     const bool cover = false);

	static bool FileExists(const QString& strFile);

	static bool DirCopy(const QString& src, const QString& dst);

	static bool DirExist(const QString& path);

	static bool DirMake(const QString& path);

	static bool DirRemove(const QString& path);

	static bool DeleteFileOrFolder(const QString& strPath);

	static QString GetFileNameFromPath(const QString& path);

	static QString GetDirectoryFromPath(const QString& path);

	static QString GetFullAbsolutePath(const QString& path);

	static QString SelectDirectory(
		QString defaultDir = QCoreApplication::applicationDirPath());

	static QString SelectFile(
		const QString& filter,
		QString defaultDir = QCoreApplication::applicationDirPath());

	static void OpenDirectoryInExplorer(const QString& path);

	static QIcon getFileIcon(const QString& filePath);
};

#endif // FILEUTIL_H
