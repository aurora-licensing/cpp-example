#include <Windows.h>
#include <iostream>
#include <string>
#include <sstream>
#include <conio.h>

#include <curl/curl.h>
#include <json/include/nlohmann/json.hpp>

#include <utils/utils.h>

using json = nlohmann::json;

// Response data structure
struct Response {
    bool valid;
    std::string response;
};

class Aurora {
public:
    std::string serverAddress;
    std::string name;
    std::string secret;
    std::string hash;
    std::string version;
    Response result; // Store API response here

    // Handle response information
    void handleError(const std::string& errorMessage) {
        result.valid = false;
        result.response = errorMessage;
    }

    // Helper function to handle HTTP response
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
        size_t totalSize = size * nmemb;
        output->append(static_cast<char*>(contents), totalSize);
        return totalSize;
    }

    // Helper function to handle writing the response into a buffer (vector)
    static size_t WriteToBufferCallback(void* contents, size_t size, size_t nmemb, std::vector<uint8_t>* output) {
        size_t totalSize = size * nmemb;
        output->insert(output->end(), static_cast<uint8_t*>(contents), static_cast<uint8_t*>(contents) + totalSize);
        return totalSize;
    }

    // Helper function to send HTTP GET request
    std::string sendGetRequest(const std::string& endpoint, const json& params) {
        std::string response;
        CURL* curl = curl_easy_init();

        if (curl) {
            // Build URL with query parameters
            std::string url = serverAddress + endpoint + "?";
            for (auto& param : params.items()) {
                url += param.key() + xorstr_("=") + param.value().get<std::string>() + xorstr_("&");
            }

            // Remove the trailing "&" character
            if (!params.empty()) {
                url.pop_back();
            }

            // Set up curl options
            curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

            // Perform the request
            CURLcode res = curl_easy_perform(curl);

            // Check for errors
            if (res != CURLE_OK) {
                std::cerr << xorstr_("Error while sending HTTP request: ") << curl_easy_strerror(res) << std::endl;
            }

            // Clean up curl
            curl_easy_cleanup(curl);
        }

        return response;
    }

    /*
    The official Internet standards are defined in the so-called RFCs (Requests for Comments).
    In the relevant RFC 3986, spaces are defined as 'unsafe characters'.
    It is stipulated that spaces must not be left untreated in a URL and must instead be converted (encoded).
    */
    std::string urlEncode(const std::string& value) {
        static const char* hexChars = "0123456789ABCDEF";
        std::ostringstream escaped;

        for (char c : value) {
            if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
                escaped << c;
            }
            else {
                escaped << '%' << hexChars[static_cast<unsigned char>(c) >> 4]
                    << hexChars[static_cast<unsigned char>(c) & 0xF];
            }
        }

        return escaped.str();
    }

