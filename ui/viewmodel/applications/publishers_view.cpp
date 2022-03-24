#include "publishers_view.h"
#include "viewmodel/ui_helpers.h"


PublisherItem::PublisherItem(const QVariantMap& publisherInfo)
{
    _publisherKey = publisherInfo["publisherKey"].toString();
    _nickname = publisherInfo["nickname"].toString();
    _shortTitle = publisherInfo["shortTitle"].toString();
    _aboutMe = publisherInfo["aboutMe"].toString();
    _website = publisherInfo["website"].toString();
    _twitter = publisherInfo["twitter"].toString();
    _linkedin = publisherInfo["linkedin"].toString();
    _instagram = publisherInfo["instagram"].toString();
    _telegram = publisherInfo["telegram"].toString();
    _discord = publisherInfo["discord"].toString();
}

QString PublisherItem::publisherKey() const
{
    return _publisherKey;
}

QString PublisherItem::nickname() const
{
    return _nickname;
}

QString PublisherItem::shortTitle() const
{
    return _shortTitle;
}

QString PublisherItem::aboutMe() const
{
    return _aboutMe;
}

QString PublisherItem::website() const
{
    return _website;
}

QString PublisherItem::twitter() const
{
    return _twitter;
}

QString PublisherItem::linkedin() const
{
    return _linkedin;
}

QString PublisherItem::instagram() const
{
    return _instagram;
}

QString PublisherItem::telegram() const
{
    return _telegram;
}

QString PublisherItem::discord() const
{
    return _discord;
}


PublishersViewModel::PublishersViewModel()
{

    for (size_t i = 0; i < 6; ++i)
    {
        QVariantMap info;
        info["nickname"] = QString::number(i) + " Fluffy the Mouse";
        info["shortTitle"] = "Beam biggest fan biggest fan biggest fan biggest fan biggest fan";
        info["aboutMe"] = "Bilbo was very rich and very peculiar, and had been the wonder of the Shire for sixty years, ever since his remarkable disappearance and unexpected return. The riches he had brought back from his travels had now become a local legend, and it was popularly believed, whatever the old folk might say.";
        info["website"] = "https://website.org";
        info["publisherKey"] = "128dhwue8yfhy7f9fy9e3hfouf";
        m_publishersInfo.append(info);
    }

    m_publishersInfo[1]["nickname"] = ("https://website.org");

    m_publishersInfo[1]["discord"] = ("https://website.org");
    m_publishersInfo[2]["discord"] = ("https://website.org");
    m_publishersInfo[3]["discord"] = ("https://website.org");
    m_publishersInfo[4]["discord"] = ("https://website.org");
    m_publishersInfo[5]["discord"] = ("https://website.org");

    m_publishersInfo[2]["twitter"] = ("https://website.org");
    m_publishersInfo[3]["twitter"] = ("https://website.org");
    m_publishersInfo[4]["twitter"] = ("https://website.org");
    m_publishersInfo[5]["twitter"] = ("https://website.org");

    m_publishersInfo[3]["instagram"] = ("https://website.org");
    m_publishersInfo[4]["instagram"] = ("https://website.org");
    m_publishersInfo[5]["instagram"] = ("https://website.org");

    m_publishersInfo[4]["linkedin"] = ("https://website.org");
    m_publishersInfo[5]["linkedin"] = ("https://website.org");

    m_publishersInfo[5]["telegram"] = ("https://website.org");
    updatePublishers();
}

PublishersViewModel::~PublishersViewModel()
{
    qDeleteAll(m_publishers);
}

QQmlListProperty<PublisherItem> PublishersViewModel::getPublishers()
{
    return beamui::CreateQmlListProperty<PublisherItem>(this, m_publishers);
}

void PublishersViewModel::setPublishersInfo(QList<QVariantMap> info)
{
    m_publishersInfo = info;

    updatePublishers();
}

QList<QVariantMap> PublishersViewModel::getPublishersInfo() const
{
    return m_publishersInfo;
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
        [sortOrder = m_sortOrder](const PublisherItem* left, const PublisherItem* right)
        {
            if (sortOrder == Qt::DescendingOrder)
                return left->nickname() > right->nickname();
            return left->nickname() < right->nickname();
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

QString PublishersViewModel::getRoleValue(const int row, QByteArray roleName)
{
    // TODO: redo
    if (row < 0 || row > m_publishers.size())
    {
        return QString();
    }

    if (roleName == getNicknameRole())
        return m_publishers.at(row)->nickname();
    else if (roleName == getShortTitleRole())
        return m_publishers.at(row)->shortTitle();
    else if (roleName == getAboutRole())
        return m_publishers.at(row)->aboutMe();
    else if (roleName == getWebsiteRole())
        return m_publishers.at(row)->website();
    else if (roleName == getTwitterRole())
        return m_publishers.at(row)->twitter();
    else if (roleName == getLinkedinRole())
        return m_publishers.at(row)->linkedin();
    else if (roleName == getInstagramRole())
        return m_publishers.at(row)->instagram();
    else if (roleName == getTelegramRole())
        return m_publishers.at(row)->telegram();
    else if (roleName == getDiscordRole())
        return m_publishers.at(row)->discord();
    else if (roleName == getPublisherLinkRole())
        return m_publishers.at(row)->publisherKey();

    return QString();
}