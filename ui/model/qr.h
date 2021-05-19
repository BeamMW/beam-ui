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
// limitations under the License

#pragma once

#include <QObject>
#include <QtCore>
#include "utility/common.h"

class QR : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString address      READ getAddress     WRITE setAddr   NOTIFY addressChanged)
    Q_PROPERTY(QString amount       READ getAmount      WRITE setAmount NOTIFY amountChanged)
    Q_PROPERTY(QString data         READ getEncoded                     NOTIFY qrDataChanged)
public:
    QR();
    QR(const QString& addr,
       uint width = 200,
       uint height = 200,
       beam::Amount amount = 0);
    ~QR();
    QString getAmount() const;
    void setAmount(const QString& value);
    const QString& getAddress() const;
    void setAddr(const QString& addr);
    void setDimensions(uint width, uint height);

    const QString& getEncoded() const;

signals:
    void qrDataChanged();
    void addressChanged();
    void amountChanged();
private:
    void update();

    QString m_addr;
    uint m_width = 270;
    uint m_height = 270;
    beam::Amount m_amountGrothes = 0;
    QString m_amountGrothesStr;
    QString m_qrData;
};
