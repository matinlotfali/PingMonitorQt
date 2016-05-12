#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "QFileInfo"
#include "QFile"
#include "QTextStream"
#include "pingthread.h"
#include "QDebug"
#include "adddialog.h"
#include "QMenu"
#include "QList"

enum columns
{
    COLUMN_IMAGE,
    COLUMN_NAME,
    COLUMN_URL,
    COLUMN_TIME,
    COLUMN_STATUS
};

#define STATUS_PINGING "p"
#define STATUS_IDLE "i"

MainWindow::MainWindow(QString AppDir, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    closeAllowed = false;

    setWindowIcon(QIcon(":/images/Images/Project1.ico"));

    statusLabel = new QLabel(this);
    statusIcon = new QLabel(this);
    statusBar()->addWidget(statusIcon);
    statusBar()->addWidget(statusLabel);

    ui->tableWidget->setColumnWidth(COLUMN_IMAGE,20);
    ui->tableWidget->setColumnHidden(COLUMN_URL,true);
    ui->tableWidget->setColumnHidden(COLUMN_STATUS,true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(COLUMN_NAME,QHeaderView::Stretch);
    ui->tableWidget->horizontalHeader()->setDefaultAlignment(Qt::AlignLeft);
    ui->tableWidget->horizontalHeader()->setSortIndicator(COLUMN_TIME,Qt::AscendingOrder);


    iconSuccess = new QIcon(":/images/Images/Sample 07.ico");
    iconTimedOut = new QIcon(":/images/Images/Sample 02.ico");
    iconInfo = new QIcon(":/images/Images/info.ico");

    trayMenu = new QMenu(this);
    QList<QAction*> *list = new QList<QAction*>();
    list->push_back(ui->actionExit);
    trayMenu->addActions(*list);
    delete list;

    tray = new QSystemTrayIcon(this);
    tray->setIcon(*iconInfo);
    tray->setVisible(true);
    tray->setContextMenu(trayMenu);

    dataFile = AppDir +"/"+ dataFile;
    QFileInfo fileInfo(dataFile);
    if(fileInfo.exists() && fileInfo.isFile())
    {
        QFile file(dataFile,this);
        if(file.open(QIODevice::ReadOnly))
        {
            QTextStream textStream(&file);
            for(;;)
            {
                QString name = textStream.readLine();
                if(name.isNull()) break;
                QString url = textStream.readLine();
                if(url.isNull()) break;
                AddRow(name,url);
            }
        }
    }

    errorTimer = new QTimer(this);    
    errorTimer->start(500);

    normalTimer = new QTimer(this);    
    normalTimer->start(30000);

    connect(tray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(TrayClicked(QSystemTrayIcon::ActivationReason)));
    connect(errorTimer,&QTimer::timeout,this,&MainWindow::ErrorTick);
    connect(normalTimer,&QTimer::timeout,this,&MainWindow::NormalTick);
    connect(ui->actionAdd,SIGNAL(triggered(bool)),this,SLOT(AddClicked(bool)));
    connect(ui->actionRemove,SIGNAL(triggered(bool)),this,SLOT(DeleteClicked(bool)));
    connect(ui->actionExit, SIGNAL(triggered(bool)),this,SLOT(ExitClicked(bool)));
}

MainWindow::~MainWindow()
{
    delete trayMenu;
    delete statusLabel;
    delete statusIcon;
    delete iconInfo;
    delete iconSuccess;
    delete iconTimedOut;
    delete errorTimer;
    delete normalTimer;
    delete tray;
    delete ui;
}

void MainWindow::AddRow(QString name, QString url)
{
    ui->tableWidget->insertRow(0);
    ui->tableWidget->setItem(0, COLUMN_IMAGE, new QTableWidgetItem());
    ui->tableWidget->setItem(0, COLUMN_NAME, new QTableWidgetItem(name));
    ui->tableWidget->setItem(0, COLUMN_URL, new QTableWidgetItem(url));
    ui->tableWidget->setItem(0, COLUMN_TIME, new QTableWidgetItem());
    ui->tableWidget->setItem(0, COLUMN_STATUS, new QTableWidgetItem(STATUS_IDLE));
}

void MainWindow::ErrorTick()
{
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QString time = ui->tableWidget->item(i,COLUMN_TIME)->text();
        QString state = ui->tableWidget->item(i,COLUMN_STATUS)->text();
        if(time == NULL && state == STATUS_IDLE)
        {
            ui->tableWidget->item(i,COLUMN_STATUS)->setText(STATUS_PINGING);
            QString url = ui->tableWidget->item(i,COLUMN_URL)->text();
            PingThread *p = new PingThread(url,this);
            p->start();
            connect(p,SIGNAL(Result(PingThread*,QString,QString)),this,SLOT(PingResult(PingThread*,QString,QString)));
        }
    }
}

