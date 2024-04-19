#ifndef FileTableView_H
#define FileTableView_H

#include <QObject>
#include <QTableView>

class FileModel;

class FileTableView : public QTableView {
	Q_OBJECT

public:

	FileTableView(QWidget* parent = nullptr);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;

private slots:

	void AddProject();

private:

	QAction* m_AddAction;
	FileModel* m_Model;
};

#endif // LOGTABLEWIDGET_H
