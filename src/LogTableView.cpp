#include "LogTableView.h"

#include <QHeaderView>
#include <QMenu>
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

	horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	selectionModel()->connect(
		selectionModel(), &QItemSelectionModel::selectionChanged,
		this, &LogTableView::SlotSelectionChanged);
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

	QModelIndexList indexes = selected.indexes();

	if (!indexes.isEmpty()) {
		QList<int> vers;

		foreach(auto index, indexes)
		{
			vers << m_Model->GetIndexVersion(index);
		}
		emit SgnChangeSelectLog(vers);
	}
}
