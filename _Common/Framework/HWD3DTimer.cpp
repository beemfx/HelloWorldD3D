// D3D Hello World

#include "HWD3DTimer.h"

HWD3DTimer::HWD3DTimer()
{
	QueryPerformanceFrequency(&m_Freq);
	QueryPerformanceCounter(&m_LastUpdateTime);
	m_CurrentTime = m_LastUpdateTime;
}

void HWD3DTimer::UpdateAtRate(const float InRate, const int MaxUpdatesThreshold, const std::function<void(const float DeltaTime)>& InCallback)
{
	if (!InCallback)
	{
		return;
	}

	BeginFrame();

	int NumUpdatesThisFrame = 0;
	while (GetCumulativeDelta() >= InRate)
	{
		InCallback(InRate);
		ConsumeCumulativeDelta(InRate);
		NumUpdatesThisFrame++;

		if (NumUpdatesThisFrame >= MaxUpdatesThreshold)
		{
			ResetCumulativeDelta();
			break;
		}
	}
}

void HWD3DTimer::BeginFrame()
{
	m_LastUpdateTime = m_CurrentTime;

	QueryPerformanceCounter(&m_CurrentTime);

	const float FrameDelta = GetElapsedSinceLastCommit();
	const float TotalDelta = FrameDelta + m_CumulativeDelta;
	m_CumulativeDelta = TotalDelta;
}

void HWD3DTimer::ConsumeCumulativeDelta(float InTimeToConsume)
{
	m_CumulativeDelta -= InTimeToConsume;
	assert(m_CumulativeDelta >= 0.f);
}

void HWD3DTimer::ResetCumulativeDelta()
{
	m_CumulativeDelta = 0.f;
}

float HWD3DTimer::GetElapsedSinceLastCommit() const
{
	return GetRawTimeElapsedSec(m_LastUpdateTime, m_CurrentTime, m_Freq);
}

float HWD3DTimer::GetRawTimeElapsedSec(const LARGE_INTEGER& Start, const LARGE_INTEGER& End, const LARGE_INTEGER& Freq)
{
	assert(Start.QuadPart <= End.QuadPart);
	const LONGLONG Diff = End.QuadPart - Start.QuadPart;
	const double TimeMs = (Diff * 1000.) / Freq.QuadPart;
	const float TimeSec = static_cast<float>(TimeMs) / 1000.f;
	return TimeSec;
}
