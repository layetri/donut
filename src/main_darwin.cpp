//
// Created by Daniël Kamp on 09/11/2021.
//

#include <main_darwin.h>

mutex mtx;

// This function runs the TUI
void ui(bool& running, GUI& gui, queue<Event *>& events, ParameterPool& parameters, PresetEngine& presetEngine, vector<Voice*>& voices) {
	// Setup UI
	CommandPool cmd_pool(&events, &gui, &parameters, &presetEngine, &running);

	while (running) {
		auto m = gui.input();
		
		mtx.lock();
		cmd_pool.handleCommand(m);
		mtx.unlock();
		usleep(1000);
	}
}

// This thread handles internal application events
void event(vector<Voice*>& voices, Sampler& sampler, GUI& gui, NoteHandler& nh, ParameterPool& parameters, queue<Event *>& events, SampleLibrary& lib, RtMidiIn* midiIn, RtMidiOut* midiOut, bool& running) {
	bool remap_mode = false;
	uint8_t remap_cc = 0;
	ControlMap cm(&parameters);

	while(running) {
		mtx.lock();

		if(!events.empty()) {
			Event* e = events.front();
			events.pop();
			
			switch(e->type) {
				case e_Midi:
					if(remap_mode) {
						cm.changeCC(ParameterID(remap_cc), (uint16_t) e->cc);
						gui.output("Remapped " + parameters.translate(ParameterID(remap_cc)) + " to " + to_string(e->cc));
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
						gui.output(parameters.translate(cm.getPID(e->cc)) + ": " + to_string(e->value), false, 0, 1);
						// Handle all parameters assigned to this CC
						for(auto& p : cm.getPIDs(e->cc)) {
							// Send the event's value to the NoteHandler to, well, handle it
							nh.set(p, e->value);
						}
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
						
					} else if(e->cid == c_SampleList) {
						lib.list();
					} else if(e->cid == c_SampleLoad) {
						lib.load(e->str_content);
					} else if(e->cid == c_SampleLoaded) {
						lib.listLoaded();
					} else if(e->cid == c_SamplerAddRegion) {
						sampler.addRegion(e->str_content);
					} else if(e->cid == c_SamplerSetRoot) {
						sampler.setRoot(e->str_content, e->value);
					} else if(e->cid == c_ControlsLoad) {
						cm.loadMap(e->str_content);
						gui.output("Controls loaded.");
					} else if(e->cid == c_ControlsStore) {
						cm.storeMap(e->str_content);
						gui.output("Controls saved.");
					} else if(e->cc >= p_ADSR1_Attack && e->cc <= p_Exit) {
						nh.set(ParameterID(e->cc - 21), e->value);
					}
					break;
				case e_System:
					if((e->cc - 21) == p_MIDI_List) {
						listMidiDevices(midiIn, midiOut, &gui);
						
					} else if((e->cc - 21) == p_MIDI_In) {
						switchMidiInputs(midiIn, e->value, &gui);
						
					} else if((e->cc - 21) == p_MIDI_Out) {
						switchMidiOutputs(midiOut, e->value, &gui);
					}
				default:
					break;
			}
		}

		mtx.unlock();
		usleep(100);
	}
}

// This thread handles incoming and outgoing MIDI events
void midi(NoteHandler& handler, GUI& gui, queue<Event*>& event_queue, RtMidiIn* midi_in, RtMidiOut* midi_out, bool& running) {
	midi_message_t message;
	vector<Note> sustained;
	bool sustain = false;

	size_t nBytes;
	int row, col;
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
			
			gui.output("type: " + to_string(type) + ", channel: " + to_string(channel) + ", control: " + to_string(message[1]) + ", value: " + to_string(message[2]), false, 0, 5);

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
			
			#ifdef BUILD_GUI_NCURSES
				if(handler.inUse() < 10) {
					gui.output(" " + to_string(handler.inUse()) + " / " + to_string(NUMBER_OF_VOICES), false, col - 8, 1);
				} else {
					gui.output(to_string(handler.inUse()) + " / " + to_string(NUMBER_OF_VOICES), false, col - 8, 1, 2);
				}
			#endif
			
			mtx.unlock();
		}

		// Sleep for 10 milliseconds.
		usleep(100);
	}
}

// This thread runs an automatic backup of parameters every 10 seconds
void backup(PresetEngine& pe, bool& running) {
	long counter = 0;
	while(running) {
		counter++;
		
		if(counter == 10000000) {
			mtx.lock();
			// Backup the current settings
			pe.autosave();
			mtx.unlock();
			counter = 0;
		}
		
		// Wait a minute... oh wait
		usleep(1);
	}
}

// This thread runs the scheduling engine (BPM sync etc)
void schedule(Scheduler& scheduler, bool& running) {
	auto tp1 = chrono::steady_clock::now();
	while(running) {
		auto tp2 = chrono::steady_clock::now();
		if(duration_cast<chrono::microseconds>(tp2 - tp1).count() > scheduler.getSchedulingInterval()) {
			scheduler.fire();
			tp1 = tp2;
		}
		usleep(1);
	}
}


void init_midi(RtMidiIn *midi_in) {
	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	if(nPorts > 0) {
		midi_in->openPort(1);
		midi_in->ignoreTypes(false, false, false);
	}
}

