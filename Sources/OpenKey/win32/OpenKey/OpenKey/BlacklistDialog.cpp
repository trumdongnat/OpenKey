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
#include "BlacklistDialog.h"
#include "AppDelegate.h"
#include "OpenKeyHelper.h"
#include <tlhelp32.h>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <commdlg.h>

set<string> BlacklistDialog::s_blacklistApps;

BlacklistDialog::BlacklistDialog(const HINSTANCE& hInstance, const int& resourceId)
    : BaseDialog(hInstance, resourceId) {
}

BlacklistDialog::~BlacklistDialog() {
}

void BlacklistDialog::initDialog() {
    SET_DIALOG_ICON(IDI_APP_ICON);
    
    hComboRunningApps = GetDlgItem(hDlg, IDC_COMBO_RUNNING_APPS);
    hButtonBrowse = GetDlgItem(hDlg, IDC_BUTTON_BROWSE_APP);
    hListBlacklist = GetDlgItem(hDlg, IDC_LIST_BLACKLIST_APPS);
    hButtonAdd = GetDlgItem(hDlg, IDC_BUTTON_BLACKLIST_ADD);
    hButtonRemove = GetDlgItem(hDlg, IDC_BUTTON_BLACKLIST_REMOVE);
    hButtonClose = GetDlgItem(hDlg, IDC_BUTTON_BLACKLIST_CLOSE);
    
    // Initialize list view
    ListView_SetExtendedListViewStyle(hListBlacklist, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
    
    LVCOLUMN lvc = {0};
    lvc.mask = LVCF_TEXT | LVCF_WIDTH;
    lvc.cx = 250;
    lvc.pszText = (LPWSTR)L"Application Name";
    ListView_InsertColumn(hListBlacklist, 0, &lvc);
    
    loadBlacklistFromFile();
    populateRunningApps();
    fillData();
}

void BlacklistDialog::fillData() {
    populateBlacklistView();
}

void BlacklistDialog::loadBlacklistFromFile() {
    blacklistApps.clear();
    string filePath = getBlacklistFilePath();
    
    ifstream file(filePath);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            // Remove whitespace and convert to uppercase
            line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
            if (!line.empty()) {
                transform(line.begin(), line.end(), line.begin(), ::toupper);
                blacklistApps.insert(line);
            }
        }
        file.close();
    }
    
    // Update static variable
    s_blacklistApps = blacklistApps;
}

void BlacklistDialog::saveBlacklistToFile() {
    string filePath = getBlacklistFilePath();
    
    ofstream file(filePath);
    if (file.is_open()) {
        for (const string& app : blacklistApps) {
            file << app << endl;
        }
        file.close();
    }
    
    // Update static variable
    s_blacklistApps = blacklistApps;
}

string BlacklistDialog::getBlacklistFilePath() {
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    
    // Get directory path
    wstring pathStr = exePath;
    size_t lastSlash = pathStr.find_last_of(L"\\");;
    if (lastSlash != wstring::npos) {
        pathStr = pathStr.substr(0, lastSlash + 1);
    }
    pathStr += L"blacklist.txt";
    
    // Convert to string
    string result;
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, pathStr.c_str(), -1, NULL, 0, NULL, NULL);
    if (size_needed > 0) {
        result.resize(size_needed - 1);
        WideCharToMultiByte(CP_UTF8, 0, pathStr.c_str(), -1, &result[0], size_needed, NULL, NULL);
    }
    return result;
}

void BlacklistDialog::populateRunningApps() {
    runningApps.clear();
    SendMessage(hComboRunningApps, CB_RESETCONTENT, 0, 0);
    
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) {
        return;
    }
    
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);
    
    if (Process32First(hSnapshot, &pe32)) {
        set<string> uniqueApps;
        do {
            string exeName;
            // Convert wide string to regular string
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, NULL, 0, NULL, NULL);
            if (size_needed > 0) {
                exeName.resize(size_needed - 1);
                WideCharToMultiByte(CP_UTF8, 0, pe32.szExeFile, -1, &exeName[0], size_needed, NULL, NULL);
            }
            
            transform(exeName.begin(), exeName.end(), exeName.begin(), ::toupper);
            
            if (uniqueApps.find(exeName) == uniqueApps.end() && 
                exeName != "SYSTEM" && exeName != "EXPLORER.EXE") {
                uniqueApps.insert(exeName);
                runningApps.push_back(exeName);
            }
        } while (Process32Next(hSnapshot, &pe32));
    }
    
    CloseHandle(hSnapshot);
    
    // Sort and add to combo box
    sort(runningApps.begin(), runningApps.end());
    for (const string& app : runningApps) {
        wstring wApp;
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, app.c_str(), -1, NULL, 0);
        if (size_needed > 0) {
            wApp.resize(size_needed - 1);
            MultiByteToWideChar(CP_UTF8, 0, app.c_str(), -1, &wApp[0], size_needed);
        }
        SendMessage(hComboRunningApps, CB_ADDSTRING, 0, (LPARAM)wApp.c_str());
    }
}

void BlacklistDialog::populateBlacklistView() {
    ListView_DeleteAllItems(hListBlacklist);
    
    int index = 0;
    for (const string& app : blacklistApps) {
        LVITEM lvi = {0};
        lvi.mask = LVIF_TEXT;
        lvi.iItem = index++;
        lvi.iSubItem = 0;
        
        wstring wApp;
        int size_needed = MultiByteToWideChar(CP_UTF8, 0, app.c_str(), -1, NULL, 0);
        if (size_needed > 0) {
            wApp.resize(size_needed - 1);
            MultiByteToWideChar(CP_UTF8, 0, app.c_str(), -1, &wApp[0], size_needed);
        }
        lvi.pszText = (LPWSTR)wApp.c_str();
        ListView_InsertItem(hListBlacklist, &lvi);
    }
}

