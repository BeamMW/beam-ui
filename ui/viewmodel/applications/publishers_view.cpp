#include "publishers_view.h"

namespace beamui::applications
{
    PublishersViewModel::PublishersViewModel(QObject *parent)
        : QAbstractTableModel(parent)
    {
        for(size_t i = 0; i < 5; ++i)
        {
            AppsViewModel::Ptr info(new AppsViewModel());
            info->nickname(QString::number(i) + " Fluffy the Mouse");
            info->shortTitle("Beam biggest fan________________________");
            info->aboutMe("Bilbo was very rich and very peculiar, and had been the wonder of the Shire for sixty years, ever since his remarkable disappearance and unexpected return. The riches he had brought back from his travels had now become a local legend, and it was popularly believed, whatever the old folk might say.");
            info->website("https://website.org");
            info->publisherKey("128dhwue8yfhy7f9fy9e3hfouf");
            publishersInfo.append(info);
        }
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
