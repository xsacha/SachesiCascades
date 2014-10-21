#include "utils.h"

Utils::Utils() : QObject()
{

}

void Utils::writeDisplayFile(QString type, QString writeText) {
    QString dir = "shared/misc/Sachesi/";
    QDir(dir).mkpath(".");
    QString name = dir + type + ".txt";
    QFile links(name);
    if (!links.open(QIODevice::WriteOnly)) {
        bb::system::SystemDialog* dialog = new bb::system::SystemDialog();
        dialog->setTitle(tr("Unable to display ") + type);
        dialog->setBody(tr("The permission 'Shared Files' is required to create and display the links. Please enable this permission, restart the application and try again."));
        dialog->exec();
        if (dialog->buttonSelection() == dialog->confirmButton()) {
            QDesktopServices::openUrl(QUrl("settings://permissions"));
        }
        dialog->deleteLater();
        return;
    }
    links.write(writeText.toUtf8());
    links.close();
    QDesktopServices::openUrl(QUrl::fromLocalFile(QString("/accounts/1000/") + name));
}
