#include "ProjectTreeModel.h"

#include <QDataStream>
#include <QDataStream>

ProjectTreeModel::ProjectTreeModel(QObject *parent)
    : QAbstractItemModel(parent), rootNode(new Node("Root"))
{
    // 添加示例数据
    Node *child1 = new Node("Folder 1", rootNode);
    child1->children.append(new Node("Item 1-1", child1));
    child1->children.append(new Node("Item 1-2", child1));
    rootNode->children.append(child1);

    Node *child2 = new Node("Folder 2", rootNode);
    child2->children.append(new Node("Item 2-1", child2));
    child2->children.append(new Node("Item 2-2", child2));
    rootNode->children.append(child2);

    Node *child3 = new Node("Folder 3", rootNode);
    child3->children.append(new Node("Item 3-1", child3));
    child3->children.append(new Node("Item 3-2", child3));
    rootNode->children.append(child3);
}

ProjectTreeModel::~ProjectTreeModel()
{
    delete rootNode;
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    Node *node = static_cast<Node*>(index.internalPointer());
    return node->name;
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()){
        return Qt::ItemIsEnabled | Qt::ItemIsDropEnabled;
    }

    return QAbstractItemModel::flags(index) |
           Qt::ItemIsEditable |
           Qt::ItemIsDragEnabled |
           Qt::ItemIsDropEnabled;
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)){
        return QModelIndex();
    }

    Node *parentNode;

    if (!parent.isValid()){
        parentNode = rootNode;
    }
    else{
        parentNode = static_cast<Node*>(parent.internalPointer());
    }
    Node *childNode = parentNode->children.value(row);
    if (childNode){
        return createIndex(row, column, childNode);
    }
    else{
        return QModelIndex();
    }
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    Node *childNode = static_cast<Node*>(index.internalPointer());
    Node *parentNode = childNode->parentNode;

    if (parentNode == rootNode){
        return QModelIndex();
    }

    int row = 0; // parentNode->children.indexOf(childNode);
    if (parentNode->parentNode) {
        row = parentNode->parentNode->children.indexOf(parentNode);
    }
    return createIndex(row, 0, parentNode);
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0){
         return 0;
    }

    Node *parentNode;
    if (!parent.isValid())
        parentNode = rootNode;
    else
        parentNode = static_cast<Node*>(parent.internalPointer());

    return parentNode->children.size();
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

bool ProjectTreeModel::dropMimeData(
    const QMimeData *data, Qt::DropAction action, int row, int column, const QModelIndex &parent)
{
    if (action == Qt::IgnoreAction)
        return true;

    if (!data->hasFormat("text/plain"))
        return false;

    if (column > 0)
        return false;

    int beginRow;

    if (row != -1)
        beginRow = row;
    else if (parent.isValid())
        beginRow = parent.row();
    else
        beginRow = rowCount(QModelIndex());

    QByteArray encodedData = data->data("text/plain");
    QDataStream stream(&encodedData, QIODevice::ReadOnly);

    while (!stream.atEnd()) {
        int sourceRow, sourceColumn;
        stream >> sourceRow >> sourceColumn;
        QModelIndex sourceIndex = index(sourceRow, sourceColumn, QModelIndex());
        MoveItem(sourceIndex, parent.child(beginRow, column));
        beginRow++;
    }

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

QMimeData *ProjectTreeModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData * mimeData = new QMimeData();

    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);
    foreach (const QModelIndex& index, indexes) {
        if (index.isValid()) {
            stream << index.row();
            stream << index.column();
        }
    }
    mimeData->setData("text/plain", encodedData);
    return mimeData;
}

void ProjectTreeModel::ProjectTreeModel::MoveItem(
    const QModelIndex &srcIndex, const QModelIndex &targetIndex)
{
    // 检查源索引和目标索引是否有效
    if (!srcIndex.isValid() || !targetIndex.isValid())
        return;

    // 获取源节点和目标节点
    Node *srcNode = static_cast<Node*>(srcIndex.internalPointer());
    Node *targetParentNode = static_cast<Node*>(targetIndex.internalPointer());

    // 检查目标节点是否有子节点列表
    if (!targetParentNode)
        return;

    // 在源节点的父节点中查找源节点的位置
    Node *srcParentNode = srcNode->parentNode;
    if (!srcParentNode)
        return;

    int srcRow = srcParentNode->children.indexOf(srcNode);
    if (srcRow == -1)
        return;

    // 从源节点的父节点中移除源节点
    beginRemoveRows(srcIndex.parent(), srcRow, srcRow);
    srcParentNode->children.removeAt(srcRow);
    endRemoveRows();

    // 插入源节点到目标节点的子节点列表中
    int targetRow = targetIndex.row();
    beginInsertRows(targetIndex.parent(), targetRow, targetRow);
    targetParentNode->children.insert(targetRow, srcNode);
    srcNode->parentNode = targetParentNode;
    endInsertRows();
}
