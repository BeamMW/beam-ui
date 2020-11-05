#pragma once

namespace beamui::applications {
    class AppsViewModel : public QObject
    {
    Q_OBJECT
    public:
        AppsViewModel();
        ~AppsViewModel();

    public:
        Q_INVOKABLE void onCompleted(QObject *webView);
    };
}
