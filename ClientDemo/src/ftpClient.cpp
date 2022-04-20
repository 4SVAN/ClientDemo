#include <QTimer>
#include <QMimeDatabase>
#include <sys/time.h>

#include "ftpClient.h"
#include "ftpcontroller.h"

/*-----------------------------------------------------------------------------|
 |                                 FtpClient                                   |
 |----------------------------------------------------------------------------*/

//struct timeval tpstart,tpend;
//float timeuse;

FtpClient::FtpClient(QObject *parent)
    : QObject(parent)
{

}

FtpClient::~FtpClient()
{
    qDebug() << "deleted FtpClient";
}

void FtpClient::resetDir()
{
    // 初始化数据，保证下次使用
    dirList.clear();
    dirList.resize(row);
    count = 0;
    it = 0;
}

void FtpClient::listFileInfo()
{
    qDebug() << is;
    if(is){
//        gettimeofday(&tpstart,NULL);
        resetDir();
        QObject::connect(m_ftp, SIGNAL(commandFinished(int,bool)),
                         this, SLOT(startNextSerach(int,bool)));
        QObject::connect(m_ftp, SIGNAL(listInfo(const QUrlInfo&)),
                         this, SLOT(bfsFolder(const QUrlInfo&)));
        m_ftp->list();
    }
}

QFtp::Command FtpClient::currentCmd()
{
    return m_ftp->currentCommand();
}

QFtp::State FtpClient::currentState()
{
    return m_ftp->state();
}

void FtpClient::init()
{
    qRegisterMetaType<QList<QVariant>>();
    qRegisterMetaType<QUrlInfo>();
//    m_pUrl.setScheme("ftp");    // 设置协议为ftp协议
//    m_pUrl.setPath("/MSETP-StudentSuite/log/");
//    qDebug() << m_pUrl;

//    QObject::connect(&workThread, &QThread::finished, worker, &QObject::deleteLater);
    qDebug()<<"current thread : FtpClient, ID:"<<QThread::currentThreadId();
}

QString FtpClient::getSize(const qint64 &file_size)
{
    int count = -1;
    qint64 tmp = file_size;
    if(tmp < KBYTE){
        QString str = QString::number(tmp) + "B";
        qDebug() << str;
        return str;
    }
    tmp = tmp / KBYTE;
    count++;
    QString str = QString::number(tmp);
    for(int i=str.length()-3;i>0;i-=3){
        str.insert(i,",");
    }
    str+=" KB";
    return str;
}

void FtpClient::closeClient()
{
    if(!m_ftp){
        qWarning() << "m_ftp does not exist";
        return;
    }
    m_ftp->abort();;
    m_ftp->close();
    m_ftp->disconnect();
    m_ftp->deleteLater();
    m_ftp = NULL;
}

void FtpClient::bfsFolder(const QUrlInfo &urlInfo)
{
//    qDebug() << "current level: " << count;
//    qDebug() << urlInfo.name() << "is dir?" << urlInfo.isDir();
    QString dir_t;

    if(count==0){
        dir_t = "/" + urlInfo.name();
//        qDebug() << dir_t;
        if(urlInfo.isDir()){
            if(dir_t.contains(LOGDIR)){
                QObject::disconnect(m_ftp, SIGNAL(listInfo(const QUrlInfo&)), 0, 0);
                QObject::disconnect(m_ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(startNextSerach(int,bool)));
                qInfo() << "已找到目标文件夹！路径 : " << dir_t;
                emit findRes(dir_t);
                return;
            }
            else if(!dirList[count].contains(dir_t)){
//                qDebug() << "append: " << dir_t << "to level: " << count;
                dirList[count].append(dir_t);
                it++;
            }
        }
    }
    else{
        int tmp_ctr=count-1;    // 指向上一层目录
//        qDebug() << "tmp_ctr: " << tmp_ctr << ", it: " << it;
//        qDebug() << dirList[tmp_ctr];
//        qDebug() << "Current Dir:" << dirList[tmp_ctr].at(it-1);
        dir_t = dirList[tmp_ctr].at(it-1) + "/" + urlInfo.name();
//        qDebug() << dir_t;
        if(urlInfo.isDir()){
            if(dir_t.contains(LOGDIR)){
                QObject::disconnect(m_ftp, SIGNAL(listInfo(const QUrlInfo&)), 0, 0);
                QObject::disconnect(m_ftp, SIGNAL(commandFinished(int,bool)), this, SLOT(startNextSerach(int,bool)));
                qInfo() << "已找到目标文件夹！路径 : " << dir_t;
                emit findRes(dir_t);
                return;
            }
            else if(!dirList[tmp_ctr+1].contains(dir_t)){
//                qDebug() << "append: " << dir_t << "to level: " << tmp_ctr+1;
                dirList[tmp_ctr+1].append(dir_t);
            }
        }
    }

}

