#ifndef FileTableView_H
#define FileTableView_H

#include <QObject>
#include <QTableView>

class FileTableModel;

class FileTableView : public QTableView {
	Q_OBJECT

public:

	FileTableView(QWidget* parent = nullptr);

	void ChangeLog(const QList<QString>& versions);
	void ChangeProPath(const QString& path);

Q_SIGNALS:

	void SgnStateLabChange(const int& index, const int& num);

protected:

	void contextMenuEvent(QContextMenuEvent* event) override;
	void showEvent(QShowEvent* event) override;
	void hideEvent(QHideEvent* event) override;
	void mouseDoubleClickEvent(QMouseEvent* event) override;

private:

	void InitUI();
	void InitConnect();

	void SlotSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected);

	void OnCompareWithBaseAction();
	void OnShowLogAction();
	void OnExportAction();
	void OnOpenAction();
	void OnBrowseAction();
	void OnCompareAction();
	void OnMarkCompareAction();
	void OnCopyFullPathAction();
	void OnCopyRelativePathAction();
	void OnCopyFileNameAction();

	QList<QModelIndex>GetSelectIndexs() const;

private:

	QAction* m_CompareWithBaseAction;
	QAction* m_ShowLogAction;
	QAction* m_ExportAction;
	QAction* m_OpenAction;
	QAction* m_BrowseAction;
	QAction* m_CompareAction;
	QAction* m_MarkCompareAction;
	QMenu* m_CopySubMenu;
	QAction* m_CopyFullPathAction;
	QAction* m_CopyRelativePathAction;
	QAction* m_CopyFileNameAction;

	FileTableModel* m_Model;
	QString m_CurPaht;
	QList<QString>m_CurVersions;
};

#endif // LOGTABLEWIDGET_H
