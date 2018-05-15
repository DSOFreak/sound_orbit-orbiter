#pragma once
#include <string>
class MusicScale
{
	int n_notes = 12;
	std::string notes[12] = { "A", "A#", "B", "C" , "C#", "D", "D#", "E", "F", "F#", "G", "G#" };
	float pitches[12] = { 1.0f,
		1.0594630943592953f,
		1.122462048309373f,
		1.189207115002721f,
		1.2599210498948732f,
		1.3348398541700344f,
		1.4142135623730951f,
		1.4983070768766815f,
		1.5874010519681994f,
		1.681792830507429f,
		1.7817974362806785f,
		1.8877486253633868f };

	float A_freqs[9] = { 22.5f, 55.0f,110.0f,220.0f,440.0f,880.0f,1760.0f,3520.0f,7040.0f };
public:
	MusicScale();
	~MusicScale();
	float getPitch(const int& octave, const std::string& note);
	float getPitch(const int& octave, const int& note);
};

