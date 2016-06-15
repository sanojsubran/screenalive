#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QThread>
#include <QSystemTrayIcon>
#include <thread>

class TimerComponent;
class QTime;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    TimerComponent *m_timerComponent;
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    void initializeComponents();
    void connectSignalSlots();
    void adjustMainWindowProperties();

public slots:
    void exitApp();
    void startCountDown();
    void stopCountDown();
    void updateClockLabel( QString time );
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
    void triggerEvent( QString time );
    void signalAppExit();
    void sendTimeStamp( QString stamp );

private:
    QSharedPointer< QTime > m_startTime;
    bool m_threadExit;
    QString m_toolTip;

protected:
    void run();
};

#endif // MAINWINDOW_H
