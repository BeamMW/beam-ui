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
    };
    return roles;
}

QVariant DexOrdersList::data(const QModelIndex &index, int role) const
{
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
        return QString::fromStdString(order.orderID.to_string());
    case Roles::RType:
        return order.sellCoin == 0 ? "Sell BEAM" : "Buy BEAM";
    case Roles::RPrice:
        return QString("10 BEAM-X");
    case Roles::RSize:
        return QString::number(order.amount) + " BEAM";
    case Roles::RTotal:
        return QString::number(order.amount * 10) + " BEAM-X";
    case Roles::RExpiration:
        return beamui::toString(order.expiration);
    case Roles::RStatus:
        return QString("N/A");
    case Roles::RIsMine:
        return order.isMy;
    default:
        return QVariant();
    }
}
