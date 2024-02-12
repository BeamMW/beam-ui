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
#include <QVariant>

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
        Q_PROPERTY(bool    enabled      READ enabled      CONSTANT)

public:
    PublisherItem() = default;
    PublisherItem(const QVariantMap& publisherInfo);

    [[nodiscard]] const QString& publisherKey() const;
    [[nodiscard]] const QString& nickname() const;
    [[nodiscard]] const QString& shortTitle() const;
    [[nodiscard]] const QString& aboutMe() const;
    [[nodiscard]] const QString& website() const;
    [[nodiscard]] const QString& twitter() const;
    [[nodiscard]] const QString& linkedin() const;
    [[nodiscard]] const QString& instagram() const;
    [[nodiscard]] const QString& telegram() const;
    [[nodiscard]] const QString& discord() const;
    [[nodiscard]] bool enabled() const;

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
    bool _enabled;
};

class PublishersViewModel: public QObject
{
    Q_OBJECT
    Q_PROPERTY(QList<QVariantMap> publishersInfo          READ getPublishersInfo      WRITE setPublishersInfo)

    Q_PROPERTY(const QList<QObject*>& publishers              READ getPublishers          NOTIFY publishersChanged)
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
    Q_PROPERTY(QString publisherStatusRole                READ getPublisherStatusRole CONSTANT)

    Q_PROPERTY(Qt::SortOrder sortOrder READ sortOrder  WRITE setSortOrder)
        
public:
    PublishersViewModel();
    virtual ~PublishersViewModel();

    const QObjectList& getPublishers();

    void setPublishersInfo(QList<QVariantMap> info);
    [[nodiscard]] const QList<QVariantMap>& getPublishersInfo() const;

    [[nodiscard]] const QString& getNicknameRole() const;
    [[nodiscard]] const QString& getShortTitleRole() const;
    [[nodiscard]] const QString& getAboutRole() const;
    [[nodiscard]] const QString& getWebsiteRole() const;
    [[nodiscard]] const QString& getTwitterRole() const;
    [[nodiscard]] const QString& getLinkedinRole() const;
    [[nodiscard]] const QString& getInstagramRole() const;
    [[nodiscard]] const QString& getTelegramRole() const;
    [[nodiscard]] const QString& getDiscordRole() const;
    [[nodiscard]] const QString& getPublisherLinkRole() const;
    [[nodiscard]] const QString& getPublisherStatusRole() const;

    [[nodiscard]] Qt::SortOrder sortOrder() const;
    void setSortOrder(Qt::SortOrder);

signals:
    void publishersChanged();

private:
    void updatePublishers();
    void sortPublishers();

private:
    QList<QVariantMap> m_publishersInfo;
    QList<QObject*> m_publishers;
    Qt::SortOrder m_sortOrder = Qt::AscendingOrder;
};
