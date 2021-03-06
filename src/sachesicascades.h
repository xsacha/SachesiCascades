#pragma once

#include <QObject>
#include "world.h"
#include "search/carrierinfo.h"
#include "search/search.h"
#include "search/scanner.h"

namespace bb
{
    namespace cascades
    {
        class LocaleHandler;
    }
}

class QTranslator;

/*!
 * @brief Application UI object
 *
 * Use this object to create and init app UI, to create context objects, to register the new meta types etc.
 */
class ApplicationUI : public QObject
{
    Q_OBJECT
public:
    ApplicationUI();
    virtual ~ApplicationUI() {}
    AppWorld world;
private slots:
    void onSystemLanguageChanged();
private:
    QTranslator* m_pTranslator;
    CarrierInfo carrier;
    Search search;
    Scanner scanner;
    bb::cascades::LocaleHandler* m_pLocaleHandler;
};
