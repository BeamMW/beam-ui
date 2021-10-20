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

#include <memory>
#include "viewmodel/helpers/list_model.h"
#include "asset_object.h"
#include "assets_manager.h"
#include "wallet_model.h"
#include "exchange_rates_manager.h"

class AssetsList: public ListModel<std::shared_ptr<AssetObject>>
{
    Q_OBJECT
public:
    typedef std::shared_ptr<AssetsList> Ptr;

    AssetsList(WalletModel::Ptr wallet, AssetsManager::Ptr assets, ExchangeRatesManager::Ptr rates);
    ~AssetsList() override = default;

    enum class Roles
    {
        Search = Qt::UserRole + 1,
        RId,
        RUnitName,
        RAmount,
        RAmountRegular,
        RAmountShielded,
        RLocked,
        RChange,
        RMaturingRegular,
        RMaturingMP,
        RMaturingTotal,
        RIcon,
        RColor,
        RSelectionColor,
        RRateUnit,
        RRate,
        RName,
        RSmallestUnitName,
        RShortDesc,
        RLongDesc,
        RSiteUrl,
        RWhitePaper,
        RVerified,
    };

    Q_ENUM(Roles)

    Q_INVOKABLE [[nodiscard]] int getRoleId(QString name) const;
    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

private slots:
    void onNewRates();
    void onWalletStatus();
    void onAssetInfo(beam::Asset::ID assetId);

private:
    bool touch(beam::Asset::ID id);
    std::shared_ptr<AssetObject> getAsset(beam::Asset::ID id);
    bool hasAsset(beam::Asset::ID id);

    WalletModel::Ptr _wallet;
    AssetsManager::Ptr _amgr;
    ExchangeRatesManager::Ptr _rates;
};
