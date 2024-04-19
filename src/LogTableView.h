#ifndef LOGTABLEView_H
#define LOGTABLEView_H

#include <QObject>
#include <QTableView>

class LogModel;

class LogTableView : public QTableView {
	Q_OBJECT

public:

	LogTableView(QWidget* parent = nullptr);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;

private slots:

	void AddProject();

private:

	QAction* m_AddAction;
	LogModel* m_Model;
};

#endif // LOGTABLEWIDGET_H
