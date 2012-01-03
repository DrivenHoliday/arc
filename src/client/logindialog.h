#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>

#include "serverlistmodel.h"

namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit LoginDialog(ServerListModel *model, QWidget *parent = 0);
    ~LoginDialog();

private slots:
    void nicknameChanged(QString);
    void addServer();
    void delServer();
    
private:
    Ui::LoginDialog *ui;
    ServerListModel *mModel;
};

#endif // LOGINDIALOG_H
