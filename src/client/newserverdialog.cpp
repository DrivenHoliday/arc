#include "newserverdialog.h"
#include "ui_newserverdialog.h"

#include <QPushButton>

NewServerDialog::NewServerDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewServerDialog)
{
    ui->setupUi(this);

    serverChanged(ui->nameEdit->text());

    connect(ui->nameEdit, SIGNAL(textChanged(QString)), SLOT(serverChanged(QString)));
}

NewServerDialog::~NewServerDialog()
{
    delete ui;
}

QString NewServerDialog::server() const
{
    return ui->nameEdit->text();
}

arc::client::port_t NewServerDialog::port() const
{
    return ui->portBox->value();
}

void NewServerDialog::serverChanged(QString server)
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(server.size() != 0);
}
