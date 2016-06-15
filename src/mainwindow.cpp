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
#include "ui_mainwindow.h"

const QTime dailyWorkHours( 8, 0 );

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_trayIcon( nullptr ),
    m_trayMenu( nullptr )
{
    ui->setupUi(this);
    initializeComponents();
    connectSignalSlots();
    m_trayIcon->show();
    m_timerComponent->start();
}



MainWindow::~MainWindow()
{
    delete m_timerComponent;
    delete ui;
}

void MainWindow::initializeComponents()
{
    ui->settingsButton->setDisabled( true );
    ui->addNewTimerButton->setDisabled( true );
    m_timerComponent = new TimerComponent();
    ui->stopTimerButton->setDisabled( true );
    m_trayIcon = new QSystemTrayIcon();
    m_trayMenu = new QMenu();
    m_trayMenu->addAction( "Quit" );
    QIcon icon( "icon.png" );
    m_trayIcon->setIcon( icon );
    m_trayIcon->setToolTip( "" );
    m_trayIcon->setContextMenu( m_trayMenu );
    setWindowIcon( icon );
}

void MainWindow::connectSignalSlots()
{
//    QObject::connect( ui->startTimerButton,
//                      SIGNAL(clicked(bool) ),
//                      this,
//                      SLOT( startCountDown() ),
//                      Qt::UniqueConnection );
//    QObject::connect( ui->stopTimerButton,
//                      SIGNAL( clicked(bool) ),
//                      this,
//                      SLOT( stopCountDown() ),
//                      Qt::UniqueConnection );
    QObject::connect( m_timerComponent,
                      SIGNAL( signalAppExit() ),
                      this,
                      SLOT( exitApp() ),
                      Qt::UniqueConnection );
    QObject::connect( m_timerComponent,
                      SIGNAL( triggerEvent( QString ) ),
                      this,
                      SLOT( updateClockLabel( QString ) ),
                      Qt::UniqueConnection );
    QObject::connect( m_trayIcon,
                      SIGNAL( activated( QSystemTrayIcon::ActivationReason ) ),
                      this,
                      SLOT( triggeredSystemTrayIcon(
                                QSystemTrayIcon::ActivationReason) ) ,
                      Qt::UniqueConnection );
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

void MainWindow::adjustMainWindowProperties()
{
    QRect screenSize = QApplication::desktop()->screenGeometry();
    this->setWindowTitle( "Daily Quota" );
    this->setFixedSize( 367, 201);
    this->setWindowFlags( (windowFlags() | Qt::CustomizeWindowHint)
                          & ~Qt::WindowMaximizeButtonHint
                          & ~Qt::WindowMinimizeButtonHint
                          & ~Qt::WindowCloseButtonHint );
    //int x = screenSize.bottomLeft().x();
    //int y = screenSize.bottomLeft().y();
    this->setGeometry( screenSize.bottomLeft().x() + 100,
                       screenSize.bottomLeft().y() - 250,
                       370,
                       200 );
}

void MainWindow::exitApp()
{
   m_timerComponent->exitThread();
   QTime waitingPeriod;
   waitingPeriod.start();
   while( true ) {
       if( m_timerComponent->isThreadFinished()
               || ( 5000 == waitingPeriod.elapsed() ) )  {
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


void MainWindow::startCountDown()
{
    ui->startTimerButton->setDisabled( true );
    ui->stopTimerButton->setEnabled( true );
    m_timerComponent->start();
}


void MainWindow::stopCountDown()
{
    m_timerComponent->exitThread();
    ui->startTimerButton->setEnabled( true );
    ui->stopTimerButton->setDisabled( true );
    //ui->countDownLabel->setText( "08:00:00" );
}


void MainWindow::updateClockLabel( QString time )
{
        ui->countDownLabel->setText( time );
}

TimerComponent::TimerComponent()
                : m_threadExit( false ),
                  m_startTime( nullptr )
{
    m_startTime = QSharedPointer< QTime >();
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

qlonglong findTimeLapse( QTime &initialTime, QTime &finalTime ) {
    auto timeinMS = []( QTime &time )->qlonglong {
        qlonglong milliSecs = static_cast< qlonglong >(
                              ( time.hour() * 60 * 60 * 1000 ) +
                              ( time.minute() * 60 * 1000 ) +
                              ( time.second() * 1000 ) +
                              ( time.msec() ) );
        return milliSecs;
    };
    qlonglong timeLapsed = 0;
    qlonglong initialTimeisMs = timeinMS( initialTime );
    qlonglong finalTimeisMs = timeinMS( finalTime );
    timeLapsed = finalTimeisMs - initialTimeisMs;
    //timeLapsed = timeinMS( finalTime ) - timeinMS( initialTime );
    return timeLapsed;
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
        Sleep( 5 * 60 * 1000 );
    }
}
