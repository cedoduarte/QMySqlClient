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

#include "qmysqlclient.h"

QMySqlClient::QMySqlClient()
{
    m_preparedQuery = "";
    m_hostAddress = "";
    m_username = "";
    m_password = "";
    m_databaseName = "";
    m_port = 0;
    m_connection = nullptr;
    m_result = nullptr;
    m_currentRow = nullptr;
    m_numFields = 0;
    m_blobHandler = new QMySqlBlobDataTypeHandler;
}

QMySqlClient::~QMySqlClient()
{
    delete m_blobHandler;
}

bool QMySqlClient::open()
{
    m_connection = mysql_init(nullptr);
    return connectToServer();
}

bool QMySqlClient::exec()
{
    if (mysql_real_query(m_connection, m_preparedQuery.toStdString().c_str(), m_preparedQuery.size()) == 0)
    {
        return handleSelectStatement();
    }
    else
    {
        return false;
    }
}

int QMySqlClient::getInteger(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return 0;
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]).toInt();
        }
        else
        {
            oktrue(ok, false);
            return 0;
        }
    }
}

double QMySqlClient::getDouble(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return 0.0;
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]).toDouble();
        }
        else
        {
            oktrue(ok, false);
            return 0.0;
        }
    }
}

bool QMySqlClient::getBoolean(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return false;
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]).toInt() == 0 ? false : true;
        }
        else
        {
            oktrue(ok, false);
            return false;
        }
    }
}

QString QMySqlClient::getString(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return "";
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QString(m_currentRow[columnIndex]);
        }
        else
        {
            oktrue(ok, false);
            return "";
        }
    }
}

QDateTime QMySqlClient::getDateTime(int columnIndex, bool *ok) const
{
    if (wrongColumnIndex(columnIndex))
    {
        oktrue(ok, false);
        return QDateTime();
    }
    else
    {
        if (m_currentRow[columnIndex])
        {
            oktrue(ok);
            return QDateTime::fromString(m_currentRow[columnIndex], "yyyy-MM-dd hh:mm:ss");
        }
        else
        {
            oktrue(ok, false);
            return QDateTime();
        }
    }
}

bool QMySqlClient::next()
{
    if (m_result)
    {
        m_currentRow = mysql_fetch_row(m_result);
        return (m_currentRow != nullptr);
    }
    else
    {
        return false;
    }
}

void QMySqlClient::endNext()
{
    mysql_free_result(m_result);
    m_result = nullptr;
}

bool QMySqlClient::handleSelectStatement()
{
    if (m_preparedQuery.startsWith("select", Qt::CaseInsensitive))
    {
        m_result = mysql_store_result(m_connection);
        if (m_result)
        {
            m_numFields = mysql_num_fields(m_result);
        }
        else
        {
            m_numFields = 0;
        }
        return true;
    }
    else
    {
        m_numFields = 0;
        return true;
    }
}

bool QMySqlClient::connectToServer()
{
    if (m_connection)
    {
        if (mysql_real_connect(m_connection, m_hostAddress.toStdString().c_str(), m_username.toStdString().c_str(),
            m_password.toStdString().c_str(), m_databaseName.toStdString().c_str(), m_port, nullptr, 0))
        {
            m_blobHandler->m_connection = m_connection;
            return true;
        }
        else
        {
            qdebug(__FILE__, __LINE__);
            close();
            return false;
        }
    }
    else
    {
        qdebug(__FILE__, __LINE__);
        return false;
    }
}

void QMySqlClient::oktrue(bool *ok, bool value)
{
    if (ok)
    {
        *ok = value;
    }
}
