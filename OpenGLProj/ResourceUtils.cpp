#include "ResourceUtils.h"

#include <filesystem>
#include <format>

void assertFileExists(const std::string& filePath)
{
	if (!std::filesystem::exists(filePath))
		throw std::exception(std::format("Resource file does not exist: {}", filePath).c_str());
}
