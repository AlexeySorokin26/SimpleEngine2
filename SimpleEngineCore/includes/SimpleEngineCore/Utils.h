#pragma once 

#include <filesystem>
#include <string>

static std::string getBasePath() {
	// Get the current working directory
	std::filesystem::path currentPath = std::filesystem::current_path();

	// Construct the path relative to the current directory
	// \ it handles the file system paths correctly across different platforms
	std::filesystem::path basePath = currentPath / "SimpleEngine2" / "SimpleEngineCore";

	return basePath.string();
}