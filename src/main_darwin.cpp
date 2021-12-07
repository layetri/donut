//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/main_darwin.h"

#ifdef DEVMODE
	#include "Header/TestSynth.h"
#endif

mutex mtx;

// Abstraction to easily provide a text coloring utility
string color(const string& text, int color) {
	return "\033[" + to_string(color) + "m" + text + "\033[0m";
}

void ui(bool& running) {
	cout << "Welcome to " << color("Donut", 32) << endl;
	while (running) {
//    CommandPool.namespace(ns).find(command).assign(arguments);
		string cmd;
		getline(cin, cmd);

		if(cmd == "exit") {
			running = false;
		}
	}
}


void event(vector<Voice*>& voices, queue<Event *>& events, bool& running) {
	while(running) {
		mtx.lock();

		if(!events.empty()) {
			Event* e = events.front();
			events.pop();
			cout << e->value << endl;

			if(e->cc > 20 && e->cc < 29) {
				cout << ParameterStore(e->cc - 21);
				for (auto &v: voices) {
					v->set(ParameterStore(e->cc - 21), e->value);
				}
			}
		}

		mtx.unlock();
		usleep(100);
	}
}


void midi(NoteHandler& handler, queue<Event*>& event_queue, RtMidiIn* midi_in, bool& running) {
	vector<unsigned char> message;
	size_t nBytes;

	// Periodically check input queue.
	while (running) {
		midi_in->getMessage(&message);
		nBytes = message.size();

		if (nBytes > 0) {
			mtx.lock();
			Note n = {message[1], message[2]};
			int type = message[0] & 0b11110000;
			int channel = message[0] & 0b00001111;

			cout << "type: " << to_string(type) <<
				", channel: " << to_string(channel) <<
				", control: " << to_string(message[1]) <<
				", value: " << to_string(message[2]) << endl;

			if(type == 176) {
				Event e = {message[1], message[2]};
				event_queue.push(&e);
				// Handle CC
				// Handle sustain pedal
				if (message[1] == 64) {
					if (message[2] == 127) {
						cout << "Sustain on!" << endl;
					} else {
						cout << "Sustain off!" << endl;
					}
				}
				if (message[1] == 74 || message[1] == 22) {
					// Knob 2
					cout << "knob2: " << to_string(message[2]) << endl;
				}
				if (message[1] == 71 || message[1] == 23) {
					// Knob 3
				}
			} else if(type == 128 || message[2] == 0) {
				handler.noteOff(&n);
			} else if(type == 144) {
				handler.noteOn(&n);
			}
			mtx.unlock();
		}

		// Sleep for 10 milliseconds.
		usleep(100);
	}
}


void init_midi(RtMidiIn *midi_in) {
	// RTMidi implementation mostly copied from http://www.music.mcgill.ca/~gary/rtmidi/

	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	cout << "\nThere are " << color(to_string(nPorts), 35) << " MIDI input sources available.\n";

	// List inputs
	string portName;
	for (unsigned int i = 0; i < nPorts; i++) {
		try {
			portName = midi_in->getPortName(i);
		} catch (RtMidiError &error) {
			error.printMessage();
			delete midi_in;
			return;
		}
		cout << "  Input Port #" << i << ": " << portName << '\n';
	}

	// Prompt user selection
	if (nPorts > 0) {
		cout << "\nChoose a MIDI port to use: ";

		int port;
		cin >> port;
		midi_in->openPort(port);

		cout << "\n\n";

		// Don't ignore sysex, timing, or active sensing messages.
		midi_in->ignoreTypes(false, false, false);
	}
}


void program() {
	bool running = true;
	extern unsigned int samplerate;

	#ifdef ENGINE_JACK
		// Do Jack setup
		JackModule jack;
		jack.init("donut");
		samplerate = jack.getSamplerate();
		if (samplerate == 0) {
			samplerate = 44100;
		}
	#endif

	Buffer output(samplerate);

	queue<Event *> event_queue;
	vector<Voice *> voices;
	vector<Buffer *> voice_buffers;

	voices.reserve(NUMBER_OF_VOICES);
	voice_buffers.reserve(NUMBER_OF_VOICES);
	for (int i = 0; i < NUMBER_OF_VOICES; i++) {
		voice_buffers.push_back(new Buffer(samplerate, "voice_"+to_string(i)));

		voices.push_back(new Voice(voice_buffers[i], i));
	}

	NoteHandler handle(&voices);

	#ifdef ENGINE_JACK
		// Assign the Jack callback function
		jack.onProcess = [&voices, &output](jack_default_audio_sample_t **inBufs,jack_default_audio_sample_t *outBuf_L,jack_default_audio_sample_t *outBuf_R,
			jack_nframes_t nframes) {
			for (unsigned int i = 0; i < nframes; i++) {
				int s = 0;
				long m = 0;

				output.flush();
				for (auto &v: voices) {
					v->process();
					m += v->getSample();
					s += (v->getSample() != 0);
				}
				s = s > 0 ? s : 1;

				// Apply conversion and soft clipping on master out
				float smp = (m * (0.5 + (0.5 / s))) / (float) SAMPLE_MAX_VALUE;
				smp = (smp > 1.0) * 0.666666666666667 +
					(smp < -1.0) * -0.666666666666667 +
					(smp > -1.0 && smp < 1.0) * (smp - (pow(smp, 3) / 3.0));

				outBuf_L[i] = smp;
				outBuf_R[i] = outBuf_L[i];

				output.tick();

				for(auto& v : voices) {
					v->tick();
				}
			}

			return 0;
		};
		jack.autoConnect();
	#endif

	RtMidiIn *midi_in = 0;
	try {
		midi_in = new RtMidiIn();
	} catch (RtMidiError &error) {
		error.printMessage();
		exit(EXIT_FAILURE);
	}
	init_midi(midi_in);

	thread midi_handler(midi, ref(handle), ref(event_queue), ref(midi_in), ref(running));
	thread event_handler(event, ref(voices), ref(event_queue), ref(running));
	ui(running);

	midi_handler.join();
	event_handler.join();

	delete midi_in;
	exit(0);
}

// Each module has
// - RegisterCommands
//
// CommandPool has all commands for all modules