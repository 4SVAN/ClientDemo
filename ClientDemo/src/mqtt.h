#ifndef MQTT_H
#define MQTT_H

#if _MSC_VER >= 1600
#pragma execution_character_set("utf-8")
#endif

#include <QSettings>
#include <QObject>
#include <QtMqtt>
#include <QQmlEngine>
#include <QStack>

#include "singleton.h"

class Mqtt : public QMqttClient
{
    Q_OBJECT

//    Q_PROPERTY(QString studentSuiteIP READ studentSuiteIP WRITE setStudentSuiteIP NOTIFY studentSuiteIPChanged) //学生端IP
    Q_PROPERTY(quint16 studentSuitePort READ studentSuitePort WRITE setStudentSuitePort NOTIFY studentSuitePortChanged) //学生端端口
    Q_PROPERTY(QString hostIP READ hostIP WRITE setHostIP NOTIFY hostIPChanged) //本机IP
    Q_PROPERTY(QString schoolPlatformIP READ schoolPlatformIP WRITE setSchoolPlatformIP NOTIFY schoolPlatformIPChanged) //平台端IP
    Q_PROPERTY(quint16 schoolPlatformPort READ schoolPlatformPort WRITE setSchoolPlatformPort NOTIFY schoolPlatformPortChanged) //平台端端口
    Q_PROPERTY(int seatNumber READ seatNumber WRITE setSeatNumber NOTIFY seatNumberChanged)

public:
    Q_INVOKABLE void init();
    qint32 publish(const QMqttTopicName &topic, const QByteArray &message = QByteArray(), quint8 qos = 2, bool retain = false);
    QString byteArrayToQString(QByteArray message);
    static Mqtt *getInstance();
    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE quint16 studentSuitePort() {return m_studentSuitePort;}
    Q_INVOKABLE QString hostIP() {return m_hostIP;}
    Q_INVOKABLE QString schoolPlatformIP() {return m_schoolPlatformIP;}
    Q_INVOKABLE quint16 schoolPlatformPort() {return m_schoolPlatformPort;}
    Q_INVOKABLE int seatNumber() {return m_seatNumber;}

    Q_INVOKABLE void setStudentSuitePort(const quint16 &port) {m_studentSuitePort = port; emit studentSuitePortChanged();}
    Q_INVOKABLE void setHostIP(const QString &ip) {m_hostIP = ip; emit hostIPChanged();}
    Q_INVOKABLE void setSchoolPlatformIP(const QString &ip)
    {
        m_schoolPlatformIP = ip;
        setHostname(m_schoolPlatformIP);
        emit schoolPlatformIPChanged();
        qDebug() << "校级平台IP地址已更新：" << hostname();
    }
    Q_INVOKABLE void setSchoolPlatformPort(quint16 inputPort)
    {
        m_schoolPlatformPort = inputPort;
        setPort(m_schoolPlatformPort);
        emit schoolPlatformPortChanged();
        qDebug() << "校级平台端口已更新：" << port();
    }
    Q_INVOKABLE void setSeatNumber(int number) {m_seatNumber = number; emit seatNumberChanged(m_seatNumber);}

public Q_SLOTS:
    void handelConnected();
    QString getSeatHostname(const int &seat);
    QList<QString> getSeatHostnameList();
    QString getHostIP();

    void setSeatHostname(const int &seat, QString hostname = "127.0.0.1");

Q_SIGNALS:
    void changeStudentSuiteIP(const QString &ip);
    void studentSuitePortChanged();
    void hostIPChanged();
    void schoolPlatformIPChanged();
    void schoolPlatformPortChanged();
    void seatNumberChanged(const int &seatNumber);
    void seatStateOn(const int &seatNumber);
    void mqttStateChanged(const QList<bool> &list);
    void heartbeatPackageReceived(const QJsonObject &jsonObject);
    void errorOccured(const QString &errorMsg);

private Q_SLOTS:
    void handelMessage(const QByteArray &message, const QMqttTopicName &topic);
    void handelError(QMqttClient::ClientError error);
    QMqttSubscription *subscribe(const QMqttTopicFilter &topic, quint8 qos = 2);
    void unsubscribe(const QMqttTopicFilter &topic);
    void setStudentSuiteIP(const QString &ip);

private:
    explicit Mqtt(QObject *parent = nullptr);
    ~Mqtt();

    QString m_studentSuiteIP;   // 服务器（学生端）ip
    quint16 m_studentSuitePort; // 服务器（学生端）端口
    QString m_hostIP;   // 客户端（本机）ip
    QString m_schoolPlatformIP; // MQTT协议通信ip（平台端）
    quint16 m_schoolPlatformPort;   // MQTT协议通信端口（平台端）

    int m_seatNumber;   // 当前选择的座位号
    QStringList m_seatHostnameList;  // 学生端ip列表
    QList<bool> m_seatState;    // 学生端连接状态

    QTimer timer;
    QThread mqttThread;
};

#endif // MQTT_H
