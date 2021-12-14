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
#include "assets_list.h"
#include "viewmodel/ui_helpers.h"

AssetsList::AssetsList(WalletModel::Ptr wallet, AssetsManager::Ptr assets, ExchangeRatesManager::Ptr rates)
    : _wallet(std::move(wallet))
    , _amgr(std::move(assets))
    , _rates(std::move(rates))
{
    connect(_rates.get(),  &ExchangeRatesManager::rateUnitChanged, this,  &AssetsList::onNewRates);
    connect(_rates.get(),  &ExchangeRatesManager::activeRateChanged, this,  &AssetsList::onNewRates);
    connect(_wallet.get(), &WalletModel::walletStatusChanged, this, &AssetsList::onWalletStatus);
    connect(_amgr.get(),   &AssetsManager::assetInfo, this,  &AssetsList::onAssetInfo);
    _wallet->getAsync()->getWalletStatus();
    // Transactions table would be created later and get this for us
    // Need to refactor multiple requests in the future
    //_wallet.getAsync()->getTransactions();
}

QHash<int, QByteArray> AssetsList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        {static_cast<int>(Roles::Search),           "search"},
        {static_cast<int>(Roles::RId),              "id"},
        {static_cast<int>(Roles::RUnitName),        "unitName"},
        {static_cast<int>(Roles::RAmount),          "amount"},
        {static_cast<int>(Roles::RAmountRegular),   "amountRegular"},
        {static_cast<int>(Roles::RAmountShielded),  "amountShielded"},
        {static_cast<int>(Roles::RIcon),            "icon"},
        {static_cast<int>(Roles::RColor),           "color"},
        {static_cast<int>(Roles::RSelectionColor),  "selectionColor"},
        {static_cast<int>(Roles::RRateUnit),        "rateUnit"},
        {static_cast<int>(Roles::RRate),            "rate"},
        {static_cast<int>(Roles::RChange),          "change"},
        {static_cast<int>(Roles::RLocked),          "locked"},
        {static_cast<int>(Roles::RMaturingRegular), "maturingRegular"},
        {static_cast<int>(Roles::RMaturingMP),      "maturingMP"},
        {static_cast<int>(Roles::RMaturingTotal),   "maturingTotal"},
        {static_cast<int>(Roles::RName),            "assetName"},
        {static_cast<int>(Roles::RSmallestUnitName),"smallestUnitName"},
        {static_cast<int>(Roles::RShortDesc),       "shortDesc"},
        {static_cast<int>(Roles::RLongDesc),        "longDesc"},
        {static_cast<int>(Roles::RSiteUrl),         "siteUrl"},
        {static_cast<int>(Roles::RWhitePaper),      "whitePaper"},
        {static_cast<int>(Roles::RVerified),        "verified"},
    };
    return roles;
}

int AssetsList::getRoleId(QString name) const
{
    const auto roles = roleNames();
    const auto byteName = name.toUtf8();

    QHashIterator<int, QByteArray> iter(roles);
    while(iter.hasNext()) {
        iter.next();
        if (iter.value() == byteName) {
            return iter.key();
        }
    }

    return -1;
}

