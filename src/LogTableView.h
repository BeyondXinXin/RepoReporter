#ifndef LOGTABLEView_H
#define LOGTABLEView_H

#include <QObject>
#include <QTableView>

class LogModel;

class LogTableView : public QTableView {
	Q_OBJECT

public:

	LogTableView(QWidget* parent = nullptr);

	void ChangeProPath(const QString& path);

Q_SIGNALS:

	void SgnChangeSelectLog(const QList<QString>& versions);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;

private slots:

	void AddProject();
	void SlotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:

	QAction* m_AddAction;
	LogModel* m_Model;
};

#endif // LOGTABLEWIDGET_H
