#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QQmlEngine>
#include <QTimer>
#include <QThread>

//#include "singleton.h"

class TcpClient : public QTcpSocket
{
    Q_OBJECT

    Q_PROPERTY(QString message READ message WRITE setMessage NOTIFY messageChanged)
    Q_PROPERTY(QString studentIP READ studentIP WRITE setStudentIP NOTIFY studentIPChanged)

public:
    explicit TcpClient(QObject *parent = nullptr);
    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);
    static TcpClient *getInstance();

    Q_INVOKABLE QString studentIP() {return serverIP;}
    Q_INVOKABLE QString message() {return m_message;}

    Q_INVOKABLE void setStudentIP(const QString &newStudentIP) {serverIP = newStudentIP; emit studentIPChanged();}
    Q_INVOKABLE void setMessage(QString msg) {m_message = msg; emit messageChanged(m_message);}

    Q_INVOKABLE QString clientState();
    Q_INVOKABLE bool isSocketDeleted();


Q_SIGNALS:
    void serverIPChanged();
    void messageChanged(const QString &msg);
    void studentIPChanged();
    void tcpDisconnect(const QString &msg);

public slots:
    void init();
    void handleDisconnect();
    void handleTcpConnect(const QString &ip);

private slots:
    void handleError(const QAbstractSocket::SocketError &error);

private:
    QString serverIP = "";  // 服务器（目标）IP
    quint16 serverPort = 4400;  // 服务器端口（默认4400）
    QString m_message;  // TCP数据流
    QTimer timer;

    QTcpSocket *tcpSocket;  // 通信套接字
};



#endif // TCPCLIENT_H
