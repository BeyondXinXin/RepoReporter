﻿#ifndef LOGTABLEView_H
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
	void SgnUpdateDescription(const QString& str);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private slots:

	void InitUI();

	void AddProject();
	void SlotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

private:

	QAction* m_AddAction;

	LogModel* m_Model;

	QString m_CurPaht;
};

#endif // LOGTABLEWIDGET_H
