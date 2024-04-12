#include "LogTableView.h"

#include <QHeaderView>

#include "LogModel.h"

LogTableView::LogTableView(QWidget * parent)
	: QTableView(parent)
{
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

}

void LogTableView::AddProject()
{
	
}
