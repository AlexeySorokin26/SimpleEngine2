#pragma once

#include <string>

#include "stb_image.h"

namespace SimpleEngine {

	class Texture2D {
	public:
		Texture2D(const std::string& fileLocation, const int w, const int h);
		~Texture2D();
		Texture2D(const Texture2D&) = delete;
		Texture2D& operator=(const Texture2D&) = delete;
		Texture2D& operator=(Texture2D&& texture) noexcept;
		Texture2D(Texture2D&& texture) noexcept;
		void bind(const unsigned int unit) const;
	private:
		unsigned int m_id = 0;
		int m_width = 0;
		int m_height = 0;
		int nrChannels = 0; // rgba

		char* fileLocation;
	};

}