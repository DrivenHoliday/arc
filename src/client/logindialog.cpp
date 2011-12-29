#include "logindialog.h"
#include "ui_logindialog.h"

LoginDialog::LoginDialog(ServerListModel *model, QWidget *parent/* = 0*/)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    ui->serverList->setModel(model);
}

LoginDialog::~LoginDialog()
{
    delete ui;
}