void FtpClient::startNextSerach(int id, bool error)
{
    if(!error && m_ftp->currentCommand() == QFtp::List){
//        qDebug() << dirList;
//        qDebug() << "current level:" << count << "it : " << it;
        if(count==0 && !dirList[count].isEmpty()){
            it=0;
            count++;
            QString dir_t = dirList[count-1].at(it);
//            qDebug() << dir_t;
            m_ftp->list(dir_t);
            it++;
        }
        else if(!dirList[count-1].isEmpty()){
//            qDebug() << dirList[count-1] << " size:" << dirList[count-1].size();

            if(it==dirList[count-1].size()){
                count++;
                it=0;
//                qDebug() << "upgrade Level to : " << count;
//                qDebug() << dirList[count-1];
            }

            QString dir_t = dirList[count-1].at(it);
//            qDebug() << dir_t;
            m_ftp->list(dir_t);
            it++;
//            qDebug() << "Current iterator: " << it;
        }

    }
    else{
        qDebug() << "commandId:" << id << " error state: " << error;
    }
}

void FtpClient::handleFoundDir(const QString &dir)
{
    // 找到日志文件夹目录(LOGDIR)后重置is状态和fileList，方便下次调用
//    gettimeofday(&tpend,NULL);
//    timeuse=(1000000*(tpend.tv_sec-tpstart.tv_sec) + tpend.tv_usec-tpstart.tv_usec)/1000000.0;
//    qDebug() << "搜索耗时 : " << timeuse << "s";

    QTimer::singleShot(200,this,[=](){
        // 连接信号与槽
        QObject::connect(m_ftp, SIGNAL(listInfo(const QUrlInfo&)),
                         this, SLOT(printFtpFileList(const QUrlInfo&)));
//        QString tmp = dir + "/log";
        m_ftp->cd(dir);
        m_ftp->list();
    });
}

void FtpClient::handleFileData(qint64 readBytes, qint64 totalBytes)
{
    auto percent = (qreal)readBytes/totalBytes;
    if(percent == 1){
        qDebug() << m_file->fileName() << "download finished!";
        if(m_file->isOpen()){
            qDebug() << "hi";
            m_file->close();
            fileStream.flush();
            closeClient();
        }
        emit finishDownload();
    }
}

void FtpClient::readData()
{
//    qDebug() << "received data from thread : " << QThread::currentThreadId();
    fileStream << m_ftp->readAll();
}

void FtpClient::commandStarted(int id)
{
    QFtp::Command command = m_ftp->currentCommand();
//    qDebug() << "commandStarted " << id;
    switch (command) {
    case QFtp::List: {  // 正在执行 list() - 列出目录下的文件
//        qDebug() << is;
        if(is){
            qInfo() << "正在读取文件列表...";
            // 连接信号与槽
            is = false;
        }
        break;
    }
    case QFtp::Get: {
        qInfo() << "正在下载...当前线程 : " << QThread::currentThreadId();
    }
    default:{
        qDebug() << "当前命令是第 : " << id << "个"
                 << ", current command : " << QMetaEnum::fromType<FtpController::Command>().valueToKey(m_ftp->currentCommand())
                 << ", current thread : " << QThread::currentThreadId();
        break;
    }
    }

}

void FtpClient::setHostPort(const QString &host, quint16 port)
{
    m_ftpHost = host;
    m_ftpPort = port;
//    m_pUrl.setHost(host);
//    m_pUrl.setPort(port);
}

void FtpClient::setUserInfo(const QString &userName, const QString &password)
{
    m_username = userName;
    m_password = password;
//    m_pUrl.setUserName(userName);
//    m_pUrl.setPassword(password);
}

void FtpClient::startFtpConnection()
{
    m_ftp = new QFtp(this);
    qDebug() << "user name: " << m_username
             << ", password: " << m_password
             << ", host: " << m_ftpHost
             << ", port: " << m_ftpPort
             << ", current Thread: " << QThread::currentThreadId();

    m_ftp->connectToHost(m_ftpHost, m_ftpPort);
    m_ftp->login(m_username, m_password);

    // 连接信号与槽
    QObject::connect(m_ftp, SIGNAL(stateChanged(int)),
                     this, SIGNAL(stateChanged(int)));
    QObject::connect(m_ftp, SIGNAL(commandStarted(int)),
                     this, SLOT(commandStarted(int)));
    QObject::connect(m_ftp, SIGNAL(commandFinished(int,bool)),
                     this, SIGNAL(commandFinished(int,bool)));
}

