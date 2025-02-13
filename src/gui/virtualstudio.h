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
 * \file virtualstudio.h
 * \author Aaron Wyatt
 * \date March 2022
 */

#ifndef VIRTUALSTUDIO_H
#define VIRTUALSTUDIO_H

#include <QList>
#include <QMutex>
#include <QScopedPointer>
#include <QSharedPointer>
#include <QTimer>
#include <QtNetworkAuth>

#include "../JackTrip.h"
#include "vsDevice.h"
#include "vsQuickView.h"
#include "vsServerInfo.h"
#include "vsUrlHandler.h"
#include "vsWebSocket.h"

#ifdef __APPLE__
#include "NoNap.h"
#endif

class QJackTrip;

class VirtualStudio : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showFirstRun READ showFirstRun NOTIFY showFirstRunChanged)
    Q_PROPERTY(bool hasRefreshToken READ hasRefreshToken NOTIFY hasRefreshTokenChanged)
    Q_PROPERTY(QString versionString READ versionString CONSTANT)
    Q_PROPERTY(QString logoSection READ logoSection NOTIFY logoSectionChanged)
    Q_PROPERTY(bool selectableBackend READ selectableBackend CONSTANT)
    Q_PROPERTY(QString audioBackend READ audioBackend WRITE setAudioBackend NOTIFY
                   audioBackendChanged)
    Q_PROPERTY(
        int inputDevice READ inputDevice WRITE setInputDevice NOTIFY inputDeviceChanged)
    Q_PROPERTY(int outputDevice READ outputDevice WRITE setOutputDevice NOTIFY
                   outputDeviceChanged)
    Q_PROPERTY(
        int bufferSize READ bufferSize WRITE setBufferSize NOTIFY bufferSizeChanged)
    Q_PROPERTY(int currentStudio READ currentStudio NOTIFY currentStudioChanged)
    Q_PROPERTY(QJsonObject regions READ regions NOTIFY regionsChanged)
    Q_PROPERTY(QJsonObject userMetadata READ userMetadata NOTIFY userMetadataChanged)
    Q_PROPERTY(bool showInactive READ showInactive WRITE setShowInactive NOTIFY
                   showInactiveChanged)
    Q_PROPERTY(bool showSelfHosted READ showSelfHosted WRITE setShowSelfHosted NOTIFY
                   showSelfHostedChanged)
    Q_PROPERTY(QString connectionState READ connectionState NOTIFY connectionStateChanged)
    Q_PROPERTY(QJsonObject networkStats READ networkStats NOTIFY networkStatsChanged)
    Q_PROPERTY(QString updateChannel READ updateChannel WRITE setUpdateChannel NOTIFY
                   updateChannelChanged)
    Q_PROPERTY(float fontScale READ fontScale CONSTANT)
    Q_PROPERTY(float uiScale READ uiScale WRITE setUiScale NOTIFY uiScaleChanged)
    Q_PROPERTY(bool darkMode READ darkMode WRITE setDarkMode NOTIFY darkModeChanged)
    Q_PROPERTY(bool showDeviceSetup READ showDeviceSetup WRITE setShowDeviceSetup NOTIFY
                   showDeviceSetupChanged)
    Q_PROPERTY(bool showWarnings READ showWarnings WRITE setShowWarnings NOTIFY
                   showWarningsChanged)
    Q_PROPERTY(bool noUpdater READ noUpdater CONSTANT)
    Q_PROPERTY(bool psiBuild READ psiBuild CONSTANT)
    Q_PROPERTY(QString failedMessage READ failedMessage NOTIFY failedMessageChanged)

   public:
    explicit VirtualStudio(bool firstRun = false, QObject* parent = nullptr);
    ~VirtualStudio() override;

    void setStandardWindow(QSharedPointer<QJackTrip> window);
    void show();
    void raiseToTop();

    bool showFirstRun();
    bool hasRefreshToken();
    QString versionString();
    QString logoSection();
    bool selectableBackend();
    QString audioBackend();
    void setAudioBackend(const QString& backend);
    int inputDevice();
    void setInputDevice(int device);
    int outputDevice();
    void setOutputDevice(int device);
    int bufferSize();
    void setBufferSize(int index);
    int currentStudio();
    QJsonObject regions();
    QJsonObject userMetadata();
    QString connectionState();
    QJsonObject networkStats();
    QString updateChannel();
    void setUpdateChannel(const QString& channel);
    bool showInactive();
    void setShowInactive(bool inactive);
    bool showSelfHosted();
    void setShowSelfHosted(bool selfHosted);
    float fontScale();
    float uiScale();
    void setUiScale(float scale);
    bool darkMode();
    void setDarkMode(bool dark);
    QUrl studioToJoin();
    void setStudioToJoin(const QUrl& url);
    bool showDeviceSetup();
    void setShowDeviceSetup(bool show);
    bool showWarnings();
    void setShowWarnings(bool show);
    bool noUpdater();
    bool psiBuild();
    QString failedMessage();

   public slots:
    void toStandard();
    void toVirtualStudio();
    void login();
    void logout();
    void refreshStudios(int index);
    void refreshDevices();
    void revertSettings();
    void applySettings();
    void connectToStudio(int studioIndex);
    void completeConnection();
    void disconnect();
    void manageStudio(int studioIndex);
    void createStudio();
    void editProfile();
    void showAbout();
    void exit();

   signals:
    void authSucceeded();
    void authFailed();
    void failed();
    void connected();
    void disconnected();
    void refreshFinished(int index);
    void showFirstRunChanged();
    void hasRefreshTokenChanged();
    void logoSectionChanged();
    void audioBackendChanged();
    void inputDeviceChanged();
    void outputDeviceChanged();
    void bufferSizeChanged();
    void currentStudioChanged();
    void regionsChanged();
    void userMetadataChanged();
    void showInactiveChanged();
    void showSelfHostedChanged();
    void connectionStateChanged();
    void networkStatsChanged();
    void updateChannelChanged();
    void showDeviceSetupChanged();
    void showWarningsChanged();
    void uiScaleChanged();
    void newScale();
    void darkModeChanged();
    void studioToJoinChanged();
    void signalExit();
    void periodicRefresh();
    void failedMessageChanged();

   private slots:
    void slotAuthSucceded();
    void slotAuthFailed();
    void processFinished();
    void processError(const QString& errorMessage);
    void receivedConnectionFromPeer();
    void checkForHostname();
    void endRetryPeriod();
    void launchBrowser(const QUrl& url);
    void joinStudio();
    void updatedStats(const QJsonObject& stats);

   private:
    void setupAuthenticator();

    void sendHeartbeat();
    void getServerList(bool firstLoad = false, int index = -1);
    void getUserId();
    void getSubscriptions();
    void getRegions();
    void getUserMetadata();
