#ifndef FileModel_H
#define FileModel_H

#include <QAbstractItemModel>
#include <QMap>

class FileModel : public QAbstractItemModel {
	Q_OBJECT

public:

	explicit FileModel(QObject* parent = nullptr);
	~FileModel() override;

	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& child) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:

	QList<QPair<QString, QString> >m_files;
};

#endif // LOGMODEL_H
