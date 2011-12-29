#include "serverlistmodel.h"

ServerListModel::ServerListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int ServerListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return 10;
}

int ServerListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 4;
}

QVariant ServerListModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
            case 0:
                return QString("Header 0");
            case 1:
                return QString("Header 1");
            case 2:
                return QString("Header 2");
            case 3:
                return QString("Header 3");
            default:
                return QString("Header default");
        }
    }

    return QVariant();
}

QVariant ServerListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 /*|| index.row() >= lst.size()*/)
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case 0:
            return QString("Spalte 0");
        case 1:
            return QString("Spalte 1");
        case 2:
            return QString("Spalte 2");
        case 3:
            return QString("Spalte 3");
        default:
            return QString("default");
        }
    }

    return QVariant();
}

Qt::ItemFlags ServerListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index) | Qt::ItemIsDropEnabled;

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable | Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled;
}

bool ServerListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 /*&& index.row() < lst.size()*/
        && (role == Qt::EditRole || role == Qt::DisplayRole))
    {
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

bool ServerListModel::insertRows(int row, int count, const QModelIndex &parent)
{
    if (count < 1 || row < 0 || row > rowCount(parent))
        return false;

    beginInsertRows(QModelIndex(), row, row + count - 1);



    endInsertRows();

    return true;
}

bool ServerListModel::removeRows(int row, int count, const QModelIndex &parent)
{
    if (count <= 0 || row < 0 || (row + count) > rowCount(parent))
        return false;

    beginRemoveRows(QModelIndex(), row, row + count - 1);



    endRemoveRows();

    return true;
}


//void ServerListModel::sort(int, Qt::SortOrder order)
//{
//    emit layoutAboutToBeChanged();

//    QList<QPair<QString, int> > list;
//    for (int i = 0; i < lst.count(); ++i)
//        list.append(QPair<QString, int>(lst.at(i), i));

//    if (order == Qt::AscendingOrder)
//        qSort(list.begin(), list.end(), ascendingLessThan);
//    else
//        qSort(list.begin(), list.end(), decendingLessThan);

//    lst.clear();
//    QVector<int> forwarding(list.count());
//    for (int i = 0; i < list.count(); ++i) {
//        lst.append(list.at(i).first);
//        forwarding[list.at(i).second] = i;
//    }

//    QModelIndexList oldList = persistentIndexList();
//    QModelIndexList newList;
//    for (int i = 0; i < oldList.count(); ++i)
//        newList.append(index(forwarding.at(oldList.at(i).row()), 0));
//    changePersistentIndexList(oldList, newList);

//    emit layoutChanged();
//}

Qt::DropActions ServerListModel::supportedDropActions() const
{
    return QAbstractItemModel::supportedDropActions() | Qt::MoveAction;
}
