#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QLabel>
#include <QEvent>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QCloseEvent>

namespace Ui {
class MainWindow;
}

class PingThread;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QString AppDir, QWidget *parent = 0);
    ~MainWindow();    

private:
    Ui::MainWindow *ui;
    QSystemTrayIcon *tray;
    QString dataFile = "data.txt";
    QTimer *errorTimer, *normalTimer;
    QIcon *iconSuccess, *iconTimedOut, *iconInfo;
    QLabel *statusIcon;
    QLabel *statusLabel;
    QMenu *trayMenu;
    bool closeAllowed;

    void AddRow(QString name, QString url);
    void RefreshStatus();
    void changeEvent(QEvent *event);
    void closeEvent(QCloseEvent *event);
    void Save();

private slots:
    void ErrorTick();
    void NormalTick();
    void PingResult(PingThread *thread, QString url, QString time);
    void TrayClicked(QSystemTrayIcon::ActivationReason reason);
    void DeleteClicked(bool);
    void AddClicked(bool);
    void ExitClicked(bool);
};

#endif // MAINWINDOW_H
