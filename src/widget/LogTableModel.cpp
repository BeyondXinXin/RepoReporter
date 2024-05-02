#include "LogTableModel.h"

#include <QRegularExpression>

#include "utils/VersionControlManager.h"


LogTableModel::LogTableModel(QObject* parent)
	: QAbstractItemModel(parent)
{}

LogTableModel::~LogTableModel()
{}

void LogTableModel::UpdataLog(const QString& path)
{
	beginResetModel();
	m_Logs.clear();
	m_CurVersion = "";

	if (!path.isEmpty()) {
		m_Logs = VersionControlManager::FetchLog(path, m_CurVersion);
	}
	endResetModel();

	UpdateCurrentVersion();
}

QString LogTableModel::GetIndexVersion(const QModelIndex& index) const
{
	if (!index.isValid() || (index.row() < 0) || (index.row() >= m_Logs.size())) {
		return -1;
	}
	const VCLogEntry& entry = m_Logs[index.row()];

	return entry.version;
}

QString LogTableModel::GetIndexMessage(const QModelIndex& index) const
{
	if (!index.isValid() || (index.row() < 0) || (index.row() >= m_Logs.size())) {
		return -1;
	}
	const VCLogEntry& entry = m_Logs[index.row()];

	return entry.message;
}

int LogTableModel::rowCount(const QModelIndex& parent) const
{
	if (parent.isValid()) {
		return 0;
	}

	return m_Logs.size();
}

int LogTableModel::columnCount(const QModelIndex& parent) const
{
	return 5;
}

QVariant LogTableModel::data(const QModelIndex& index, int role) const
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

		case 4: {
			QString message = entry.message;
			message.replace(QRegularExpression("\\n+"), " ");
			message.replace(QRegularExpression("\\s+"), " ");
			return message;
		}

		default: return QVariant();
		}
	}
	return QVariant();
}

QVariant LogTableModel::headerData(int section, Qt::Orientation orientation, int role) const
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

bool LogTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (data(index, role) != value) {
		// 如果需要，在此处进行数据修改
		emit dataChanged(index, index, { role });

		return true;
	}
	return false;
}

QModelIndex LogTableModel::index(int row, int column, const QModelIndex& parent) const
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

QModelIndex LogTableModel::parent(const QModelIndex& index) const
{
	return QModelIndex();
}

Qt::ItemFlags LogTableModel::flags(const QModelIndex& index) const
{
	if (!index.isValid())
		return Qt::NoItemFlags;

	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

void LogTableModel::UpdateCurrentVersion()
{
	int id = -1;
	for (int i = 0; i < m_Logs.size(); i++) {
		if (m_CurVersion.startsWith(m_Logs.at(i).version)) {
			id = i;
			break;
		}
	}
	emit SgnCurVerChange(id);
}
