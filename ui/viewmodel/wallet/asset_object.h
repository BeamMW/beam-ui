// Copyright 2019 The Beam Team
//
// Licensed under the Apache License, Version 2.0 (the "License");
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

#include <QString>

class AssetObject
{
public:
    AssetObject(uint64_t id);
    bool operator==(const AssetObject& other) const;

    [[nodiscard]] uint64_t id() const;
    [[nodiscard]] uint32_t inTxCnt() const;
    [[nodiscard]] uint32_t outTxCnt() const;

    void resetTxCnt();
    void addIntTx();
    void addOutTx();

protected:
    uint64_t  _id;
    uint64_t  _inTxCnt;
    uint64_t  _outTxCnt;
};
