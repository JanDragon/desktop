/*
 * Copyright (C) by Klaas Freitag <freitag@owncloud.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 */

#ifndef OWNCLOUDGUI_H
#define OWNCLOUDGUI_H

#include "systray.h"
#include "connectionvalidator.h"
#include "progressdispatcher.h"

#include <QObject>
#include <QPointer>
#include <QAction>
#include <QMenu>
#include <QSize>
#include <QTimer>
#ifdef WITH_LIBCLOUDPROVIDERS
#include <QDBusConnection>
#endif

namespace OCC {

class Folder;

class SettingsDialog;
class ShareDialog;
class Application;
class LogBrowser;
class AccountState;

enum class ShareDialogStartPage {
    UsersAndGroups,
    PublicLinks,
};

/**
 * @brief The ownCloudGui class
 * @ingroup gui
 */
class ownCloudGui : public QObject
{
    Q_OBJECT
public:
    explicit ownCloudGui(Application *parent = nullptr);

    bool checkAccountExists(bool openSettings);

    static void raiseDialog(QWidget *raiseWidget);
    static QSize settingsDialogSize() { return QSize(800, 500); }
    void setupOverlayIcons();
#ifdef WITH_LIBCLOUDPROVIDERS
    void setupCloudProviders();
    bool cloudProviderApiAvailable();
#endif

    /// Whether the tray menu is visible
    bool contextMenuVisible() const;

signals:
    void setupProxy();
    void serverError(int code, const QString &message);
    void isShowingSettingsDialog();

public slots:
    void setupContextMenu();
    void updateContextMenu();
    void updateContextMenuNeeded();
    void slotContextMenuAboutToShow();
    void slotContextMenuAboutToHide();
    void slotComputeOverallSyncStatus();
    void slotShowTrayMessage(const QString &title, const QString &msg);
    void slotShowOptionalTrayMessage(const QString &title, const QString &msg);
    void slotFolderOpenAction(const QString &alias);
    void slotRebuildRecentMenus();
    void slotUpdateProgress(const QString &folder, const ProgressInfo &progress);
    void slotShowGuiMessage(const QString &title, const QString &message);
    void slotFoldersChanged();
    void slotShowSettings();
    void slotShowSyncProtocol();
    void slotShutdown();
    void slotSyncStateChange(Folder *);
    void slotTrayClicked(QSystemTrayIcon::ActivationReason reason);
    void slotToggleLogBrowser();
    void slotOpenOwnCloud();
    void slotOpenSettingsDialog();
    void slotSettingsDialogActivated();
    void slotHelp();
    void slotOpenPath(const QString &path);
    void slotAccountStateChanged();
    void slotTrayMessageIfServerUnsupported(Account *account);
    void slotNavigationAppsFetched(const QJsonDocument &reply, int statusCode);
    void slotEtagResponseHeaderReceived(const QByteArray &value, int statusCode);


    /**
     * Open a share dialog for a file or folder.
     *
     * sharePath is the full remote path to the item,
     * localPath is the absolute local path to it (so not relative
     * to the folder).
     */
    void slotShowShareDialog(const QString &sharePath, const QString &localPath, ShareDialogStartPage startPage);

    void slotRemoveDestroyedShareDialogs();

protected slots:
    void slotOcsError(int statusCode, const QString &message);

private slots:
    void slotLogin();
    void slotLogout();
    void slotUnpauseAllFolders();
    void slotPauseAllFolders();
    void slotNewAccountWizard();

private:
    void setPauseOnAllFoldersHelper(bool pause);
    void setupActions();
    void addAccountContextMenu(AccountStatePtr accountState, QMenu *menu, bool separateMenu);
    void fetchNavigationApps(AccountStatePtr account);
    void buildNavigationAppsMenu(AccountStatePtr account, QMenu *accountMenu);

    QPointer<Systray> _tray;
    QPointer<SettingsDialog> _settingsDialog;
    QPointer<LogBrowser> _logBrowser;
    // tray's menu
    QScopedPointer<QMenu> _contextMenu;

    // Manually tracking whether the context menu is visible via aboutToShow
    // and aboutToHide. Unfortunately aboutToHide isn't reliable everywhere
    // so this only gets used with _workaroundManualVisibility (when the tray's
    // isVisible() is unreliable)
    bool _contextMenuVisibleManual = false;

#ifdef WITH_LIBCLOUDPROVIDERS
    QDBusConnection _bus;
#endif

    QMenu *_recentActionsMenu;
    QVector<QMenu *> _accountMenus;
    bool _workaroundShowAndHideTray = false;
    bool _workaroundNoAboutToShowUpdate = false;
    bool _workaroundFakeDoubleClick = false;
    bool _workaroundManualVisibility = false;
    QTimer _delayedTrayUpdateTimer;
    QMap<QString, QPointer<ShareDialog>> _shareDialogs;

    QAction *_actionNewAccountWizard;
    QAction *_actionSettings;
    QAction *_actionStatus;
    QAction *_actionEstimate;
    QAction *_actionRecent;
    QAction *_actionHelp;
    QAction *_actionQuit;
    QAction *_actionCrash;

    QMenu *_navLinksMenu;
    QMap<AccountStatePtr, QJsonArray> _navApps;

    QList<QAction *> _recentItemsActions;
    Application *_app;
};

} // namespace OCC

#endif // OWNCLOUDGUI_H
