#include "FileModel.h"

#include <QDebug>

FileModel::FileModel(QObject* parent)
	: QAbstractItemModel(parent)
{}

FileModel::~FileModel()
{}

void FileModel::UpdataFile(const QList<QString>& versions)
{
	beginResetModel();
	m_files.clear();
	m_files.append(QPair<QString, QString>(QString::fromUtf8(u8"path/to/file1.txt"), QString::fromUtf8(u8"操作1")));
	m_files.append(QPair<QString, QString>(QString::fromUtf8(u8"path/to/file2.txt"), QString::fromUtf8(u8"操作2")));
	m_files.append(QPair<QString, QString>(QString::fromUtf8(u8"path/to/file3.txt"), QString::fromUtf8(u8"操作3")));
	endResetModel();
}

QModelIndex FileModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (!parent.isValid()) {
		// 确保行号有效
		if ((row < 0) || (row >= m_files.size()))
			return QModelIndex();

		// 将文件路径转换为哈希值
		quintptr pointer = qHash(m_files.at(row).first);

		// 创建索引
		return createIndex(row, column, reinterpret_cast<void *>(pointer));
	}

	return QModelIndex();
}

QModelIndex FileModel::parent(const QModelIndex& child) const
{
	return QModelIndex(); // 文件列表不是一个树形结构，所以不需要父索引
}

int FileModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;  // 文件列表没有子项

	return m_files.size();
}

int FileModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 2; // 固定为两列：路径和操作
}

QVariant FileModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole) {
		const QPair<QString, QString>& file = m_files.at(index.row());

		switch (index.column()) {
		case 0:
			return file.first;

		case 1:
			return file.second;

		default:
			return QVariant();
		}
	}

	return QVariant();
}

QVariant FileModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
		if (section == 0)
			return u8"路径";
		else if (section == 1)
			return u8"操作";
	}

	return QVariant();
}