void BlacklistDialog::onAddButton() {
    TCHAR buffer[256];
    GetWindowText(hComboRunningApps, buffer, 256);
    
    string appName;
    if (wcslen(buffer) > 0) {
        int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL, NULL);
        if (size_needed > 0) {
            appName.resize(size_needed - 1);
            WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &appName[0], size_needed, NULL, NULL);
        }
    }
    
    if (!appName.empty()) {
        addAppToBlacklist(appName);
    }
}

void BlacklistDialog::onRemoveButton() {
    int selectedItem = ListView_GetNextItem(hListBlacklist, -1, LVNI_SELECTED);
    if (selectedItem >= 0) {
        TCHAR buffer[256];
        ListView_GetItemText(hListBlacklist, selectedItem, 0, buffer, 256);
        
        string appName;
        if (wcslen(buffer) > 0) {
            int size_needed = WideCharToMultiByte(CP_UTF8, 0, buffer, -1, NULL, 0, NULL, NULL);
            if (size_needed > 0) {
                appName.resize(size_needed - 1);
                WideCharToMultiByte(CP_UTF8, 0, buffer, -1, &appName[0], size_needed, NULL, NULL);
            }
        }
        
        if (!appName.empty()) {
            removeAppFromBlacklist(appName);
        }
    }
}

void BlacklistDialog::onBrowseButton() {
    OPENFILENAME ofn = {0};
    TCHAR szFile[260] = {0};
    
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hDlg;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = L"Executable Files\0*.exe\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
    
    if (GetOpenFileName(&ofn)) {
        // Extract filename from full path
        wstring fullPath = szFile;
        size_t lastSlash = fullPath.find_last_of(L"\\");
        wstring fileName = (lastSlash != wstring::npos) ? fullPath.substr(lastSlash + 1) : fullPath;
        
        // Set to combo box
        SetWindowText(hComboRunningApps, fileName.c_str());
    }
}

void BlacklistDialog::addAppToBlacklist(const string& appName) {
    string upperAppName = appName;
    transform(upperAppName.begin(), upperAppName.end(), upperAppName.begin(), ::toupper);
    
    if (blacklistApps.find(upperAppName) == blacklistApps.end()) {
        blacklistApps.insert(upperAppName);
        saveBlacklistToFile();
        populateBlacklistView();
        
        // Clear combo box selection
        SetWindowText(hComboRunningApps, L"");
    }
}

void BlacklistDialog::removeAppFromBlacklist(const string& appName) {
    string upperAppName = appName;
    transform(upperAppName.begin(), upperAppName.end(), upperAppName.begin(), ::toupper);
    
    auto it = blacklistApps.find(upperAppName);
    if (it != blacklistApps.end()) {
        blacklistApps.erase(it);
        saveBlacklistToFile();
        populateBlacklistView();
    }
}

INT_PTR BlacklistDialog::eventProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_INITDIALOG:
        this->hDlg = hDlg;
        initDialog();
        return TRUE;
        
    case WM_COMMAND: {
        int wmId = LOWORD(wParam);
        switch (wmId) {
        case IDC_BUTTON_BLACKLIST_ADD:
            onAddButton();
            break;
        case IDC_BUTTON_BLACKLIST_REMOVE:
            onRemoveButton();
            break;
        case IDC_BUTTON_BROWSE_APP:
            onBrowseButton();
            break;
        case IDC_BUTTON_BLACKLIST_CLOSE:
        case IDCANCEL:
            AppDelegate::getInstance()->closeDialog(this);
            break;
        }
        break;
    }
    }
    
    return FALSE;
}

bool BlacklistDialog::isAppBlacklisted(const string& appName) {
    string upperAppName = appName;
    transform(upperAppName.begin(), upperAppName.end(), upperAppName.begin(), ::toupper);
    return s_blacklistApps.find(upperAppName) != s_blacklistApps.end();
}

void BlacklistDialog::loadBlacklistApps() {
    s_blacklistApps.clear();
    
    // Get blacklist file path
    TCHAR exePath[MAX_PATH];
    GetModuleFileName(NULL, exePath, MAX_PATH);
    
    // Get directory path
    wstring pathStr = exePath;
    size_t lastSlash = pathStr.find_last_of(L"\\");
    if (lastSlash != wstring::npos) {
        pathStr = pathStr.substr(0, lastSlash + 1);
    }
    pathStr += L"blacklist.txt";
    
    // Convert to string for file operations
    string filePath;
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, pathStr.c_str(), -1, NULL, 0, NULL, NULL);
    if (size_needed > 0) {
        filePath.resize(size_needed - 1);
        WideCharToMultiByte(CP_UTF8, 0, pathStr.c_str(), -1, &filePath[0], size_needed, NULL, NULL);
    }
    
    ifstream file(filePath);
    if (file.is_open()) {
        string line;
        while (getline(file, line)) {
            // Remove whitespace and convert to uppercase
            line.erase(remove_if(line.begin(), line.end(), ::isspace), line.end());
            if (!line.empty()) {
                transform(line.begin(), line.end(), line.begin(), ::toupper);
                s_blacklistApps.insert(line);
            }
        }
        file.close();
    }
}