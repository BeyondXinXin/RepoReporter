#include "FileTableView.h"

#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <iostream>

#include "FileTableModel.h"
#include "ConfigManager.h"

FileTableView::FileTableView(QWidget* parent)
	: QTableView(parent)
{
	m_Model = new FileTableModel(this);
	setModel(m_Model);

	InitUI();
	InitConnect();
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
	QMenu menu(this);

	menu.addAction(m_CompareWithBaseAction);
	menu.addSeparator();
	menu.addAction(m_ShowLogAction);
	menu.addAction(m_ExportAction);
	menu.addAction(m_SaveAsAction);
	menu.addAction(m_OpenAction);
	menu.addAction(m_BrowseAction);
	menu.addSeparator();
	menu.addAction(m_CompareAction);
	menu.addAction(m_MarkCompareAction);
	menu.addSeparator();
	menu.addMenu(m_CopySubMenu);

	menu.exec(event->globalPos() - QPoint(20, 10));
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

void FileTableView::InitConnect()
{
	auto fun = [&](QAction *& action, const QString& iconPath, const QString& name,
	               void (FileTableView::*SlogAction)())
		   {
			   action = new QAction(QIcon(iconPath), name, this);
			   connect(action, &QAction::triggered, this, SlogAction);
		   };

	fun(m_CompareWithBaseAction,  "", u8"比较",    &FileTableView::OnCompareWithBaseAction);
	fun(m_ShowLogAction,          "", u8"显示日志",  &FileTableView::OnShowLogAction);
	fun(m_ExportAction,           "", u8"导出",    &FileTableView::OnExportAction);
	fun(m_SaveAsAction,           "", u8"另存",    &FileTableView::OnSaveAsAction);
	fun(m_OpenAction,             "", u8"打开",    &FileTableView::OnOpenAction);
	fun(m_BrowseAction,           "", u8"浏览",    &FileTableView::OnBrowseAction);
	fun(m_CompareAction,          "", u8"与标记比较", &FileTableView::OnCompareAction);
	fun(m_MarkCompareAction,      "", u8"标记供比较", &FileTableView::OnMarkCompareAction);
	fun(m_CopyFullPathAction,     "", u8"完整路径",  &FileTableView::OnCopyFullPathAction);
	fun(m_CopyRelativePathAction, "", u8"相对路径",  &FileTableView::OnCopyRelativePathAction);
	fun(m_CopyFileNameAction,     "", u8"文件名称",  &FileTableView::OnCopyFileNameAction);

	m_CopySubMenu = new QMenu(u8"复制到剪贴板", this);
	m_CopySubMenu->addAction(m_CopyFullPathAction);
	m_CopySubMenu->addAction(m_CopyRelativePathAction);
	m_CopySubMenu->addAction(m_CopyFileNameAction);

	connect(selectionModel(), &QItemSelectionModel::selectionChanged,
	        this, &FileTableView::SlotSelectionChanged);
}

void FileTableView::SlotSelectionChanged(
	const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected)
	Q_UNUSED(selected)

	QModelIndexList indexes = selectionModel()->selection().indexes();
	if (indexes.isEmpty()) {
		emit SgnStateLabChange(1, 0);
		return;
	}
	QSet<int> selectedRows;
	foreach(auto index, indexes)
	{
		if (selectedRows.contains(index.row())) {
			continue;
		}
		selectedRows << index.row();
	}
	emit SgnStateLabChange(2, selectedRows.size());
}

void FileTableView::OnCompareWithBaseAction()
{
}

void FileTableView::OnShowLogAction()
{
}

void FileTableView::OnExportAction()
{
}

void FileTableView::OnSaveAsAction()
{
}

void FileTableView::OnOpenAction()
{
}

void FileTableView::OnBrowseAction()
{
}

void FileTableView::OnCompareAction()
{
}

void FileTableView::OnMarkCompareAction()
{
}

void FileTableView::OnCopyFullPathAction()
{
}

void FileTableView::OnCopyRelativePathAction()
{
}

void FileTableView::OnCopyFileNameAction()
{
}
