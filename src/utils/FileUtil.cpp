#include "FileUtil.h"

#include <QDir>
#include <QDebug>
#include <QFileDialog>
#include <QDesktopServices>
#include <QIcon>
#include <QFileIconProvider>

bool FileUtil::FileCopy(const QString& src, const QString& dst, const bool cover)
{
	if (!QFile::exists(src)) {
		return false;
	}
	if (cover && QFile::exists(dst)) {
		QFile::remove(dst);
	}
	return QFile::copy(src, dst);
}

bool FileUtil::FileExists(const QString& strFile)
{
	QFile tempFile(strFile);
	return tempFile.exists();
}

bool FileUtil::DirCopy(const QString& src, const QString& dst)
{
	QDir dir(src);
	if (!dir.exists()) {
		return false;
	}
	foreach(QString d, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
	{
		QString dst_path = dst + QDir::separator() + d;
		FileUtil::DirMake(dst_path);
		FileUtil::DirCopy(src + QDir::separator() + d, dst_path);
	}
	foreach(QString f, dir.entryList(QDir::Files))
	{
		QFile::copy(src + QDir::separator() + f, dst + QDir::separator() + f);
	}
	return true;
}

bool FileUtil::DirExist(const QString& path)
{
	QDir dir(path);
	return dir.exists();
}

bool FileUtil::DirMake(const QString& path)
{
	QString fullPath = GetDirectoryFromPath(path);

	QDir dir(fullPath);
	if (dir.exists()) {
		return true;
	} else {
		return dir.mkpath(fullPath);
	}
}

bool FileUtil::DirRemove(const QString& path)
{
	if (path.isEmpty()) {
		qWarning() << "the path is empty error! ";
		return false;
	}
	QDir dir(path);
	return dir.removeRecursively();
}

bool FileUtil::DeleteFileOrFolder(const QString& strPath)
{
	if (strPath.isEmpty() || !QDir().exists(strPath)) {
		return false;
	}
	QFileInfo FileInfo(strPath);
	if (FileInfo.isFile()) {
		QFile::remove(strPath);
	} else if (FileInfo.isDir()) {
		QDir qDir(strPath);
		qDir.removeRecursively();
	}
	return true;
}

QString FileUtil::GetFileNameFromPath(const QString& path)
{
	QFileInfo fileInfo(path);
	return fileInfo.fileName();
}

QString FileUtil::GetDirectoryFromPath(const QString& path)
{
	QFileInfo fileInfo(path);
	return fileInfo.absolutePath();
}

QString FileUtil::GetFullAbsolutePath(const QString& path)
{
	QFileInfo fileInfo(path);
	return fileInfo.absoluteFilePath();
}

QString FileUtil::SelectDirectory(QString defaultDir)
{
	return QFileDialog::getExistingDirectory(nullptr, "选择文件夹", defaultDir);
}

QString FileUtil::SelectFile(const QString& filter, QString defaultDir)
{
	return QFileDialog::getOpenFileName(nullptr, "选择文件", defaultDir, filter);
}

void FileUtil::OpenDirectoryInExplorer(const QString& path)
{
	QString directory = path;
	int     iterations = 0;
	while (!QDir(directory).exists()) {
		if (directory == QDir::rootPath()) {
			break;
		}
		QDir parentDir(directory);
		parentDir.cdUp();
		directory = parentDir.path();
		iterations++;
		if (iterations > 20) {
			QDesktopServices::openUrl(QUrl("file:///"));
			return;
		}
	}
	QDesktopServices::openUrl(QUrl::fromLocalFile(directory));
}

QIcon FileUtil::getFileIcon(const QString& filePath)
{
	QFileIconProvider iconProvider;
	QFileInfo fileInfo(filePath);
	return iconProvider.icon(fileInfo);
}
