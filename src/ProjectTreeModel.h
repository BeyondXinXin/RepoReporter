#ifndef PROJECTTREEMODE_H
#define PROJECTTREEMODE_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QMimeData>

class ProjectTreeModel : public QAbstractItemModel
{
	Q_OBJECT

public:
	explicit ProjectTreeModel(QObject *parent = nullptr);
	~ProjectTreeModel() override;

	bool InsertData(const QModelIndex &index, QString name);
	bool DeleteData(const QModelIndex &index);

	QVariant data(const QModelIndex &index, int role) const override;
	QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex &index) const override;
	Qt::ItemFlags flags(const QModelIndex &index) const override;
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	int columnCount(const QModelIndex &parent = QModelIndex()) const override;

private:
	bool dropMimeData(const QMimeData *data, Qt::DropAction action,
					  int row, int column, const QModelIndex &parent) override;
	Qt::DropActions supportedDropActions() const override;
	QStringList mimeTypes() const override;
	QMimeData *mimeData(const QModelIndexList &indexes) const override;

	void MoveItem(const QModelIndex &srcIndex, const QModelIndex &targetIndex);

private:

	struct Node
	{
		QString name;
		QVector<Node *> children;
		Node *parentNode;
		QString path;

		Node(const QString &name, Node *parent = nullptr)
			: name(name)
			, parentNode(parent)
		{
			if(parentNode!= nullptr){
				parentNode->children.append(this);
			}
		}
		~Node() { qDeleteAll(children); }
	};

	Node *rootNode;
};



#endif // PROJECTTREEMODE_H
