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


class PublisherItem : public QObject
{
    Q_OBJECT
        Q_PROPERTY(QString publisherKey READ publisherKey CONSTANT)
        Q_PROPERTY(QString nickname     READ nickname     CONSTANT)
        Q_PROPERTY(QString shortTitle   READ shortTitle   CONSTANT)
        Q_PROPERTY(QString aboutMe      READ aboutMe      CONSTANT)
        Q_PROPERTY(QString website      READ website      CONSTANT)
        Q_PROPERTY(QString twitter      READ twitter      CONSTANT)
        Q_PROPERTY(QString linkedin     READ linkedin     CONSTANT)
        Q_PROPERTY(QString instagram    READ instagram    CONSTANT)
        Q_PROPERTY(QString telegram     READ telegram     CONSTANT)
        Q_PROPERTY(QString discord      READ discord      CONSTANT)

public:
    PublisherItem() = default;
    PublisherItem(const QVariantMap& publisherInfo);

    [[nodiscard]] QString publisherKey() const;
    [[nodiscard]] QString nickname() const;
    [[nodiscard]] QString shortTitle() const;
    [[nodiscard]] QString aboutMe() const;
    [[nodiscard]] QString website() const;
    [[nodiscard]] QString twitter() const;
    [[nodiscard]] QString linkedin() const;
    [[nodiscard]] QString instagram() const;
    [[nodiscard]] QString telegram() const;
    [[nodiscard]] QString discord() const;

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

class PublishersViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariantMap> publishersInfo          READ getPublishersInfo      WRITE setPublishersInfo)

    Q_PROPERTY(QQmlListProperty<PublisherItem> publishers READ getPublishers          NOTIFY publishersChanged)
    Q_PROPERTY(QString nicknameRole                       READ getNicknameRole        CONSTANT)
    Q_PROPERTY(QString shortTitleRole                     READ getShortTitleRole      CONSTANT)
    Q_PROPERTY(QString aboutRole                          READ getAboutRole           CONSTANT)
    Q_PROPERTY(QString websiteRole                        READ getWebsiteRole         CONSTANT)
    Q_PROPERTY(QString twitterRole                        READ getTwitterRole         CONSTANT)
    Q_PROPERTY(QString linkedinRole                       READ getLinkedinRole        CONSTANT)
    Q_PROPERTY(QString instagramRole                      READ getInstagramRole       CONSTANT)
    Q_PROPERTY(QString telegramRole                       READ getTelegramRole        CONSTANT)
    Q_PROPERTY(QString discordRole                        READ getDiscordRole         CONSTANT)
    Q_PROPERTY(QString publisherLinkRole                  READ getPublisherLinkRole   CONSTANT)

    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder  WRITE setSortOrder)
        
public:
    PublishersViewModel();
    virtual ~PublishersViewModel();

    QQmlListProperty<PublisherItem> getPublishers();

    void setPublishersInfo(QList<QVariantMap> info);
    [[nodiscard]] QList<QVariantMap> getPublishersInfo() const;

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

    [[nodiscard]] Qt::SortOrder sortOrder() const;
    void setSortOrder(Qt::SortOrder);

    Q_INVOKABLE QString getRoleValue(const int row, QByteArray roleName);

signals:
    void publishersChanged();

private:
    void updatePublishers();
    void sortPublishers();

private:
    QList<QVariantMap> m_publishersInfo;
    QList<PublisherItem*> m_publishers;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
};
