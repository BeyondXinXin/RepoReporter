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

private slots:

	void AddProject();
	void EditProject();
	void DeleteProject();

	void SlotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void SlotItemMoved(const QModelIndex& oldParentIndex, const QModelIndex& newParentIndex);

private:

	void CheckRepoState();

private:

	QAction* m_AddAction;
	QAction* m_EditAction;
	QAction* m_DeleteAction;

	ProjectTreeModel* m_Model;
	ProjectTreeDelegate* m_Delegate;

	QModelIndex m_LastSelectItem;
};

#endif // PROJECTTREE_H
