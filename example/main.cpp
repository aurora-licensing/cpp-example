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

int main() {
    // connect to the api
    aurora.connectApi();

    // check if the response was valid if not return error
    if (aurora.result.valid == false) {
        std::cerr << aurora.result.response << std::endl;
        Sleep(1500);
        exit(0);
    }

    // license key storage
    std::string license;

    // check to see if license file exists
    if (utils::file_exists(xorstr_("C:\\ProgramData\\key.txt"))) {
        // if the license file exists read the license key
        license = utils::read_license(xorstr_("C:\\ProgramData\\key.txt"));
    }
    else {
        // if the license file does not exist then promt user to input their license key
        std::cout << xorstr_("Enter license: ");
        std::cin >> license;
    }

    // check to see if the license key is valid
    aurora.checkLicense(license);
    if (aurora.result.valid == false) {
        // if the license key is invalid return error
        std::cerr << aurora.result.response << std::endl;
        Sleep(1500);
        exit(0);
    }
    else {
        // if the license key was valid save the license key
        utils::save_license(license, xorstr_("C:\\ProgramData\\key.txt"));
    }

    // check the expiry date of the license key
    aurora.checkLicenseExpiry(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("License expires on the: ") << aurora.result.response << std::endl;
    }

    // check the date the license key was used on
    aurora.usedDate(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("License redeemed on the: ") << aurora.result.response << std::endl;
    }

    // check the sub of the license key
    aurora.checkLicenseSub(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("Subscription: ") << aurora.result.response << std::endl;
    }

    // check the license key note, if added by application owner
    aurora.licenseNote(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("Note: ") << aurora.result.response << std::endl;
    }

    // get the hwid of the license key 
    aurora.getHwid(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("HWID: ") << aurora.result.response << std::endl;
    }

    // get the ip address of the license key
    aurora.getIP(license);
    if (aurora.result.valid == true) {
        std::cout << xorstr_("IP Address: ") << aurora.result.response << std::endl;
    }

    return 0;
}
