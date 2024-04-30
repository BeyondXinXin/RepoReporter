#ifndef FileTableView_H
#define FileTableView_H

#include <QObject>
#include <QTableView>

class FileModel;

class FileTableView : public QTableView {
	Q_OBJECT

public:

	FileTableView(QWidget* parent = nullptr);

	void ChangeLog(const QList<QString>& versions);

	void ChangeProPath(const QString& path);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private:

	void InitUI();

private:

	QAction* m_AddAction;
	FileModel* m_Model;
	QString m_CurPaht;
};

#endif // LOGTABLEWIDGET_H
