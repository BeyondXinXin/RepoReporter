﻿#include "LogModel.h"

LogModel::LogModel(QObject* parent)
	: QAbstractItemModel(parent)
{}

LogModel::~LogModel()
{}

void LogModel::UpdataLog(const QString& path)
{
	beginResetModel();
	m_Logs.clear();

	// 添加新数据
	m_Logs.append({ 1, u8"新增", u8"张三", u8"2024-04-12", path });
	m_Logs.append({ 2, u8"修改", u8"李四", u8"2024-04-13", path });
	m_Logs.append({ 3, u8"删除", u8"王五", u8"2024-04-14", path });

	endResetModel();
}

int LogModel::GetIndexVersion(const QModelIndex& index) const
{
	if (!index.isValid() || (index.row() < 0) || (index.row() >= m_Logs.size())) {
		return -1;
	}
	const LogEntry& entry = m_Logs[index.row()];

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
		const LogEntry& entry = m_Logs[index.row()];

		switch (index.column()) {
		case 0: return entry.version;

		case 1: return entry.operation;

		case 2: return entry.author;

		case 3: return entry.date;

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
