#ifndef FTPCLIENTH_H
#define FTPCLIENTH_H

#include <QObject>
#include <QQmlEngine>
#include <QUrl>
#include <QFileInfo>
#include <QDebug>
#include <QMutexLocker>
#include <QTextStream>
#include <QtFtp/qftp.h>

#include "modelmanager.h"
#include "clientManager.h"

#define LOGDIR "MSETP-StudentSuite/log"
//#define LOGDIR "cache/log"
#define TBYTE (1024 * 1024 * 1024 * 1024LL)
#define GBYTE (1024 * 1024 * 1024LL)
#define MBYTE (1024 * 1024LL)
#define KBYTE (1024LL)

Q_DECLARE_METATYPE(QUrlInfo)

class FtpClient : public QObject
{
    Q_OBJECT

public:
    explicit FtpClient(QObject *parent = nullptr);
    ~FtpClient();
    void init();
//    static FtpClient *getInstance();
//    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    /**
     * @brief 根据文件大小返回对应的单位(QString)
     * @param file_size
     * @return file_unit(QString)
     */
    QString getSize(const qint64 &file_size);
    /// @brief 获取登录信息
    /// @return 记录登录信息的QUrl变量
    QUrl getUrl() {return m_pUrl;}
    /// @brief 关闭m_ftp
    void closeClient();

public slots:
    /// @brief 设置地址和端口
    void setHostPort(const QString &host, quint16 port = 2121);
    /// @brief 设置登录 FTP 服务器的用户名和密码
    void setUserInfo(const QString &userName, const QString &password);
    /// @brief 建立FTP连接
    void startFtpConnection();
    /// @brief 下载文件(server_path为ftp服务器指定文件路径，local_file为保存到本地的路径)
    void get(const QString &server_path, const QString &server_file, const QString local_savepath);
    /// @brief 展示文件列表
    void listFileInfo();
    /// @brief 处理找到的文件夹
    void handleFoundDir(const QString &dir);
    /// @brief 返回当前ftp正在执行的命令
    QFtp::Command currentCmd();
    /// @brief 返回当前ftp连接状态
    QFtp::State currentState();

Q_SIGNALS:
//    void error(QNetworkReply::NetworkError);
//    // 上传进度
//    void uploadProgress(qint64 bytesSent, qint64 bytesTotal);
//    // 下载进度
//    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void fileRecv(const QString &file);
    void newAppend();
    void findRes(const QString &dir);
    void listFile(const int &command);
    void stateChanged(const int &state);
    void commandFinished(int id, bool error);
    void finishDownload();

private slots:
//    void readContent();

//    void replyError(QNetworkReply::NetworkError error);

//    void replyFinished(QNetworkReply *reply);
    /// @brief 打印文件列表
    void printFtpFileList(const QUrlInfo &urlInfo);
    /// @brief BFS搜索当前文件夹（根据qftp->list搜索策略制定）
    void bfsFolder(const QUrlInfo &urlInfo);
    /// @brief 完成当前搜索后进入下一层搜索（若bfs保存的路径不存在，list不消耗时间）
    void startNextSerach(int id, bool error);
    /// @brief 更新进度条与关闭文件
    void handleFileData(qint64 readBytes, qint64 totalBytes);
    /// @brief 读取数据到文件流
    void readData();
    /// @brief 核查正在执行的命令
    void commandStarted(int id);

private:
    void resetDir();

    /// ftp_ip
    QString m_ftpHost;
    /// ftp_port
    quint16 m_ftpPort=2121;
    /// ftp登录账号
    QString m_username;
    /// ftp登录密码
    QString m_password;
    /// 记录ftp连接信息
    QUrl m_pUrl;
    /// 本地保存文件
    QFile *m_file;
    /// 读取从ftp获取的数据流
    QTextStream fileStream;
    /// ftp服务器对应的学生端座位号
    int m_seatNumber;

    QFtp* m_ftp;
    /// ftp服务器文件树
    QVector<QStringList> dirList;

    bool is = true;     // 判断是否第一次调用bfsFolder()
    int it = 0;         // int类型迭代器，将读取的文件名保存到服务器文件树对应的下标
    int row = 15;       // ftp服务器文件树高度上限
    int count = 0;      // int类型迭代器，记录当前访问的层数
};

class FtpThread : public QObject
{
    Q_OBJECT
public:
    FtpThread();
    ~FtpThread();

public slots:
    /// @brief 初始化数据
    void initThread(const QList<QVariant> list);
    /// @brief 多服务器下载最新
    void download();

Q_SIGNALS:
    void recvFile();

private:
    FtpClient *client;
    /// info组成
    ///  0 - (服务器ip)   1 - (服务器端口)  2 - (Ftp登录用户名)  3 - (Ftp登录密码)
    ///  4 - (本地保存路径)
    ///  5 - (服务器日志文件夹路径)
    QList<QVariant> info;
    /// 服务器文件名列表
    QStringList fileList;

    QMutex mutex;
    QTimer timer;
};

#endif // FTPCLIENTH_H
