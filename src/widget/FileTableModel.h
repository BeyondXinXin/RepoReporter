#ifndef FileModel_H
#define FileModel_H

#include <QAbstractItemModel>
#include <QMap>
#include <QStyledItemDelegate>

#include "data/VCSDataStructures.h"

class FileTableModel : public QAbstractItemModel {
	Q_OBJECT

public:

	explicit FileTableModel(QObject* parent = nullptr);
	~FileTableModel() override;

	void UpdataFile(const QString& path, const QList<QString>& versions);

	QString GetFileName(const QModelIndex& index)const;

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:

	QList<VCFileEntry>m_Files;
};

class FileTableDelegate : public QStyledItemDelegate {
public:

	FileTableDelegate(QObject* parent = nullptr);

	void paint(
		QPainter* painter,
		const QStyleOptionViewItem& option,
		const QModelIndex& index) const override;
};

#endif // LOGMODEL_H
