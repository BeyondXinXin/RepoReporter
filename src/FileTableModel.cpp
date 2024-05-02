#include "FileTableModel.h"

#include <QDebug>

#include "VersionControlManager.h"

FileTableModel::FileTableModel(QObject* parent)
	: QAbstractItemModel(parent)
{}

FileTableModel::~FileTableModel()
{}

void FileTableModel::UpdataFile(const QString& path, const QList<QString>& versions)
{
	beginResetModel();
	m_Files.clear();
	if (!versions.isEmpty()) {
		m_Files = VersionControlManager::GetChangesForVersion(path, versions);
	}
	endResetModel();
}

QModelIndex FileTableModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	if (!parent.isValid()) {
		return createIndex(row, column);
	} else {
		return QModelIndex();
	}
}

QModelIndex FileTableModel::parent(const QModelIndex& child) const
{
	return QModelIndex(); // 文件列表不是一个树形结构，所以不需要父索引
}

int FileTableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return 0;
	}
	return m_Files.size();
}

int FileTableModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 5;
}

QVariant FileTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	if (role == Qt::DisplayRole) {
		const VCFileEntry& file = m_Files.at(index.row());

		switch (index.column()) {
		case 0:
			return file.filePath;

		case 1:
			return file.extensionName;

		case 2:
			return FileOperationToString(file.operation);

		case 3:
			return file.addNum;

		case 4:
			return file.deleteNum;

		default:
			return QVariant();
		}
	}

	return QVariant();
}

QVariant FileTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
		switch (section) {
		case 0:
			return u8"路径";

		case 1:
			return u8"拓展名";

		case 2:
			return u8"操作";

		case 3:
			return u8"添加行数";

		case 4:
			return u8"删除行数";

		default:
			return u8"";
		}
	}

	return QVariant();
}
