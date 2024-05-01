#include "LogTableView.h"

#include <QHeaderView>
#include <QMenu>
#include <QTableWidget>
#include <QContextMenuEvent>

#include "LogTableModel.h"
#include "VersionControlManager.h"
#include "ConfigManager.h"
#include "LogTableDelegate.h"

LogTableView::LogTableView(QWidget* parent)
	: QTableView(parent)
{
	m_Model = new LogTableModel(this);
	setModel(m_Model);

	m_Delegate = new LogTableDelegate(this);
	setItemDelegate(m_Delegate);

	InitUI();
	InitConnect();
}

void LogTableView::ChangeProPath(const QString& path)
{
	m_CurPaht = path;
	m_Model->UpdataLog(path);
}

void LogTableView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);

	menu.addMenu(m_CopySubMenu);
	menu.exec(event->globalPos() - QPoint(20, 10));
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

void LogTableView::InitConnect()
{
	connect(selectionModel(), &QItemSelectionModel::selectionChanged,
	        this, &LogTableView::SlotSelectionChanged);

	connect(m_Model,          &LogTableModel::SgnCurVerChange,
	        m_Delegate, &LogTableDelegate::SetCurrentVersionRow);


	m_CopyFullContentAction = new QAction(QIcon(":/icons/add.png"), u8"完整内容", this);
	connect(m_CopyFullContentAction, &QAction::triggered,
	        this, &LogTableView::OnCopyFullContentAction);

	m_CopyAuthorNameAction = new QAction(QIcon(":/icons/add.png"), u8"作者姓名", this);
	connect(m_CopyAuthorNameAction, &QAction::triggered,
	        this, &LogTableView::OnCopyAuthorNameAction);

	m_CopyInformationAction = new QAction(QIcon(":/icons/add.png"), u8"信息", this);
	connect(m_CopyInformationAction, &QAction::triggered,
	        this, &LogTableView::OnCopyInformationAction);

	m_CopySubMenu = new QMenu(u8"复制到剪贴板", this);
	m_CopySubMenu->addAction(m_CopyFullContentAction);
	m_CopySubMenu->addAction(m_CopyAuthorNameAction);
	m_CopySubMenu->addAction(m_CopyInformationAction);
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

void LogTableView::OnCopyFullContentAction()
{
}

void LogTableView::OnCopyAuthorNameAction()
{
}

void LogTableView::OnCopyInformationAction()
{
}
