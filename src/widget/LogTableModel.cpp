#include "LogTableModel.h"

#include <QRegularExpression>
#include <QPainter>

#include "utils/VersionControlManager.h"


LogTableModel::LogTableModel(QObject* parent)
	: QAbstractItemModel(parent)
{}

LogTableModel::~LogTableModel()
{}

void LogTableModel::Update(
	const QList<VCLogEntry>& logs, const QString& version)
{
	beginResetModel();
	m_CurVersion = version;
	m_Logs = logs;
	endResetModel();
	UpdateCurrentVersion();
}

void LogTableModel::Clear()
{
	beginResetModel();
	m_CurVersion = "";
	m_Logs.clear();
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

QString LogTableModel::GetIndexAuthor(const QModelIndex& index) const
{
	if (!index.isValid() || (index.row() < 0) || (index.row() >= m_Logs.size())) {
		return -1;
	}
	const VCLogEntry& entry = m_Logs[index.row()];

	return entry.author;
}

VCLogEntry LogTableModel::GetIndexLogEntry(const QModelIndex& index) const
{
	if (!index.isValid() || (index.row() < 0) || (index.row() >= m_Logs.size())) {
		return VCLogEntry();
	}
	const VCLogEntry& entry = m_Logs[index.row()];

	return entry;
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
			return QVariant();
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

	if ((role == Qt::UserRole + 1) && (index.column() == 1)) {
		QStringList operationStrings;
		const VCLogEntry& entry = m_Logs[index.row()];
		for (const FileOperation& operation : entry.operations) {
			operationStrings.append(FileOperationToString(operation));
		}
		return operationStrings;
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

bool LogTableSortFilterProxyModel::filterAcceptsRow(
	int sourceRow, const QModelIndex& sourceParent) const
{
	foreach(auto filterIten, m_FilterItems)
	{
		QModelIndex index = sourceModel()->index(sourceRow, filterIten, sourceParent);
		if (sourceModel()->data(index).toString().contains(filterRegExp())) {
			return true;
		}
	}
	QModelIndex index0 = sourceModel()->index(sourceRow, 1, sourceParent);
	QModelIndex index1 = sourceModel()->index(sourceRow, 2, sourceParent);
	return sourceModel()->data(index0).toString().contains(filterRegExp()) ||
	       sourceModel()->data(index1).toString().contains(filterRegExp());
}

bool LogTableSortFilterProxyModel::lessThan(
	const QModelIndex& left, const QModelIndex& right) const
{
	QVariant leftData = sourceModel()->data(left);
	QVariant rightData = sourceModel()->data(right);
	if (leftData.type() == QVariant::DateTime) {
		return leftData.toDateTime() < rightData.toDateTime();
	} else if (leftData.canConvert<int>() && leftData.canConvert<int>()) {
		return leftData.toInt() < rightData.toInt();
	} else {
		static QRegExp emailPattern("[\\w\\.]*@[\\w\\.]*)");
		QString leftString = leftData.toString();
		if ((left.column() == 1) && (emailPattern.indexIn(leftString) != -1)) {
			leftString = emailPattern.cap(1);
		}
		QString rightString = rightData.toString();
		if ((right.column() == 1) && (emailPattern.indexIn(rightString) != -1)) {
			rightString = emailPattern.cap(1);
		}
		return QString::localeAwareCompare(leftString, rightString) < 0;
	}
}

void LogTableSortFilterProxyModel::SetFilterItems(QList<int>filterItems)
{
	m_FilterItems = filterItems;
}

LogTableDelegate::LogTableDelegate(QObject* parent) : QStyledItemDelegate(parent)
{}

void LogTableDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	if (1 == index.column()) {
		auto model = index.model();
		if (model) {
			QStringList operations = model->data(index, Qt::UserRole + 1).toStringList();
			QRect rect = option.rect;
			int   size = 24;
			if (operations.contains("M")) {
				QRect iconRect(rect.left() + 0 * size, rect.top() + 1, size, size);
				painter->drawPixmap(iconRect, QPixmap(":/image/icon/modified.ico"));
			}
			if (operations.contains("A")) {
				QRect iconRect(rect.left() + 1 * size, rect.top() + 1, size, size);
				painter->drawPixmap(iconRect, QPixmap(":/image/icon/add.ico"));
			}
			if (operations.contains("D")) {
				QRect iconRect(rect.left() + 2 * size, rect.top() + 1, size, size);
				painter->drawPixmap(iconRect, QPixmap(":/image/icon/delete.ico"));
			}
			if (operations.contains("R")) {
				QRect iconRect(rect.left() + 3 * size, rect.top() + 1, size, size);
				painter->drawPixmap(iconRect, QPixmap(":/image/icon/rename.ico"));
			}
		}
	}

	QStyleOptionViewItem newOption(option);
	if (index.row() == m_CurVerRow) {
		newOption.font.setBold(true);
		newOption.palette.setColor(QPalette::Text,            Qt::blue);
		newOption.palette.setColor(QPalette::HighlightedText, Qt::blue);
	} else {
		newOption.palette.setColor(QPalette::Text,            Qt::black);
		newOption.palette.setColor(QPalette::HighlightedText, Qt::black);
	}
	QStyledItemDelegate::paint(painter, newOption, index);
}

void LogTableDelegate::SetCurrentVersionRow(int row)
{
	m_CurVerRow = row;
}
