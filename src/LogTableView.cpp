#include "LogTableView.h"

#include <QHeaderView>
#include <QMenu>
#include <QContextMenuEvent>

#include "LogModel.h"

LogTableView::LogTableView(QWidget * parent)
	: QTableView(parent)
{

	m_AddAction = new QAction(QIcon(":/icons/add.png"), tr(u8"添加项目"), this);
	connect(m_AddAction, &QAction::triggered, this, &LogTableView::AddProject);

	m_Model= new LogModel(this);
	setModel(m_Model);

	setDragEnabled(false);
	setAcceptDrops(false);
	setDragDropMode(QAbstractItemView::NoDragDrop);

	horizontalHeader()->setSectionResizeMode(4, QHeaderView::Stretch);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	show();
}

void LogTableView::contextMenuEvent(QContextMenuEvent * event)
{
	QMenu menu(this);
	menu.addAction(m_AddAction);
	menu.exec(event->globalPos());
}

void LogTableView::AddProject()
{
	m_Model->UpdataLog("111");
}
