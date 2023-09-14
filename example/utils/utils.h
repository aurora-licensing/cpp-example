#pragma once
#include <Windows.h>
#include <string>
#include <fstream>
#include <sddl.h>
#include <iostream>
#include <vector>
#include <algorithm>

namespace utils {
	std::string get_hwid();

	bool file_exists(const std::string& filename);
	std::string read_license(const std::string& filename);
	void save_license(const std::string& licenseKey, const std::string& filename);
}
