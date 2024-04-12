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
	m_AddAction=new QAction(QIcon(":/icons/add.png"), tr(u8"添加项目"), this);
	connect(m_AddAction, &QAction::triggered,this, &ProjectTreeView::AddProject);
	
	m_Model= new ProjectTreeModel(this);
	setModel(m_Model);
	show();
	
	setDragEnabled(true);
	setAcceptDrops(true);
	setDragDropMode(QAbstractItemView::InternalMove);
	
	header()->setVisible(false);
	header()->resizeSection(0, 200);
	header()->setSectionResizeMode(0, QHeaderView::Stretch);
}

void ProjectTreeView::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu menu(this);
	menu.addAction(m_AddAction);
	menu.exec(event->globalPos());
}

void ProjectTreeView::AddProject()
{
	
}