public:
    Aurora(const std::string& name, const std::string& secret, const std::string& hash, const std::string& version, const std::string& serverAddress)
        : name(name), secret(secret), hash(hash), version(version), serverAddress(serverAddress) {}

    void connectApi() {
        try {
            json params = {
                { xorstr_("action"), xorstr_("init") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find("error") != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("message")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred during API call: " + std::string(e.what()));
        }
    }

    void checkLicense(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("check") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license },
                { xorstr_("hwid"), utils::get_hwid() }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("message")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while validating license: " + std::string(e.what()));
        }
    }

    void checkSession(const std::string& sessionId) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("check_session") },
                { xorstr_("id"), sessionId }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("message")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while checking the session: " + std::string(e.what()));
        }
    }

    void killSession(const std::string& sessionId) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("kill_session") },
                { xorstr_("id"), sessionId }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("message")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while killing the session: " + std::string(e.what()));
        }
    }

    void checkLicenseExpiry(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("check_expiry") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("expiry_date")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while checking expiry date: " + std::string(e.what()));
        }
    }

    void checkLicenseSub(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("get_license_sub") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("sub_level")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while checking license sub: " + std::string(e.what()));
        }
    }

    void getVarMessage(const std::string& varSecret) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("getvar") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("var"), varSecret }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("var_message")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while fetching var: " + std::string(e.what()));
        }
    }

    /* return url to download only
    void downloadFile(const std::string& fileSecret) {
        json params = {
            { xorstr_("action"), xorstr_("download") },
            { xorstr_("name"), name },
            { xorstr_("secret"), secret },
            { xorstr_("hash"), hash },
            { xorstr_("version"), version },
            { xorstr_("file_secret"), fileSecret }
        };

        std::string response = sendGetRequest(xorstr_("/index.php"), params);

        // Parse JSON response
        json jsonResponse = json::parse(response);

        if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
            result.valid = false;
            result.response = jsonResponse[xorstr_("error")].get<std::string>();
        }
        else {
            result.valid = true;
            result.response = jsonResponse[xorstr_("file_link")].get<std::string>();
        }
    }
    */

    void downloadFile(const std::string& fileSecret, std::vector<uint8_t>& fileData) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("download") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("file_secret"), fileSecret }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
                return;
            }

            // Check if the response contains a valid download link
            if (jsonResponse.find(xorstr_("file_link")) == jsonResponse.end()) {
                result.valid = false;
                result.response = xorstr_("Invalid or missing download link in the response.");
                return;
            }

            // Get the download link from the response
            std::string downloadLink = jsonResponse[xorstr_("file_link")].get<std::string>();

            CURL* curl = curl_easy_init();
            if (!curl) {
                std::cerr << xorstr_("Failed to initialize cURL") << std::endl;
                result.valid = false;
                result.response = xorstr_("Failed to initialize cURL.");
                return;
            }

            // Set up curl options for downloading the file
            curl_easy_setopt(curl, CURLOPT_URL, downloadLink.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToBufferCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fileData);

            // Perform the request to download the file
            CURLcode res = curl_easy_perform(curl);

            // Check for errors during the download
            if (res != CURLE_OK) {
                result.valid = false;
                result.response = xorstr_("Error while downloading file.");
            }
            else {
                result.valid = true;
                result.response = xorstr_("File successfully downloaded.");
            }

            // Clean up curl
            curl_easy_cleanup(curl);
        }
        catch (const std::exception& e) {
            handleError("An error occurred during file download: " + std::string(e.what()));
        }
    }

    void uniqueDownload(const std::string& id, const std::string& license, std::vector<uint8_t>& fileData) {
        try {
            if (id != xorstr_("1") && id != xorstr_("2")) {
                handleError(xorstr_("Invalid ID"));
                return;
            }

            json params = {
                { xorstr_("action"), xorstr_("unique_download") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license },
                { xorstr_("id"), id }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
                return;
            }

            // Check if the response contains a valid download link
            if (jsonResponse.find(xorstr_("download_link")) == jsonResponse.end()) {
                result.valid = false;
                result.response = xorstr_("Invalid or missing download link in the response.");
                return;
            }

            // Get the download link from the response
            std::string downloadLink;

            if (jsonResponse[xorstr_("download_link")].is_string()) {
                downloadLink = jsonResponse[xorstr_("download_link")].get<std::string>();
            }
            else if (jsonResponse[xorstr_("download_link")].is_number()) {
                // Convert the numeric value to a string
                downloadLink = std::to_string(jsonResponse[xorstr_("download_link")].get<int>());
            }
            else {
                result.valid = false;
                result.response = xorstr_("Invalid format for download link in the response.");
                return;
            }

            CURL* curl = curl_easy_init();
            if (!curl) {
                std::cerr << xorstr_("Failed to initialize cURL") << std::endl;
                result.valid = false;
                result.response = xorstr_("Failed to initialize cURL.");
                return;
            }

            // Set up curl options for downloading the file
            curl_easy_setopt(curl, CURLOPT_URL, downloadLink.c_str());
            curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteToBufferCallback);
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, &fileData);

            // Perform the request to download the file
            CURLcode res = curl_easy_perform(curl);

            // Check for errors during the download
            if (res != CURLE_OK) {
                result.valid = false;
                result.response = xorstr_("Error while downloading file.");
            }
            else {
                result.valid = true;
                result.response = xorstr_("File successfully downloaded.");
            }

            // Clean up curl
            curl_easy_cleanup(curl);
        }
        catch (const std::exception& e) {
            handleError(xorstr_("An error occurred during unique file download: ") + std::string(e.what()));
        }
    }

    void sendWebhook(const std::string& botName, const std::string& iconUrl, const std::string& embedTitle, const std::string& message) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("webhook") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("bot_name"), urlEncode(botName) },
                { xorstr_("image"), iconUrl },
                { xorstr_("title"), urlEncode(embedTitle) },
                { xorstr_("message"), urlEncode(message) },
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("message")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while submiting webhook request: " + std::string(e.what()));
        }
    }

    void banLicense(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("ban") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("message")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while banning the license: " + std::string(e.what()));
        }
    }

    void getHwid(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("get_hwid") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("hwid")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred during hwid information request: " + std::string(e.what()));
        }
    }

    void licenseNote(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("get_license_note") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("note")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while getting license information: " + std::string(e.what()));
        }
    }

    void usedDate(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("get_used_date") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("used_date")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while fetching used date: " + std::string(e.what()));
        }
    }

    void getIP(const std::string& license) {
        try {
            json params = {
                { xorstr_("action"), xorstr_("get_license_ip") },
                { xorstr_("name"), name },
                { xorstr_("secret"), secret },
                { xorstr_("hash"), hash },
                { xorstr_("version"), version },
                { xorstr_("license"), license }
            };

            std::string response = sendGetRequest(xorstr_("/index.php"), params);

            // Parse JSON response
            json jsonResponse = json::parse(response);

            if (jsonResponse.find(xorstr_("error")) != jsonResponse.end()) {
                result.valid = false;
                result.response = jsonResponse[xorstr_("error")].get<std::string>();
            }
            else {
                result.valid = true;
                result.response = jsonResponse[xorstr_("ip_address")].get<std::string>();
            }
        }
        catch (const std::exception& e) {
            handleError("An error occurred while fetching users ip address: " + std::string(e.what()));
        }
    }
};
