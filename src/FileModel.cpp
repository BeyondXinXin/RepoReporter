#include "FileModel.h"

FileModel::FileModel(QObject* parent)
	: QAbstractItemModel(parent)
{
	m_files.append(QPair<QString, QString>(QString::fromUtf8(u8"path/to/file1.txt"), QString::fromUtf8(u8"����1")));
	m_files.append(QPair<QString, QString>(QString::fromUtf8(u8"path/to/file2.txt"), QString::fromUtf8(u8"����2")));
	m_files.append(QPair<QString, QString>(QString::fromUtf8(u8"path/to/file3.txt"), QString::fromUtf8(u8"����3")));
}

FileModel::~FileModel()
{
}

QModelIndex FileModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent))
		return QModelIndex();

	if (!parent.isValid())
	{
		// ȷ���к���Ч
		if (row < 0 || row >= m_files.size())
			return QModelIndex();

		// ���ļ�·��ת��Ϊ��ϣֵ
		quintptr pointer = qHash(m_files.at(row).first);

		// ��������
		return createIndex(row, column, reinterpret_cast<void*>(pointer));
	}

	return QModelIndex();
}


QModelIndex FileModel::parent(const QModelIndex& child) const
{
	return QModelIndex(); // �ļ��б���һ�����νṹ�����Բ���Ҫ������
}

int FileModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0; // �ļ��б�û������

	return m_files.size();
}

int FileModel::columnCount(const QModelIndex& parent) const
{
	Q_UNUSED(parent);
	return 2; // �̶�Ϊ���У�·���Ͳ���
}

QVariant FileModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole)
	{
		const QPair<QString, QString>& file = m_files.at(index.row());
		switch (index.column())
		{
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
	if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		if (section == 0)
			return u8"·��";
		else if (section == 1)
			return u8"����";
	}

	return QVariant();
}