void listMidiDevices(RtMidiIn* midi_in, RtMidiOut* midi_out, GUI* gui) {
	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	unsigned int nOutPorts = midi_out->getPortCount();
	gui->output("\nThere are " + to_string(nPorts) + " MIDI input sources available.\n", false);

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
		gui->output("  Input Port #" + to_string(i) + ": " + portName + "\n", false);
	}
	gui->output("\nUse ", false);
	gui->output("midi in [device_number]", false, -1, -1, 5);
	gui->output(" to select a MIDI input device.", false);
	
	// List outputs
	gui->output("\nThere are " + to_string(nOutPorts) + " MIDI output destinations available.\n", false);
	for (unsigned int i = 0; i < nOutPorts; i++) {
		try {
			portName = midi_out->getPortName(i);
		} catch (RtMidiError &error) {
			error.printMessage();
			delete midi_out;
			return;
		}
		gui->output("  Output Port #" + to_string(i) + ": " + portName + "\n", false);
	}
	gui->output("\nUse ", false);
	gui->output("midi out [device_number]", false, -1, -1, 5);
	gui->output(" to select a MIDI output device.", true);
}

void switchMidiInputs(RtMidiIn* midi_in, uint port, GUI* gui) {
	// Check inputs.
	unsigned int nPorts = midi_in->getPortCount();
	if(port < nPorts) {
		midi_in->closePort();
		midi_in->openPort(port);
		
		string name = midi_in->getPortName(port);
		gui->output("Now using " + name + ".");
	} else {
		gui->output("That MIDI input doesn't exist.");
	}
}

void switchMidiOutputs(RtMidiOut* midi_out, uint port, GUI* gui) {
	// Check outputs.
	unsigned int nPorts = midi_out->getPortCount();
	if(port < nPorts) {
		midi_out->closePort();
		midi_out->openPort(port);
		
		string name = midi_out->getPortName(port);
		gui->output("Now using " + name + ".");
	} else {
		gui->output("That MIDI output doesn't exist.");
	}
}

void displayMidiStatus(RtMidiIn* midi_in, RtMidiOut* midi_out) {
	// Display the status of the internal MIDI devices
}

void program() {
	GUI gui;
	gui.initgui();
	
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
	
	srand(time(NULL));
	
	ParameterPool parameters;
	ModMatrix mm;
	Scheduler scheduler(parameters.get(p_BPM, 0));
	
//	scheduler.store([]() {verbose("one measure!");}, Timestamp {1, 0, 0}, "liveness");
	
	SampleLibrary lib(&gui);
	Sampler sampler(&parameters, &lib);
	if(lib.load("default")) {
		sampler.addRegion("default");
		sampler.setRoot("default", 50);
	}
	Particles particles(&lib);
	
	PresetEngine pe(&parameters, &mm, &sampler, &particles, &lib, &gui);
	
	Tables tables;
	tables.generateWaveforms();
	
	queue<Event *> event_queue;
	vector<Voice *> voices;
	vector<Buffer *> voice_buffers;

	voices.reserve(NUMBER_OF_VOICES);
	voice_buffers.reserve(NUMBER_OF_VOICES);
	for (int i = 0; i < NUMBER_OF_VOICES; i++) {
		voice_buffers.push_back(new Buffer(441, "voice_"+to_string(i)));
		voices.push_back(new Voice(voice_buffers[i], &parameters, &mm, &tables, &sampler, &particles, &gui, (uint8_t) i));
	}
	
	NoteHandler handle(&voices);
	AutoMaster sensei(&voices, &parameters, parameters.get(p_Master, 0));
	pe.load("_autosave");
	
	#ifdef ENGINE_JACK
		// Assign the Jack callback function
		jack.onProcess = [&sensei, &mm, &voices](jack_default_audio_sample_t *outBuf_L, jack_default_audio_sample_t *outBuf_R, jack_nframes_t nframes) {
			auto time1 = chrono::steady_clock::now();
			for(auto& v : voices) {
				v->block((size_t) nframes);
			}
			
			for (unsigned int i = 0; i < nframes; i++) {
				mm.process();
				sensei.process();

				outBuf_L[i] = sensei.getLeftChannel();
				outBuf_R[i] = sensei.getRightChannel();

				sensei.tick();
			}
			verbose(chrono::duration_cast<chrono::microseconds>(chrono::steady_clock::now() - time1).count());

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

	thread midi_handler(midi, ref(handle), ref(gui), ref(event_queue), ref(midi_in), ref(midi_out), ref(running));
	thread event_handler(event, ref(voices), ref(sampler), ref(gui), ref(handle), ref(parameters), ref(event_queue), ref(lib), ref(midi_in), ref(midi_out), ref(running));
//	thread scheduling_handler(schedule, ref(scheduler), ref(running));
	
	#ifndef BUILD_GUI_COUT
		thread autosave_handler(backup, ref(pe), ref(running));
	#endif
	
	ui(running, ref(gui), event_queue, ref(parameters), ref(pe), voices);

	midi_handler.join();
	event_handler.join();
//	scheduling_handler.join();
	#ifndef BUILD_GUI_COUT
		autosave_handler.join();
	#endif
	
	delete midi_in;
	delete midi_out;
	exit(0);
}