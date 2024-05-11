#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractItemModel>
#include <QSortFilterProxyModel>
#include <QStyledItemDelegate>

#include "data/VCSDataStructures.h"

class LogTableModel : public QAbstractItemModel {
	Q_OBJECT

public:

	explicit LogTableModel(QObject* parent = nullptr);
	~LogTableModel() override;

	void Update(const QList<VCLogEntry>& logs, const QString& version);
	void Clear();

	QString GetIndexVersion(const QModelIndex& index)const;
	QString GetIndexMessage(const QModelIndex& index)const;
	QString GetIndexAuthor(const QModelIndex& index)const;
	VCLogEntry GetIndexLogEntry(const QModelIndex& index)const;

	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& index) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;

Q_SIGNALS:

	void SgnCurVerChange(const int& curVer);

private:

	void UpdateCurrentVersion();

private:

	QList<VCLogEntry>m_Logs;
	QString m_CurVersion;
};

class LogTableSortFilterProxyModel : public QSortFilterProxyModel {
public:

	bool filterAcceptsRow(int sourceRow, const QModelIndex& sourceParent) const override;
	bool lessThan(const QModelIndex& left, const QModelIndex& right) const override;

	void SetFilterItems(QList<int>filterItems);

private:

	QList<int>m_FilterItems;
};

class LogTableDelegate : public QStyledItemDelegate {
public:

	LogTableDelegate(QObject* parent = nullptr);

	void paint(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override;

	void SetCurrentVersionRow(int row);

private:

	int m_CurVerRow = -1;
};

#endif // LOGMODEL_H
