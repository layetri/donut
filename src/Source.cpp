//
// Created by Daniël Kamp on 14/03/2022.
//

#include <Source/Source.h>

Source::Source(ParameterPool *parameters, uint8_t voice_id) {
	this->output = new Buffer(500);
	this->parameters = parameters;
	this->voice_id = voice_id;
}

Source::~Source() {}
/*!
 * @brief Performs sample calculations
 */
void Source::process() {}
/*!
 * @brief Refresh parameters used by the source (mostly obsolete)
 */
void Source::refresh() {}
/*!
 * @brief Modulate the Source's frequency (used mostly by WaveShaper)
 * @param val
 * @param amount
 */
void Source::fm(float val, float amount) {}
/*!
 * @brief Performs calculations for an entire sample block
 * @param block_size
 */
void Source::block (size_t block_size) {}

/*!
 * @brief Set the Source's pitch, and perform other actions related to a Note On event
 * @param midi_note
 */
void Source::pitch(uint8_t midi_note) {
	frequency = mtof(midi_note);
}
/*!
 * @brief Sets the Source's frequency (mostly obsolete)
 * @param frequency
 */
void Source::setFrequency (float frequency) {
	this->frequency = frequency;
}

/*!
 * @brief Ticks the Source's output buffer. May be overridden when using multiple buffers, etc
 */
void Source::tick() {
	output->tick();
}

/*!
 * @brief Returns a pointer to the Source's output buffer
 * @return A pointer to the Source's output buffer
 */
Buffer* Source::getBuffer() {
	return this->output;
}

/*!
 * @brief Translate a MIDI note number to a frequency
 * @param midi_note
 * @param base_frequency
 * @return The frequency corresponding to the MIDI note
 */
float Source::mtof(uint8_t midi_note, float base_frequency) {
	return pow(2.0, (midi_note - 69.0) / 12.0) * base_frequency;
}

// TODO: Clean this up
/*!
 * @brief Obsolete alias for std::clamp
 * @param value
 * @param bottom
 * @param top
 * @return The clamped value
 */
float Source::clip(float value, float bottom=0.0f, float top=1.0f) {
	return clamp(value, bottom, top);
}