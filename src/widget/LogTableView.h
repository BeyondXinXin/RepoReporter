﻿#ifndef LOGTABLEView_H
#define LOGTABLEView_H

#include <QObject>
#include <QTableView>

class LogTableModel;
class LogTableDelegate;
class LogTableSortFilterProxyModel;

class LogTableView : public QTableView {
	Q_OBJECT

public:

	LogTableView(QWidget* parent = nullptr);

	void ChangeProPath(const QString& path, const bool& allBranch);
	void RefreshRepo(const bool& allBranch);
	void setFilterRegExp(const QRegExp& regExp, QList<int>filterItems);

	QString GetCurrentRepoPath()const;

Q_SIGNALS:

	void SgnChangeSelectLog(const QList<QString>& versions);
	void SgnUpdateDescription(const QString& str);

	void SgnStateLabChange(const int& index, const int& num);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;

private slots:

	void InitUI();
	void InitConnect();

	void SlotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	void OnCopyFullContentAction();
	void OnCopyAuthorNameAction();
	void OnCopyInformationAction();

	QList<QModelIndex>GetSelectIndexs() const;

private:

	QMenu* m_CopySubMenu;
	QAction* m_CopyFullContentAction;
	QAction* m_CopyAuthorNameAction;
	QAction* m_CopyInformationAction;

	LogTableModel* m_Model;
	LogTableDelegate* m_Delegate;
	LogTableSortFilterProxyModel* m_FilterProxyModel;

	QString m_CurPaht;
};


#endif // LOGTABLEWIDGET_H
