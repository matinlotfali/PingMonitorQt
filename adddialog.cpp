#include "adddialog.h"
#include "ui_adddialog.h"

AddDialog::AddDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddDialog)
{
    ui->setupUi(this);
    this->setFixedSize(this->width(),this->height());
}

AddDialog::~AddDialog()
{
    delete ui;
}

void AddDialog::accept()
{
    name = ui->nameEdit->text();
    url = ui->urlEdit->text();
    QDialog::accept();
}
