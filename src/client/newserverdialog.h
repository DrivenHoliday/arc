#ifndef NEWSERVERDIALOG_H
#define NEWSERVERDIALOG_H

#include <QDialog>
#include <QString>

#include "arc_client.hpp"

namespace Ui {
class NewServerDialog;
}

class NewServerDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit NewServerDialog(QWidget *parent = 0);
    ~NewServerDialog();

    QString server() const;
    arc::client::port_t port() const;

private slots:
    void serverChanged(QString server);
    
private:
    Ui::NewServerDialog *ui;
};

#endif // NEWSERVERDIALOG_H
