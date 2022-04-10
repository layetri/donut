//
// Created by Daniël Kamp on 09/11/2021.
//

#include <main_darwin.h>
#include <assert.h>

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
	printw(" version 0.2");
	refresh();
	
	move(4,0);
	clrtoeol();
	attron(COLOR_PAIR(2));
	printw("Incoming MIDI:");
	attroff(COLOR_PAIR(2));
	refresh();

	move(8, 0);
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

		move(8, 4);
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
		usleep(1000);
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
						cout << "Remapped " << remap_cc << " to " << CommandPool::color(to_string(e->cc), 36) << endl;
						remap_mode = false;
						
					} else if(cm.getPID(e->cc) == p_Master) {
						// Set the master volume
						parameters.set(p_Master, 0, e->value / 127.0f);
						
					} else if(cm.getPID(e->cc) == p_FX_Delay_Amount) {
						// Set the master volume
						parameters.set(p_FX_Delay_Amount, 0, e->value / 127.0f);
						
					} else if(cm.getPID(e->cc) == p_FX_Delay_DTLeft) {
						// Set the master volume
						parameters.set(p_FX_Delay_DTLeft, 0, (e->value / 127.0f) * 10000);
						parameters.set(p_FX_Delay_DTRight, 0, (e->value / 127.0f) * 10000);
						
					} else if(cm.getPID(e->cc) == p_FX_Delay_FBLeft) {
						// Set the master volume
						parameters.set(p_FX_Delay_FBLeft, 0, (e->value / 127.0f) * 0.99);
						parameters.set(p_FX_Delay_FBRight, 0, (e->value / 127.0f) * 0.99);
						
					} else {
						// Display the value
						printAtLocation(0, 1, parameters.translate(cm.getPID(e->cc)) + ": " + to_string(e->value));
						// Send the event's value to the NoteHandler to, well, handle it
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
						
					} else if(e->cc >= p_ADSR1_Attack && e->cc <= p_Exit) {
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
			mvprintw(5, 0, "type: %u, channel: %u, control: %u, value: %u ", type, channel, message[1], message[2]);
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
				printAtLocation(col - 8, 1, to_string(handler.inUse()) + " / " + to_string(NUMBER_OF_VOICES), 2);
			}
			move(winy, winx);
			refresh();
			mtx.unlock();
		}

		// Sleep for 10 milliseconds.
		usleep(100);
	}
}

void printAtLocation(int x, int y, string text, int c) {
	int winx, winy;
	getyx(stdscr,winy,winx);
	
	move(y, x);
	clrtoeol();
	attron(COLOR_PAIR(c));
	printw(text.c_str());
	attroff(COLOR_PAIR(c));
	
	move(winy, winx);
}


void init_midi(RtMidiIn *midi_in) {
	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	if(nPorts > 0) {
		midi_in->openPort(1);
		midi_in->ignoreTypes(false, false, false);
	}
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

void displayMidiStatus(RtMidiIn* midi_in, RtMidiOut* midi_out) {
	// Display the status of the internal MIDI devices
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
	uint16_t voices_done = 0;
	extern unsigned int samplerate;
	
	ApplicationState app = app_Idle;
	ParameterPool parameters;
	ModMatrix mm;
	PresetEngine pe(&parameters, &mm);
	
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
	#endif
	
//	mtx.lock();
	queue<Event *> event_queue;
	vector<Voice *> voices;
	vector<Buffer *> voice_buffers;

	voices.reserve(NUMBER_OF_VOICES);
	voice_buffers.reserve(NUMBER_OF_VOICES);
	for (int i = 0; i < NUMBER_OF_VOICES; i++) {
		voice_buffers.push_back(new Buffer(441, "voice_"+to_string(i)));
		voices.push_back(new Voice(voice_buffers[i], &parameters, &mm, &tables, (uint8_t) i));
		voices_done++;
	}
	
	NoteHandler handle(&voices);
	AutoMaster sensei(&voices, &parameters, parameters.get(p_Master, 0));
//	mtx.unlock();
	
	#ifdef ENGINE_JACK
		// Assign the Jack callback function
		jack.onProcess = [&sensei, &mm, &voices_done](jack_default_audio_sample_t **inBufs,jack_default_audio_sample_t *outBuf_L,jack_default_audio_sample_t *outBuf_R,
			jack_nframes_t nframes) {
			if(voices_done == NUMBER_OF_VOICES) {
				mtx.lock();
				for (unsigned int i = 0; i < nframes; i++) {
					mm.process();
					sensei.process();

					outBuf_L[i] = sensei.getLeftChannel();
					outBuf_R[i] = sensei.getRightChannel();

					sensei.tick();
				}
				mtx.unlock();
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