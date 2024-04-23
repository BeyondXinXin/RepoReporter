#include "FileTableView.h"

#include <QHeaderView>

#include "FileModel.h"

FileTableView::FileTableView(QWidget* parent)
	: QTableView(parent)
{
	m_Model = new FileModel(this);
	setModel(m_Model);

	InitUI();
}

void FileTableView::ChangeLog(const QList<QString>& versions)
{
	m_Model->UpdataFile(m_CurPaht, versions);
}

void FileTableView::ChangeProPath(const QString& path)
{
	m_CurPaht = path;
}

void FileTableView::contextMenuEvent(QContextMenuEvent* event)
{
}

void FileTableView::InitUI()
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
