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

    // Return when the license was used on
    aurora.usedDate(license);
    if (aurora.result.valid == true) {
        // If the request was successful, return the date the license was used
        std::cout << xorstr_("License redeemed on the: ") << aurora.result.response << std::endl;
    }

    // Check the subscription level of the license
    aurora.checkLicenseSub(license);
    if (aurora.result.valid == true) {
        // If the subscription check was successful, display the subscription level
        std::cout << xorstr_("Subscription: ") << aurora.result.response << std::endl;
    }

    // Return the note for current license
    aurora.licenseNote(license);
    if (aurora.result.valid == true) {
        // If the request was successful, return the users note information
        std::cout << xorstr_("Note: ") << aurora.result.response << std::endl;
    }

    // Return the hwid for current license
    aurora.getHwid(license);
    if (aurora.result.valid == true) {
        // If the license hwid request was successful, display the information
        std::cout << xorstr_("HWID: ") << aurora.result.response << std::endl;
    }

    // Retrun the ip address for current license
    aurora.getIP(license);
    if (aurora.result.valid == true) {
        // If the license hwid request was successful, display the information
        std::cout << xorstr_("IP Address: ") << aurora.result.response << std::endl;
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


    // Send a webhook to your discord server with ease, you could use the getIP function also to for logging if debugger triggered ect.
    aurora.sendWebhook(botName, botUrl, xorstr_("User Login"), xorstr_("License: `") + license + xorstr_("`"));
    if (aurora.result.valid == true) {
        // If the webhook was successfully sent, display the response message
        std::cout << aurora.result.response << std::endl;
    }

    // Example on banning license, example if debuging ect
    //aurora.banLicense(license);

    return 0;
}
