#include <QDebug>

#include "ftpcontroller.h"
#include "clientManager.h"

void FtpController::init()
{
    timer.setInterval(1000);
    for(int i=0;i<24;i++){
        m_ftpHostList.append("");
    }
    QObject::connect(this, &FtpController::download, this, &FtpController::downloadFile);
    QObject::connect(this, &FtpController::batchDownload, this, &FtpController::batchDownloadFile);
}

FtpController *FtpController::getInstance()
{
    static FtpController ftpManager;
    return &ftpManager;
}

QObject *FtpController::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return FtpController::getInstance();
}

void FtpController::setUserInfo(const QString &userName, const QString &password, quint16 port)
{
    m_ftpPort = port;
    m_username = userName;
    m_password = password;
}

void FtpController::listFileInfo(const int &seat)
{
    m_ftpHostList[seat] = ClientManager::getInstance()->readDeviceInfoSettings(seat, "hostname").toString();
    qDebug() << m_ftpHostList[seat];
    FtpClient *worker = new FtpClient(this);
    worker->init();
    worker->setHostPort(m_ftpHostList[seat], m_ftpPort);
    worker->setUserInfo(m_username, m_password);
    worker->startFtpConnection();

    // 连接信号与槽
    QObject::connect(worker, SIGNAL(stateChanged(int)),
                     this, SLOT(ftpStateChanged(int)));
    QObject::connect(this, &FtpController::stateChanged, worker, [=](int state){
        if(state == LoggedIn){
            qDebug() << QMetaEnum::fromType<State>().valueToKey(state) << "正在读取文件列表...";
            worker->listFileInfo();
        }
    });
    QObject::connect(worker, &FtpClient::findRes, this, [=](const QString &dir){
        // 保存日志所在路径
        logDir = dir;
        worker->handleFoundDir(logDir);
    });
    QObject::connect(worker, &FtpClient::fileRecv,
                     this, &FtpController::handleRecvFile);
    QObject::connect(&timer, &QTimer::timeout, this, [=](){
        qDebug() << "已列出所有文件!";
        worker->closeClient();
        worker->deleteLater();
        timer.stop();
    });

}

void FtpController::downloadFile(const int &seat, const QString &server_file, const QString local_file)
{
    m_ftpHostList[seat] = ClientManager::getInstance()->readDeviceInfoSettings(seat, "hostname").toString();
    // 单线程
//    qDebug() << "current thread : " << QThread::currentThreadId();
    FtpClient *worker = new FtpClient(this);
    QObject::connect(worker, &FtpClient::commandFinished, this, [=](int id, bool error){
        qDebug() << "第" << id << "条命令完成"
                 << ", 当前命令 : " << QMetaEnum::fromType<Command>().valueToKey(worker->currentCmd())
                 << ", 完成状态 : " << !error;
        if(worker->currentCmd() == QFtp::Login && !error){
            worker->get(logDir , server_file, local_file);
        }
    });

    worker->init();
    worker->setHostPort(m_ftpHostList[seat], m_ftpPort);
    worker->setUserInfo(m_username, m_password);
    worker->startFtpConnection();

//    // 多线程
//    QThread *ftpThread = new QThread;
//    FtpThread *worker = new FtpThread;
//    worker->moveToThread(ftpThread);


//    // 连接信号与槽
//    QObject::connect(this, &FtpController::startDownload,
//                     worker, &FtpThread::downloadFile);
//    QObject::connect(ftpThread, &QThread::finished,
//                     worker, &QObject::deleteLater);
//    ftpThread->start();

//    // list组成
//    //  0 - (服务器ip)   1 - (服务器端口)  2 - (Ftp登录用户名)  3 - (Ftp登录密码)
//    //  4 - (服务器日志文件夹路径)  5 - (服务器日志名)  6 - (本地保存路径)
//    QList<QVariant> list;
//    list << m_ftpHostList[seat] << m_ftpPort << m_username << m_password
//         << logDir << server_file << local_file;
//    emit startDownload(list);
}

