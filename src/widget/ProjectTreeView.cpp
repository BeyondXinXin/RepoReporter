#include "ProjectTreeView.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QContextMenuEvent>
#include <QPainter>
#include <QStandardItem>
#include <QHeaderView>
#include <QIcon>
#include <QApplication>
#include <QPalette>

#include "ProjectTreeModel.h"
#include "window/ProjectDialog.h"
#include "utils/ConfigManager.h"
#include "utils/VersionControlManager.h"

ProjectTreeView::ProjectTreeView(QWidget* parent)
	: QTreeView(parent)
{
	m_Model = new ProjectTreeModel(this);
	setModel(m_Model);

	m_Delegate = new ProjectTreeDelegate(this);
	setItemDelegate(m_Delegate);

	InitUI();
	InitConnect();
}

void ProjectTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);

	if (selectedIndexes().isEmpty())
		menu.addAction(m_AddAction);
	else {
		menu.addAction(m_BrowseAction);
		menu.addAction(m_CheckAction);
		menu.addAction(m_ShowLogAction);
		menu.addSeparator();
		menu.addAction(m_PullAction);
		menu.addAction(m_SyncAction);
		menu.addSeparator();
		menu.addAction(m_AddAction);
		menu.addAction(m_EditAction);
		menu.addAction(m_DeleteAction);
	}
	menu.exec(event->globalPos());
}

void ProjectTreeView::mouseReleaseEvent(QMouseEvent* event)
{
	QTreeView::mouseReleaseEvent(event);

	if (!indexAt(event->pos()).isValid())
		clearSelection();
}

void ProjectTreeView::showEvent(QShowEvent* event)
{
	QList<bool> expandedStates =
		ConfigManager::GetInstance()
		.ReadList<bool>("ProjectTreeViewExpandedStates", QList<bool>{});

	for (int row = 0; row < expandedStates.size(); ++row) {
		QModelIndex index = model()->index(row, 0);

		if (expandedStates.at(row)) {
			expand(index);
		}
	}

	QList<int> selectItemList = ConfigManager::GetInstance().ReadList<int>(
		"ProjectTreeViewSelectItem", QList<int>{ -1, -1, -1, -1 });
	QModelIndex index = model()->index(selectItemList.at(0), selectItemList.at(1));
	QModelIndex selectItem;

	if (index.isValid()) {
		selectItem = model()->index(selectItemList.at(2), selectItemList.at(3), index);
	} else {
		selectItem = model()->index(selectItemList.at(2), selectItemList.at(3));
	}

	if (selectItem.isValid()) {
		m_LastSelectItem = selectItem;
		setCurrentIndex(m_LastSelectItem);
	}

	CheckRepoState();
	QTreeView::showEvent(event);
}

void ProjectTreeView::hideEvent(QHideEvent* event)
{
	QList<bool> expandedStates;
	int rowCount = model()->rowCount();

	for (int row = 0; row < rowCount; ++row) {
		QModelIndex index = model()->index(row, 0);
		bool expanded = isExpanded(index);

		expandedStates.append(expanded);
	}

	ConfigManager::GetInstance().WriteList<bool>(
		"ProjectTreeViewExpandedStates", expandedStates);


	QList<int> selectItemList;

	if (m_LastSelectItem.isValid()) {
		selectItemList << m_LastSelectItem.parent().row()
		               << m_LastSelectItem.parent().column()
		               << m_LastSelectItem.row()
		               << m_LastSelectItem.column();
	}
	ConfigManager::GetInstance().WriteList<int>(
		"ProjectTreeViewSelectItem", selectItemList);

	QTreeView::hideEvent(event);
}

void ProjectTreeView::InitUI()
{
	setIndentation(20);

	header()->setVisible(false);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	setDragEnabled(true);
	setAcceptDrops(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::SingleSelection);
}

