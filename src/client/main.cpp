#include <QtGui/QApplication>
#include "mainwindow.h"
#include "logindialog.h"
#include "serverlistmodel.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    ServerListModel *serverListModel = new ServerListModel;

    LoginDialog login(serverListModel);
    login.exec();

    MainWindow w;
    w.show();
    
    return a.exec();
}
