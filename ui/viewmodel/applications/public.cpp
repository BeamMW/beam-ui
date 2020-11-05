
#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable: 4127)
#endif
#include <QQmlEngine>
#if defined(_MSC_VER)
#pragma warning (pop)
#endif

#include "public.h"
#include "apps_view.h"

#if defined(_MSC_VER)
#pragma warning (push)
#pragma warning (disable: 4251)
#endif
#include "webapi_beam.h"
#if defined(_MSC_VER)
#pragma warning (pop)
#endif



namespace beamui::applications
{
    void RegisterQMLTypes()
    {
        qmlRegisterType<AppsViewModel>("Beam.Wallet", 1, 0, "ApplicationsViewModel");
        qmlRegisterType<WebAPI_Beam>("Beam.Wallet", 1, 0, "WebAPIBeam");
    }
}
