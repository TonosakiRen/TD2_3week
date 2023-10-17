#pragma once

#include <array>
#include <Windows.h>
#include <wrl.h>
#include "Mymath.h"

#define DIRECTINPUT_VERSION 0x0800 
#include <dinput.h>

class Input {
public: 

	struct MouseState {
		DIMOUSESTATE state;
		Vector2 position;
	};

	static Input* GetInstance();

	void Initialize(HINSTANCE hInstance, HWND hwnd);
	void Update();
	bool PushKey(BYTE keyNumber);
	bool TriggerKey(BYTE keyNumber);
	bool ReleaseKey(BYTE keyNumber);
	Vector2 GetMouseMove();
	float GetWheel();
	bool IsPressMouse(int32_t mouseNumber);
	const std::array<BYTE, 256>& GetAllKey() { return key_; }

private: 
	Microsoft::WRL::ComPtr<IDirectInput8> dInput_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devKeyboard_;
	Microsoft::WRL::ComPtr<IDirectInputDevice8> devMouse_;
	std::array<BYTE, 256> key_{};
	std::array<BYTE, 256> keyPre_{};
	MouseState mouseState_{};
	MouseState preMouseState_{};
private: 
	Input() = default;
	~Input() = default;
	Input(const Input&) = delete;
	const Input& operator=(const Input&) = delete;
};
