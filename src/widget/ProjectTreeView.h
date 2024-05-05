#ifndef ProjectTreeView_H
#define ProjectTreeView_H

#include <QTreeView>
#include <QModelIndex>


class ProjectTreeDelegate;
class ProjectTreeModel;

class ProjectTreeView : public QTreeView {
	Q_OBJECT

public:

	ProjectTreeView(QWidget* parent = nullptr);

Q_SIGNALS:

	void SgnSelectPathChange(const QString& path);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;
	void mouseReleaseEvent(QMouseEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private:

	void InitUI();
	void InitConnect();
	void CheckRepoState();
	QModelIndex GetSelectIndexs()const;

private slots:

	void OnBrowseAction();
	void OnCheckAction();
	void OnShowLogAction();

	void OnPullAction();
	void OnSyncAction();

	void OnAddAction();
	void OnEditAction();
	void OnDeleteAction();

	void SlotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void SlotItemMoved(const QModelIndex& oldParentIndex, const QModelIndex& newParentIndex);

private:

	QAction* m_BrowseAction, * m_CheckAction, * m_ShowLogAction;
	QAction* m_PullAction, * m_SyncAction;
	QAction* m_AddAction, * m_EditAction, * m_DeleteAction;

	ProjectTreeModel* m_Model;
	ProjectTreeDelegate* m_Delegate;

	QModelIndex m_LastSelectItem;
};

#endif // PROJECTTREE_H
