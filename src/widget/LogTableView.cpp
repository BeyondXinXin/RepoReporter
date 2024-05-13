#include "LogTableView.h"

#include <QHeaderView>
#include <QMenu>
#include <QTableWidget>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QClipboard>
#include <QTimer>

#include "LogTableModel.h"
#include "utils/VersionControlManager.h"
#include "utils/ConfigManager.h"
#include "utils/FileUtil.h"


LogTableView::LogTableView(QWidget* parent)
	: QTableView(parent)
{
	m_FilterProxyModel = new LogTableSortFilterProxyModel();
	m_Model = new LogTableModel(this);
	m_FilterProxyModel->setSourceModel(m_Model);
	setModel(m_FilterProxyModel);

	m_Delegate = new LogTableDelegate(this);
	setItemDelegate(m_Delegate);

	InitUI();
	InitConnect();
}

void LogTableView::ChangeRepo(const QList<VCLogEntry>& logs, const QString& version)
{
	m_Model->Update(logs, version);
	emit SgnStateLabChange(-1, 0);
	emit SgnStateLabChange(0, m_Model->rowCount());
	setCurrentIndex(model()->index(0, 0));
}

void LogTableView::setFilterRegExp(const QRegExp& regExp, QList<int>filterItems)
{
	m_FilterProxyModel->SetFilterItems(filterItems);
	m_FilterProxyModel->setFilterRegExp(regExp);
}

void LogTableView::Clear()
{
	m_Model->Clear();
	emit SgnStateLabChange(-1, 0);
	emit SgnChangeSelectLog(QStringList());
	emit SgnUpdateDescription("");
}

void LogTableView::contextMenuEvent(QContextMenuEvent* event)
{
	QMenu menu(this);

	menu.addMenu(m_CopySubMenu);
	menu.exec(event->globalPos() - QPoint(20, 10));
}

void LogTableView::showEvent(QShowEvent* event)
{
	QTableView::showEvent(event);
	QHeaderView* header = horizontalHeader();
	int column = m_Model->columnCount();
	QList<int> sizeList = ConfigManager::GetInstance().ReadList<int>("LogTableViewSectionSize", QList<int>{});

	if (sizeList.size() == column) {
		for (int i = 0; i < column; i++) {
			header->resizeSection(i, sizeList.at(i));
		}
	}
}

void LogTableView::hideEvent(QHideEvent* event)
{
	QTableView::hideEvent(event);
	QHeaderView* header = horizontalHeader();
	QList<int>   sizeList;
	int column = m_Model->columnCount();

	for (int i = 0; i < column; i++) {
		sizeList << header->sectionSize(i);
	}

	ConfigManager::GetInstance().WriteList<int>("LogTableViewSectionSize", sizeList);
}

void LogTableView::InitUI()
{
	setShowGrid(false);

	setDragEnabled(false);
	setAcceptDrops(false);
	setDragDropMode(QAbstractItemView::NoDragDrop);

	verticalHeader()->setDefaultSectionSize(10);

	setSelectionBehavior(QAbstractItemView::SelectRows);
	setSelectionMode(QAbstractItemView::ExtendedSelection);

	horizontalHeader()->setHighlightSections(false);
	horizontalHeader()->setSectionResizeMode(1, QHeaderView::Fixed);
	horizontalHeader()->resizeSection(1, 24 * 4);
}

void LogTableView::InitConnect()
{
	connect(selectionModel(), &QItemSelectionModel::selectionChanged,
	        this, &LogTableView::SlotSelectionChanged);

	connect(m_Model,          &LogTableModel::SgnCurVerChange,
	        m_Delegate, &LogTableDelegate::SetCurrentVersionRow);


	m_CopyFullContentAction = new QAction(QIcon(":/icons/add.png"), u8"完整内容", this);
	connect(m_CopyFullContentAction, &QAction::triggered,
	        this, &LogTableView::OnCopyFullContentAction);

	m_CopyAuthorNameAction = new QAction(QIcon(":/icons/add.png"), u8"作者姓名", this);
	connect(m_CopyAuthorNameAction, &QAction::triggered,
	        this, &LogTableView::OnCopyAuthorNameAction);

	m_CopyInformationAction = new QAction(QIcon(":/icons/add.png"), u8"信息", this);
	connect(m_CopyInformationAction, &QAction::triggered,
	        this, &LogTableView::OnCopyInformationAction);

	m_CopySubMenu = new QMenu(u8"复制到剪贴板", this);
	m_CopySubMenu->addAction(m_CopyFullContentAction);
	m_CopySubMenu->addAction(m_CopyAuthorNameAction);
	m_CopySubMenu->addAction(m_CopyInformationAction);
}

void LogTableView::SlotSelectionChanged(
	const QItemSelection& selected, const QItemSelection& deselected)
{
	Q_UNUSED(deselected)
	Q_UNUSED(selected)

	QList<QModelIndex>selectIndexs = GetSelectIndexs();
	if (selectIndexs.isEmpty()) {
		emit SgnStateLabChange(1, 0);
		return;
	}

	QList<QString> vers;
	QStringList    descriptions;
	foreach(auto index, selectIndexs)
	{
		vers << m_Model->GetIndexVersion(index);
		descriptions << m_Model->GetIndexMessage(index);
	}

	if (descriptions.size() > 1) {
		for (int i = 0; i < descriptions.size(); i++) {
			descriptions[i] = vers[i] + u8"\n" + descriptions[i];
		}
	}

	emit SgnStateLabChange(1, descriptions.size());
	emit SgnChangeSelectLog(vers);
	emit SgnUpdateDescription(descriptions.join("\n- - - - - - - - - - -\n"));
}

void LogTableView::OnCopyFullContentAction()
{
	QStringList contentStrs;
	foreach(auto index, GetSelectIndexs())
	{
		VCLogEntry entry = m_Model->GetIndexLogEntry(index);
		contentStrs << QString(u8"版本:%1\n作者：%2\n日期：%3\n信息：\n%4")
		        .arg(entry.version)
		        .arg(entry.author)
		        .arg(entry.date.toString("yyyy/M/d hh:mm:ss"))
		        .arg(entry.message);
	}
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(contentStrs.join("\n\n\n"));
}

void LogTableView::OnCopyAuthorNameAction()
{
	QStringList authorStrs;
	foreach(auto index, GetSelectIndexs())
	{
		authorStrs << m_Model->GetIndexAuthor(index);
	}
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(authorStrs.join("\n"));
}

void LogTableView::OnCopyInformationAction()
{
	QStringList masageStrs;
	foreach(auto index, GetSelectIndexs())
	{
		masageStrs << m_Model->GetIndexMessage(index);
	}
	QClipboard* clipboard = QGuiApplication::clipboard();
	clipboard->setText(masageStrs.join("\n\n"));
}

QList<QModelIndex>LogTableView::GetSelectIndexs() const
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
