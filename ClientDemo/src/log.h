#ifndef LOG_H
#define LOG_H

#include <QObject>
#include <QTextStream>
#include <QFile>
#include <QDate>
#include <QMutex>
#include <QTcpServer>
#include <QTcpSocket>
#include <QThread>

#define TB (1024 * 1024 * 1024 * 1024LL)
#define GB (1024 * 1024 * 1024LL)
#define MB (1024 * 1024L)
#define KB (1024)

class Log : public QObject
{
    Q_OBJECT

public:
    static Log *getInstance();

signals:
    void send(const QString &content);

public slots:
    void start();                                       //启动日志服务
    void stop();                                        //暂停日志服务
    void setDir(const QString &dir);                    //设置日志文件存放路径
    void save(QtMsgType type, const QString &content);  //保存日志

private:
    explicit Log(QObject *parent = nullptr);
    ~Log();

private:
    QMutex mutex;           //文件互斥锁
    QFile file;             //文件对象
    QString dir;            //日志文件目录
    QString path;           //日志文件路径
    QDate date;             //日志日期
    int size;               //日志文件大小
    QTextStream logStream;  //文件流

    QThread logThread;
};

#endif // LOG_H
