#ifndef PROJECTTREEMODE_H
#define PROJECTTREEMODE_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include <QMimeData>
#include <QJsonArray>

#include "vcdata/VCSDataStructures.h"

class ProjectTreeModel : public QAbstractItemModel {
	Q_OBJECT

public:

	explicit ProjectTreeModel(QObject* parent = nullptr);
	~ProjectTreeModel() override;

	bool InsertData(const QModelIndex& index, VCProjectPath newData);
	bool DeleteData(const QModelIndex& index);

	QVariant data(const QModelIndex& index, int role) const override;
	QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
	QModelIndex parent(const QModelIndex& index) const override;
	Qt::ItemFlags flags(const QModelIndex& index) const override;
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	int columnCount(const QModelIndex& parent = QModelIndex()) const override;

	QString GetIndexPath(const QModelIndex& index)const;

	void ClearData();

	bool SaveToJson(const QString& filePath) const;
	bool LoadFromJson(const QString& filePath);

Q_SIGNALS:

	void SgnItemMoved(const QModelIndex& oldParentIndex, const QModelIndex& newParentIndex);

private:

	struct Node
	{
		VCProjectPath data;
		QList<Node *> children;
		Node* parent;

		Node(const VCProjectPath& inData, Node * parent = nullptr);
		~Node();
	};

private:

	bool dropMimeData(const QMimeData* data, Qt::DropAction action, int row, int column, const QModelIndex& parent) override;
	Qt::DropActions supportedDropActions() const override;
	QStringList mimeTypes() const override;
	QMimeData* mimeData(const QModelIndexList& indexes) const override;

	void MoveItem(const QModelIndex& srcIndex, const QModelIndex& targetParentIndex, const int& targetRrow);

	void SaveNodeToJson(Node* node, QJsonArray& jsonArray) const;
	void LoadNodeFromJson(const QJsonArray& jsonArray, Node* parentNode);

private:

	Node* m_RootNode { nullptr };
	QString m_FilePath;
};


#endif // PROJECTTREEMODE_H
