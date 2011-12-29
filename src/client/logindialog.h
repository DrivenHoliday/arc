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
    
private:
    Ui::LoginDialog *ui;
};

#endif // LOGINDIALOG_H
