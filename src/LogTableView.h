#ifndef LOGTABLEView_H
#define LOGTABLEView_H

#include <QObject>
#include <QTableView>

class LogTableModel;
class LogTableDelegate;

class LogTableView : public QTableView {
	Q_OBJECT

public:

	LogTableView(QWidget* parent = nullptr);

	void ChangeProPath(const QString& path);

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

private:

	QMenu* m_CopySubMenu;
	QAction* m_CopyFullContentAction;
	QAction* m_CopyAuthorNameAction;
	QAction* m_CopyInformationAction;

	LogTableModel* m_Model;
	LogTableDelegate* m_Delegate;

	QString m_CurPaht;
};

#endif // LOGTABLEWIDGET_H
