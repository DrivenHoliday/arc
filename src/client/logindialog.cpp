#include "logindialog.h"
#include "ui_logindialog.h"

#include "newserverdialog.h"

LoginDialog::LoginDialog(ServerListModel *model, QWidget *parent/* = 0*/)
    : QDialog(parent)
    , ui(new Ui::LoginDialog)
    , mModel(model)
{
    ui->setupUi(this);
    ui->serverList->setModel(model);

    connect(ui->nicknameEdit, SIGNAL(textChanged(QString)), SLOT(nicknameChanged(QString)));
    connect(ui->addButton, SIGNAL(clicked()), SLOT(addServer()));
    connect(ui->deleteButton, SIGNAL(clicked()), SLOT(delServer()));
}

LoginDialog::~LoginDialog()
{
    delete ui;
}

void LoginDialog::nicknameChanged(QString text)
{
    ui->loginButton->setEnabled(text.size() != 0);
}

void LoginDialog::addServer()
{
    NewServerDialog *dialog = new NewServerDialog(this);

    const int result = dialog->exec();

    if(result == QDialog::Accepted)
    {
        mModel->addServer(dialog->server().toStdString(), dialog->port());
    }

    dialog->deleteLater();
}

void LoginDialog::delServer()
{
    QItemSelectionModel *selection =  ui->serverList->selectionModel();
    QModelIndexList list = selection->selectedRows();

    Q_FOREACH(QModelIndex i, list)
    {
        mModel->removeRow(i.row());
    }
}
