#include "soundWave.h"
#include "constants.h"
#include "logger.h"

std::string waveForm2String(WaveForm w)
{
	std::string wave;
	switch (w)
	{
		case WaveForm::SINE:
			wave = "Sine";
			break;
		case WaveForm::SQUARE:
			wave = "Square";
			break;
		case WaveForm::SAWTOOTH:
			wave = "Sawtooth";
			break;
		case WaveForm::TRIANGLE:
			wave = "Tringle";
			break;
		default:
			wave = "Unkown";
			break;
	}
	return wave;
}

SoundWave::SoundWave(int f, int a, WaveForm w, value_type phase)
	: m_f(f)
	, m_A(a)
	, m_phase(phase)
	, m_waveForm(w)
{
	reEquateValues();
}

void SoundWave::setNextWaveForm()
{
	int val = (int)m_waveForm;
	val++;
	if (val >= (int)WaveForm::MAX)
	{
		val = 0;
	}
	m_waveForm = (WaveForm)val;
}

void SoundWave::changeFrequency(int deltaChange)
{
	m_f += deltaChange;
	reEquateValues();
}

void SoundWave::changeAmplitude(int deltaChange)
{
	m_A += deltaChange;
	reEquateValues();
}

void SoundWave::reEquateValues()
{
	m_2A = 2 * m_A;
	m_halfA = m_A / 2.0;
	m_p = 1 / (value_type)m_f;
	m_af = M_PI_2 * m_f;
}

SoundWave::value_type SoundWave::getSample(value_type t) const
{
	value_type value{};
	switch (m_waveForm)
	{
		case WaveForm::SINE:
			// A sine wave, sinusodial wave, or just sinusoid is a mathematical curve defined in terms of the sine function, of which it is the graph.
			// It is a time of continuous wave and also a smooth periodic function.
			// Its most basic form as a function of time (t) is :
			//							y(t) = A * sin(2πft + φ) = A * sin(ωt + φ)
			// wehre :
			//		A, amplitude, the peak deviation of the function from zero.
			//		f, ordinary frequency, the number of oscillations(cycles) that occur each second of time.
			//		ω = 2πf, angular frequency, the rate of change of the function argument in units of radians per seconds
			//		φ, phase, specifies (in radians) where in its cycle the oscillation is at t = 0
			// When φ is non-zero, the entire waveform appears to be shifted in time by the amount φ/ω seconds.
			//	A -ve value represents a delay, and a +ve value represents an advance.
			
			value = m_af * t;
			value = m_A * std::sin(value + m_phase);
			break;

		case WaveForm::SQUARE:
			// 0	if |t| > 0.5
			// 1/2	if |t| == 0.5
			// 1	if |t| < 0.5
			value = m_af * t;
			value = std::sin(value + m_phase);
			value = value > 0 ? m_A : -m_A;
			break;

		case WaveForm::SAWTOOTH:
			// y(t) = - ((2*A)/π)*arctan(cot(πt/p))
			// p = period = 1 / freq
			// cot = sin / cos
			// A, amplitude
			value = (M_PI * t) / m_p;
			value = std::atan(cot(value));
			value = -(m_2A / M_PI) * value;
			break;

		case WaveForm::TRIANGLE:
			// y(t) = (2*A)/π)*arsin(sin(2πt/p)
			// p = period = 1 / freq
			// A, amplitude
			value = (M_PI_2 * t) / m_p;
			value = (m_2A / M_PI) * std::asin(std::sin(value));
			break;
		default:
			break;
	}
	return value;
}

void SoundWave::print(const std::string& prefix) const
{
	using ns_Util::Logger;
	Logger::LOG_MSG(prefix, "SoundWave: \n");
	Logger::LOG_MSG(prefix, "    Frequency          : ", m_f, ", Angular frequency : ", m_af, '\n');
	Logger::LOG_MSG(prefix, "    Amplitude          : ", m_A, ", 2*A : ", m_2A, ", 0.5*A : ", m_halfA, '\n');
	Logger::LOG_MSG(prefix, "    Period             : ", m_p, '\n');
	Logger::LOG_MSG(prefix, "    Phase              : ", m_phase, '\n');
	Logger::LOG_MSG(prefix, "    Waveform           : ", waveForm2String(m_waveForm), '\n');
	
}