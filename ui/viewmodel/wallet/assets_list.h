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
#include "asset_object.h"
#include "viewmodel/helpers/list_model.h"
#include "assets_manager.h"
#include "viewmodel/notifications/exchange_rates_manager.h"

class AssetsList : public ListModel<std::shared_ptr<AssetObject>>
{
    Q_OBJECT
public:
    AssetsList();
    ~AssetsList() override = default;

    enum class Roles
    {
        Search = Qt::UserRole + 1,
        RId,
        RUnitName,
        RAmount,
        RInTxCnt,
        ROutTxCnt,
        RIcon,
        RColor,
        RSelectionColor,
        RUnitName2,
        RRate,
    };

    Q_ENUM(Roles)

    [[nodiscard]] QHash<int, QByteArray> roleNames() const override;
    [[nodiscard]] QVariant data(const QModelIndex &index, int role) const override;

private slots:
    void onNewRates();
    void onWalletStatus();
    void onAssetInfo(beam::Asset::ID assetId);
    void onTransactionsChanged(beam::wallet::ChangeAction action, const std::vector<beam::wallet::TxDescription>& items);

private:
    void touch(beam::Asset::ID id);
    std::shared_ptr<AssetObject> get(beam::Asset::ID id);

    mutable AssetsManager _amgr;
    mutable ExchangeRatesManager _ermgr;
    WalletModel& _wallet;

    typedef std::vector<beam::wallet::TxDescription> TxList;
    TxList _txlist;
};
