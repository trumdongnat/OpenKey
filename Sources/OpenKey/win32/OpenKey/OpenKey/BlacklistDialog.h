/*----------------------------------------------------------
OpenKey - The Cross platform Open source Vietnamese Keyboard application.

Copyright (C) 2019 Mai Vu Tuyen
Contact: maivutuyen.91@gmail.com
Github: https://github.com/tuyenvm/OpenKey
Fanpage: https://www.facebook.com/OpenKeyVN

This file is belong to the OpenKey project, Win32 version
which is released under GPL license.
You can fork, modify, improve this program. If you
redistribute your new version, it MUST be open source.
-----------------------------------------------------------*/
#pragma once
#include "BaseDialog.h"
#include <vector>
#include <string>
#include <set>

using namespace std;

class BlacklistDialog : public BaseDialog {
private:
    HWND hComboRunningApps;
    HWND hButtonBrowse;
    HWND hListBlacklist;
    HWND hButtonAdd;
    HWND hButtonRemove;
    HWND hButtonClose;
    
    set<string> blacklistApps;
    vector<string> runningApps;

private:
    void initDialog();
    void loadBlacklistFromFile();
    void saveBlacklistToFile();
    void populateRunningApps();
    void populateBlacklistView();
    void onAddButton();
    void onRemoveButton();
    void onBrowseButton();
    void addAppToBlacklist(const string& appName);
    void removeAppFromBlacklist(const string& appName);
    string getBlacklistFilePath();

protected:
    INT_PTR eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
    BlacklistDialog(const HINSTANCE& hInstance, const int& resourceId);
    virtual ~BlacklistDialog();
    virtual void fillData() override;
    
    static bool isAppBlacklisted(const string& appName);
    static void loadBlacklistApps();
    static set<string> s_blacklistApps;
};