//*****************************************************************
/*
  JackTrip: A System for High-Quality Audio Network Performance
  over the Internet

  Copyright (c) 2008-2022 Juan-Pablo Caceres, Chris Chafe.
  SoundWIRE group at CCRMA, Stanford University.

  Permission is hereby granted, free of charge, to any person
  obtaining a copy of this software and associated documentation
  files (the "Software"), to deal in the Software without
  restriction, including without limitation the rights to use,
  copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the
  Software is furnished to do so, subject to the following
  conditions:

  The above copyright notice and this permission notice shall be
  included in all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
  NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
  HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
  WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  OTHER DEALINGS IN THE SOFTWARE.
*/
//*****************************************************************

/**
 * \file vsAuth.cpp
 * \author Dominick Hing
 * \date May 2023
 */

#include "vsAuth.h"

#include "./vsConstants.h"

VsAuth::VsAuth(VsQuickView* view, QNetworkAccessManager* networkAccessManager, VsApi* api)
    : m_clientId(AUTH_CLIENT_ID), m_authorizationServerHost(AUTH_SERVER_HOST)
{
    m_view                 = view;
    m_networkAccessManager = networkAccessManager;
    m_api                  = api;
    m_deviceCodeFlow.reset(new VsDeviceCodeFlow(networkAccessManager));

    connect(m_deviceCodeFlow.data(), &VsDeviceCodeFlow::deviceCodeFlowInitialized, this,
            &VsAuth::initializedCodeFlow);
    connect(m_deviceCodeFlow.data(), &VsDeviceCodeFlow::deviceCodeFlowError, this,
            &VsAuth::authFailed);
    connect(m_deviceCodeFlow.data(), &VsDeviceCodeFlow::receivedAccessToken, this,
            &VsAuth::codeFlowCompleted);

    m_view->engine()->rootContext()->setContextProperty(QStringLiteral("auth"), this);
}

void VsAuth::authenticate(QString currentRefreshToken)
{
    if (currentRefreshToken.isEmpty()) {
        m_deviceCodeFlow->grant();
    } else {
        refreshAccessToken(currentRefreshToken);
    }
}

void VsAuth::initializedCodeFlow(QString code, QString verificationUrl)
{
    m_deviceCode          = code;
    m_authenticationStage = QStringLiteral("device flow");

    std::cout << "Verify at: " << verificationUrl.toStdString() << std::endl;

    emit updatedAuthenticationStage(m_authenticationStage);
    emit updatedDeviceCode(m_deviceCode);
}

void VsAuth::fetchUserInfo(QString accessToken)
{
    QNetworkReply* reply = m_api->getAuth0UserInfo();
    connect(reply, &QNetworkReply::finished, this, [=]() {
        if (reply->error() != QNetworkReply::NoError) {
            std::cout << "Error: " << reply->errorString().toStdString() << std::endl;
            authFailed();
            reply->deleteLater();
            return;
        }

        QByteArray response    = reply->readAll();
        QJsonDocument userInfo = QJsonDocument::fromJson(response);
        QString userId         = userInfo.object()[QStringLiteral("sub")].toString();

        authSucceeded(userId, accessToken);
    });
}

void VsAuth::refreshAccessToken(QString refreshToken)
{
    QNetworkRequest request = QNetworkRequest(
        QUrl(QString("https://%1/oauth/token").arg(m_authorizationServerHost)));

    request.setRawHeader(QByteArray("Content-Type"),
                         QByteArray("application/x-www-form-urlencoded"));

    QString data = QString("grant_type=refresh_token&client_id=%1&refresh_token=%2")
                       .arg(m_clientId, refreshToken);

    // send request
    QNetworkReply* reply = m_networkAccessManager->post(request, data.toUtf8());

    connect(reply, &QNetworkReply::finished, this, [=]() {
        QByteArray buffer = reply->readAll();

        // Error: failed to get device code
        if (reply->error()) {
            std::cout << "Failed to get new access token: " << buffer.toStdString()
                      << std::endl;
            authFailed();
            reply->deleteLater();
            return;
        }

        // parse JSON from string response
        QJsonParseError parseError;
        QJsonDocument data = QJsonDocument::fromJson(buffer, &parseError);
        if (parseError.error) {
            std::cout << "Error parsing JSON for Access Token: "
                      << parseError.errorString().toStdString() << std::endl;
            authFailed();
            reply->deleteLater();
            return;
        }

        // get fields
        QJsonObject object  = data.object();
        QString accessToken = object.value(QLatin1String("access_token")).toString();
        m_api->setAccessToken(accessToken);
        fetchUserInfo(accessToken);
        reply->deleteLater();
    });
}

void VsAuth::codeFlowCompleted(QString accessToken)
{
    m_api->setAccessToken(accessToken);
    fetchUserInfo(accessToken);
}

void VsAuth::authSucceeded(QString userId, QString accessToken)
{
    std::cout << "Successful Authentication!" << std::endl;
    std::cout << "User ID: " << m_userId.toStdString();

    m_userId              = userId;
    m_deviceCode          = QStringLiteral("");
    m_accessToken         = accessToken;
    m_authenticationStage = QStringLiteral("success");
    m_isAuthenticated     = true;

    emit updatedUserId(m_userId);
    emit updatedAuthenticationStage(m_authenticationStage);
    emit updatedDeviceCode(m_deviceCode);
    emit updatedIsAuthenticated(m_isAuthenticated);
}

void VsAuth::authFailed()
{
    std::cout << "Failed Authentication!" << std::endl;
    
    m_userId              = QStringLiteral("");
    m_deviceCode          = QStringLiteral("");
    m_accessToken         = QStringLiteral("");
    m_authenticationStage = QStringLiteral("failed");
    m_isAuthenticated     = false;

    emit updatedUserId(m_userId);
    emit updatedAuthenticationStage(m_authenticationStage);
    emit updatedDeviceCode(m_deviceCode);
    emit updatedIsAuthenticated(m_isAuthenticated);
}

void VsAuth::logout()
{
    if (!m_isAuthenticated) {
        std::cout << "Warning: attempting to logout while not authenticated" << std::endl;
    }

    m_userId              = QStringLiteral("");
    m_deviceCode          = QStringLiteral("");
    m_accessToken         = QStringLiteral("");
    m_authenticationStage = QStringLiteral("unauthenticated");
    m_isAuthenticated     = false;

    emit updatedUserId(m_userId);
    emit updatedAuthenticationStage(m_authenticationStage);
    emit updatedDeviceCode(m_deviceCode);
    emit updatedIsAuthenticated(m_isAuthenticated);
}