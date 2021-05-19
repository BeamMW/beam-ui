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
#include "dex_orders_list.h"
#include "viewmodel/ui_helpers.h"
#include "viewmodel/qml_globals.h"

DexOrdersList::DexOrdersList()
{
}

QHash<int, QByteArray> DexOrdersList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        {static_cast<int>(Roles::RId),         "id"},
        {static_cast<int>(Roles::RType),       "type"},
        {static_cast<int>(Roles::RPrice),      "price"},
        {static_cast<int>(Roles::RSize),       "size"},
        {static_cast<int>(Roles::RTotal),      "total"},
        {static_cast<int>(Roles::RExpiration), "expiration"},
        {static_cast<int>(Roles::RStatus),     "status"},
        {static_cast<int>(Roles::RIsMine),     "isMine"},
        {static_cast<int>(Roles::RProgress),   "progress"},
        {static_cast<int>(Roles::RIsActive),   "isActive"},
        {static_cast<int>(Roles::RCanAccept),  "canAccept"},
    };
    return roles;
}

QVariant DexOrdersList::data(const QModelIndex &index, int role) const
{
    using namespace beam;
    using namespace beamui;
    using namespace beam::wallet;

    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
        assert(false);
        return QVariant();
    }

    const auto row = index.row();
    const auto& order = m_list[row];

    switch (static_cast<Roles>(role))
    {
    case Roles::RId:
        return QString::fromStdString(order.getID().to_string());

    case Roles::RType:
        return order.getSide() == DexMarketSide::Sell ? "Sell BEAM-X" : "Buy BEAM-X";

    case Roles::RPrice:
        return AmountToUIString(order.getPrice()) + " BEAM";

    case Roles::RSize:
        return AmountToUIString(order.getSize()) + " BEAM-X";

    case Roles::RTotal:
        // TODO - correct?
        return AmountToUIString(order.getSize() * order.getPrice() / Rules::Coin) + " BEAM";

    case Roles::RExpiration:
        return beamui::toString(order.getExpiration());

    case Roles::RStatus:
        {
            if (order.IsExpired())
            {
                //% "Expired"
                return qtTrId("dex-order-expired");
            }
            else if (order.IsCompleted())
            {
                //% "Fulfilled"
                return qtTrId("dex-order-fulfilled");
            }
            else
            {
                //% "Active"
                return qtTrId("dex-order-active");
            }
        }

    case Roles::RIsMine:
        return order.IsMine();

    case Roles::RProgress:
        // TODO
        return 0;

    case Roles::RCanAccept:
        return order.CanAccept();

    default:
        return QVariant();
    }
}
