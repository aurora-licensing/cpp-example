#pragma once
#include <Windows.h>
#include <string>
#include <iostream>
#include <wininet.h>
#include <fstream>
#include <string>
#include <tchar.h>
#include <tlhelp32.h>
#include <vector>
#include <urlmon.h>
#include <sddl.h>

#pragma comment(lib, "WinINet.lib")
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "urlmon.lib")

namespace utils {
	const std::string encode = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" "abcdefghijklmnopqrstuvwxyz" "0123456789+/";
	std::string base_64_encode(const unsigned char* data, size_t length);

	std::string get_hwid();

	std::string read_license(const std::string& filename);
	void save_license(const std::string& licenseKey, const std::string& filename);

	std::string replace_all(std::string subject, const std::string& search, const std::string& replace);
	std::string download_string(std::string url);
}
