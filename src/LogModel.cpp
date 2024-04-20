﻿#include "LogModel.h"

#include "VersionControlManager.h"


LogModel::LogModel(QObject* parent)
	: QAbstractItemModel(parent)
{}

LogModel::~LogModel()
{}

void LogModel::UpdataLog(const QString& path)
{
	static int i = 0;

	beginResetModel();
	m_Logs.clear();

	switch (i % 5) {
	case 0:

		m_Logs = VersionControlManager::FetchLog("D:\\GitHub\\RepoReporter");
		break;

	case 1:

		m_Logs = VersionControlManager::FetchLog("D:\\GitHub\\qt-creator-settings");
		break;

	case 2:

		m_Logs = VersionControlManager::FetchLog("D:\\GitHub\\BlogDemo");
		break;

	case 3:

		m_Logs = VersionControlManager::FetchLog("D:\\GitHub\\RepoReporter");
		break;

	case 4:

		m_Logs = VersionControlManager::FetchLog("D:\\GitHub\\YiHangPavilion");
		break;

	default:
		break;
	}


	endResetModel();
	i++;
}

QString LogModel::GetIndexVersion(const QModelIndex& index) const
{
	if (!index.isValid() || (index.row() < 0) || (index.row() >= m_Logs.size())) {
		return -1;
	}
	const VCLogEntry& entry = m_Logs[index.row()];

	return entry.version;
}

int LogModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid())
		return 0;  // 仅支持顶层节点

	return m_Logs.size();
}

int LogModel::columnCount(const QModelIndex& parent) const
{
	return 5; // 固定为5列
}

QVariant LogModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();

	if (role == Qt::DisplayRole) {
		const VCLogEntry& entry = m_Logs[index.row()];

		switch (index.column()) {
		case 0: return entry.version;

		case 1: {
			QStringList operationStrings;

			for (const FileOperation& operation : entry.operations) {
				operationStrings.append(FileOperationToString(operation));
			}
			return operationStrings.join(", ");
		}

		case 2: return entry.author;

		case 3: return entry.date.toString("yyyy/M/d hh:mm:ss");

		case 4: return entry.message;

		default: return QVariant();
		}
	}
	return QVariant();
}

QVariant LogModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if ((role == Qt::DisplayRole) && (orientation == Qt::Horizontal)) {
		switch (section) {
		case 0: return u8"版本";

		case 1: return u8"操作";

		case 2: return u8"作者";

		case 3: return u8"日期";

		case 4: return u8"信息";

		default: return QVariant();
		}
	}
	return QVariant();
}

bool LogModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (data(index, role) != value) {
		// 如果需要，在此处进行数据修改
		emit dataChanged(index, index, { role });

		return true;
	}
	return false;
}

QModelIndex LogModel::index(int row, int column, const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) // Check if the index is valid
		return QModelIndex();

	if (!parent.isValid())              // For top-level items
		return createIndex(row, column);
	else
		return QModelIndex();       // Since it's not a hierarchical model, return an invalid index for non-top-level items
}

QModelIndex LogModel::parent(const QModelIndex& index) const
{
	return QModelIndex(); // Since it's a flat model, return an invalid index
}

Qt::ItemFlags LogModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable; // You can add Qt::ItemIsEditable if you want the items to be editable
}
