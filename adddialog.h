#ifndef ADDDIALOG_H
#define ADDDIALOG_H

#include <QDialog>

namespace Ui {
class AddDialog;
}

class AddDialog : public QDialog
{
    Q_OBJECT

public:
    QString name,url;
    explicit AddDialog(QWidget *parent = 0);
    ~AddDialog();    

private:
    Ui::AddDialog *ui;

    void accept();
};

#endif // ADDDIALOG_H
