# Aurora Project

![Aurora Logo](/logo.png)

## Purpose

Aurora is a licensing and authentication system designed to help developers manage software licenses for their applications. It provides secure license validation, user authentication, and subscription management services.

## C++ API

The C++ API provided by Aurora allows developers to integrate the licensing and authentication functionality directly into their C++ applications. By using the API, developers can perform various tasks, including initializing the API, checking license validity, managing subscription levels, and downloading files securely.

## Utilizing the x86 Illustration

To use the x86 version of the loader, download the curl and replace the current curl files with the newly compiled files, then change the build to x86 from x64 and debug to release mode.

### How to Use the C++ API

```cpp
#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <conio.h>
#include <fstream> 
#include <xorstr.h>

#include <xorstr.h>
#include <aurora.h>
#include <hash.h>

Aurora aurora(xorstr_("app_name"), xorstr_("app_secret"), xorstr_("app_hash"), xorstr_("app_version"), xorstr_("https://aurora-licensing.pro/api/"));

std::string botName = (xorstr_("Aurora"));
std::string botUrl = (xorstr_("https://cdn.discordapp.com/attachments/1138554545811767409/1141806885595521064/favicon.png"));

int main()
{
    aurora.connectApi();
    if (aurora.result.valid == false) {
        std::cerr << aurora.result.response << std::endl;
        Sleep(1500);
        exit(0);
    }

    std::string license;
    std::cout << xorstr_("Enter license: ");
    std::cin >> license;

    aurora.checkLicense(license);
    if (aurora.result.valid == false) {
        std::cerr << aurora.result.response << std::endl;
        Sleep(1500);
        exit(0);
    }

    aurora.checkLicenseExpiry(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("License expires on the: ") << aurora.result.response << std::endl;
    }

    aurora.usedDate(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("License redeemed on the: ") << aurora.result.response << std::endl;
    }

    aurora.checkLicenseSub(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("Subscription: ") << aurora.result.response << std::endl;
    }

    aurora.licenseNote(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("Note: ") << aurora.result.response << std::endl;
    }

    aurora.getHwid(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("HWID: ") << aurora.result.response << std::endl;
    }

    aurora.getVarMessage(xorstr_("secret"));
    if (aurora.result.valid == true) {
        std::cout << aurora.result.response << std::endl;
    }

    std::vector<uint8_t> bytes;
    aurora.downloadFile(xorstr_("secret"), bytes);

    if (aurora.result.valid == true) {
        if (aurora.SaveFileToDisk(bytes, xorstr_("C:\\Users\\Ash\\Desktop\\file.exe"))) {
            std::cout << "File saved successfully." << std::endl;
        }
        else {
            std::cerr << xorstr_("Failed to save the file.") << std::endl;
        }
    }
    else {
        std::cerr << aurora.result.response << std::endl;
    }


    aurora.sendWebhook(botName, botUrl, xorstr_("User Login"), xorstr_("License: `") + license + xorstr_("`"));
    if (aurora.result.valid == true) {
        std::cout << aurora.result.response << std::endl;
    }

    return 0;
}
