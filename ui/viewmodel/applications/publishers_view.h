// Copyright 2018 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
#pragma once

#include "apps_view.h"
#include "../ui/viewmodel/ui_helpers.h"

#include <QAbstractTableModel>
#include <QList>
#include <QObject>
#include <QString>

#include <memory>

namespace beamui::applications
{
    class PublisherItem : public QObject
    {
        Q_OBJECT
        Q_PROPERTY(QString publisherKey READ publisherKey NOTIFY publisherKeyChanged)
        Q_PROPERTY(QString nickname READ nickname WRITE nickname NOTIFY nicknameChanged)
        Q_PROPERTY(QString shortTitle READ shortTitle WRITE shortTitle NOTIFY shortTitleChanged)
        Q_PROPERTY(QString aboutMe READ aboutMe WRITE aboutMe NOTIFY aboutMeChanged)
        Q_PROPERTY(QString website READ website WRITE website NOTIFY websiteChanged)
        Q_PROPERTY(QString twitter READ twitter WRITE twitter NOTIFY twitterChanged)
        Q_PROPERTY(QString linkedin READ linkedin WRITE linkedin NOTIFY linkedinChanged)
        Q_PROPERTY(QString instagram READ instagram WRITE instagram NOTIFY instagramChanged)
        Q_PROPERTY(QString telegram READ telegram WRITE telegram NOTIFY telegramChanged)
        Q_PROPERTY(QString discord READ discord WRITE discord NOTIFY discordChanged)

    public:
        using Ptr = std::shared_ptr<PublisherItem>;

        [[nodiscard]] QString publisherKey() const;
        void publisherKey(const QString& value);
        [[nodiscard]] QString nickname() const;
        void nickname(const QString& name);
        [[nodiscard]] QString shortTitle() const;
        void shortTitle(const QString& value);
        [[nodiscard]] QString aboutMe() const;
        void aboutMe(const QString& value);
        [[nodiscard]] QString website() const;
        void website(const QString& value);
        [[nodiscard]] QString twitter() const;
        void twitter(const QString& value);
        [[nodiscard]] QString linkedin() const;
        void linkedin(const QString& value);
        [[nodiscard]] QString instagram() const;
        void instagram(const QString& value);
        [[nodiscard]] QString telegram() const;
        void telegram(const QString& value);
        [[nodiscard]] QString discord() const;
        void discord(const QString& value);

    signals:
        void appsChanged();
        void isPublisherChanged();
        void publisherKeyChanged();
        void nicknameChanged();
        void shortTitleChanged();
        void aboutMeChanged();
        void websiteChanged();
        void twitterChanged();
        void linkedinChanged();
        void instagramChanged();
        void telegramChanged();
        void discordChanged();

    private:
        QString _publisherKey;
        QString _nickname;
        QString _shortTitle;
        QString _aboutMe;
        QString _website;
        QString _twitter;
        QString _linkedin;
        QString _instagram;
        QString _telegram;
        QString _discord;
    };

    class PublishersViewModel: public QAbstractTableModel
    {
        Q_OBJECT
        Q_PROPERTY(QString nicknameRole         READ getNicknameRole        CONSTANT)
        Q_PROPERTY(QString shortTitleRole       READ getShortTitleRole      CONSTANT)
        Q_PROPERTY(QString aboutRole            READ getAboutRole           CONSTANT)
        Q_PROPERTY(QString websiteRole          READ getWebsiteRole         CONSTANT)
        Q_PROPERTY(QString twitterRole          READ getTwitterRole         CONSTANT)
        Q_PROPERTY(QString linkedinRole         READ getLinkedinRole        CONSTANT)
        Q_PROPERTY(QString instagramRole        READ getInstagramRole       CONSTANT)
        Q_PROPERTY(QString telegramRole         READ getTelegramRole        CONSTANT)
        Q_PROPERTY(QString discordRole          READ getDiscordRole         CONSTANT)
        Q_PROPERTY(QString publisherLinkRole    READ getPublisherLinkRole   CONSTANT)
        
    public:
        explicit PublishersViewModel(QObject *parent = nullptr);

        // Basic functionality:
        int rowCount(const QModelIndex &parent = QModelIndex()) const override;
        int columnCount(const QModelIndex &parent = QModelIndex()) const override;
        QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
        QHash<int, QByteArray> roleNames() const override;

        enum Roles {
            NicknameRole = Qt::UserRole,
            ShortTitleRole,
            AboutRole,
            WebsiteRole,
            TwitterRole,
            LinkedinRole,
            InstagramRole,
            TelegramRole,
            DiscordRole,
            PublisherLinkRole
        };
        Q_ENUM(Roles)

        [[nodiscard]] QString getNicknameRole() const;
        [[nodiscard]] QString getShortTitleRole() const;
        [[nodiscard]] QString getAboutRole() const;
        [[nodiscard]] QString getWebsiteRole() const;
        [[nodiscard]] QString getTwitterRole() const;
        [[nodiscard]] QString getLinkedinRole() const;
        [[nodiscard]] QString getInstagramRole() const;
        [[nodiscard]] QString getTelegramRole() const;
        [[nodiscard]] QString getDiscordRole() const;
        [[nodiscard]] QString getPublisherLinkRole() const;

        Q_INVOKABLE QVariant getRoleValue(const int row, QByteArray roleName)
        {
            QHashIterator<int, QByteArray> it(roleNames());
            while (it.hasNext())
            {
                it.next();
                if (roleName == it.value())
                   return data(index(row, 0), it.key());
            }
            return QVariant();
        }

    private:
        QList<PublisherItem::Ptr> publishersInfo;
    };
}
