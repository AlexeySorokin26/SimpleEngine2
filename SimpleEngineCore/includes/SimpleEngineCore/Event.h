#pragma once

#include <functional>
#include <array>

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
		//void add_event_listener(std::function<void(BaseEvent&)> callback) {
		//	auto baseCallback = [func = std::move(callback)](BaseEvent& e)
		//		{
		//			func(static_cast<TEventType&>(e));
		//		};
		//	m_eventCallbacks[static_cast<size_t>(TEventType::type)] = std::move(baseCallback);
		//}

		template<typename TEventType>
		void add_event_listener(std::function<void(TEventType&)> callback) {
			auto baseCallback = [func = std::move(callback)](BaseEvent& e)
				{
					func(static_cast<TEventType&>(e));
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
}