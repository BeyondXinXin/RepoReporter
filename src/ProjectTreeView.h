#ifndef ProjectTreeView_H
#define ProjectTreeView_H

#include <QTreeView>


class ProjectTreeModel;

class ProjectTreeView : public QTreeView
{
	Q_OBJECT
public:
	ProjectTreeView(QWidget *parent = nullptr);

protected:
	void contextMenuEvent(QContextMenuEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
	void AddProject();

private:
	QAction * m_AddAction;
	ProjectTreeModel * m_Model;
};

#endif // PROJECTTREE_H
