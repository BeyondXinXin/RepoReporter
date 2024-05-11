#ifndef LOGTABLEView_H
#define LOGTABLEView_H

#include <QObject>
#include <QTableView>

#include "data/VCSDataStructures.h"

class LogTableModel;
class LogTableDelegate;
class LogTableSortFilterProxyModel;

class LogTableView : public QTableView {
	Q_OBJECT

public:

	LogTableView(QWidget* parent = nullptr);

	void ChangeRepo(const QList<VCLogEntry>& logs, const QString& version);
	void setFilterRegExp(const QRegExp& regExp, QList<int>filterItems);
	void Clear();

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
};


#endif // LOGTABLEWIDGET_H
