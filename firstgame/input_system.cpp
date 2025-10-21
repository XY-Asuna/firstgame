#include "input_system.h"

namespace {
	sf::Keyboard::Scan to_sfml_keycode(input_keycode key) {
		using Scan = sf::Keyboard::Scan;
		switch (key) {
		case input_keycode::A: return Scan::A;
		case input_keycode::B: return Scan::B;
		case input_keycode::C: return Scan::C;
		case input_keycode::D: return Scan::D;
		case input_keycode::E: return Scan::E;
		case input_keycode::F: return Scan::F;
		case input_keycode::G: return Scan::G;
		case input_keycode::H: return Scan::H;
		case input_keycode::I: return Scan::I;
		case input_keycode::J: return Scan::J;
		case input_keycode::K: return Scan::K;
		case input_keycode::L: return Scan::L;
		case input_keycode::M: return Scan::M;
		case input_keycode::N: return Scan::N;
		case input_keycode::O: return Scan::O;
		case input_keycode::P: return Scan::P;
		case input_keycode::Q: return Scan::Q;
		case input_keycode::R: return Scan::R;
		case input_keycode::S: return Scan::S;
		case input_keycode::T: return Scan::T;
		case input_keycode::U: return Scan::U;
		case input_keycode::V: return Scan::V;
		case input_keycode::W: return Scan::W;
		case input_keycode::X: return Scan::X;
		case input_keycode::Y: return Scan::Y;
		case input_keycode::Z: return Scan::Z;
		case input_keycode::NUM_0: return Scan::Num0;
		case input_keycode::NUM_1: return Scan::Num1;
		case input_keycode::NUM_2: return Scan::Num2;
		case input_keycode::NUM_3: return Scan::Num3;
		case input_keycode::NUM_4: return Scan::Num4;
		case input_keycode::NUM_5: return Scan::Num5;
		case input_keycode::NUM_6: return Scan::Num6;
		case input_keycode::NUM_7: return Scan::Num7;
		case input_keycode::NUM_8: return Scan::Num8;
		case input_keycode::NUM_9: return Scan::Num9;
		case input_keycode::ESCAPE: return Scan::Escape;
		case input_keycode::LEFT_CTRL: return Scan::LControl;
		case input_keycode::LEFT_SHIFT: return Scan::LShift;
		case input_keycode::LEFT_ALT: return Scan::LAlt;
		case input_keycode::RIGHT_CTRL: return Scan::RControl;
		case input_keycode::RIGHT_SHIFT: return Scan::RShift;
		case input_keycode::RIGHT_ALT: return Scan::RAlt;
		case input_keycode::LEFT_BRACKET: return Scan::LBracket;
		case input_keycode::RIGHT_BRACKET: return Scan::RBracket;
		case input_keycode::SEMICOLON: return Scan::Semicolon;
		case input_keycode::COMMA: return Scan::Comma;
		case input_keycode::PERIOD: return Scan::Period;
		case input_keycode::APOSTROPHE: return Scan::Apostrophe;
		case input_keycode::SLASH: return Scan::Slash;
		case input_keycode::BACKSLASH: return Scan::Backslash;
		case input_keycode::GRAVE: return Scan::Grave;
		case input_keycode::EQUAL: return Scan::Equal;
		case input_keycode::MINUS: return Scan::Hyphen;
		case input_keycode::SPACE: return Scan::Space;
		case input_keycode::ENTER: return Scan::Enter;
		case input_keycode::BACKSPACE: return Scan::Backspace;
		case input_keycode::TAB: return Scan::Tab;
		case input_keycode::PAGE_UP: return Scan::PageUp;
		case input_keycode::PAGE_DOWN: return Scan::PageDown;
		case input_keycode::END: return Scan::End;
		case input_keycode::HOME: return Scan::Home;
		case input_keycode::INSERT: return Scan::Insert;
		case input_keycode::DELETE: return Scan::Delete;
		case input_keycode::LEFT: return Scan::Left;
		case input_keycode::RIGHT: return Scan::Right;
		case input_keycode::UP: return Scan::Up;
		case input_keycode::DOWN: return Scan::Down;
		case input_keycode::NUMPAD_0: return Scan::Numpad0;
		case input_keycode::NUMPAD_1: return Scan::Numpad1;
		case input_keycode::NUMPAD_2: return Scan::Numpad2;
		case input_keycode::NUMPAD_3: return Scan::Numpad3;
		case input_keycode::NUMPAD_4: return Scan::Numpad4;
		case input_keycode::NUMPAD_5: return Scan::Numpad5;
		case input_keycode::NUMPAD_6: return Scan::Numpad6;
		case input_keycode::NUMPAD_7: return Scan::Numpad7;
		case input_keycode::NUMPAD_8: return Scan::Numpad8;
		case input_keycode::NUMPAD_9: return Scan::Numpad9;
		case input_keycode::F1: return Scan::F1;
		case input_keycode::F2: return Scan::F2;
		case input_keycode::F3: return Scan::F3;
		case input_keycode::F4: return Scan::F4;
		case input_keycode::F5: return Scan::F5;
		case input_keycode::F6: return Scan::F6;
		case input_keycode::F7: return Scan::F7;
		case input_keycode::F8: return Scan::F8;
		case input_keycode::F9: return Scan::F9;
		case input_keycode::F10: return Scan::F10;
		case input_keycode::F11: return Scan::F11;
		case input_keycode::F12: return Scan::F12;
		default:
			return Scan::Unknown;
		}
	}
}

