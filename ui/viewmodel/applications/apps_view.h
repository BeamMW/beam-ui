#pragma once

namespace beamui::applications {
    class AppsViewModel : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString devAppURL  READ getDevAppURL CONSTANT)
        Q_PROPERTY(QString devAppName READ getDevAppName CONSTANT)

    public:
        AppsViewModel();
        ~AppsViewModel();

        QString getDevAppURL() const;
        QString getDevAppName() const;

    public:
        Q_INVOKABLE void onCompleted(QObject *webView);
    };
}
