#include "sachesicascades.h"

#include <bb/cascades/Application>
#include <bb/cascades/QmlDocument>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/LocaleHandler>
#include <bb/cascades/QListDataModel>
#include <bb/system/InvokeManager>

using namespace bb::cascades;

#include <QLocale>
#include <QTranslator>

#include <Qt/qdeclarativedebug.h>
#include "WebImageView.h"
#include "codepicker.h"

using namespace bb::cascades;

Q_DECL_EXPORT int main(int argc, char **argv)
{
    Application app(argc, argv);

    ApplicationUI appui;
    bb::system::InvokeManager invokeManager;
    QObject::connect(&invokeManager, SIGNAL(invoked(const bb::system::InvokeRequest&)), &appui.world, SLOT(onInvoke(const bb::system::InvokeRequest&)));
    switch(invokeManager.startupMode()) {
        case bb::system::ApplicationStartupMode::InvokeApplication:
            // Wait for signal which will launch what we want
            break;
        case bb::system::ApplicationStartupMode::LaunchApplication:
        default:
            appui.world.showHome();
            break;
    }

    return Application::exec();
}


ApplicationUI::ApplicationUI() :
                                        QObject(), world()
{
    // prepare the localization
    m_pTranslator = new QTranslator(this);
    m_pLocaleHandler = new LocaleHandler(this);

    QObject::connect(m_pLocaleHandler, SIGNAL(systemLanguageChanged()), this, SLOT(onSystemLanguageChanged()));

    qmlRegisterType<CodePickerProvider>("custom.pickers", 1, 0, "CodePickerProvider");

    // initial load
    onSystemLanguageChanged();

    // Create scene document from main.qml asset, the parent is set
    // to ensure the document gets destroyed properly at shut down.
    qmlRegisterType<AppWorldApps>();
    qmlRegisterType<WebImageView>("org.labsquare", 1, 0, "WebImageView");
    QmlDocument *qml = QmlDocument::create("asset:///main.qml").parent(this);
    qml->setContextProperty("world", &world);
    qml->setContextProperty("carrier", &carrier);
    qml->setContextProperty("search", &search);

    // Create root object for the UI
    AbstractPane *root = qml->createRootObject<AbstractPane>();

    // Set created root object as the application scene
    Application::instance()->setScene(root);
}

void ApplicationUI::onSystemLanguageChanged()
{
    QCoreApplication::instance()->removeTranslator(m_pTranslator);
    // Initiate, load and install the application translation files.
    QString locale_string = QLocale::languageToString(QLocale().language());
    QString file_name = QString("SachesiCascades_%1").arg(locale_string);
    if ((QLocale().name() != "zh_HK") && m_pTranslator->load(file_name, "app/native/qm")) {
        QCoreApplication::instance()->installTranslator(m_pTranslator);
    }
}
