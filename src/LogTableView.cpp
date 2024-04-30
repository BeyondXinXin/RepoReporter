#include "LogTableView.h"

#include <QHeaderView>
#include <QMenu>
#include <QTableWidget>
#include <QContextMenuEvent>

#include "LogModel.h"
#include "VersionControlManager.h"
#include "ConfigManager.h"

LogTableView::LogTableView(QWidget* parent)
	: QTableView(parent)
{
	m_Model = new LogModel(this);
	setModel(m_Model);

	m_AddAction = new QAction(QIcon(":/icons/add.png"), tr(u8"添加项目"), this);
	connect(m_AddAction,      &QAction::triggered, this, &LogTableView::AddProject);

	connect(selectionModel(), &QItemSelectionModel::selectionChanged,
	        this, &LogTableView::SlotSelectionChanged);

	InitUI();
}

void LogTableView::ChangeProPath(const QString& path)
{
	m_CurPaht = path;
	m_Model->UpdataLog(path);
}

void LogTableView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);

	menu.addAction(m_AddAction);
	menu.exec(event->globalPos());
}

void LogTableView::showEvent(QShowEvent* event)
{
	QTableView::showEvent(event);
	QHeaderView* header = horizontalHeader();
	int column = m_Model->columnCount();
	QList<int> sizeList = ConfigManager::GetInstance().ReadList<int>("LogTableViewSectionSize", QList<int>{});

	if (sizeList.size() == column) {
		for (int i = 0; i < column; i++) {
			header->resizeSection(i, sizeList.at(i));
		}
	}
}

void LogTableView::hideEvent(QHideEvent* event)
{
	QTableView::hideEvent(event);
	QHeaderView* header = horizontalHeader();
	QList<int>   sizeList;
	int column = m_Model->columnCount();

	for (int i = 0; i < column; i++) {
		sizeList << header->sectionSize(i);
	}

	ConfigManager::GetInstance().WriteList<int>("LogTableViewSectionSize", sizeList);
}

void LogTableView::InitUI()
{
	setDragEnabled(false);
	setAcceptDrops(false);
	setDragDropMode(QAbstractItemView::NoDragDrop);

	verticalHeader()->setDefaultSectionSize(10);

	horizontalHeader()->setHighlightSections(false);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	setShowGrid(false);
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

	if (descriptions.size() > 1) {
		for (int i = 0; i < descriptions.size(); i++) {
			descriptions[i] = vers[i] + u8"\n" + descriptions[i];
		}
	}

	emit SgnChangeSelectLog(vers);
	emit SgnUpdateDescription(descriptions.join("\n- - - - - - - - - - -\n"));
}