#ifdef RT_AUDIO
    void getDeviceList(QStringList* list, bool isInput);
#endif
    void stopStudio();

    bool m_showFirstRun = false;
    bool m_checkSsl     = true;
    QString m_updateChannel;
    QString m_refreshToken;
    QString m_userId;
    VsQuickView m_view;
    QSharedPointer<QJackTrip> m_standardWindow;
    QScopedPointer<QOAuth2AuthorizationCodeFlow> m_authenticator;

    QList<QObject*> m_servers;
    QStringList m_subscribedServers;
    QJsonObject m_regions;
    QJsonObject m_userMetadata;
    QString m_logoSection     = QStringLiteral("Your Studios");
    bool m_selectableBackend  = true;
    bool m_useRtAudio         = false;
    int m_currentStudio       = -1;
    QString m_connectionState = QStringLiteral("Waiting");
    QScopedPointer<JackTrip> m_jackTrip;
    QTimer m_startTimer;
    QTimer m_retryPeriodTimer;
    bool m_startedStudio = false;
    bool m_retryPeriod;
    bool m_jackTripRunning = false;

    QTimer m_refreshTimer;
    QMutex m_refreshMutex;
    bool m_allowRefresh      = true;
    bool m_refreshInProgress = false;

    QJsonObject m_networkStats;

    QTimer m_heartbeatTimer;
    VsWebSocket* m_heartbeatWebSocket = NULL;
    VsDevice* m_device                = NULL;

    bool m_onConnectedScreen = false;
    bool m_isExiting         = false;
    bool m_showInactive      = false;
    bool m_showSelfHosted    = false;
    bool m_showDeviceSetup   = true;
    bool m_showWarnings      = true;
    float m_fontScale        = 1;
    float m_uiScale;
    float m_previousUiScale;
    bool m_darkMode         = false;
    QString m_failedMessage = "";
    QUrl m_studioToJoin;
    bool m_authenticated = false;

#ifdef RT_AUDIO
    QStringList m_inputDeviceList;
    QStringList m_outputDeviceList;
    QString m_inputDevice;
    QString m_outputDevice;
    quint16 m_bufferSize;
    QString m_previousInput;
    QString m_previousOutput;
    quint16 m_previousBuffer;
    bool m_previousUseRtAudio = false;
#endif
    QStringList m_bufferOptions        = {"16", "32", "64", "128", "256", "512", "1024"};
    QStringList m_updateChannelOptions = {"Stable", "Edge"};

#ifdef __APPLE__
    NoNap m_noNap;
#endif
};

#endif  // VIRTUALSTUDIO_H
