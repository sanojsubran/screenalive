#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSystemTrayIcon>
#include <thread>

class TimerComponent;
class QTime;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    TimerComponent *m_timerComponent;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    void initializeComponents();
    void connectSignalSlots();

public slots:
    void exitApp();
    void updateTooltip( QString stamp );
    void triggeredSystemTrayIcon( QSystemTrayIcon::ActivationReason );
    void  triggeredAction( QAction * );
};


class TimerComponent : public QThread
{
    Q_OBJECT

public:
    TimerComponent();
    ~TimerComponent();
    void exitThread();
    bool isThreadFinished();
    void generateKeyStroke();

signals:
    void sendTimeStamp( QString stamp );

private:
    bool m_threadExit;
    QString m_toolTip;

protected:
    void run();
};

#endif // MAINWINDOW_H
