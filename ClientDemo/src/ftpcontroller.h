#ifndef FTPCONTROLLER_H
#define FTPCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QQmlEngine>

#include "ftpClient.h"

class FtpController : public QObject
{
    Q_OBJECT
public:
    /// @brief ftp服务器连接状态
    enum State {
        Unconnected,
        HostLookup,
        Connecting,
        Connected,
        LoggedIn,
        Closing
    };
    Q_ENUM(State)

    /// @brief ftp服务器当前执行的命令
    enum Command {
        None,
        SetTransferMode,
        SetProxy,
        ConnectToHost,
        Login,
        Close,
        List,
        Cd,
        Get,
        Put,
        Remove,
        Mkdir,
        Rmdir,
        Rename,
        RawCommand
    };
    Q_ENUM(Command)

    void init();
    static FtpController *getInstance();
    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine);

    Q_INVOKABLE void setUserInfo(const QString &userName, const QString &password, quint16 port = 2121);

    Q_INVOKABLE void listFileInfo(const int &seat);

public slots:
    /// @brief 单服务器批量下载
    void downloadFile(const int &seat, const QString &server_path, const QString local_file);
    /// @brief 多服务器下载最新文件
    void batchDownloadFile(const int &seat, const QString &local_file);

private slots:
    /// @brief 打印当前的ftp状态
    void ftpStateChanged(int state);
    /// @brief 处理接收到的文件信息
    void handleRecvFile(const QString &file);
    /// @brief 处理接收到的文件信息
    void handleData(const QString &file);

Q_SIGNALS:
    void startDownload(const QList<QVariant> list);
    void download(const int &seat, const QString &server_file, const QString local_file);
    void fileRecv(const QString &file);
    void stateChanged(const FtpController::State state);
    void batchDownload(const int &seat, const QString &local_file);

private:
    explicit FtpController(QObject *parent = nullptr);
    ~FtpController();

    /// 保存所有学生机的ip
    QStringList m_ftpHostList;
    /// ftp端口(默认2121)
    quint16 m_ftpPort=2121;
    /// ftp登录用户名
    QString m_username;
    /// ftp登录密码
    QString m_password;
    /// 当前学生机日志保存路径(绝对路径)
    QString logDir;
    /// 当前学生机日志文件名的集合
    QStringList fileList;

    FtpClient *client;
    QTimer timer;           // 计时器——列举文件
};

#endif // FTPCONTROLLER_H
