#ifndef ProjectTreeView_H
#define ProjectTreeView_H

#include <QTreeView>
#include <QModelIndex>


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
	void DeleteProject();
	void SlotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);
	void SlotItemMoved(const QModelIndex& oldParentIndex, const QModelIndex& newParentIndex);

private:

private:

	QAction* m_AddAction;
	QAction* m_DeleteAction;

	ProjectTreeModel* m_Model;

	QModelIndex m_LastSelectItem;
};

#endif // PROJECTTREE_H