void MainWindow::NormalTick()
{
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QString time = ui->tableWidget->item(i,COLUMN_TIME)->text();
        QString state = ui->tableWidget->item(i,COLUMN_STATUS)->text();
        if(time != NULL && state == STATUS_IDLE)
        {
            ui->tableWidget->item(i,COLUMN_STATUS)->setText(STATUS_PINGING);
            QString url = ui->tableWidget->item(i,COLUMN_URL)->text();
            PingThread *p = new PingThread(url,this);
            p->start();
            connect(p,SIGNAL(Result(PingThread*,QString,QString)),this,SLOT(PingResult(PingThread*,QString,QString)));
        }
    }
}

void MainWindow::PingResult(PingThread *thread,QString url, QString time)
{
    thread->wait();
    delete thread;
    qDebug()<<url<<" time: "<<time;
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
        if(ui->tableWidget->item(i,COLUMN_URL)->text() == url)
        {
            ui->tableWidget->item(i,COLUMN_STATUS)->setText(STATUS_IDLE);
            if(time == NULL)
            {
                ui->tableWidget->item(i,COLUMN_IMAGE)->setIcon(*iconTimedOut);
                ui->tableWidget->item(i,COLUMN_TIME)->setText(NULL);
            }
            else
            {
                ui->tableWidget->item(i,COLUMN_IMAGE)->setIcon(*iconSuccess);
                ui->tableWidget->item(i,COLUMN_TIME)->setData(Qt::DisplayRole,time.toDouble());
            }
            break;
        }
    RefreshStatus();
}

void MainWindow::RefreshStatus()
{
    bool foundSuccess = false;
    bool foundError = false;
    for(int i=0; i<ui->tableWidget->rowCount(); i++)
    {
        QString time = ui->tableWidget->item(i,COLUMN_TIME)->text();
        if(time == NULL)
            foundError = true;
        else
            foundSuccess = true;
    }

    if(foundSuccess && !foundError)
    {
        statusLabel->setText("All Successfull");
        statusIcon->setPixmap(iconSuccess->pixmap(16,16));
        tray->setIcon(*iconSuccess);
    }
    else if(foundSuccess && foundError)
    {
        statusLabel->setText("Partial Successfull");
        statusIcon->setPixmap(iconInfo->pixmap(16,16));
        tray->setIcon(*iconInfo);
    }
    else if(!foundSuccess && foundError)
    {
        statusLabel->setText("Partial Successfull");
        statusIcon->setPixmap(iconTimedOut->pixmap(16,16));
        tray->setIcon(*iconTimedOut);
    }
    tray->setToolTip(statusLabel->text());
}

void MainWindow::changeEvent(QEvent *event)
{
    if(isMinimized())
    {
        hide();
        event->ignore();
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if(!closeAllowed)
    {
       hide();
       event->ignore();
    }
}

void MainWindow::TrayClicked(QSystemTrayIcon::ActivationReason reason)
{
    if(reason == QSystemTrayIcon::Trigger)
    {
        show();
        setWindowState(Qt::WindowNoState);
    }
    else if(reason == QSystemTrayIcon::Context)
    {
        tray->contextMenu()->show();
    }

}

void MainWindow::DeleteClicked(bool)
{
    if(ui->tableWidget->selectedItems().length() > 0)
    {
        int index = ui->tableWidget->selectedItems().at(0)->row();
        ui->tableWidget->removeRow(index);
        Save();
    }
}

void MainWindow::AddClicked(bool)
{
    AddDialog dialog (this);
    if(dialog.exec() == QDialog::Accepted)
    {
        AddRow(dialog.name,dialog.url);
        Save();
    }
}

void MainWindow::Save()
{
    QFile file(dataFile,this);
    if(file.open(QIODevice::WriteOnly))
    {
        QTextStream stream( &file );
        for(int i=0;i<ui->tableWidget->rowCount();i++)
        {
            stream<<ui->tableWidget->item(i,COLUMN_NAME)->text()<<endl;
            stream<<ui->tableWidget->item(i,COLUMN_URL)->text()<<endl;
        }
    }
}

void MainWindow::ExitClicked(bool)
{
    closeAllowed = true;
    close();
}
