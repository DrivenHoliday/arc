#include "serverlistmodel.h"

#include <algorithm>

#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include <boost/foreach.hpp>
#include <boost/serialization/nvp.hpp>
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/range/algorithm/find.hpp>
#include <boost/range/algorithm/reverse.hpp>
#include <boost/range/algorithm/stable_sort.hpp>

#include <QIcon>

ServerListModel::ServerListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

int ServerListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_server_list.size();
}

int ServerListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return 2;
}

QVariant ServerListModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
    if(orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        switch(section)
        {
            case 0:
                return QString("Name/IP");
            case 1:
                return QString("Port");
            default:
                return QVariant();
        }
    }

    return QVariant();
}

QVariant ServerListModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_server_list.size())
        return QVariant();

    server_list_entry srv = m_server_list.at(index.row());

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case 0:
            return QVariant(QString::fromStdString(srv.name));
        case 1:
            return QVariant(QString("%1").arg(srv.port));
        default:
            return QVariant();
        }
    }
    else if(role == Qt::DecorationRole && index.column() == 0)
    {
        switch(srv.status)
        {
            case server_list_entry::online:
                return QIcon(":/dots/green_dot.svg");
                break;
            case server_list_entry::offline:
                return QIcon(":/dots/red_dot.svg");
                break;
            case server_list_entry::unknown:
                return QIcon(":/dots/gray_dot.svg");
                break;
            default:
                return QVariant();
        }
    }

    return QVariant();
}

Qt::ItemFlags ServerListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return QAbstractItemModel::flags(index);

    return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
}

bool ServerListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (index.row() >= 0 && index.row() < m_server_list.size()
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

    for(int n = row; n < row + count; ++n)
    {
        m_server_list.erase(m_server_list.begin() + n);
    }

    endRemoveRows();

    return true;
}


//void ServerListModel::sort(int, Qt::SortOrder order)
void ServerListModel::sort()
{
    emit layoutAboutToBeChanged();

    server_list_vec old = m_server_list;

    boost::range::stable_sort(m_server_list);

//    if(order == Qt::DescendingOrder)
//    {
//        boost::range::reverse(m_server_list);
//    }

    std::vector<int> forwarding;

    BOOST_FOREACH(server_list_entry &e, m_server_list)
    {
        forwarding.push_back(std::distance(old.begin(),boost::range::find(old,e)));
    }

    QModelIndexList oldList = persistentIndexList();
    QModelIndexList newList;
    for (int i = 0; i < oldList.count(); ++i)
        newList.append(index(forwarding.at(oldList.at(i).row()), 0));
    changePersistentIndexList(oldList, newList);

    emit layoutChanged();
}

Qt::DropActions ServerListModel::supportedDropActions() const
{
    return QAbstractItemModel::supportedDropActions() | Qt::MoveAction;
}

void ServerListModel::load(std::istream &istream)
{
    boost::archive::xml_iarchive ia(istream);
    ia >> boost::serialization::make_nvp("server_list", m_server_list);
    sort();
}

void ServerListModel::save(std::ostream &ostream)
{
    boost::archive::xml_oarchive oa(ostream);
    oa << boost::serialization::make_nvp("server_list", m_server_list);
}

void ServerListModel::addServer(const std::string &server, arc::client::port_t port)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount() + 1);

    m_server_list.push_back(server_list_entry(server, port));

    endInsertRows();

    sort();
}
