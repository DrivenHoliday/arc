#ifndef SERVERLISTMODEL_H
#define SERVERLISTMODEL_H

#include <string>
#include <vector>

#include "arc_client.hpp"

#include <QAbstractListModel>

class ServerListModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit ServerListModel(QObject *parent = 0);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &/*parent*/) const;

    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    QVariant data(const QModelIndex &index, int role) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Qt::ItemFlags flags(const QModelIndex &index) const;

    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex());
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex());

//    void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);
    void sort();

    Qt::DropActions supportedDropActions() const;

    void load(std::istream &istream);
    void save(std::ostream &ostream);

    void addServer(const std::string &server, arc::client::port_t port);

private:
    struct server_list_entry
    {
        enum status_e
        {
            online = 0,
            unknown = 1,
            offline = 2
        };

        server_list_entry()
            : status(unknown), port(0)
        { }

        server_list_entry(const std::string &name, arc::client::port_t port)
            : status(unknown), name(name), port(port)
        { }

        std::string name;
        arc::client::port_t port;
        status_e status;

        bool operator<(const server_list_entry &other) const
        {
            if(status == other.status)
                return name < other.name;
            else
                return status < other.status;
        }

        bool operator==(const server_list_entry &other) const
        {
            return status == other.status && name == other.name && port == other.port;
        }

        template<class Archive>
        void serialize(Archive & ar, const unsigned int)
        {
            ar & BOOST_SERIALIZATION_NVP(name);
            ar & BOOST_SERIALIZATION_NVP(port);
        }
    };

    typedef std::vector<server_list_entry> server_list_vec;
    server_list_vec m_server_list;
    
};

#endif // SERVERLISTMODEL_H
