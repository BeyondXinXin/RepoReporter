#include "FileTableView.h"

#include <QHeaderView>
#include <QAction>
#include <QMenu>
#include <QContextMenuEvent>
#include <iostream>
#include <QDesktopServices>
#include <QClipboard>
#include <QGuiApplication>

#include "FileTableModel.h"
#include "utils/ConfigManager.h"
#include "utils/VersionControlManager.h"
#include "utils/FileUtil.h"

FileTableView::FileTableView(QWidget* parent)
	: QTableView(parent)
{
	m_Model = new FileTableModel(this);
	setModel(m_Model);
	m_Delegate = new FileTableDelegate(this);
	setItemDelegate(m_Delegate);

	InitUI();
	InitConnect();
}

void FileTableView::ChangeLog(const QList<QString>& versions)
{
	if (VersionControlManager::CurrentRepoType == RepoType::Svn) {
		hideColumn(3);
		hideColumn(4);
	} else {
		showColumn(3);
		showColumn(4);
		QHeaderView* header = horizontalHeader();
		header->resizeSection(3, 55);
		header->resizeSection(4, 55);
	}

	QMap<QString, VCFileEntry> fileMaps;
	foreach(QString version, versions)
	{
		QMap<QString, VCFileEntry>& fileEntrys = m_RepoFileDictionary[version];
		foreach(VCFileEntry fileEntry, fileEntrys.values())
		{
			if (fileMaps.contains(fileEntry.filePath)) {
				fileMaps[fileEntry.filePath].addNum += fileEntry.addNum;
				fileMaps[fileEntry.filePath].deleteNum += fileEntry.deleteNum;
			} else {
				fileMaps[fileEntry.filePath] = fileEntry;
			}
		}
	}
	m_Model->Update(fileMaps.values());
	m_CurVersions = versions;
}

void FileTableView::ChangeRepo(
	const QString& path,
	QHash<QString, QMap<QString, VCFileEntry> >repoFileDictionary)
{
	m_CurPaht = path;
	m_RepoFileDictionary = repoFileDictionary;
}

void FileTableView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);
	QList<QModelIndex> selectedIndexs = GetSelectIndexs();
	if (selectedIndexs.isEmpty()) {
		return;
	} else if (1 == selectedIndexs.size()) {
		menu.addAction(m_CompareWithBaseAction);
		menu.addSeparator();
		menu.addAction(m_ShowLogAction);
		menu.addAction(m_ExportAction);
		menu.addAction(m_OpenAction);
		menu.addAction(m_BrowseAction);
		menu.addSeparator();
		menu.addAction(m_CompareAction);
		menu.addAction(m_MarkCompareAction);
		menu.addSeparator();
		menu.addMenu(m_CopySubMenu);
	} else {
		menu.addAction(m_CompareWithBaseAction);
		menu.addSeparator();
		menu.addAction(m_ExportAction);
		menu.addSeparator();
		menu.addMenu(m_CopySubMenu);
	}
	menu.exec(event->globalPos() - QPoint(20, 10));
}

