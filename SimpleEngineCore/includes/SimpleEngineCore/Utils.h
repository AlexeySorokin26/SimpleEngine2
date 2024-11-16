#pragma once 

#include <filesystem>
#include <string>

static std::filesystem::path getBasePath() {
	// Get the current working directory
	std::filesystem::path sourcePath = SOURCE_DIR;

	// Construct the path relative to the current directory
	// / it handles the file system paths / or \ or // correctly across different platforms
	std::filesystem::path basePath = sourcePath / "SimpleEngineCore";

	return basePath;
}
