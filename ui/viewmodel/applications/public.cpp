#include <QQmlEngine>
#include "public.h"
#include "apps_view.h"
#include "webapi_beam.h"

namespace beamui::applications
{
    void RegisterQMLTypes()
    {
        qmlRegisterType<AppsViewModel>("Beam.Wallet", 1, 0, "ApplicationsViewModel");
        qmlRegisterType<WebAPI_Beam>("Beam.Wallet", 1, 0, "WebAPIBeam");
    }
}