void ProjectTreeView::InitConnect()
{
	auto fun = [&](QAction *& action, const QString& iconPath, const QString& name,
	               void (ProjectTreeView::*SlogAction)())
		   {
			   action = new QAction(QIcon(iconPath), name, this);
			   connect(action, &QAction::triggered, this, SlogAction);
		   };
	fun(m_BrowseAction,  "", u8"浏览",   &ProjectTreeView::OnBrowseAction);
	fun(m_CheckAction,   "", u8"检查",   &ProjectTreeView::OnCheckAction);
	fun(m_ShowLogAction, "", u8"日志",   &ProjectTreeView::OnShowLogAction);
	fun(m_PullAction,    "", u8"拉取",   &ProjectTreeView::OnPullAction);
	fun(m_SyncAction,    "", u8"同步",   &ProjectTreeView::OnSyncAction);
	fun(m_AddAction,     "", u8"添加项目", &ProjectTreeView::OnAddAction);
	fun(m_EditAction,    "", u8"编辑项目", &ProjectTreeView::OnEditAction);
	fun(m_DeleteAction,  "", u8"删除项目", &ProjectTreeView::OnDeleteAction);

	selectionModel()->connect(
		selectionModel(), &QItemSelectionModel::selectionChanged,
		this, &ProjectTreeView::SlotSelectionChanged);

	connect(m_Model, &ProjectTreeModel::SgnItemMoved,
	        this, &ProjectTreeView::SlotItemMoved);
}

void ProjectTreeView::CheckRepoState()
{
	m_Model->CheckAllRepoState();
}

QModelIndex ProjectTreeView::GetSelectIndexs() const
{
	QModelIndexList indexes = selectionModel()->selection().indexes();
	if (indexes.isEmpty()) {
		return QModelIndex();
	}
	return indexes.first();
}

void ProjectTreeView::OnBrowseAction()
{
	VersionControlManager::OpenFileDirectory(
		m_Model->GetIndexPath(m_LastSelectItem), "");
}

void ProjectTreeView::OnPullAction()
{
	VersionControlManager::RepoPull(
		m_Model->GetIndexPath(m_LastSelectItem));
}

void ProjectTreeView::OnSyncAction()
{
	VersionControlManager::RepoSync(
		m_Model->GetIndexPath(m_LastSelectItem));
}

void ProjectTreeView::OnCheckAction()
{
	VersionControlManager::RepoCheck(
		m_Model->GetIndexPath(m_LastSelectItem));
}

void ProjectTreeView::OnShowLogAction()
{
	VersionControlManager::ShowLog(
		m_Model->GetIndexPath(m_LastSelectItem), "");
}

void ProjectTreeView::OnAddAction()
{
	QModelIndex currentIndex;

	if (selectedIndexes().isEmpty())
		currentIndex = QModelIndex();
	else {
		QModelIndexList indexes = selectedIndexes();
		currentIndex = indexes.first();
	}

	ProjectDialog dialog(ProjectDialog::Add);
	int result = dialog.exec();

	if (result == QDialog::Accepted) {
		if (dialog.ProjectPathFromInputIsEmpty()) {
			currentIndex = QModelIndex();
		}

		VCRepoEntry projectPath = dialog.GetProjectPathFromInput();
		qInfo() << projectPath;

		m_Model->InsertData(currentIndex, projectPath);
		if (currentIndex.isValid()) {
			expand(currentIndex);
		}
	}
}

void ProjectTreeView::OnEditAction()
{
	QModelIndex currentIndex = selectedIndexes().first();
	VCRepoEntry projectPath = m_Model->GetIndexProjectPath(currentIndex);

	ProjectDialog dialog(ProjectDialog::Edit);
	dialog.SetProjectData(projectPath);
	int result = dialog.exec();

	if (result == QDialog::Accepted) {
		projectPath = dialog.GetProjectPathFromInput();
		m_Model->ChangeData(currentIndex, projectPath);
		emit SgnSelectPathChange(projectPath.path);
	}
}

void ProjectTreeView::OnDeleteAction()
{
	QModelIndexList indexes = selectedIndexes();
	QModelIndex     currentIndex = indexes.first();

	m_Model->DeleteData(currentIndex);
}

void ProjectTreeView::SlotSelectionChanged(
	const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected)

	QModelIndex indexe = GetSelectIndexs();

	if (indexe.isValid()) {
		m_LastSelectItem = indexe;
		emit SgnSelectPathChange(m_Model->GetIndexPath(indexe));
	} else {
		m_LastSelectItem = QModelIndex();
	}
}

void ProjectTreeView::SlotItemMoved(
	const QModelIndex& oldParentIndex, const QModelIndex& newParentIndex)
{
	expand(oldParentIndex);
	expand(newParentIndex);
}
