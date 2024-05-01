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
	void InitConnect();

	void OnCompareWithBaseAction();
	void OnShowLogAction();
	void OnExportAction();
	void OnSaveAsAction();
	void OnOpenAction();
	void OnBrowseAction();
	void OnCompareAction();
	void OnMarkCompareAction();
	void OnCopyFullPathAction();
	void OnCopyRelativePathAction();
	void OnCopyFileNameAction();

private:

	QAction* m_CompareWithBaseAction;
	QAction* m_ShowLogAction;
	QAction* m_ExportAction;
	QAction* m_SaveAsAction;
	QAction* m_OpenAction;
	QAction* m_BrowseAction;
	QAction* m_CompareAction;
	QAction* m_MarkCompareAction;
	QMenu* m_CopySubMenu;
	QAction* m_CopyFullPathAction;
	QAction* m_CopyRelativePathAction;
	QAction* m_CopyFileNameAction;

	FileModel* m_Model;
	QString m_CurPaht;
};

#endif // LOGTABLEWIDGET_H
