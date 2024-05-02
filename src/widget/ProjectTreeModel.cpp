#include "ProjectTreeModel.h"

#include <QDataStream>
#include <QStyle>
#include <QApplication>
#include <QTreeView>
#include <QStack>
#include <QJsonObject>
#include <QFile>
#include <QJsonDocument>
#include <QStandardPaths>
#include <QDir>

ProjectTreeModel::ProjectTreeModel(QObject* parent)
	: QAbstractItemModel(parent),
	m_RootNode(new Node(VCProjectPath("Root")))
{
	QString configDir = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);

	m_FilePath = configDir + "/project_data.json";

	QDir dir(configDir);

	if (!dir.exists()) {
		if (!dir.mkpath(configDir)) {
			qInfo() << u8"无法创建配置目录。";
			m_FilePath = "project_data.json";
		}
	}


	LoadFromJson(m_FilePath);
}

ProjectTreeModel::~ProjectTreeModel()
{
	SaveToJson(m_FilePath);
	delete m_RootNode;
}

bool ProjectTreeModel::InsertData(const QModelIndex& index, VCProjectPath newData)
{
	QModelIndex parentIndex;
	Node* parentNode;
	int   row;

	if (index.isValid()) {
		parentNode = static_cast<Node *>(index.internalPointer());

		if (parentNode->parent != m_RootNode) {
			return false;
		}
		row = rowCount(index);
		parentIndex = index;
	} else {
		parentNode = m_RootNode;
		row = m_RootNode->children.size();
		parentIndex = QModelIndex();
	}

	beginInsertRows(parentIndex, row, row);
	Node* child = new Node(newData, parentNode);

	Q_UNUSED(child)
	endInsertRows();

	return true;
}

bool ProjectTreeModel::DeleteData(const QModelIndex& index)
{
	if (!index.isValid()) {
		return false;
	}

	Node* deleteNode = static_cast<Node *>(index.internalPointer());

	if (!deleteNode) {
		return false;
	}

	Node* parentNode = deleteNode->parent;
	QModelIndex parentIndex = index.parent();

	if (parentNode) {
		int row = parentNode->children.indexOf(deleteNode);

		beginRemoveRows(parentIndex, row, row);
		parentNode->children.removeAt(row);
		endRemoveRows();
	} else {
		int row = m_RootNode->children.indexOf(deleteNode);

		beginRemoveRows(QModelIndex(), row, row);
		m_RootNode->children.removeAt(row);
		endRemoveRows();
	}

	emit dataChanged(index.parent(), index.parent());

	delete deleteNode;
	return true;
}

QVariant ProjectTreeModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || (index.model() != this))
		return QVariant();

	if (index.column() == 0) {
		if ((role == Qt::EditRole) || (role == Qt::DisplayRole)) {
			Node* node = static_cast<Node *>(index.internalPointer());

			return node->data.name;
		} else if (role == Qt::DecorationRole) {
			Node* node = static_cast<Node *>(index.internalPointer());

			if (node->data.path.isEmpty()) {
				return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_DirIcon));
			}
			return QVariant::fromValue(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
		} else if (role == Qt::TextAlignmentRole) {
			if (index.column() == 1) {
				return QVariant(Qt::AlignTrailing | Qt::AlignVCenter);
			}
		}
	}

	return QVariant();
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
	}

	return QAbstractItemModel::flags(index) |
	       Qt::ItemIsEditable |
	       Qt::ItemIsDragEnabled |
	       Qt::ItemIsDropEnabled;
}

QModelIndex ProjectTreeModel::index(int row, int column,
                                    const QModelIndex& parent) const
{
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}

	Node* parentNode;

	if (!parent.isValid()) {
		parentNode = m_RootNode;
	} else {
		parentNode = static_cast<Node *>(parent.internalPointer());
	}
	Node* childNode = parentNode->children.value(row);

	if (childNode) {
		return createIndex(row, column, childNode);
	} else {
		return QModelIndex();
	}
}

QModelIndex ProjectTreeModel::parent(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return QModelIndex();
	}

	Node* childNode = static_cast<Node *>(index.internalPointer());
	Node* parentNode = childNode->parent;

	if (parentNode == m_RootNode) {
		return QModelIndex();
	}

	int row = 0;

	if (parentNode->parent) {
		row = parentNode->parent->children.indexOf(parentNode);
	}
	return createIndex(row, 0, parentNode);
}

int ProjectTreeModel::rowCount(const QModelIndex& parent) const
{
	if (parent.column() > 0) {
		return 0;
	}

	Node* parentNode;

	if (!parent.isValid())
		parentNode = m_RootNode; else {
		parentNode = static_cast<Node *>(parent.internalPointer());
	}
	return parentNode->children.size();
}

int ProjectTreeModel::columnCount(const QModelIndex& parent) const
{
	return 1;
}

QString ProjectTreeModel::GetIndexPath(const QModelIndex& index) const
{
	if (!index.isValid()) {
		return "";
	}

	Node* node = static_cast<Node *>(index.internalPointer());


	return node->data.path;
}

void ProjectTreeModel::ClearData()
{
	beginResetModel();
	qDeleteAll(m_RootNode->children);
	endResetModel();
}

bool ProjectTreeModel::SaveToJson(const QString& filePath) const
{
	QFile file(filePath);

	if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		qInfo() << u8"无法打开文件进行写入:" << file.errorString();
		return false;
	}

	QJsonArray jsonArray;

	SaveNodeToJson(m_RootNode, jsonArray);

	QJsonDocument jsonDoc(jsonArray);

	file.write(jsonDoc.toJson());
	file.close();

	return true;
}

