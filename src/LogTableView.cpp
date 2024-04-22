#include "LogTableView.h"

#include <QHeaderView>
#include <QMenu>
#include <QTableWidget>
#include <QContextMenuEvent>

#include "LogModel.h"

LogTableView::LogTableView(QWidget* parent)
	: QTableView(parent)
{
	m_AddAction = new QAction(QIcon(":/icons/add.png"), tr(u8"添加项目"), this);
	connect(m_AddAction, &QAction::triggered, this, &LogTableView::AddProject);

	m_Model = new LogModel(this);
	setModel(m_Model);

	setDragEnabled(false);
	setAcceptDrops(false);
	setDragDropMode(QAbstractItemView::NoDragDrop);

	verticalHeader()->setDefaultSectionSize(10);
	horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	horizontalHeader()->setHighlightSections(false);
	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	connect(selectionModel(), &QItemSelectionModel::selectionChanged,
	        this, &LogTableView::SlotSelectionChanged);

	setShowGrid(false);
}

void LogTableView::ChangeProPath(const QString& path)
{
	m_Model->UpdataLog(path);
}

void LogTableView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);

	menu.addAction(m_AddAction);
	menu.exec(event->globalPos());
}

void LogTableView::AddProject()
{
}

void LogTableView::SlotSelectionChanged(
	const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected)
	Q_UNUSED(selected)

	QItemSelectionModel* model = selectionModel();
	QItemSelection  range = model->selection();
	QModelIndexList indexes = range.indexes();

	if (indexes.isEmpty()) {
		return;
	}

	QSet<int> selectedRows;
	QList<QString> vers;
	QStringList    descriptions;

	foreach(auto index, indexes)
	{
		if (selectedRows.contains(index.row())) {
			continue;
		}
		selectedRows << index.row();
		vers << m_Model->GetIndexVersion(index);
		descriptions << m_Model->GetIndexMessage(index);
	}

	emit SgnChangeSelectLog(vers);
	emit SgnUpdateDescription(descriptions.join("\n------------\n"));
}
