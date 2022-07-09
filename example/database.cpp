/****************************************************************************
**
** Copyright (C) 2022 Carlos Enrique Duarte Ortiz (carlosduarte.1@hotmail.com)
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
****************************************************************************/

#include "database.h"

#include <QDebug>

Database *Database::s_instance = nullptr;

Database::Database()
{
    m_client = new QMySqlClient;
}

Database::~Database()
{
    m_client->close();
    delete m_client;
    m_client = nullptr;
    qDebug() << "db bye!";
}

void Database::open(const QString &host, const QString &user, const QString &pass, unsigned port, const QString &dbname)
{
    s_instance = new Database;
    s_instance->m_client->setHostAddress(host);
    s_instance->m_client->setUsername(user);
    s_instance->m_client->setPassword(pass);
    s_instance->m_client->setPort(port);
    s_instance->m_client->setDatabaseName(dbname);
    if (s_instance->m_client->open())
    {
        s_instance->createTable();
        s_instance->insertValues();
    }
}

void Database::close()
{
    delete s_instance;
    s_instance = nullptr;
}

void Database::createTable()
{
    s_instance->m_client->prepare("create table if not exists mytable1(id integer auto_increment not null, primary key(id), integervalue integer, doublevalue double, booleanvalue bool, stringvalue text, datetimevalue datetime, blobvalue longblob)");
    s_instance->m_client->exec();
}

void Database::insertValues()
{
    s_instance->m_client->prepare("insert into mytable1(integervalue, doublevalue, booleanvalue, stringvalue, datetimevalue) values(11,1.15,1,'hello','2020-02-01 05:33:01')");
    s_instance->m_client->exec();

    s_instance->m_client->prepare("insert into mytable1(integervalue, doublevalue, booleanvalue, stringvalue, datetimevalue) values(12,2.15,0,'bye','2021-02-01 06:33:01')");
    s_instance->m_client->exec();

    s_instance->m_client->prepare("insert into mytable1(integervalue, doublevalue, booleanvalue, stringvalue, datetimevalue) values(13,3.15,1,'world','2022-02-01 07:33:01')");
    s_instance->m_client->exec();

    s_instance->m_client->prepare("insert into mytable1(integervalue, doublevalue, booleanvalue, stringvalue, datetimevalue) values(14,4.15,0,'mysql','2023-02-01 08:33:01')");
    s_instance->m_client->exec();

    s_instance->m_client->prepare("insert into mytable1(integervalue, doublevalue, booleanvalue, stringvalue, datetimevalue) values(15,5.15,1,'Qt','2024-02-01 09:33:01')");
    s_instance->m_client->exec();
}

std::vector<MyTable> Database::selectAll()
{
    std::vector<MyTable> records;
    s_instance->m_client->prepare("select id, integervalue, doublevalue, booleanvalue, stringvalue, datetimevalue from mytable1");
    s_instance->m_client->exec();
    while (s_instance->m_client->next())
    {
        MyTable record {
            s_instance->m_client->getInteger(0),
            s_instance->m_client->getInteger(1),
            s_instance->m_client->getDouble(2),
            s_instance->m_client->getBoolean(3),
            s_instance->m_client->getString(4),
            s_instance->m_client->getDateTime(5),
            ""
        };
        records.push_back(record);
    }
    s_instance->m_client->endNext();
    return records;
}
