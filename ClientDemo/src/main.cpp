#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QtCore/QMap>
#include <QDebug>
#include <QLoggingCategory>
#include <QQmlContext>

#include "mqtt.h"
#include "log.h"
#include "tcpClient.h"
#include "clientManager.h"
#include "listElements.h"
#include "mymodels.h"
#include "modelmanager.h"
#include "ftpClient.h"
#include "ftpcontroller.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);

//    QLoggingCategory::setFilterRules(QStringLiteral("qt.scenegraph.general=true"));
//    qSetMessagePattern("%{category}: %{message}");

    // 设置组织名称和应用名称
    app.setOrganizationName("TcpClient");
    app.setApplicationName("ClientDEMO");
    app.setQuitOnLastWindowClosed(true);

    //启动日志记录
    Log::getInstance()->setDir(QCoreApplication::applicationDirPath().replace("\\", "/") + "/log");
    // 创建客户端与ini文件
    QSettings settings;
    settings.setDefaultFormat(QSettings::IniFormat);
    qInfo() << settings.fileName();

    Log::getInstance()->start();

    qDebug() << "********************************************";
    qDebug() << "*********** Running Client Demo ************";
    qDebug() << "********************************************";

    QQmlApplicationEngine engine;

    //注册 QML 插件
    qmlRegisterSingletonType<Mqtt>("Mqtt", 1, 1, "Mqtt", &Mqtt::qmlInstance);
    qmlRegisterSingletonType<TcpClient>("TcpClient", 1, 1,"TcpClient", &TcpClient::qmlInstance);
    qmlRegisterSingletonType<ClientManager>("ClientManager", 1, 1, "ClientManager", &ClientManager::qmlInstance);
    qmlRegisterSingletonType<FtpController>("FtpController", 1, 1,"FtpController", &FtpController::qmlInstance);

    qInfo() << "主线程Thread ID: " << QThread::currentThreadId();
    if(settings.value("schoolPlatformIP") == QVariant() || settings.value("schoolPlatformPort") == QVariant()){
        quint16 port = 9527;
        settings.setValue("schoolPlatformIP", "192.168.6.61");
        settings.setValue("schoolPlatformPort", port);
        qInfo() << "初始化成功！当前校级平台地址 : " << settings.value("schoolPlatformIP")
                << ", 当前校级平台端口 : " << settings.value("schoolPlatformPort");
    }

    QQmlContext *content=engine.rootContext();

    ModelManager* mqttModelManager = new ModelManager(ObjectModel::Mqtt, &app);
    ModelManager* ftpModelManager = new ModelManager(ObjectModel::Ftp, &app);
    TableModel* infoModel = new TableModel(&app);

    // 初始化model
    mqttModelManager->initMqttModel(24);
    ftpModelManager->initFtpModel();

    Mqtt::getInstance()->init();
    ClientManager::getInstance()->init();
    FtpController::getInstance()->init();

    content->setContextProperty("MqttModel", mqttModelManager->getModel());
    content->setContextProperty("FtpModel", ftpModelManager->getModel());
    content->setContextProperty("InfoModel", infoModel);

    const QUrl url(QStringLiteral("qrc:/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl){
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
