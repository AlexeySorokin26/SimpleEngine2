#pragma once

#include <functional>
#include <array>

#include "Keys.h"

namespace SimpleEngine {

	enum class EventType {
		WindowResize = 0,
		WindowClose,

		KeyPressed,
		KeyReleased,

		MouseButtonPressed,
		MouseButtonReleased,
		MouseMoved,


		EventsCount
	};

	struct BaseEvent {
		virtual ~BaseEvent() = default;
		virtual EventType get_type() const = 0;
	};

	class EventDispatcher {
	public:
		template<typename TEventType>
		void add_event_listener(std::function<void(TEventType&)> callback) {
			auto baseCallback = [callback](BaseEvent& e)
				{
					callback(static_cast<TEventType&>(e));
				};
			m_eventCallbacks[static_cast<size_t>(TEventType::type)] = std::move(baseCallback);
		}

		void dispatch(BaseEvent& event) {
			auto& callback = m_eventCallbacks[static_cast<size_t>(event.get_type())];
			if (callback)
				callback(event);
		}
	private:
		std::array<std::function<void(BaseEvent&)>, static_cast<size_t>(EventType::EventsCount)> m_eventCallbacks;
	};

	struct EventWindowClosed : public BaseEvent {
		EventWindowClosed() {}
		EventType get_type() const override {
			return type;
		}
		static const EventType type = EventType::WindowClose;
	};

	struct EventMouseMoved : public BaseEvent {
		EventMouseMoved(const double x, const double y) :
			x(x), y(y) {}
		EventType get_type() const override {
			return type;
		}
		double x;
		double y;

		static const EventType type = EventType::MouseMoved;
	};

	struct EventWindowResize : public BaseEvent {
		EventWindowResize(const unsigned int w, const unsigned int h) :
			w(w), h(h) {}
		EventType get_type() const override {
			return type;
		}
		unsigned int w;
		unsigned int h;

		static const EventType type = EventType::WindowResize;
	};

	struct EventKeyPressed : public BaseEvent {
		EventKeyPressed(const KeyCode key_code, const bool repeat) :
			key_code(key_code), repeat(repeat) {}
		EventType get_type() const override {
			return type;
		}
		const KeyCode key_code;
		const bool repeat;

		static const EventType type = EventType::KeyPressed;
	};

	struct EventKeyReleased : public BaseEvent {
		EventKeyReleased(const KeyCode key_code) :
			key_code(key_code) {}
		EventType get_type() const override {
			return type;
		}
		const KeyCode key_code;

		static const EventType type = EventType::KeyReleased;
	};

	struct EventMouseButtonPressed : public BaseEvent {
		EventMouseButtonPressed(
			const MouseButtonCode mouse_button_code, const double x_pos, const double y_pos) :
			mouse_button_code(mouse_button_code), x_pos(x_pos), y_pos(y_pos) {}
		EventType get_type() const override { return type; }
		const MouseButtonCode mouse_button_code;
		const double x_pos; const double y_pos;
		static const EventType type = EventType::MouseButtonPressed;
	};

	struct EventMouseButtonReleased : public BaseEvent {
		EventMouseButtonReleased(
			const MouseButtonCode mouse_button_code, const double x_pos, const double y_pos) :
			mouse_button_code(mouse_button_code), x_pos(x_pos), y_pos(y_pos) {}
		EventType get_type() const override { return type; }
		const MouseButtonCode mouse_button_code;
		const double x_pos; const double y_pos;
		static const EventType type = EventType::MouseButtonReleased;
	};
}