sfml_input_impl::sfml_input_impl(sf::Window& window, std::function<void(sf::Event)> callback) : window(&window), event_callback(callback) {

}

void sfml_input_impl::use_contexts(std::span<input_context> contexts) {
	this->contexts = contexts;
}

void sfml_input_impl::poll_events() {
	while (const std::optional event = window->pollEvent()) {
		[&, this] {
			for (auto& context : contexts) {
				for (auto& action : context.actions) {
					auto check_modifiers = [&action](auto event) -> bool {
						bool ok = true;
						if ((static_cast<int>(action.modifiers) & static_cast<int>(input_modifier::SHIFT))) {
							ok &= event->shift;
						}
						if ((static_cast<int>(action.modifiers) & static_cast<int>(input_modifier::CTRL))) {
							ok &= event->control;
						}
						if ((static_cast<int>(action.modifiers) & static_cast<int>(input_modifier::ALT))) {
							ok &= event->alt;
						}
						return ok;
						};
					if (action.trigger == input_trigger_type::PRESSED && event->is<sf::Event::KeyPressed>()) {
						auto concrete_event = event->getIf<sf::Event::KeyPressed>();
						if (check_modifiers(concrete_event) &&
							(to_sfml_keycode(action.key) == concrete_event->scancode || action.key == input_keycode::UNKNOWN)) {
							action.callback();
							return; // Early out to avoid multiple actions on same event
						}
					}
					else if (action.trigger == input_trigger_type::RELEASED && event->is<sf::Event::KeyReleased>()) {
						auto concrete_event = event->getIf<sf::Event::KeyReleased>();
						if (check_modifiers(concrete_event) &&
							(to_sfml_keycode(action.key) == concrete_event->scancode || action.key == input_keycode::UNKNOWN)) {
							action.callback();
							return; // Same as above;
						}
					}
				}
			}
		}();
		event_callback(*event);
	}
	// Handle states
	for (auto& context : contexts) {
		for (auto& state : context.states) {
			auto check_modifiers = [&state] {
				bool ok = true;
				if ((static_cast<int>(state.modifiers) & static_cast<int>(input_modifier::SHIFT))) {
					ok &= sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LShift) ||
					      sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RShift);
				}
				if ((static_cast<int>(state.modifiers) & static_cast<int>(input_modifier::CTRL))) {
					ok &= sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LControl) ||
					      sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RControl);
				}
				if ((static_cast<int>(state.modifiers) & static_cast<int>(input_modifier::ALT))) {
					ok &= sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::LAlt) ||
					      sf::Keyboard::isKeyPressed(sf::Keyboard::Scan::RAlt);
				}
				return ok;
				};

			if (sf::Keyboard::isKeyPressed(to_sfml_keycode(state.key)) && check_modifiers()) {
				state.callback(true);
			}
			else {
				state.callback(false);
			}
		}
	}
}

void sfml_input_impl::start_event_loop() {
	// no-op
	assert(false && "Unimplemented");
}

void sfml_input_impl::stop_event_loop() {
	// no-op
	assert(false && "Unimplemented");
}
