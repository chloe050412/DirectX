#pragma once
#include <windows.h>

class TimeSystem
{
public:
	static TimeSystem* GetInstance();

public:
	void Initialize();
	void Update();
	void Finalize();

	float GetDeltaTime();
	int GetFramePerSecond();

private:
	TimeSystem() = default;
	~TimeSystem() = default;
	static TimeSystem* _timeSystem;

	float _deltaTime;
	LARGE_INTEGER			_frequency;
	LARGE_INTEGER			_startTime;
	LARGE_INTEGER			_stopTime;
};