bool ProjectTreeModel::LoadFromJson(const QString& filePath)
{
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
		qInfo() << u8"无法打开文件进行读取:" << file.errorString();
		return false;
	}

	QByteArray jsonData = file.readAll();
	QJsonDocument jsonDoc(QJsonDocument::fromJson(jsonData));
	QJsonObject   obj = jsonDoc.array().first().toObject();

	if (!obj.contains("name") || (obj["name"] != "Root")) {
		return false;
	}

	ClearData();
	beginResetModel();
	QJsonArray childrenArray = obj["children"].toArray();

	LoadNodeFromJson(childrenArray, m_RootNode);
	endResetModel();
	return true;
}

bool ProjectTreeModel::dropMimeData(
	const QMimeData* data, Qt::DropAction action,
	int targetRow, int targetColumn, const QModelIndex& parent)
{
	if (parent.parent().isValid()) {
		return false;
	}

	if ((action == Qt::IgnoreAction) || !data->hasFormat("text/plain")) {
		return false;
	}

	if (-1 == targetColumn) {
		targetColumn = 0;
	}

	if (-1 == targetRow) {
		targetRow = 0;
	}

	QByteArray  encodedData = data->data("text/plain");
	QDataStream stream(&encodedData, QIODevice::ReadOnly);

	QList<QPair<int, int> > srcRowTree;

	while (!stream.atEnd()) {
		int srcRow, srcColumn;

		stream >> srcRow >> srcColumn;
		srcRowTree.append(QPair<int, int>(srcRow, srcColumn));
	}

	std::reverse(srcRowTree.begin(), srcRowTree.end());

	QModelIndex sourceIndex = index(srcRowTree.at(0).first, srcRowTree.at(0).second, QModelIndex());

	for (int i = 1; i < srcRowTree.size(); i++) {
		sourceIndex = index(srcRowTree.at(i).first, srcRowTree.at(i).second, sourceIndex);
	}

	MoveItem(sourceIndex, parent, targetRow);

	return true;
}

Qt::DropActions ProjectTreeModel::supportedDropActions() const
{
	return Qt::CopyAction | Qt::MoveAction;
}

QStringList ProjectTreeModel::mimeTypes() const
{
	QStringList types;

	types << "text/plain";
	return types;
}

QMimeData * ProjectTreeModel::mimeData(const QModelIndexList& indexes) const
{
	QMimeData* mimeData = new QMimeData();

	if (1 == indexes.size()) {
		QModelIndex index = indexes.first();

		if (index.isValid()) {
			QByteArray  encodedData;
			QDataStream stream(&encodedData, QIODevice::WriteOnly);

			stream << index.row();
			stream << index.column();

			QModelIndex parentIndex = index.parent();

			while (parentIndex.isValid()) {
				stream << parentIndex.row();
				stream << parentIndex.column();
				parentIndex = parentIndex.parent();
			}
			mimeData->setData("text/plain", encodedData);
		}
	}
	return mimeData;
}

void ProjectTreeModel::ProjectTreeModel::MoveItem(
	const QModelIndex& srcIndex, const QModelIndex& targetParentIndex, const int& targetRow)
{
	if (!srcIndex.isValid()) {
		return;
	}
	QModelIndex srcParentIndex = srcIndex.parent();
	Node* srcNode = static_cast<Node *>(srcIndex.internalPointer());
	Node* srcParentNode = srcNode->parent;
	Node* targetParentNode;

	if (targetParentIndex.isValid()) {
		targetParentNode = static_cast<Node *>(targetParentIndex.internalPointer());
	} else {
		targetParentNode = m_RootNode;
	}

	if (!targetParentNode || !srcParentNode || !srcNode->children.isEmpty()) {
		return;
	}

	int srcRow = srcParentNode->children.indexOf(srcNode);

	if (srcRow == -1) {
		return;
	}

	beginResetModel();
	srcParentNode->children.removeAt(srcRow);
	targetParentNode->children.insert(targetRow, srcNode);
	srcNode->parent = targetParentNode;
	endResetModel();

	QModelIndex targetIndex = index(targetRow, 0, targetParentIndex);

	if (targetIndex.isValid()) {
		emit SgnItemMoved(srcParentIndex, srcIndex.parent());
	}
}

void ProjectTreeModel::SaveNodeToJson(Node* node, QJsonArray& jsonArray) const
{
	QJsonObject jsonObject;

	jsonObject["name"] = node->data.name;
	jsonObject["path"] = node->data.path;

	QJsonArray childrenArray;

	for (Node* child : node->children) {
		SaveNodeToJson(child, childrenArray);
	}
	jsonObject["children"] = childrenArray;

	jsonArray.append(jsonObject);
}

void ProjectTreeModel::LoadNodeFromJson(const QJsonArray& jsonArray, Node* parentNode)
{
	foreach(const QJsonValue& value, jsonArray)
	{
		QJsonObject   obj = value.toObject();
		VCProjectPath data(obj["name"].toString(), obj["path"].toString());

		Node* newNode = new Node(data, parentNode);

		if (obj.contains("children") && obj["children"].isArray()) {
			QJsonArray childrenArray = obj["children"].toArray();

			LoadNodeFromJson(childrenArray, newNode);
		}
	}
}

ProjectTreeModel::Node::Node(const VCProjectPath& inData, Node* parent)
	: data(inData),
	parent(parent)
{
	if (parent != nullptr) {
		parent->children.append(this);
	}
}

ProjectTreeModel::Node::~Node()
{
	qDeleteAll(children);
}