void FileTableView::showEvent(QShowEvent* event)
{
	QTableView::showEvent(event);
	QHeaderView* header = horizontalHeader();
	int column = m_Model->columnCount();
	QList<int> sizeList =
		ConfigManager::GetInstance().ReadList<int>("FileTableViewSectionSize", QList<int>{});

	for (int i = 0; i < column; i++) {
		header->resizeSection(i, sizeList.at(i));
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

void FileTableView::mouseDoubleClickEvent(QMouseEvent* event)
{
	QModelIndex index = indexAt(event->pos());
	if (index.isValid()) {
		QModelIndex newIndex = index.sibling(index.row(), 0);
		VersionControlManager::ShowCompare(
			m_CurPaht, m_Model->GetFileName(newIndex),
			m_CurVersions.last(), m_CurVersions.first());
	}
	QTableView::mouseDoubleClickEvent(event);
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

	horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	horizontalHeader()->setSectionResizeMode(2, QHeaderView::Fixed);
	horizontalHeader()->setSectionResizeMode(3, QHeaderView::Fixed);
	horizontalHeader()->setSectionResizeMode(4, QHeaderView::Fixed);
	horizontalHeader()->resizeSection(1, 95);
	horizontalHeader()->resizeSection(2, 55);
	horizontalHeader()->resizeSection(3, 55);
	horizontalHeader()->resizeSection(4, 55);

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

	m_CompareAction->setEnabled(false);

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
	QList<QModelIndex> selectedIndexs = GetSelectIndexs();
	for (int i = 0; i < selectedIndexs.size(); i++) {
		VersionControlManager::ShowCompare(
			m_CurPaht, m_Model->GetFileName(selectedIndexs.at(i)),
			m_CurVersions.last(), m_CurVersions.first());
	}
}

void FileTableView::OnShowLogAction()
{
	QString filePath = m_Model->GetFileName(GetSelectIndexs().at(0));
	VersionControlManager::ShowLog(m_CurPaht, filePath);
}

void FileTableView::OnExportAction()
{
	QString targetPath = FileUtil::SelectDirectory();
	if (targetPath.isEmpty()) {
		return;
	}
	QStringList files;
	foreach(auto index, GetSelectIndexs())
	{
		files << m_Model->GetFileName(index);
	}
	VersionControlManager::ExportFile(m_CurPaht, files, m_CurVersions.first(), targetPath);
}

void FileTableView::OnOpenAction()
{
	QString filePath = m_Model->GetFileName(GetSelectIndexs().at(0));
	VersionControlManager::OpenFile(m_CurPaht, filePath, m_CurVersions.first());
}

void FileTableView::OnBrowseAction()
{
	QString filePath = m_Model->GetFileName(GetSelectIndexs().at(0));
	VersionControlManager::OpenFileDirectory(m_CurPaht, filePath);
}

void FileTableView::OnCompareAction()
{
	QString CompareFile = m_Model->GetFileName(GetSelectIndexs().at(0));
	QString CompareVersion = m_CurVersions.first();
	if (!CompareFile.isEmpty() && !m_MarkCompareFile.isEmpty()) {
		VersionControlManager::CompareFiles(
			m_CurPaht, CompareFile, CompareVersion, m_MarkCompareFile, m_MarkCompareVersion);
	}
}

void FileTableView::OnMarkCompareAction()
{
	m_MarkCompareFile = m_Model->GetFileName(GetSelectIndexs().at(0));
	m_MarkCompareVersion = m_CurVersions.first();

	if (m_MarkCompareFile.isEmpty()) {
		m_CompareAction->setText(u8"与标记比较");
		m_CompareAction->setEnabled(false);
	} else {
		m_CompareAction->setText(
			QString(u8"与'%1'比较").arg(FileUtil::GetFileNameFromPath(m_MarkCompareFile)));
		m_CompareAction->setEnabled(true);
	}
}

void FileTableView::OnCopyFullPathAction()
{
	QStringList filePaths;
	foreach(auto index, GetSelectIndexs())
	{
		filePaths << FileUtil::GetFullAbsolutePath(m_Model->GetFileName(index));
	}
	QString filePath = filePaths.join("\n");
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(filePath);
}

void FileTableView::OnCopyRelativePathAction()
{
	QStringList filePaths;
	foreach(auto index, GetSelectIndexs())
	{
		filePaths << m_Model->GetFileName(index);
	}
	QString filePath = filePaths.join("\n");
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(filePath);
}

void FileTableView::OnCopyFileNameAction()
{
	QStringList fileNames;
	foreach(auto index, GetSelectIndexs())
	{
		fileNames << FileUtil::GetFileNameFromPath(m_Model->GetFileName(index));
	}
	QString fileName = fileNames.join("\n");
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(fileName);
}

QList<QModelIndex>FileTableView::GetSelectIndexs() const
{
	QModelIndexList indexes = selectionModel()->selection().indexes();
	QSet<int> selectedRows;
	QList<QModelIndex> selectedIndexs;
	foreach(auto index, indexes)
	{
		if (selectedRows.contains(index.row())) {
			continue;
		}
		selectedRows << index.row();
		selectedIndexs << index;
	}

	for (int i = 0; i < selectedIndexs.size(); i++) {
		selectedIndexs[i] = selectedIndexs.at(i).sibling(selectedIndexs.at(i).row(), 0);
	}
	return selectedIndexs;
}
