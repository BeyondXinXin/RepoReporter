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

ProjectTreeView::ProjectTreeView(QWidget* parent)
	: QTreeView(parent)
{
	m_AddAction = new QAction(QIcon(":/icons/add.png"), tr(u8"添加项目"), this);
	connect(m_AddAction,    &QAction::triggered, this, &ProjectTreeView::AddProject);

	m_EditAction = new QAction(QIcon(":/icons/add.png"), tr(u8"编辑项目"), this);
	connect(m_EditAction,   &QAction::triggered, this, &ProjectTreeView::EditProject);

	m_DeleteAction = new QAction(QIcon(":/icons/delete.png"), tr(u8"删除项目"), this);
	connect(m_DeleteAction, &QAction::triggered, this, &ProjectTreeView::DeleteProject);

	m_Model = new ProjectTreeModel(this);
	setModel(m_Model);

	m_Delegate = new ProjectTreeDelegate(this);
	setItemDelegate(m_Delegate);

	setIndentation(20);

	header()->setVisible(false);
	header()->setSectionResizeMode(QHeaderView::ResizeToContents);

	setDragEnabled(true);
	setAcceptDrops(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	setEditTriggers(QAbstractItemView::NoEditTriggers);
	setSelectionMode(QAbstractItemView::SingleSelection);

	selectionModel()->connect(
		selectionModel(), &QItemSelectionModel::selectionChanged,
		this, &ProjectTreeView::SlotSelectionChanged);

	connect(m_Model, &ProjectTreeModel::SgnItemMoved,
	        this, &ProjectTreeView::SlotItemMoved);
}

void ProjectTreeView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);

	if (selectedIndexes().isEmpty())
		menu.addAction(m_AddAction);
	else {
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

void ProjectTreeView::AddProject()
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

void ProjectTreeView::EditProject()
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

void ProjectTreeView::DeleteProject()
{
	QModelIndexList indexes = selectedIndexes();
	QModelIndex     currentIndex = indexes.first();

	m_Model->DeleteData(currentIndex);
}

void ProjectTreeView::SlotSelectionChanged(
	const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected)

	QModelIndexList indexes = selected.indexes();

	if (!indexes.isEmpty()) {
		m_LastSelectItem = indexes.first();
		emit SgnSelectPathChange(m_Model->GetIndexPath(indexes.first()));
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

void ProjectTreeView::CheckRepoState()
{
	m_Model->CheckAllRepoState();
}