QVariant AssetsList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
        assert(false);
        return QVariant();
    }

    const auto& asset  = m_list[index.row()];
    const auto assetId = beam::Asset::ID(asset->id());

    switch (static_cast<Roles>(role))
    {
        case Roles::RId:
            return static_cast<qint64>(assetId);
        case Roles::RUnitName:
            return _amgr->getUnitName(assetId, AssetsManager::NoShorten);
        case Roles::RAmount:
            return beamui::AmountBigToUIString(_wallet->getAvailable(assetId));
        case Roles::RAmountRegular:
            return beamui::AmountBigToUIString(_wallet->getAvailableRegular(assetId));
        case Roles::RAmountShielded:
            return beamui::AmountBigToUIString(_wallet->getAvailableShielded(assetId));
        case Roles::RMaturingRegular:
            return beamui::AmountBigToUIString(_wallet->getMaturing(assetId));
        case Roles::RMaturingMP:
            return beamui::AmountBigToUIString(_wallet->getMatutingMP(assetId));
        case Roles::RMaturingTotal:
        {
            auto total = _wallet->getMaturing(assetId);
            total += _wallet->getMatutingMP(assetId);
            return beamui::AmountBigToUIString(total);
        }
        case Roles::RChange:
            return beamui::AmountBigToUIString(_wallet->getReceivingChange(assetId));
        case Roles::RLocked:
        {
             auto locked = _wallet->getMaturing(assetId);
             locked += _wallet->getMatutingMP(assetId);
             locked += _wallet->getReceivingChange(assetId);
             return beamui::AmountBigToUIString(locked);
        }
        case Roles::Search:
            return _amgr->getName(assetId) + _amgr->getUnitName(assetId, AssetsManager::NoShorten);
        case Roles::RIcon:
            return _amgr->getIcon(assetId);
        case Roles::RColor:
            return _amgr->getColor(assetId);
        case Roles::RSelectionColor:
            return _amgr->getSelectionColor(assetId);
        case Roles::RRateUnit:
            return beamui::getCurrencyUnitName(_rates->getRateCurrency());
        case Roles::RName:
            return _amgr->getName(assetId);
        case Roles::RSmallestUnitName:
            return _amgr->getSmallestUnitName(assetId);
        case Roles::RShortDesc:
            return _amgr->getShortDesc(assetId);
        case Roles::RLongDesc:
            return _amgr->getLongDesc(assetId);
        case Roles::RSiteUrl:
            return _amgr->getSiteUrl(assetId);
        case Roles::RWhitePaper:
            return _amgr->getPaperUrl(assetId);
        case Roles::RVerified:
            return _amgr->isVerified(assetId);
        case Roles::RRate:
            {
                auto rate = _rates->getRate(beam::wallet::Currency(assetId));
                return beamui::AmountToUIString(rate);
            }
        default:
            assert(false);
            return QVariant();
    }
}

bool AssetsList::hasAsset(beam::Asset::ID id)
{
     for (auto obj: m_list)
     {
         if (obj->id() == id)
         {
             return true;
         }
     }
     return false;
}

std::shared_ptr<AssetObject> AssetsList::getAsset(beam::Asset::ID id)
{
     for (auto obj: m_list)
     {
         if (obj->id() == id)
         {
             return obj;
         }
     }
     return {};
}

bool AssetsList::touch(beam::Asset::ID id)
{
    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        if ((*it)->id() == id) {
           const auto idx = it - m_list.begin();
           ListModel::touch(idx);
           return true;
        }
    }
    return false;
}

void AssetsList::onNewRates()
{
    touch(beam::Asset::s_BeamID);
}

void AssetsList::onWalletStatus()
{
    //
    // Update/Add new assets
    //
    auto anz = _wallet->getAssetsNZ();
    std::set<beam::Asset::ID> anew;

    for (auto aid: anz)
    {
        if (!touch(aid))
        {
            anew.insert(aid);
        }
    }

    if (m_list.empty())
    {
        std::vector<std::shared_ptr<AssetObject>> newobj;
        for (auto aid: anew)
        {
            newobj.insert(newobj.begin(), std::make_shared<AssetObject>(aid));
        }
        reset(newobj);
        return;
    }

    for (auto aid: anew)
    {
        int pos = 0;
        for (; pos < m_list.size(); ++pos)
        {
            if (m_list[pos]->id() > aid)
            {
                break;
            }
        }
        insert_at(std::make_shared<AssetObject>(aid), pos);
    }

    //
    // Remove old assets
    //
    std::vector<std::shared_ptr<AssetObject>> aold;
    for (auto& ai: m_list)
    {
        if (anz.find(ai->id()) == anz.end())
        {
            aold.push_back(ai);
        }
    }
    remove(aold);
}

void AssetsList::onAssetInfo(beam::Asset::ID assetId)
{
    touch(assetId);
}
