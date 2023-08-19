#include "utils.h"

namespace utils
{
    std::string get_hwid() {
        HANDLE h_token = nullptr;

        if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &h_token)) {
            GetLastError();
            return nullptr;
        }

        DWORD dw_buffer_size = 0;

        if (!GetTokenInformation(h_token, TokenUser, nullptr, 0, &dw_buffer_size) && (GetLastError() != ERROR_INSUFFICIENT_BUFFER)) {
            GetLastError();
            CloseHandle(h_token);
            h_token = nullptr;
            return nullptr;
        }

        std::vector<BYTE> buffer;
        buffer.resize(dw_buffer_size);
        const auto p_token_user = reinterpret_cast<PTOKEN_USER>(&buffer[0]);

        if (!GetTokenInformation(h_token, TokenUser, p_token_user, dw_buffer_size, &dw_buffer_size)) {
            GetLastError();

            CloseHandle(h_token);
            h_token = nullptr;

            return nullptr;
        }

        if (!IsValidSid(p_token_user->User.Sid)) {
            CloseHandle(h_token);
            h_token = nullptr;

            return nullptr;
        }

        CloseHandle(h_token);

        h_token = nullptr;
        LPSTR psz_sid = nullptr;

        if (!ConvertSidToStringSidA(p_token_user->User.Sid, &psz_sid)) {
            return nullptr;
        }

        std::string psz_sid_str(psz_sid);
        psz_sid_str += "69420";
        return std::string(psz_sid_str);
    }

    std::string get_ip_address(bool encoded) {
        std::string return_ip;

        if (encoded) {
            std::string ip_address = download_string("https://aurora-licensing.pro/api/ip_address.php");
            return_ip = base_64_encode(reinterpret_cast<const unsigned char*>(ip_address.c_str()), ip_address.length());
        }
        else if (!encoded) {
            return_ip = download_string("https://aurora-licensing.pro/api/ip_address.php");
        }

        return return_ip;
    }

    std::string read_license(const std::string& filename)
    {
        std::ifstream inFile(filename);
        std::string licenseKey;

        if (inFile.is_open()) {
            std::getline(inFile, licenseKey);
            inFile.close();
        }

        return licenseKey;
    }

    void save_license(const std::string& licenseKey, const std::string& filename)
    {
        std::ofstream outFile(filename);
        if (outFile.is_open()) {
            outFile << licenseKey;
            outFile.close();
        }
    }

    std::string replace_all(std::string subject, const std::string& search, const std::string& replace) {
        size_t pos = 0;

        while ((pos = subject.find(search, pos)) != std::string::npos) {
            subject.replace(pos, search.length(), replace);
            pos += replace.length();
        }

        return subject;
    }

    std::string download_string(std::string url) {
        HINTERNET interwebs = InternetOpenA("Mozilla/5.0", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL);
        HINTERNET urlFile;

        std::string rtn;

        if (interwebs) {
            urlFile = InternetOpenUrlA(interwebs, url.c_str(), NULL, NULL, NULL, NULL);

            if (urlFile) {
                char buffer[2000];
                DWORD bytesRead;

                do {
                    InternetReadFile(urlFile, buffer, 2000, &bytesRead);
                    rtn.append(buffer, bytesRead);
                    memset(buffer, 0, 2000);
                } while (bytesRead);

                InternetCloseHandle(interwebs);
                InternetCloseHandle(urlFile);

                std::string p = replace_all(rtn, "|n", "\r\n");
                return p;
            }
        }

        InternetCloseHandle(interwebs);

        std::string p = replace_all(rtn, "|n", "\r\n");
        return p;
    }

    std::string base_64_encode(const unsigned char* data, size_t length)
    {
        std::string encoded;
        int i = 0;
        int j = 0;
        unsigned char char_array_3[3];
        unsigned char char_array_4[4];

        while (length--) {
            char_array_3[i++] = *(data++);
            if (i == 3) {
                char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
                char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                    ((char_array_3[1] & 0xf0) >> 4);
                char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                    ((char_array_3[2] & 0xc0) >> 6);
                char_array_4[3] = char_array_3[2] & 0x3f;

                for (i = 0; i < 4; i++) {
                    encoded += encode[char_array_4[i]];
                }
                i = 0;
            }
        }

        if (i) {
            for (j = i; j < 3; j++) {
                char_array_3[j] = '\0';
            }

            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4) +
                ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) +
                ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;

            for (j = 0; j < i + 1; j++) {
                encoded += encode[char_array_4[j]];
            }

            while (i++ < 3) {
                encoded += '=';
            }
        }

        return encoded;
    }
}