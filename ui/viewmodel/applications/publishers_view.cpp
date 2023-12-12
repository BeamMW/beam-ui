#include "publishers_view.h"
#include "viewmodel/ui_helpers.h"
namespace
{
    namespace Roles
    {
        QString kShortTitle         = "shortTitle";
        QString kNickname           = "nickname";
        QString kAbout              = "about";
        QString kWebsite            = "website";
        QString kTwitter            = "twitter";
        QString kLinkedin           = "linkedin";
        QString kInstagram          = "instagram";
        QString kTelegram           = "telegram";
        QString kDiscord            = "discord";
        QString kPublisherLink      = "publisherLink";
        QString kEnabled            = "enabled";
    }
    namespace PublisherProps
    {
        QString kPubkey                =   "pubkey";
        QString kName                  =   "name";
        QString kShortTitle            =   "short_title";
        QString kAboutMe               =   "about_me";
        QString kWebsite               =   "website";
        QString kTwitter               =   "twitter";
        QString kLinkedin              =   "linkedin";
        QString kInstagram             =   "instagram";
        QString kTelegram              =   "telegram";
        QString kDiscord               =   "discord";
        QString kEnabled               =   "enabled";
    }
}

PublisherItem::PublisherItem(const QVariantMap& publisherInfo)
{
    _publisherKey   = publisherInfo[PublisherProps::kPubkey].toString();
    _nickname       = publisherInfo[PublisherProps::kName].toString();
    _shortTitle     = publisherInfo[PublisherProps::kShortTitle].toString();
    _aboutMe        = publisherInfo[PublisherProps::kAboutMe].toString();
    _website        = publisherInfo[PublisherProps::kWebsite].toString();
    _twitter        = publisherInfo[PublisherProps::kTwitter].toString();
    _linkedin       = publisherInfo[PublisherProps::kLinkedin].toString();
    _instagram      = publisherInfo[PublisherProps::kInstagram].toString();
    _telegram       = publisherInfo[PublisherProps::kTelegram].toString();
    _discord        = publisherInfo[PublisherProps::kDiscord].toString();
    _enabled        = publisherInfo[PublisherProps::kEnabled].toBool();
}

const QString& PublisherItem::publisherKey() const
{
    return _publisherKey;
}

const QString& PublisherItem::nickname() const
{
    return _nickname;
}

const QString& PublisherItem::shortTitle() const
{
    return _shortTitle;
}

const QString& PublisherItem::aboutMe() const
{
    return _aboutMe;
}

const QString& PublisherItem::website() const
{
    return _website;
}

const QString& PublisherItem::twitter() const
{
    return _twitter;
}

const QString& PublisherItem::linkedin() const
{
    return _linkedin;
}

const QString& PublisherItem::instagram() const
{
    return _instagram;
}

const QString& PublisherItem::telegram() const
{
    return _telegram;
}

const QString& PublisherItem::discord() const
{
    return _discord;
}

bool PublisherItem::enabled() const
{
    return _enabled;
}


PublishersViewModel::PublishersViewModel()
{
    updatePublishers();
}

PublishersViewModel::~PublishersViewModel()
{
    qDeleteAll(m_publishers);
}

const QList<QObject*>& PublishersViewModel::getPublishers()
{
    return m_publishers;
}

void PublishersViewModel::setPublishersInfo(QList<QVariantMap> info)
{
    m_publishersInfo = std::move(info);

    updatePublishers();
}

const QList<QVariantMap>& PublishersViewModel::getPublishersInfo() const
{
    return m_publishersInfo;
}

const QString& PublishersViewModel::getNicknameRole() const
{
    return Roles::kNickname;
}

const QString& PublishersViewModel::getShortTitleRole() const
{
    return Roles::kShortTitle;
}

const QString& PublishersViewModel::getAboutRole() const
{
    return Roles::kAbout;
}

const QString& PublishersViewModel::getWebsiteRole() const
{
    return Roles::kWebsite;
}
const QString& PublishersViewModel::getTwitterRole() const
{
    return Roles::kTwitter;
}
const QString& PublishersViewModel::getLinkedinRole() const
{
    return Roles::kLinkedin;
}
const QString& PublishersViewModel::getInstagramRole() const
{
    return Roles::kInstagram;
}
const QString& PublishersViewModel::getTelegramRole() const
{
    return Roles::kTelegram;
}
const QString& PublishersViewModel::getDiscordRole() const
{
    return Roles::kDiscord;
}

const QString& PublishersViewModel::getPublisherLinkRole() const
{
    return Roles::kPublisherLink;
}

const QString& PublishersViewModel::getPublisherStatusRole() const
{
    return Roles::kEnabled;
}

Qt::SortOrder PublishersViewModel::sortOrder() const
{
    return m_sortOrder;
}

void PublishersViewModel::setSortOrder(Qt::SortOrder value)
{
    if (m_sortOrder != value)
    {
        m_sortOrder = value;
        sortPublishers();
    }
}

void PublishersViewModel::sortPublishers()
{
    std::sort(m_publishers.begin(), m_publishers.end(),
        [sortOrder = m_sortOrder](const QObject* left, const QObject* right)
        {
            
            if (sortOrder == Qt::DescendingOrder)
                return ((PublisherItem*)left)->nickname() > ((PublisherItem*)right)->nickname();
            return ((PublisherItem*)left)->nickname() < ((PublisherItem*)right)->nickname();
        });

    emit publishersChanged();
}

void PublishersViewModel::updatePublishers()
{
    qDeleteAll(m_publishers);
    m_publishers.clear();

    for (const auto& publisherInfo : m_publishersInfo)
    {
        m_publishers.push_back(new PublisherItem(publisherInfo));
    }

    sortPublishers();
}
