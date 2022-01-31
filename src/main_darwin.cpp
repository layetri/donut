//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/main_darwin.h"

#ifdef DEVMODE
	#include "Header/TestSynth.h"
#endif

mutex mtx;

void ui(bool& running, queue<Event *>& events, ParameterPool& parameters, ApplicationState* app, vector<Voice*>& voices) {
	// Clear screen
	cout << "\x1B[2J\x1B[H";
	int row, col;
	getmaxyx(stdscr, row, col);

	// Setup UI
	move(0, col-18);
	attron(COLOR_PAIR(3));
	printw("Donut");
	attroff(COLOR_PAIR(3));
	printw(" version 0.1");
	refresh();

	move(4, 0);
	clrtoeol();
	printw("[");
	attron(COLOR_PAIR(3));
	printw(">");
	attroff(COLOR_PAIR(3));
	printw("] ");
	refresh();

	CommandPool cmd_pool(&events, &parameters, &running);

	while (running) {
		mtx.lock();

		string cmd;
//		getline(cin, cmd);

		move(4, 4);
		clrtoeol();
		curs_set(1);

		mtx.unlock();

		char s[80];
		getstr(s);
		cmd = s;

		mtx.lock();
		curs_set(0);
		clrtobot();
		refresh();

		cmd_pool.handleCommand(cmd);
		refresh();

		mtx.unlock();
		usleep(100);
	}

}


void event(vector<Voice*>& voices, queue<Event *>& events, bool& running) {
	bool remap_mode = false;
	uint8_t remap_cc = 0;
	ControlMap cm;

	while(running) {
		mtx.lock();

		if(!events.empty()) {
			Event* e = events.front();
			events.pop();

			if(remap_mode) {
				cm.changeCC(ParameterID(remap_cc), (uint16_t) e->cc);
				cout << "Remapped " << remap_cc << " to " << CommandPool::color(to_string(e->cc), 36) << endl;
				remap_mode = false;
			} else {
				if (e->cc == 255) {
					remap_mode = true;
					remap_cc = e->value;
				} else if ((e->cc - 21) == p_Exit) {
					running = false;
				} else {
					for (auto &v: voices) {
						v->set(cm.getPID(e->cc), e->value);
					}
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
	int winy, winx, row, col;
	getmaxyx(stdscr, row, col);

	// Periodically check input queue.
	while (running) {
		midi_in->getMessage(&message);
		nBytes = message.size();

		if (nBytes > 0) {
			mtx.lock();
			Note n = {message[1], message[2]};
			int type = message[0] & 0b11110000;
			int channel = message[0] & 0b00001111;

			getyx(stdscr,winy,winx);
			mvprintw(0, 0, "type: %u, channel: %u, control: %u, value: %u ", type, channel, message[1],
				 message[2]);
			move(winy, winx);
			refresh();

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
			} else if(type == 128 || message[2] == 0) {
				handler.noteOff(&n);
			} else if(type == 144) {
				handler.noteOn(&n);
			}

			if(handler.inUse() < 10) {
				mvprintw(1, col - 8, " %u / %u", handler.inUse(), NUMBER_OF_VOICES);
			} else {
				attron(COLOR_PAIR(2));
				mvprintw(1, col - 8, "%u / %u", handler.inUse(), NUMBER_OF_VOICES);
				attroff(COLOR_PAIR(2));
			}
			move(winy, winx);
			refresh();
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
//	printw("\nThere are %s MIDI input sources available.\n", CommandPool::color(to_string(nPorts), 35).c_str());

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
//		printw("  Input Port #%u: %s\n", i, portName.c_str());
	}

	// Prompt user selection
	if (nPorts > 0) {
//		printw("\nChoose a MIDI port to use: ");
//		refresh();
//
//		int port;
//		cin >> port;
		midi_in->openPort(1);

//		cout << "\n\n";

		// Don't ignore sysex, timing, or active sensing messages.
		midi_in->ignoreTypes(false, false, false);
	}
}


void program() {
	initscr();

	curs_set(0);
	use_default_colors();
	start_color();
	init_pair(0, COLOR_CYAN, -1);
	init_pair(1, COLOR_MAGENTA, -1);
	init_pair(2, COLOR_RED, -1);
	init_pair(3, COLOR_GREEN, -1);
	init_pair(4, COLOR_WHITE, -1);


	bool running = true;
	extern unsigned int samplerate;
	ApplicationState app = app_Idle;
	ParameterPool parameters;

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

		voices.push_back(new Voice(voice_buffers[i], &parameters, i));
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
	ui(running, event_queue, ref(parameters), &app, voices);

	midi_handler.join();
	event_handler.join();

	delete midi_in;

	curs_set(1);
	cout << "\x1B[2J\x1B[H";

	exit(0);
}

// Each module has
// - RegisterCommands
//
// CommandPool has all commands for all modules