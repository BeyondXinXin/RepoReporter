#include "FileTableView.h"

#include <QHeaderView>

#include "FileModel.h"

FileTableView::FileTableView(QWidget* parent)
	: QTableView(parent)
{
	m_Model = new FileModel(this);
	setModel(m_Model);

	setDragEnabled(false);
	setAcceptDrops(false);
	setDragDropMode(QAbstractItemView::NoDragDrop);

	horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
	setSelectionBehavior(QAbstractItemView::SelectRows);

	show();
}

void FileTableView::contextMenuEvent(QContextMenuEvent* event)
{
}

void FileTableView::AddProject()
{
	
}
