#include <QApplication>
#include <QSharedPointer>
#include <QDesktopWidget>
#include <QTime>
#include <QTimer>
#include <QMenu>
#include <QRect>
#include <QMessageBox>
#include <Windows.h>
#include <WinUser.h>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    m_trayIcon( nullptr ),
    m_trayMenu( nullptr )
{
    initializeComponents();
    connectSignalSlots();
    m_trayIcon->show();
    m_timerComponent->start();
}



MainWindow::~MainWindow()
{
    delete m_timerComponent;
}

void MainWindow::initializeComponents()
{
    m_timerComponent = new TimerComponent();
    m_trayIcon = new QSystemTrayIcon();
    m_trayMenu = new QMenu();
    m_trayMenu->addAction( "Quit" );
    QIcon icon( "icon.png" );
    m_trayIcon->setIcon( icon );
    m_trayIcon->setToolTip( "" );
    m_trayIcon->setContextMenu( m_trayMenu );
}

void MainWindow::connectSignalSlots()
{
    QObject::connect( m_trayMenu,
                      SIGNAL( triggered( QAction* ) ),
                      this,
                      SLOT( triggeredAction( QAction* ) ),
                      Qt::UniqueConnection );
    QObject::connect( m_timerComponent,
                      SIGNAL( sendTimeStamp( QString ) ),
                      this,
                      SLOT( updateTooltip( QString ) ),
                      Qt::UniqueConnection );
}


void MainWindow::updateTooltip( QString stamp )
{
    m_trayIcon->setToolTip( stamp );
}


void MainWindow::exitApp()
{
   m_timerComponent->exitThread();
   QTime waitingPeriod;
   waitingPeriod.start();
   while( true ) {
       if( m_timerComponent->isThreadFinished()
               || ( 3000 < waitingPeriod.elapsed() ) )  {
           break;
       }
   }
   QApplication::exit();
}


void MainWindow::triggeredSystemTrayIcon(
        QSystemTrayIcon::ActivationReason reason )
{
    Q_UNUSED( reason )
}


void MainWindow::triggeredAction( QAction *action )
{
    if( "Quit" == action->text() ) {
        exitApp();
    }
}


TimerComponent::TimerComponent()
                : m_threadExit( false )
{
}

TimerComponent::~TimerComponent()
{
}

void TimerComponent::exitThread()
{
    m_threadExit = true;
}

bool TimerComponent::isThreadFinished()
{
    return isFinished();
}


void TimerComponent::run()
{
    generateKeyStroke();
    m_threadExit = false;
}

void TimerComponent::generateKeyStroke()
{
    INPUT input;
    QTime time;
    input.type = INPUT_KEYBOARD;
    input.ki.wScan = 0;
    input.ki.time = 0;

    input.ki.wVk = 0x7E;
    input.ki.dwFlags = 0;
    while( ! m_threadExit ) {
        input.ki.dwExtraInfo = 0;
        SendInput( 1, &input, sizeof( INPUT ) ) ;
        input.ki.dwFlags = KEYEVENTF_KEYUP;
        SendInput( 1, &input, sizeof( INPUT ) );
        m_toolTip = "Last time checked: "
                + time.currentTime().toString( Qt::SystemLocaleLongDate );
        emit sendTimeStamp( m_toolTip );
        Sleep( 1 * 60 * 1000 );
    }
}
