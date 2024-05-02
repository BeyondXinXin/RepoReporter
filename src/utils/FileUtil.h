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

	static QString GetFileName(const QString& path);

	static QString GetFullPath(const QString& path);

	static QString GetExistingDirectory(
		QString defaultDir = QCoreApplication::applicationDirPath());

	static QString GetExistingFile(
		const QString& filter,
		QString defaultDir = QCoreApplication::applicationDirPath());

	static void OpenFileBrowser(const QString& path);
};

#endif // FILEUTIL_H
