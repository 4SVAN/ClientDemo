#include "mqtt.h"
#include <QJsonDocument>
#include <QDebug>
#include <QEventLoop>
#include <QCoreApplication>

Mqtt::Mqtt(QObject *parent) : QMqttClient(parent)
{

}

Mqtt::~Mqtt(){
    qDebug() << "delete Mqtt";
}

qint32 Mqtt::publish(const QMqttTopicName &topic, const QByteArray &message, quint8 qos, bool retain)
{
    int attempts = 0;
    while (state() != Connected && attempts < 3) {
        attempts++;
        QEventLoop eventloop;
        QTimer::singleShot(attempts * 5, &eventloop, &QEventLoop::quit);
        eventloop.exec();
    }
    if (state() == Connected) {
        qInfo() << "发布主题:" << "topic=" << topic.name()
                              << "qos=" << qos
                              << "retain=" << retain
                              << "message=" << QString::fromUtf8(message);
        return QMqttClient::publish(topic, message, qos, retain);
    } else {
        return 0;
    }
}

void Mqtt::handelConnected()
{
    qInfo() << "连接成功:" << "hostname=" << hostname()
                          << "port=" << port();
    unsubscribe(QMqttTopicFilter("topic/#"));
    subscribe(QMqttTopicFilter("topic/" + getHostIP() + "/student/heartBeat"));
}

// 初始化MQTT连接
void Mqtt::init()
{
    // 座位号默认为0
    setSeatNumber(0);
    // 初始化学生端ip列表
    for(int i=0;i<24;i++){
        m_seatHostnameList.append(" ");
    }
    qDebug() << "初始化列表个数：" << m_seatHostnameList.size();

    // 关联信号槽
    QObject::disconnect(this, &QMqttClient::connected, this, &Mqtt::handelConnected);
    QObject::disconnect(this, &QMqttClient::errorChanged, this, &Mqtt::handelError);
    QObject::disconnect(this, &QMqttClient::messageReceived, this, &Mqtt::handelMessage);

    QObject::connect(this, &QMqttClient::connected, this, &Mqtt::handelConnected);
    QObject::connect(this, &QMqttClient::errorChanged, this, &Mqtt::handelError);
    QObject::connect(this, &QMqttClient::messageReceived, this, &Mqtt::handelMessage);
}

QMqttSubscription *Mqtt::subscribe(const QMqttTopicFilter &topic, quint8 qos)
{
    qInfo() << "订阅主题:" << "topic=" << topic.filter()
                          << "qos=" << qos;
    return QMqttClient::subscribe(topic, qos);
}

void Mqtt::unsubscribe(const QMqttTopicFilter &topic)
{
    qInfo() << "取消订阅:" << "topic=" << topic.filter();
    QMqttClient::unsubscribe(topic);
}

// 处理信息
void Mqtt::handelMessage(const QByteArray &message, const QMqttTopicName &topic)
{
    QSettings settings;
    qInfo() << "接收消息:" << "topic=" << topic.name()
                          << "message=" << QString(message);

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(message, &error);
    if (!message.isEmpty() && jsonDocument.isNull()) {
        qWarning() << "mqtt 接收消息为空";
        return;
    }
    QJsonObject jsonObject = jsonDocument.object();
    QString topicName = topic.name();

    // 接收心跳包，读取学生端IP地址
    if(topicName.contains("heartBeat")){
        QString studentSuiteIP = jsonObject.value("studentIP").toString();  // 获取心跳包中发送来的学生端ip
        int seatNumber = jsonObject.value("seatNumber").toInt();    // 获取心跳包中发送来的座位号

        // 更新页面MQTT指示器状态（若接收到的座位号超出限制(0~23)，则不进行更新并警告
        if(seatNumber<24){
            emit seatStateOn(seatNumber);
            m_seatHostnameList[seatNumber] = studentSuiteIP;    // 保存送来的学生端ip
            qDebug() << "心跳包座位号：" << seatNumber
                     << " 当前选择的座位号：" << m_seatNumber;
            emit heartbeatPackageReceived(jsonObject);  // 发送心跳包内容到ClientManager类进行相关处理
        }
        else{
            qWarning() << "接收到的座位号："<< seatNumber
                       <<" 超出限制(0~23)!";
        }
    }
}

void Mqtt::handelError(QMqttClient::ClientError error)
{
    //在ClientDemo中，利用按钮来进行重连（因为需要手动更改IP）
    qWarning() << "发生错误:" << error;
    emit errorOccured(QMetaEnum::fromType<QMqttClient::ClientError>().valueToKey(error));

}

QString Mqtt::byteArrayToQString(QByteArray ba) {return QString::fromUtf8(ba);}

Mqtt *Mqtt::getInstance()
{
    static Mqtt mqtt;
    return &mqtt;
}

QObject *Mqtt::qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
{
    Q_UNUSED(engine);
    Q_UNUSED(scriptEngine);
    return Mqtt::getInstance();
}

QString Mqtt::getHostIP()
{
    // 获取本机IP地址
    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost)){
            qDebug() << "本机IP地址：" << address.toString();
            m_hostIP = address.toString();
            return m_hostIP;
        }
    }
    qDebug() << "返回回环地址";
    return "127.0.0.1";
}

void Mqtt::setSeatHostname(const int &seat, QString hostname)
{
    m_seatHostnameList[seat] = hostname;
    qDebug() << "已更改ip为：" << m_seatHostnameList.at(seat);
}

void Mqtt::setStudentSuiteIP(const QString &ip)
{
    m_studentSuiteIP = ip;
    emit changeStudentSuiteIP(m_studentSuiteIP);
    qDebug() << "设置新的学生端IP：" << m_studentSuiteIP;
}

QString Mqtt::getSeatHostname(const int &seat)
{
    return m_seatHostnameList.at(seat);
}

QList<QString> Mqtt::getSeatHostnameList()
{
    return m_seatHostnameList;
}