void FtpClient::get(const QString &server_path, const QString &server_file, const QString local_savepath)
{
    QObject::connect(m_ftp, SIGNAL(dataTransferProgress(qint64,qint64)),
                     this,SLOT(handleFileData(qint64,qint64)));
    QObject::connect(m_ftp, SIGNAL(readyRead()),
                     this, SLOT(readData()));
    QString path = ClientManager::getInstance()->createSavePath(local_savepath, server_file);
    m_file = new QFile(path);
    m_file->open(QIODevice::WriteOnly |
                 QIODevice::Append |
                 QFile::Text);
    fileStream.setDevice(m_file);

    // correct example:
    // server path:  "MSETP-StudentSuite_2022_03_10_00.txt" ,
    // local save path:  "C:/Git/build-msetp-managerkits-Desktop_Qt_5_15_0_MinGW_64_bit-Debug/debug/Seat/1/MSETP-StudentSuite_2022_03_10_00.txt"
    qInfo() << "\n"
            << "          server path : " << server_path << "\n"
            << "      local save path : " << m_file->fileName() << "\n"
            << "       current thread : " << QThread::currentThreadId()
            << "  client socket state : " << QMetaEnum::fromType<FtpController::State>().valueToKey(m_ftp->state());
    m_ftp->cd(server_path);
    m_ftp->get(server_file);
}

void FtpClient::printFtpFileList(const QUrlInfo &urlInfo)
{
    if(!is) is = true;
    // 获取文件类型
    QMimeDatabase db;
    QMimeType type = db.mimeTypeForFile(urlInfo.name());
    //"ftpFileName" : out[0],
    //"ftpFileLastModified" : out[1],
    //"ftpFileType" : out[2],
    //"ftpFileSize" : out[3]
    QString out = QString("%1  %2  %3  %4\n").arg(urlInfo.name()).arg(urlInfo.lastModified().toString("MMM dd yyyy"))
                .arg(urlInfo.isDir() ? "文件夹" : type.filterString()).arg(getSize(urlInfo.size()));
    qDebug() << out;
    emit fileRecv(out);
}






/*-----------------------------------------------------------------------------|
 |                                 FtpThread                                   |
 |----------------------------------------------------------------------------*/


FtpThread::FtpThread()
{

}

FtpThread::~FtpThread()
{
    qDebug() << "结束FtpThread";
}

void FtpThread::initThread(const QList<QVariant> list)
{
    QTimer *timer = new QTimer;
    timer->setInterval(1000);
    // list组成
    //  0 - (服务器ip)   1 - (服务器端口)  2 - (Ftp登录用户名)  3 - (Ftp登录密码)
    //  4 - (本地保存路径)  5 - (服务器日志文件夹路径)
    info = list;
    client = new FtpClient(this);
    client->init();
    client->setHostPort(list.at(0).toString(),list.at(1).toUInt());
    client->setUserInfo(list.at(2).toString(), list.at(3).toString());
    client->startFtpConnection();

    QObject::connect(client, &FtpClient::commandFinished, this, [=](const int &state){
        QMutexLocker locker(&mutex);
        if(client && client->currentCmd() == QFtp::Login){
            qDebug() << "当前线程 : " << QThread::currentThreadId()
                     << QMetaEnum::fromType<FtpController::State>().valueToKey(state) << "登陆成功";
            client->listFileInfo();
        }
    });

    QObject::connect(client, &FtpClient::findRes, this, [=](const QString &dir){
        QMutexLocker locker(&mutex);
        // list组成
        //  0 - (服务器ip)   1 - (服务器端口)  2 - (Ftp登录用户名)  3 - (Ftp登录密码)
        //  4 - (本地保存路径)
        ///  5 - (服务器日志文件夹路径)
        info.append(dir);
        client->handleFoundDir(dir);
    });

    QObject::connect(client, &FtpClient::fileRecv, this, [=](const QString &file){
        // 保存读取到的文件名列表
        fileList.append(file);
        emit recvFile();
    });

    QObject::connect(this, SIGNAL(recvFile()), timer, SLOT(start()), Qt::DirectConnection);
    QObject::connect(timer, &QTimer::timeout, this, [=](){
        timer->stop();
        timer->deleteLater();
        FtpThread::download();
    });
}

void FtpThread::download()
{
    QMutexLocker locker(&mutex);
    if (client->currentState() == QFtp::LoggedIn){
        //          server_path              server_filename      local_savepath
        qDebug() << info.at(5).toString() << fileList.first() << info.at(4).toString();
        foreach(QString server_filename, fileList){
            QString file = server_filename.split("  ").at(0);
            FtpClient *worker = new FtpClient;
            worker->init();
            worker->setHostPort(info.at(0).toString(),info.at(1).toUInt());
            worker->setUserInfo(info.at(2).toString(), info.at(3).toString());
            worker->startFtpConnection();
            worker->get(info.at(5).toString(), file, info.at(4).toString());
        }
    }
}
