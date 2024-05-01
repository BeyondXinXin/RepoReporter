#ifndef LOGMODEL_H
#define LOGMODEL_H

#include <QAbstractItemModel>

#include "vcdata/VCSDataStructures.h"

class LogTableModel : public QAbstractItemModel {
	Q_OBJECT

public:

	explicit LogTableModel(QObject* parent = nullptr);
	~LogTableModel() override;

	void UpdataLog(const QString& path);

	QString GetIndexVersion(const QModelIndex& index)const;
	QString GetIndexMessage(const QModelIndex& index)const;

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

#endif // LOGMODEL_H