void FtpController::batchDownloadFile(const int &seat, const QString &local_file)
{
    qDebug() << seat;
    m_ftpHostList[seat] = ClientManager::getInstance()->readDeviceInfoSettings(seat, "hostname").toString();
    qDebug() << m_ftpHostList[seat];

//    FtpClient *worker = new FtpClient(this);
//    worker->init();
//    worker->setHostPort(m_ftpHostList[seat], m_ftpPort);
//    worker->setUserInfo(m_username, m_password);
//    worker->startFtpConnection();

//    // 连接信号与槽
//    QObject::connect(worker, SIGNAL(stateChanged(int)),
//                     this, SLOT(ftpStateChanged(int)));
//    QObject::connect(this, &FtpController::stateChanged, worker, [=](int state){
//        if(state == LoggedIn){
//            qDebug() << QMetaEnum::fromType<State>().valueToKey(state) << "正在读取文件列表...";
//            worker->listFileInfo();
//        }
//    });
//    QObject::connect(worker, &FtpClient::findRes, this, [=](const QString &dir){
//        // 保存日志所在路径
//        logDir = dir;
//        worker->handleFoundDir(logDir);
//    });
//    QObject::connect(worker, &FtpClient::fileRecv,
//                     this, &FtpController::handleData);
//    QObject::connect(&timer, &QTimer::timeout, this, [=](){
//        qDebug() << "已列出所有文件!";
//        timer.stop();
//        fileList.sort(Qt::CaseInsensitive);
//        worker->get(logDir, fileList.last(), local_file);
//    });

    // 多线程
    QThread *ftpThread = new QThread;
    FtpThread *worker = new FtpThread;
    worker->moveToThread(ftpThread);

    // 连接信号与槽
    QObject::connect(this, &FtpController::startDownload,
                     worker, &FtpThread::initThread);
    QObject::connect(ftpThread, &QThread::finished,
                     worker, &QObject::deleteLater);
    ftpThread->start();

    // list组成
    //  0 - (服务器ip)   1 - (服务器端口)  2 - (Ftp登录用户名)  3 - (Ftp登录密码)
    //  4 - (本地保存路径)  5 - (服务器日志文件夹路径)
    QList<QVariant> list;
    list << m_ftpHostList[seat] << m_ftpPort << m_username << m_password
         << local_file;
    qDebug() << list;
    emit startDownload(list);
}

void FtpController::handleRecvFile(const QString &file)
{
    timer.start();
//    qDebug() << file;
    emit fileRecv(file);
}

void FtpController::handleData(const QString &file)
{
    timer.start();
    fileList << file.split("  ").at(0);
}

FtpController::FtpController(QObject *parent) : QObject(parent)
{

}

FtpController::~FtpController()
{
    qDebug() << "destroy Ftp Controller";
}

void FtpController::ftpStateChanged(int state)
{
    switch (state) {
    case QFtp::Unconnected: {
        qDebug()<<"没有连接到主机或已经断开连接";
        emit stateChanged(Unconnected);
        break;
    }
    case QFtp::HostLookup: {
        qDebug()<<"正在进行主机名查找";
        emit stateChanged(HostLookup);
        break;
    }
    case QFtp::Connecting: {
        qDebug()<<"正在尝试连接到主机";
        emit stateChanged(Connecting);
        break;
    }
    case QFtp::Connected: {
        qDebug()<<"已实现与主机的连接";
        emit stateChanged(Connected);
        break;
    }
    case QFtp::LoggedIn: {
        qDebug()<<"已实现连接和用户登录";
        emit stateChanged(LoggedIn);
        // 列出所有文件列表
        break;
    }
    case QFtp::Closing: {
        qDebug()<<"连接正在关闭";
        emit stateChanged(Closing);
        break;
    }
    default:
        break;
    }
}
