#include "MusicScale.h"



MusicScale::MusicScale()
{
}


MusicScale::~MusicScale()
{
}

// Returns a pitch to a specific note
float MusicScale::getPitch(const int &octave, const std::string &note) {
	for (int i = 0; i < n_notes; i++) {
		if (notes[i].compare(note) == 0) return pitches[i] * A_freqs[octave];
	}
	return 0.0f;
}
// Returns a pitch to a specific note
float MusicScale::getPitch(const int &octave, const int &note) {
	if (note < n_notes) return pitches[note] * A_freqs[octave];
	return pitches[note % n_notes] * A_freqs[octave + (note / n_notes)];
}