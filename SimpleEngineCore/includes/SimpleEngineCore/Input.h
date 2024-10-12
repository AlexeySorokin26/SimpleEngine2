#pragma once

#include "Keys.h"

namespace SimpleEngine {
	class Input {
	public:
		static bool IsKeyPressed(const KeyCode key_code);
		static void PressKey(const KeyCode key_code);
		static void ReleaseKey(const KeyCode key_code);

		static bool IsMouseButtonPressed(const MouseButtonCode key_code);
		static void PressMouseButton(const MouseButtonCode key_code);
		static void ReleaseMouseButton(const MouseButtonCode key_code);
	private:
		static bool m_keys_pressed[];
		static bool m_mouse_buttons_pressed[];
	};
}