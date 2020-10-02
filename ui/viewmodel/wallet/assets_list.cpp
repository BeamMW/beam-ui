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

QHash<int, QByteArray> AssetsList::roleNames() const
{
    static const auto roles = QHash<int, QByteArray>
    {
        {static_cast<int>(Roles::Search), "search"},
        {static_cast<int>(Roles::RId), "id"},
        {static_cast<int>(Roles::RName), "name"},
        {static_cast<int>(Roles::RAmount), "amount"},
        {static_cast<int>(Roles::RInTxCnt), "inTxCnt"},
        {static_cast<int>(Roles::ROutTxCnt), "outTxCnt"},
    };
    return roles;
}

QVariant AssetsList::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() < 0 || index.row() >= m_list.size())
    {
        assert(false);
        return QVariant();
    }

    const auto& asset = m_list[index.row()];
    switch (static_cast<Roles>(role))
    {
        case Roles::RId:
            return static_cast<qint64>(asset->id());
        case Roles::RName:
            return asset->name();
        case Roles::RAmount:
            return QString(std::to_string(asset->amount()).c_str());
        case Roles::RInTxCnt:
            return static_cast<qint32>(asset->inTxCnt());
        case Roles::ROutTxCnt:
            return static_cast<qint32>(asset->outTxCnt());
        case Roles::Search:
            return asset->name();
        default:
            assert(false);
            return QVariant();
    }
}
