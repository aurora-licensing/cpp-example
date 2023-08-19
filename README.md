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

Aurora aurora(xorstr_("name"), xorstr_("secret"), xorstr_("hash"), xorstr_("version"), xorstr_("https://aurora-licensing.pro/api/"));

// Using the urlEncode to add "spacing" within messages
void webHook(std::string embedName, std::string embedIcon, std::string embedTitle, std::string embedMessage) {
    aurora.sendWebhook(embedName, embedIcon, aurora.urlEncode(embedTitle), aurora.urlEncode(embedMessage));

    if (aurora.result.valid == true) {
        // If the webhook was successfully sent, display the response message
        std::cout << aurora.result.response << std::endl;
    }
}

int main()
{
    /*
    // Calculate the MD5 hash for the current application
    ApplicationHashCalculator hashCalculator;
    hash = hashCalculator.getCurrentApplicationHash();
    */

    // Initialize the API by sending an initialization request
    aurora.connectApi();

    // Check if the initialization was successful
    if (aurora.result.valid == false) {
        // If not successful, display the error message and exit
        std::cerr << aurora.result.response << std::endl;
        Sleep(1500); // Wait for 1500 milliseconds before exiting
        exit(0);
    }

    // Prompt the user to enter a license
    std::string license;
    std::cout << xorstr_("Enter license: ");
    std::cin >> license;

    // Check the validity of the license
    aurora.checkLicense(license);
    if (aurora.result.valid == false) {
        // If the license check was unsuccessful, display the error message and exit
        std::cerr << aurora.result.response << std::endl;
        Sleep(1500); // Wait for 1500 milliseconds before exiting
        exit(0);
    }

    // Check the expiry date of the license
    aurora.checkLicenseExpiry(license);
    if (aurora.result.valid == true) {
        // If the license expiry check was successful, display the expiry date
        std::cout << xorstr_("License expires on the: ") << aurora.result.response << std::endl;
    }

    // Check the subscription level of the license
    aurora.checkLicenseSub(license);
    if (aurora.result.valid == true) {
        // If the subscription check was successful, display the subscription level
        std::cout << xorstr_("Subscription: ") << aurora.result.response << std::endl;
    }

    // Get a variable message using a secret
    aurora.getVarMessage(xorstr_("secret"));
    if (aurora.result.valid == true) {
        // If the variable message retrieval was successful, display the message
        std::cout << aurora.result.response << std::endl;
    }

    // Create a vector to store the downloaded file content as bytes
    std::vector<uint8_t> bytes;

    // Call the downloadFile function of the 'aurora' object to download the file with "qweqwesdf" as the file secret.
    // The downloaded file content will be stored in the 'bytes' vector.
    aurora.downloadFile(xorstr_("secret"), bytes);

    // Check if the file download was successful (if the 'data.success' flag is true).
    if (aurora.result.valid == true) {
        /*
        // Save the file to disk or use the bytes to do something

        if (aurora.SaveFileToDisk(bytes, xorstr_("C:\\Users\\Ash\\Desktop\\file.exe"))) {
            std::cout << "File saved successfully." << std::endl;
        }
        else {
            std::cerr << xorstr_("Failed to save the file.") << std::endl;
        }
        */

        // Use bytes to do something
    }
    else {
        // If the file download was not successful, output the error message to the standard error stream.
        std::cerr << aurora.result.response << std::endl;
    }


    // Old method of sending webhook with urlEncode to support spacing
    //aurora.sendWebhook(xorstr_("bot name"), xorstr_("image url"), xorstr_("embed title"), xorstr_("embed message"));
    //if (aurora.result.valid == true) {
        // If the webhook was successfully sent, display the response message
    //    std::cout << aurora.result.response << std::endl;
    //}


    // Example to send webhook with 
    webHook(xorstr_("Aurora"), xorstr_("https://aurora-licensing.pro/images/logo.png"), xorstr_("Login Notification"), xorstr_("License: \n`") + license + xorstr_("`\n Just loggin to the loader."));

    // You can also send users ip address via webhook either standard format or using encoded message
    //webHook(xorstr_("Aurora"), xorstr_("https://aurora-licensing.pro/images/logo.png"), xorstr_("New Login"), xorstr_("License: `") + license + xorstr_("`\nIP Address: `") + utils::get_ip_address(false));

    // Example on banning license, example if debuging ect
    aurora.banLicense(license);

    return 0;
}
