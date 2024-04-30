﻿#include "ProjectTreeView.h"

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
#include "AddProjectDialog.h"
#include "ConfigManager.h"

ProjectTreeView::ProjectTreeView(QWidget* parent)
	: QTreeView(parent)
{
	m_AddAction = new QAction(QIcon(":/icons/add.png"), tr(u8"添加项目"), this);
	connect(m_AddAction,    &QAction::triggered, this, &ProjectTreeView::AddProject);

	m_DeleteAction = new QAction(QIcon(":/icons/delete.png"), tr(u8"删除项目"), this);
	connect(m_DeleteAction, &QAction::triggered, this, &ProjectTreeView::DeleteProject);

	m_Model = new ProjectTreeModel(this);
	setModel(m_Model);

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

	AddProjectDialog dialog;
	int result = dialog.exec();

	if (result == QDialog::Accepted) {
		VCProjectPath projectPath = dialog.GetProjectPathFromInput();

		if (dialog.ProjectPathFromInputIsEmpty()) {
			currentIndex = QModelIndex();
		}

		m_Model->InsertData(currentIndex, projectPath);

		if (currentIndex.isValid()) {
			expand(currentIndex);
		}
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
		emit SgnSelectPathChange(m_Model->GetIndexPath(indexes.first()));
	}
}

void ProjectTreeView::SlotItemMoved(
	const QModelIndex& oldParentIndex, const QModelIndex& newParentIndex)
{
	expand(oldParentIndex);
	expand(newParentIndex);
}
