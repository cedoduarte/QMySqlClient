/* Copyright (c) 2000, 2021, Oracle and/or its affiliates.
   Copyright (c) 2022, Carlos Enrique Duarte Ortiz (carlosduarte.1@hotmail.com)

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License, version 2.0,
   as published by the Free Software Foundation.

   This program is also distributed with certain software (including
   but not limited to OpenSSL) that is licensed under separate terms,
   as designated in a particular file or component or in included license
   documentation.  The authors of MySQL hereby grant you an additional
   permission to link the program and your derivative works with the
   separately licensed software that they have included with MySQL.

   Without limiting anything contained in the foregoing, this file,
   which is part of C Driver for MySQL (Connector/C), is also subject to the
   Universal FOSS Exception, version 1.0, a copy of which can be found at
   http://oss.oracle.com/licenses/universal-foss-exception.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License, version 2.0, for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA */

#ifndef QMYSQLCLIENT_H
#define QMYSQLCLIENT_H

#include "QMySqlClient_global.h"
#include "qmysqlblobdatatypehandler.h"

#include <QDateTime>

class QMYSQLCLIENT_EXPORT QMySqlClient : public QMySqlDriverReference
{
public:
    enum HostAddressEnum
    {
        Localhost
    };

    enum PortEnum
    {
        DefaultPort = 3306
    };

    explicit QMySqlClient();
    virtual ~QMySqlClient();

    bool open();

    void close() const
    {
        mysql_close(m_connection);
    }

    void prepare(const QString &queryString)
    {
        m_preparedQuery = queryString;
    }

    bool exec();

    bool next();

    void endNext();

    QString preparedQuery() const
    {
        return m_preparedQuery;
    }

    int getInteger(int columnIndex, bool *ok = nullptr) const;
    double getDouble(int columnIndex, bool *ok = nullptr) const;
    bool getBoolean(int columnIndex, bool *ok = nullptr) const;
    QString getString(int columnIndex, bool *ok = nullptr) const;
    QDateTime getDateTime(int columnIndex, bool *ok = nullptr) const;

    void addInteger(int value)
    {
        addBindValue(QString::number(value));
    }

    void addDouble(double value)
    {
        addBindValue(QString::number(value));
    }

    void addBoolean(bool value)
    {
        addBindValue(value ? "1" : "0");
    }

    void addString(const QString &value)
    {
        addBindValue(QString("'%1'").arg(value));
    }

    void addDateTime(const QDateTime &value)
    {
        addBindValue(QString("'%1'").arg(value.toString("yyyy-MM-dd hh:mm:ss")));
    }

    bool insertBlob(const QString &binaryFilePath, const QString &tableName, const QString &columnName) const
    {
        return m_blobHandler->insertBlob(binaryFilePath, tableName, columnName);
    }

    bool setBlob(const QString &binaryFilePath, const QString &tableName, const QString &columnName,
                 const QString &referenceColumnName, const QString &referenceFieldValue) const
    {
        return m_blobHandler->setBlob(binaryFilePath, tableName, columnName, referenceColumnName, referenceFieldValue);
    }

    bool getBlob(const QString &tableName, const QString &columnName, const QString &referenceColumnName,
                 const QString &referenceFieldValue, const QString &outputFilepath) const
    {
        return m_blobHandler->getBlob(tableName, columnName, referenceColumnName, referenceFieldValue, outputFilepath);
    }

    QString apiVersion() const
    {
        return mysql_get_client_info();
    }

    void setHostAddress(const QString &hostAddress)
    {
        m_hostAddress = hostAddress;
    }

    void setHostAddress(HostAddressEnum)
    {
        m_hostAddress = "127.0.0.1";
    }

    void setUsername(const QString &username)
    {
        m_username = username;
    }

    void setPassword(const QString &password)
    {
        m_password = password;
    }

    void setDatabaseName(const QString &databaseName)
    {
        m_databaseName = databaseName;
    }

    void setPort(unsigned port = DefaultPort)
    {
        m_port = port;
    }

    QString hostAddress() const
    {
        return m_hostAddress;
    }

    QString username() const
    {
        return m_username;
    }

    QString password() const
    {
        return m_password;
    }

    QString databaseName() const
    {
        return m_databaseName;
    }

    unsigned port() const
    {
        return m_port;
    }

private:
    void addBindValue(const QString &value)
    {
        const int index = m_preparedQuery.indexOf("?");
        m_preparedQuery = m_preparedQuery.remove(index, 1);
        m_preparedQuery = m_preparedQuery.insert(index, value);
    }

    bool handleSelectStatement();
    bool connectToServer();

    bool wrongColumnIndex(int columnIndex) const
    {
        return (columnIndex < 0 || columnIndex >= m_numFields);
    }

    static void oktrue(bool *ok, bool value = true);

    QString m_preparedQuery;
    QString m_hostAddress;
    QString m_username;
    QString m_password;
    QString m_databaseName;
    unsigned m_port;
    MYSQL_RES *m_result;
    MYSQL_ROW m_currentRow;
    int m_numFields;
    QMySqlBlobDataTypeHandler *m_blobHandler;
};

using QMySqlDatabase = QMySqlClient;
using QMySqlConnection = QMySqlClient;

#endif // QMYSQLCLIENT_H
