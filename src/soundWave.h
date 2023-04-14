#pragma once

#include <string>
#include "constants.h"

enum class WaveForm
{
	SINE,
	SQUARE,
	SAWTOOTH,
	TRIANGLE,
	MAX
};

extern std::string waveForm2String(WaveForm w);

class SoundWave
{
public:
	using value_type = long double;

	SoundWave(int f, int a, WaveForm w, value_type phase);

	INLINE void setFrequency(int f) { m_f = f; }
	INLINE void setAmplitude(int a) { m_A = a; }
	INLINE void setPhase(value_type phase) { m_phase = phase; }
	INLINE void setWaveForm(WaveForm w) { m_waveForm = w; }

	INLINE int getFrequency() const { return m_f; }
	INLINE int getAmplitude() const { return m_A; }
	INLINE value_type getPhase() const { return m_phase; }
	INLINE WaveForm getWaveForm() const { return m_waveForm; }

	void setNextWaveForm();
	void changeFrequency(int deltaChange);
	void changeAmplitude(int deltaChange);

	void reEquateValues();

	value_type getSample(value_type t) const;

	void print(const std::string& prefix = "") const;
private:
	int			m_f{};		// Frequency
	int			m_A{};		// Amplitude

	value_type	m_p{};		// Period = 1 / frequency
	value_type	m_phase{};	// Shift

	WaveForm	m_waveForm{};

	int			m_2A{};		// 2 * a
	value_type	m_halfA{};	// a / 2
	value_type	m_af{};		// angular frequency = 2πf
};