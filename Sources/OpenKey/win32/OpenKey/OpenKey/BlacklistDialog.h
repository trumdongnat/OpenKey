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
    HWND hButtonSelectApp;
    HWND hListBlacklist;
    HWND hButtonAdd;
    HWND hButtonRemove;
    HWND hButtonClose;
    
    set<string> blacklistApps;
    vector<string> runningApps;
    
    // Window selection variables
    bool isSelectingWindow;
    HCURSOR originalCursor;
    HWND hOverlayWindow;
    
public:
    vector<HWND> hiddenWindows; // Make it public for callback access

private:
    void initDialog();
    void loadBlacklistFromFile();
    void saveBlacklistToFile();
    void populateRunningApps();
    void populateBlacklistView();
    void onAddButton();
    void onRemoveButton();
    void onBrowseButton();
    void onSelectAppButton();
    void addAppToBlacklist(const string& appName);
    void removeAppFromBlacklist(const string& appName);
    string getBlacklistFilePath();
    
    // Helper function to check if a process has GUI windows
    bool hasGUIWindows(DWORD processId);
    
    // Window selection methods
    void startWindowSelection();
    void endWindowSelection();
    void hideAllOpenKeyWindows();
    void showAllOpenKeyWindows();
    static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
    static BlacklistDialog* s_instance;

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

// Forward declaration for callback
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);