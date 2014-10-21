#pragma once
#include <bb/system/SystemDialog>

class Utils: public QObject
{
    Q_OBJECT
public:
    Utils();
    void writeDisplayFile(QString type, QString writeText);
};
