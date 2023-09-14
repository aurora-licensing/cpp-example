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

    bool file_exists(const std::string& filename)
    {
        std::ifstream file(filename);
        return file.good();
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
}
