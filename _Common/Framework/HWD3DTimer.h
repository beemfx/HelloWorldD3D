// D3D Hello World

#pragma once

#include "HWD3DTypes.h"

// HWD3DTimer uses a high performance timer to update the game at a specific rate. This prevents problems with
// the game updating at such a high rate that floating point value precision causes erratic time deltas.

class HWD3DTimer
{
private:
	
	LARGE_INTEGER m_Freq = { };
	LARGE_INTEGER m_LastUpdateTime = { };
	LARGE_INTEGER m_CurrentTime = { };
	
	float m_CumulativeDelta = 0.f;

public:
	
	HWD3DTimer();

	void UpdateAtRate(const float InRate, const int MaxUpdatesThreshold, const std::function<void(const float DeltaTime)>& InCallback);

private:
	
	void BeginFrame();
	float GetCumulativeDelta() const { return m_CumulativeDelta; }
	void ConsumeCumulativeDelta(float InTimeToConsume);
	void ResetCumulativeDelta();
	float GetElapsedSinceLastCommit() const;

private:
	
	static float GetRawTimeElapsedSec(const LARGE_INTEGER& Start, const LARGE_INTEGER& End, const LARGE_INTEGER& Freq);
};
