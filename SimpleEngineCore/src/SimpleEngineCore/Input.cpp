#include "SimpleEngineCore/Input.h"

namespace SimpleEngine {
	bool Input::m_keys_pressed[static_cast<size_t>(KeyCode::KEY_LAST) + 1] = {};
	bool Input::IsKeyPressed(const KeyCode key_code)
	{
		return m_keys_pressed[static_cast<size_t>(key_code)];
	}
	void Input::PressKey(const KeyCode key_code)
	{
		m_keys_pressed[static_cast<size_t>(key_code)] = true;
	}
	void Input::ReleaseKey(const KeyCode key_code)
	{
		m_keys_pressed[static_cast<size_t>(key_code)] = false;
	}
	bool Input::m_mouse_buttons_pressed[static_cast<size_t>(MouseButtonCode::MOUSE_BUTTON_LAST) + 1] = {};
	bool Input::IsMouseButtonPressed(const MouseButtonCode key_code)
	{
		return m_mouse_buttons_pressed[static_cast<size_t>(key_code)];
	}
	void Input::PressMouseButton(const MouseButtonCode key_code)
	{
		m_mouse_buttons_pressed[static_cast<size_t>(key_code)] = true;
	}
	void Input::ReleaseMouseButton(const MouseButtonCode key_code)
	{
		m_mouse_buttons_pressed[static_cast<size_t>(key_code)] = false;
	}
}