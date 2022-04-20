#include <QGuiApplication>
#include "tcpClient.h"
#include "mqtt.h"

TcpClient::TcpClient(QObject *parent)
    : QTcpSocket(parent)
{
    qDebug() << "构建TcpClient";
}

QObject *TcpClient::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return TcpClient::getInstance();
}

TcpClient *TcpClient::getInstance()
{
    static TcpClient client;
    return &client;
}

// 初始化客户端，
void TcpClient::init()
{
    qDebug()<< "TCP连接初始化中";
    tcpSocket = new QTcpSocket(this);
    qDebug() << tcpSocket;

    QObject::connect(tcpSocket, &QTcpSocket::readyRead, this, [=](){
        // 读取发过来的数据
        QByteArray array = tcpSocket->readAll();
        // 同步到QML中
        emit messageChanged(QString::fromUtf8(array));
    });
    QObject::connect(this, &QTcpSocket::errorOccurred, this, &TcpClient::handleError);
    qDebug()<< "TCP连接初始化完成";
}

void TcpClient::handleError(const SocketError &error)
{
    qCritical() << "发生错误:" << error;
}

// 断开TCP连接
void TcpClient::handleDisconnect()
{
    // 若套接字为空指针（未实例化）则直接返回（结束）
    if(tcpSocket == nullptr)
        return;
    tcpSocket->disconnectFromHost();
    tcpSocket->close();

    //一秒后判断TCP状态，变为UnconnectedState则视为断开连接
    QTimer::singleShot(500,this,[=](){
        if(tcpSocket->state() == UnconnectedState){
            const QString msg = "已断开TCP通信";
            qDebug() << msg;
            emit tcpDisconnect(msg);
            delete tcpSocket;
            tcpSocket = nullptr;
        }
    });
}

// 建立TCP连接
void TcpClient::handleTcpConnect(const QString &ip)
{
    if (!tcpSocket){
        qCritical() << "Socket指针不存在";
        return;
    }
    qDebug() << tcpSocket;
    serverIP = ip;
    qDebug() << "设置服务器ip" << serverIP;

    //  先断开之前的连接，再重新建立连接
    tcpSocket->disconnectFromHost();
    tcpSocket->connectToHost(serverIP,4400);    //  默认TCP通信端口4400

    //一秒后判断TCP状态，变为UnconnectedState则视为断开连接
    QTimer::singleShot(500,this,[=](){
        if(tcpSocket->state() == ConnectedState){
            qDebug() << "TCP连接成功！";
        }
    });
}

// 返回当前TCP连接状态
QString TcpClient::clientState()
{
//    qDebug() << "State: " << tcpSocket->state();
    return QMetaEnum::fromType<QAbstractSocket::SocketState>().valueToKey(tcpSocket->state());
}

bool TcpClient::isSocketDeleted()
{
    if(!tcpSocket) return true;
    return false;
}





