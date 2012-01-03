#include <QtGui/QApplication>

#include <fstream>

#include "mainwindow.h"
#include "logindialog.h"
#include "serverlistmodel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Q_INIT_RESOURCE(icons);

    ServerListModel *serverListModel = new ServerListModel;

    const char *server_list_file =  "server.xml";

    {
        std::ifstream in(server_list_file);
        if(in)
        {
            serverListModel->load(in);
        }
    }

    LoginDialog login(serverListModel);
    const int loginDialogResult = login.exec();

    {
        std::ofstream out(server_list_file);
        if(out)
        {
            serverListModel->save(out);
        }
    }

    if(loginDialogResult == QDialog::Rejected)
        return EXIT_SUCCESS;

    MainWindow w;
    w.show();
    
    return a.exec();
}
