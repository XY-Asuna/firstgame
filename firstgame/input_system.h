#pragma once

#include <SFML/Window.hpp>
#include <functional>
#include <span>
#include <vector>

enum class input_keycode {
	A, B, C, D, E, F, G,
	H, I, J, K, L, M, N,
	O, P, Q, R, S, T, U,
	V, W, X, Y, Z,

	NUM_0, NUM_1, NUM_2, NUM_3, NUM_4,
	NUM_5, NUM_6, NUM_7, NUM_8, NUM_9,

	ESCAPE, TAB, CAPS_LOCK, SHIFT, CTRL, ALT,
	SPACE, ENTER, BACKSPACE, INSERT, DELETE,
	HOME, END, PAGE_UP, PAGE_DOWN,

	LEFT, RIGHT, UP, DOWN,

	F1, F2, F3, F4, F5, F6,
	F7, F8, F9, F10, F11, F12,

	PRINT_SCREEN, SCROLL_LOCK, PAUSE,

	LEFT_SHIFT, RIGHT_SHIFT, LEFT_CTRL, RIGHT_CTRL,
	LEFT_ALT, RIGHT_ALT,

	SEMICOLON, APOSTROPHE, COMMA, PERIOD, SLASH,
	BACKSLASH, LEFT_BRACKET, RIGHT_BRACKET, MINUS, EQUAL, GRAVE,

	NUMPAD_0, NUMPAD_1, NUMPAD_2, NUMPAD_3, NUMPAD_4,
	NUMPAD_5, NUMPAD_6, NUMPAD_7, NUMPAD_8, NUMPAD_9,
	NUMPAD_DECIMAL, NUMPAD_DIVIDE, NUMPAD_MULTIPLY, NUMPAD_SUBTRACT, NUMPAD_ADD, NUMPAD_ENTER,

	MOUSE_LEFT, MOUSE_RIGHT, MOUSE_MIDDLE, MOUSE_X1, MOUSE_X2,

	UNKNOWN
};

enum class input_modifier {
	NONE = 0,
	SHIFT = 1 << 0,
	CTRL = 1 << 1,
	ALT = 1 << 2,
};

enum class input_trigger_type {
	PRESSED,
	RELEASED
};

struct input_action {
	input_keycode key;
	input_modifier modifiers = input_modifier::NONE;
	input_trigger_type trigger;
	std::function<void()> callback;
};

struct input_state {
	input_keycode key;
	input_modifier modifiers = input_modifier::NONE;
	std::function<void(bool)> callback;
};

struct input_context {
	std::vector<input_action> actions;
	std::vector<input_state> states;
};

struct input_impl {
	virtual bool support_polling() const = 0;
	virtual bool support_dedicated_event_loop() const = 0;
	virtual void use_contexts(std::span<input_context> contexts) = 0;
	virtual void poll_events() = 0;
	virtual void start_event_loop() = 0;
	virtual void stop_event_loop() = 0;
	virtual ~input_impl() = default;
};

struct sfml_input_impl : input_impl {
	sfml_input_impl(sf::Window& window, std::function<void(sf::Event)> callback);
	virtual bool support_polling() const override {
		return true;
	};

	virtual bool support_dedicated_event_loop() const override {
		return false;
	};
	virtual void use_contexts(std::span<input_context> contexts) override;
	virtual void poll_events() override;
	virtual void start_event_loop() override;
	virtual void stop_event_loop() override;
	virtual ~sfml_input_impl() override = default;
private:
	sf::Window* window;
	std::span<input_context> contexts;
	std::function<void(sf::Event)> event_callback;
};
