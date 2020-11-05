#include <QObject>
#include <QMessageBox>
#include <QtWebEngineWidgets/QWebEngineView>
#include "apps_view.h"
#include "utility/logger.h"

namespace beamui::applications {
    AppsViewModel::AppsViewModel()
    {
        LOG_INFO() << "AppsViewModel created";
    }

    AppsViewModel::~AppsViewModel()
    {
        LOG_INFO() << "AppsViewModel destroyed";
    }

    void AppsViewModel::onCompleted(QObject *webView)
    {
        assert(webView != nullptr);
        LOG_INFO() << "AppsViewModel::OnCompleted, webView is " << webView;
    }
}