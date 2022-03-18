//
// Created by Daniël Kamp on 09/11/2021.
//

#include "Header/main_darwin.h"

#ifdef DEVMODE
	#include "Header/TestSynth.h"
#endif

mutex mtx;

void ui(bool& running, queue<Event *>& events, ParameterPool& parameters, PresetEngine& presetEngine, ApplicationState* app, vector<Voice*>& voices) {
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

	CommandPool cmd_pool(&events, &parameters, &presetEngine, &running);

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

void event(vector<Voice*>& voices, NoteHandler& nh, ParameterPool& parameters, queue<Event *>& events, RtMidiIn* midiIn, RtMidiOut* midiOut, bool& running) {
	bool remap_mode = false;
	uint8_t remap_cc = 0;
	ControlMap cm;

	while(running) {
		mtx.lock();

		if(!events.empty()) {
			Event* e = events.front();
			events.pop();
			
			switch(e->type) {
				case e_Midi:
					if(remap_mode) {
						cm.changeCC(ParameterID(remap_cc), (uint16_t) e->cc);
						cout << "Remapped " << remap_cc << " to " << CommandPool::color(to_string(e->cc), 36)
							 << endl;
						remap_mode = false;
					} else if((e->cc - 21) >= 0 && (e->cc - 21) <= 8) {
						nh.set(cm.getPID(e->cc), e->value);
					}
					break;
				case e_Control:
					if(e->cid == c_Split) {
						nh.setSplit(e->value);
					} else if (e->cc == 255) {
						remap_mode = true;
						remap_cc = e->value;
					} else if((e->cc - 21) == p_Exit) {
						running = false;
					} else if(e->cc >= p_AMP_Attack && e->cc <= p_Exit) {
						nh.set(ParameterID(e->cc - 21), e->value);
					}
					break;
				case e_System:
					if((e->cc - 21) == p_MIDI_List) {
						listMidiDevices(midiIn, midiOut);
					} else if((e->cc - 21) == p_MIDI_In) {
						switchMidiInputs(midiIn, e->value);
					} else if((e->cc - 21) == p_MIDI_Out) {
						switchMidiOutputs(midiOut, e->value);
					}
				default:
					break;
			}
		}

		mtx.unlock();
		usleep(100);
	}
}


void midi(NoteHandler& handler, queue<Event*>& event_queue, RtMidiIn* midi_in, RtMidiOut* midi_out, bool& running) {
	midi_message_t message;
	vector<Note> sustained;
	bool sustain = false;

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
			mvprintw(0, 1, "type: %u, channel: %u, control: %u, value: %u ", type, channel, message[1],
				 message[2]);
			move(winy, winx);
			refresh();

			if(type == 176) {
				// Handle CC
				// Handle sustain pedal
				if (message[1] == 64) {
					sustain = message[2] > 0;
					if(!sustain) {
						for(auto& note : sustained) {
							handler.noteOff(&note);
						}
						sustained.clear();
					}
				} else {
					MidiEvent e(message[1], message[2]);
					event_queue.push(&e);
				}
			} else if(type == 128 || message[2] == 0) {
				if (!sustain) {
					handler.noteOff(&n);
				} else {
					sustained.push_back(n);
				}
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
	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	#ifdef DEVMODE
		midi_in->openPort(1);
	#else
		midi_in->openPort(0);
	#endif
	midi_in->ignoreTypes(false, false, false);
}

void listMidiDevices(RtMidiIn* midi_in, RtMidiOut* midi_out) {
	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	unsigned int nOutPorts = midi_out->getPortCount();
	printw("\nThere are %u MIDI input sources available.\n", nPorts);

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
		printw("  Input Port #%u: %s\n", i, portName.c_str());
	}
	printw("\nUse ");
	attron(COLOR_PAIR(5));
	printw("midi in [device_number]");
	attroff(COLOR_PAIR(5));
	printw(" to select a MIDI input device.");
	refresh();
	
	// List outputs
	printw("\nThere are %u MIDI output destinations available.\n", nOutPorts);
	for (unsigned int i = 0; i < nOutPorts; i++) {
		try {
			portName = midi_out->getPortName(i);
		} catch (RtMidiError &error) {
			error.printMessage();
			delete midi_out;
			return;
		}
		printw("  Output Port #%u: %s\n", i, portName.c_str());
	}
	printw("\nUse ");
	attron(COLOR_PAIR(5));
	printw("midi out [device_number]");
	attroff(COLOR_PAIR(5));
	printw(" to select a MIDI output device.");
	refresh();
}

void switchMidiInputs(RtMidiIn* midi_in, uint port) {
	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	if(port < nPorts) {
		midi_in->closePort();
		midi_in->openPort(port);
		
		string name = midi_in->getPortName(port);
		printw("Now using %s.", name.c_str());
	} else {
		printw("That MIDI input doesn't exist.");
	}
}

void switchMidiOutputs(RtMidiOut* midi_out, uint port) {
	// Check outputs.
	unsigned int nPorts = midi_out->getPortCount();
	if(port < nPorts) {
		midi_out->closePort();
		midi_out->openPort(port);
		
		string name = midi_out->getPortName(port);
		printw("Now using %s.", name.c_str());
		
	} else {
		printw("That MIDI output doesn't exist.");
	}
}

void init_curses() {
	initscr();

	curs_set(0);
	use_default_colors();
	start_color();
	init_pair(0, COLOR_CYAN, -1);
	init_pair(1, COLOR_MAGENTA, -1);
	init_pair(2, COLOR_RED, -1);
	init_pair(3, COLOR_GREEN, -1);
	init_pair(4, COLOR_WHITE, -1);
	init_pair(5, COLOR_CYAN, -1);
}

void program() {
	init_curses();

	bool running = true;
	extern unsigned int samplerate;
	ApplicationState app = app_Idle;
	ParameterPool parameters;
	PresetEngine pe(&parameters);

	Tables tables;
	tables.generateWaveforms();

	#ifdef ENGINE_JACK
		// Do Jack setup
		JackModule jack;
		jack.init("donut");
		samplerate = jack.getSamplerate();
		if (samplerate == 0) {
			samplerate = 44100;
		}
		printw("Samplerate is: %u", samplerate);
	#endif

	Buffer output(samplerate);

	queue<Event *> event_queue;
	vector<Voice *> voices;
	vector<Buffer *> voice_buffers;

	voices.reserve(NUMBER_OF_VOICES);
	voice_buffers.reserve(NUMBER_OF_VOICES);
	for (int i = 0; i < NUMBER_OF_VOICES; i++) {
		voice_buffers.push_back(new Buffer(500, "voice_"+to_string(i)));

		voices.push_back(new Voice(voice_buffers[i], &parameters, &tables, (uint8_t) i));
	}

	NoteHandler handle(&voices);
	handle.setSplit(0);

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
	RtMidiOut *midi_out = 0;
	try {
		midi_in = new RtMidiIn();
		midi_out = new RtMidiOut();
	} catch (RtMidiError &error) {
		error.printMessage();
		exit(EXIT_FAILURE);
	}
	init_midi(midi_in);

	thread midi_handler(midi, ref(handle), ref(event_queue), ref(midi_in), ref(midi_out), ref(running));
	thread event_handler(event, ref(voices), ref(handle), ref(parameters), ref(event_queue), ref(midi_in), ref(midi_out), ref(running));
	ui(running, event_queue, ref(parameters), ref(pe), &app, voices);

	midi_handler.join();
	event_handler.join();

	delete midi_in;
	delete midi_out;

	cout << "\x1B[2J\x1B[H";
	curs_set(1);

	exit(0);
}