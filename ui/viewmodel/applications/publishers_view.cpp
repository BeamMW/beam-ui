#include "publishers_view.h"

namespace beamui::applications
{
    QString PublisherItem::publisherKey() const
    {
        return _publisherKey;
    }

    void PublisherItem::publisherKey(const QString& value)
    {
        if (value != _publisherKey)
        {
            _publisherKey = value;
            emit publisherKeyChanged();
        }
    }

    QString PublisherItem::nickname() const
    {
        return _nickname;
    }

    void PublisherItem::nickname(const QString& name)
    {
        if (name != _nickname)
        {
            _nickname = name;
            emit nicknameChanged();
        }
    }

    QString PublisherItem::shortTitle() const
    {
        return _shortTitle;
    }

    void PublisherItem::shortTitle(const QString& value)
    {
        if (value != _shortTitle)
        {
            _shortTitle = value;
            emit shortTitleChanged();
        }
    }

    QString PublisherItem::aboutMe() const
    {
        return _aboutMe;
    }

    void PublisherItem::aboutMe(const QString& value)
    {
        if (value != _aboutMe)
        {
            _aboutMe = value;
            emit aboutMeChanged();
        }
    }

    QString PublisherItem::website() const
    {
        return _website;
    }

    void PublisherItem::website(const QString& value)
    {
        if (value != _website)
        {
            _website = value;
            emit websiteChanged();
        }
    }

    QString PublisherItem::twitter() const
    {
        return _twitter;
    }

    void PublisherItem::twitter(const QString& value)
    {
        if (value != _twitter)
        {
            _twitter = value;
            emit twitterChanged();
        }
    }

    QString PublisherItem::linkedin() const
    {
        return _linkedin;
    }

    void PublisherItem::linkedin(const QString& value)
    {
        if (value != _linkedin)
        {
            _linkedin = value;
            emit linkedinChanged();
        }
    }

    QString PublisherItem::instagram() const
    {
        return _instagram;
    }

    void PublisherItem::instagram(const QString& value)
    {
        if (value != _instagram)
        {
            _instagram = value;
            emit instagramChanged();
        }
    }

    QString PublisherItem::telegram() const
    {
        return _telegram;
    }

    void PublisherItem::telegram(const QString& value)
    {
        if (value != _telegram)
        {
            _telegram = value;
            emit telegramChanged();
        }
    }

    QString PublisherItem::discord() const
    {
        return _discord;
    }

    void PublisherItem::discord(const QString& value)
    {
        if (value != _discord)
        {
            _discord = value;
            emit discordChanged();
        }
    }

    PublishersViewModel::PublishersViewModel(QObject *parent)
        : QAbstractTableModel(parent)
    {
        for(size_t i = 0,k=5; i < 6; ++i, --k)
        {
            PublisherItem::Ptr info(new PublisherItem());
            info->nickname(QString::number(k) + " Fluffy the Mouse");
            info->shortTitle("Beam biggest fan________________________");
            info->aboutMe("Bilbo was very rich and very peculiar, and had been the wonder of the Shire for sixty years, ever since his remarkable disappearance and unexpected return. The riches he had brought back from his travels had now become a local legend, and it was popularly believed, whatever the old folk might say.");
            info->website("https://website.org");
            info->publisherKey("128dhwue8yfhy7f9fy9e3hfouf");
            publishersInfo.append(info);
        }

            publishersInfo[1]->discord("https://website.org");
            publishersInfo[2]->discord("https://website.org");
            publishersInfo[3]->discord("https://website.org");
            publishersInfo[4]->discord("https://website.org");
            publishersInfo[5]->discord("https://website.org");
            
            publishersInfo[2]->twitter("https://website.org");
            publishersInfo[3]->twitter("https://website.org");
            publishersInfo[4]->twitter("https://website.org");
            publishersInfo[5]->twitter("https://website.org");
            
            publishersInfo[3]->instagram("https://website.org");
            publishersInfo[4]->instagram("https://website.org");
            publishersInfo[5]->instagram("https://website.org");
            
            publishersInfo[4]->linkedin("https://website.org");
            publishersInfo[5]->linkedin("https://website.org");
            
            publishersInfo[5]->telegram("https://website.org");
    }

    QString PublishersViewModel::getNicknameRole() const
    {
        return "nickname";
    }

    QString PublishersViewModel::getShortTitleRole() const
    {
        return "shortTitle";
    }

    QString PublishersViewModel::getAboutRole() const
    {
        return "about";
    }

    QString PublishersViewModel::getWebsiteRole() const
    {
        return "website";
    }
    QString PublishersViewModel::getTwitterRole() const
    {
        return "twitter";
    }
    QString PublishersViewModel::getLinkedinRole() const
    {
        return "linkedin";
    }
    QString PublishersViewModel::getInstagramRole() const
    {
        return "instagram";
    }
    QString PublishersViewModel::getTelegramRole() const
    {
        return "telegram";
    }
    QString PublishersViewModel::getDiscordRole() const
    {
        return "discord";
    }

    QString PublishersViewModel::getPublisherLinkRole() const
    {
        return "publisherLink";
    }

    int PublishersViewModel::rowCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        return publishersInfo.size();
    }

    int PublishersViewModel::columnCount(const QModelIndex &parent) const
    {
        if (parent.isValid())
            return 0;

        return roleNames().size();
    }

    QVariant PublishersViewModel::data(const QModelIndex &index, int role) const
    {
        if (!index.isValid())
            return QVariant();

        switch(role)
        {
        case NicknameRole:
            return QVariant(publishersInfo.at(index.row())->nickname());
        case ShortTitleRole:
            return QVariant(publishersInfo.at(index.row())->shortTitle());
        case AboutRole:
            return QVariant(publishersInfo.at(index.row())->aboutMe());
        case WebsiteRole:
            return QVariant(publishersInfo.at(index.row())->website());
        case TwitterRole:
            return QVariant(publishersInfo.at(index.row())->twitter());
        case LinkedinRole:
            return QVariant(publishersInfo.at(index.row())->linkedin());
        case InstagramRole:
            return QVariant(publishersInfo.at(index.row())->instagram());
        case TelegramRole:
            return QVariant(publishersInfo.at(index.row())->telegram());
        case DiscordRole:
            return QVariant(publishersInfo.at(index.row())->discord());
        case PublisherLinkRole:
            return QVariant(publishersInfo.at(index.row())->publisherKey());
        }
        return QVariant();
    }

    QHash<int, QByteArray> PublishersViewModel::roleNames() const
    {
        QHash<int, QByteArray> names;
        names[NicknameRole] = getNicknameRole().toUtf8();
        names[ShortTitleRole] = getShortTitleRole().toUtf8();
        names[AboutRole] = getAboutRole().toUtf8();
        names[WebsiteRole] = getWebsiteRole().toUtf8();
        names[TwitterRole] = getTwitterRole().toUtf8();
        names[LinkedinRole] = getLinkedinRole().toUtf8();
        names[InstagramRole] = getInstagramRole().toUtf8();
        names[TelegramRole] = getTelegramRole().toUtf8();
        names[DiscordRole] = getDiscordRole().toUtf8();
        names[PublisherLinkRole] = getPublisherLinkRole().toUtf8();
        return names;
    }

}
