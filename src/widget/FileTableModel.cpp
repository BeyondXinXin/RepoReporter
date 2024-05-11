#include "FileTableModel.h"

#include <QDebug>
#include <QFileInfo>
#include <QtConcurrent>

#include "utils/VersionControlManager.h"
#include "utils/FileUtil.h"

FileTableModel::FileTableModel(QObject* parent)
	: QAbstractItemModel(parent)
{}

FileTableModel::~FileTableModel()
{}

void FileTableModel::Update(const QList<VCFileEntry>& files)
{
	beginResetModel();
	m_Files = files;
	endResetModel();
}

QString FileTableModel::GetFileName(const QModelIndex& index) const
{
	if (!index.isValid() || (index.row() < 0) || (index.row() >= m_Files.size())) {
		return "";
	}

	const VCFileEntry& entry = m_Files[index.row()];
	return entry.filePath;
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
	Q_UNUSED(child)
	return QModelIndex();
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
	} else if (role == Qt::DecorationRole) {
		if (0 == index.column()) {
			const VCFileEntry& file = m_Files.at(index.row());
			return QVariant::fromValue(FileUtil::getFileIcon(file.filePath));
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
			return u8"添加";

		case 4:
			return u8"删除";

		default:
			return u8"";
		}
	}

	return QVariant();
}

FileTableDelegate::FileTableDelegate(QObject* parent) : QStyledItemDelegate(parent)
{}

void FileTableDelegate::paint(
	QPainter* painter,
	const QStyleOptionViewItem& option,
	const QModelIndex& index) const
{
	QColor textColor = Qt::black;

	auto model = index.model();
	if (model) {
		QModelIndex   siblingIndex = index.siblingAtColumn(2);
		FileOperation operation =
			StringToFileOperation(model->data(siblingIndex).toString());

		switch (operation) {
		case FileOperation::Add: {
			textColor = AddColor;
			break;
		};

		case FileOperation::Modify: {
			textColor = ModifyColor;
			break;
		};

		case FileOperation::Delete: {
			textColor = DeleteColor;
			break;
		};

		case FileOperation::Rename: {
			textColor = RenameColor;
			break;
		};
		}
	}

	QStyleOptionViewItem newOption(option);
	newOption.palette.setColor(QPalette::Text,            textColor);
	newOption.palette.setColor(QPalette::HighlightedText, textColor);
	QStyledItemDelegate::paint(painter, newOption, index);
}
