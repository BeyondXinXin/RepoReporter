#include "FileTableView.h"

#include <QHeaderView>

#include "FileModel.h"
#include "ConfigManager.h"

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

void FileTableView::showEvent(QShowEvent* event)
{
	QTableView::showEvent(event);
	QHeaderView* header = horizontalHeader();
	int column = m_Model->columnCount();
	QList<int> sizeList = ConfigManager::GetInstance().ReadList<int>("FileTableViewSectionSize", QList<int>{});

	if (sizeList.size() == column) {
		for (int i = 0; i < column; i++) {
			header->resizeSection(i, sizeList.at(i));
		}
	}
}

void FileTableView::hideEvent(QHideEvent* event)
{
	QTableView::hideEvent(event);
	QHeaderView* header = horizontalHeader();
	QList<int>   sizeList;
	int column = m_Model->columnCount();

	for (int i = 0; i < column; i++) {
		sizeList << header->sectionSize(i);
	}

	ConfigManager::GetInstance().WriteList<int>("FileTableViewSectionSize", sizeList);
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
