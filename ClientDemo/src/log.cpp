#include "log.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QDateTime>
#include <QFileInfo>
#include <QGuiApplication>
#include <QMutexLocker>
#include <iostream>
#include <QThread>

#if defined(Q_OS_ANDROID)
#include <android/log.h>
#endif

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QString message = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss,zzz ");
    QStringList templist = QFileInfo(context.file).fileName().replace("/","\\").split("\\");
    QString fileName = templist[templist.count() - 1];

    switch (type) {
    case QtDebugMsg:    //调试信息提示
        message.append(QString("[Debg] %1:%2 %3 %4\n").arg(fileName).arg(context.line).arg(QString(context.function).section(QRegExp("[ (]"), 1, 1)).arg(msg));
        break;
    case QtInfoMsg:     //一般的运行信息
        message.append(QString("[Info] %1:%2 %3 %4\n").arg(fileName).arg(context.line).arg(QString(context.function).section(QRegExp("[ (]"), 1, 1)).arg(msg));
        break;
    case QtWarningMsg:  //一般的警告提示
        message.append(QString("[Warn] %1:%2 %3 %4\n").arg(fileName).arg(context.line).arg(QString(context.function).section(QRegExp("[ (]"), 1, 1)).arg(msg));
        break;
    case QtCriticalMsg: //严重错误提示
        message.append(QString("[Crit] %1:%2 %3 %4\n").arg(fileName).arg(context.line).arg(QString(context.function).section(QRegExp("[ (]"), 1, 1)).arg(msg));
        break;
    case QtFatalMsg:    //致命错误提示
        message.append(QString("[Fatl] %1:%2 %3 %4\n").arg(fileName).arg(context.line).arg(QString(context.function).section(QRegExp("[ (]"), 1, 1)).arg(msg));
    }
    Log::getInstance()->save(type, message);
}

Log::Log(QObject *parent) : QObject(parent)
{
    dir = qApp->applicationDirPath();
//    connect(this, SIGNAL(send(QString)), SendLog::Instance(), SLOT(send(QString)));
}

Log::~Log()
{
    stop();
    qDebug() << "~Log()";
}

Log *Log::getInstance()
{
    static Log instance;
    return &instance;
}

void Log::start()
{
    qInstallMessageHandler(myMessageOutput);

    // 如果已经存在当天的日志文件，则继续使用该文件
    QDir directory(dir);
    directory.setFilter(QDir::Files | QDir::NoDotAndDotDot);
    directory.setSorting(QDir::Name);
    QFileInfoList fileInfoList = directory.entryInfoList();
    QString strDate = QDate::currentDate().toString("yyyy_MM_dd");
    foreach (QFileInfo fileinfo, fileInfoList) {
        if (fileinfo.fileName().contains(strDate)) {
            file.setFileName(fileinfo.absoluteFilePath());
        }
    }

    // 如果存在则进行初始化参数
    if (file.exists()) {
        path = QFileInfo(file).absoluteFilePath();
        size = file.size();
        date = QDate::currentDate();
        file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        logStream.setDevice(&file);
    }
}

void Log::stop()
{
    qInstallMessageHandler(0);

    // 关闭日志文件
    if (file.isOpen()) {
        file.close();
    }
}

void Log::setDir(const QString &dir)
{
    this->dir = dir;
    QDir directory(dir);
    if (!directory.exists()) {
        qInfo() << "日志文件目录不存在";
        if (directory.mkpath(dir)) {
            qInfo() << "创建日志文件目录成功";
        } else {
            qWarning() << "创建日志文件目录失败";
        }
    } else {
        qInfo() << "日志文件已存在，继续使用";
    }
}

void Log::save(QtMsgType type, const QString &content)
{
    //加锁,防止多线程中qdebug太频繁导致崩溃
    QMutexLocker locker(&mutex);

    //方法改进:之前每次输出日志都打开文件,改成只有当日期改变时才新建和打开文件
    // 日期改变时创建新文件
    QDateTime dateTime = QDateTime::currentDateTime();
    if (this->date != dateTime.date()) {
        this->date = dateTime.date();
        size = 0;
        path = QString("%1/%2_%3_00.txt").arg(dir).arg(qApp->applicationName()).arg(dateTime.toString("yyyy_MM_dd"));
        if (file.isOpen())
            file.close();
        file.setFileName(path);
        file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        logStream.setDevice(&file);

        // 保留15天的日志文件
        QDir directory(dir);
        directory.setFilter(QDir::Files | QDir::NoDotAndDotDot);
        QMultiHash<QString, QString> multiHash;
        QFileInfoList fileInfoList = directory.entryInfoList();
        foreach (QFileInfo fileInfo, fileInfoList) {
            multiHash.insert(fileInfo.fileName().section(QRegExp("[_.]"), 1, 3), fileInfo.absoluteFilePath());
        }
        QStringList dateList = multiHash.keys();
        dateList.removeDuplicates();
        dateList.sort();
        while (dateList.size() > 15) {
            QStringList pathList = multiHash.values(dateList.at(0));
            foreach (QString path, pathList) {
                QFile::remove(path);
            }
            dateList.removeAt(0);
        }
    }

    // 超过限制大小时创建新文件
    size += content.size();
    if (size > 200 * MB) {
        size = content.size();
        int index = QFileInfo(file).fileName().section(QRegExp("[_.]"), 4, 4).toInt();
        path = QString("%1/%2_%3_%4.txt").arg(dir).arg(qApp->applicationName()).arg(date.toString("yyyy_MM_dd")).arg(index + 1, 2, 10, QLatin1Char('0'));
        if (file.isOpen())
            file.close();
        file.setFileName(path);
        file.open(QIODevice::WriteOnly | QIODevice::Append | QFile::Text);
        logStream.setDevice(&file);
    }

#if defined(Q_OS_ANDROID)
    switch (type) {
    case QtDebugMsg:
        __android_log_write(ANDROID_LOG_DEBUG, qApp->applicationName().toLocal8Bit().constData(), content.toLocal8Bit().constData());
        break;
    case QtInfoMsg:
        __android_log_write(ANDROID_LOG_INFO,  qApp->applicationName().toLocal8Bit().constData(), content.toLocal8Bit().constData());
        break;
    case QtWarningMsg:
        __android_log_write(ANDROID_LOG_WARN,  qApp->applicationName().toLocal8Bit().constData(), content.toLocal8Bit().constData());
        break;
    case QtCriticalMsg:
        __android_log_write(ANDROID_LOG_ERROR, qApp->applicationName().toLocal8Bit().constData(), content.toLocal8Bit().constData());
        break;
    case QtFatalMsg:
        __android_log_write(ANDROID_LOG_FATAL, qApp->applicationName().toLocal8Bit().constData(), content.toLocal8Bit().constData());
        break;
    default:
        break;
    }
#else
    if (type == QtDebugMsg || type == QtInfoMsg) {

        std::cout << content.toLocal8Bit().constData() << std::endl;
    } else {
        std::cerr << content.toLocal8Bit().constData() << std::endl;
    }
#endif
//    emit send(content);
    logStream << content << "\n";
    logStream.flush();

}






