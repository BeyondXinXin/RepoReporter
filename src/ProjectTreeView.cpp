#include "ProjectTreeView.h"

#include <QAction>
#include <QIcon>
#include <QMenu>
#include <QContextMenuEvent>
#include <QStandardItem>
#include <QHeaderView>

#include "ProjectTreeModel.h"

ProjectTreeView::ProjectTreeView(QWidget *parent)
	: QTreeView(parent)
{
	m_AddAction = new QAction(QIcon(":/icons/add.png"), tr(u8"添加项目"), this);
	connect(m_AddAction, &QAction::triggered, this, &ProjectTreeView::AddProject);

	m_DeleteAction = new QAction(QIcon(":/icons/delete.png"), tr(u8"删除项目"), this);
	connect(m_DeleteAction, &QAction::triggered, this, &ProjectTreeView::DeleteProject);

	m_Model = new ProjectTreeModel(this);
	setModel(m_Model);
	show();

	setDragEnabled(true);
	setAcceptDrops(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	setEditTriggers(QAbstractItemView::NoEditTriggers);

	header()->setVisible(false);
	header()->resizeSection(0, 200);
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void ProjectTreeView::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	if (selectedIndexes().isEmpty()) {
		menu.addAction(m_AddAction);
	} else {
		menu.addAction(m_AddAction);
		menu.addAction(m_DeleteAction);
	}
	menu.exec(event->globalPos());
}

void ProjectTreeView::mouseReleaseEvent(QMouseEvent *event)
{
	QTreeView::mouseReleaseEvent(event);
	if (!indexAt(event->pos()).isValid()) {
		clearSelection();
	}
}

void ProjectTreeView::AddProject()
{
	QModelIndex currentIndex;
	if (selectedIndexes().isEmpty()) {
		currentIndex = QModelIndex();
	} else {
		currentIndex = selectedIndexes().first();
	}
	m_Model->InsertData(currentIndex, u8"Data " + QString::number(m_Model->rowCount()));

	if (currentIndex.isValid()) {
		expand(currentIndex);
	}
}

void ProjectTreeView::DeleteProject()
{
	QModelIndex currentIndex= selectedIndexes().first();
	m_Model->DeleteData(currentIndex);
}

