#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <Windows.h>
#include <wincrypt.h>

class ApplicationHashCalculator {
public:
    std::string calculateFileHash(const std::string& filePath) {
        std::ifstream file(filePath, std::ios::binary);
        if (!file) {
            throw std::runtime_error("Failed to open file: " + filePath);
        }

        std::stringstream ss;
        ss << file.rdbuf();
        std::string fileContents = ss.str();

        HCRYPTPROV hProv = NULL;
        HCRYPTHASH hHash = NULL;

        if (!CryptAcquireContext(&hProv, NULL, NULL, PROV_RSA_AES, CRYPT_VERIFYCONTEXT)) {
            throw std::runtime_error("CryptAcquireContext failed.");
        }

        if (!CryptCreateHash(hProv, CALG_SHA_256, 0, 0, &hHash)) {
            CryptReleaseContext(hProv, 0);
            throw std::runtime_error("CryptCreateHash failed.");
        }

        if (!CryptHashData(hHash, reinterpret_cast<const BYTE*>(fileContents.c_str()), fileContents.length(), 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw std::runtime_error("CryptHashData failed.");
        }

        DWORD hashSize = 0;
        DWORD hashSizeLen = sizeof(DWORD);
        if (!CryptGetHashParam(hHash, HP_HASHSIZE, reinterpret_cast<BYTE*>(&hashSize), &hashSizeLen, 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw std::runtime_error("CryptGetHashParam failed.");
        }

        std::vector<BYTE> hashData(hashSize);
        if (!CryptGetHashParam(hHash, HP_HASHVAL, hashData.data(), &hashSize, 0)) {
            CryptDestroyHash(hHash);
            CryptReleaseContext(hProv, 0);
            throw std::runtime_error("CryptGetHashParam failed.");
        }

        CryptDestroyHash(hHash);
        CryptReleaseContext(hProv, 0);

        std::stringstream hashStream;
        hashStream << std::hex << std::setfill('0');
        for (BYTE b : hashData) {
            hashStream << std::setw(2) << static_cast<int>(b);
        }

        return hashStream.str();
    }

    std::string getCurrentApplicationHash() {
        HMODULE hModule = GetModuleHandle(NULL);
        char filePath[MAX_PATH];
        if (GetModuleFileName(hModule, filePath, MAX_PATH) == 0) {
            throw std::runtime_error("Failed to get the path of the current executable.");
        }

        return calculateFileHash(filePath);
    }
};