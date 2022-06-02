//
// Created by Daniël Kamp on 31/01/2022.
//

#include <System/PresetEngine.h>

PresetEngine::PresetEngine(ParameterPool* parameters, ModMatrix* mm, Sampler* sampler, Particles* particles, SampleLibrary* library, GUI* gui) {
	pool = parameters;
	modmatrix = mm;
	this->sampler = sampler;
	this->particles = particles;
	this->library = library;
	this->gui = gui;
	
	selected = 0;
}

PresetEngine::~PresetEngine() {}

void PresetEngine::log() {
	vector<string> available_presets;
	filesystem::path path = filesystem::current_path() / ".donut_runtime/presets";
	
	#ifdef BUILD_GUI_IMGUI
		vector<PresetGUIItem> gui_presets;
	#endif
	
	for (const auto & entry : filesystem::directory_iterator(path)) {
		auto filename = entry.path().string();
		filename = filename.substr(filename.find_last_of("/\\") + 1);
		filename = filename.substr(0, filename.find_last_of("."));
		
		available_presets.emplace_back(filename.c_str());
		#ifdef BUILD_GUI_IMGUI
			// Geez C++...
			// TODO: clean this up, someday
			auto ts = filesystem::last_write_time(entry);
		#ifdef SYS_LINUX
			auto cstime = chrono::file_clock::to_sys(ts);
			auto ftime = chrono::system_clock::to_time_t(cstime);
		#else
			auto ftime = chrono::file_clock::to_time_t(ts);
		#endif
			
			char buffer [80];
			strftime(buffer, 80, "%x, %R", localtime(&ftime));
			
			string ln = buffer;
			gui_presets.push_back(PresetGUIItem {filename, ln});
		#endif
	}
	
	#ifndef BUILD_GUI_IMGUI
		for (uint i = 0; i < available_presets.size(); i++) {
			gui->output("[" + to_string(i) + "] ", false, -1, -1, 5);
			gui->output(available_presets.at(i) + "\n", false);
		}
	#else
		gui->updatePresets(gui_presets);
	#endif
}

void PresetEngine::load(string name) {
	filesystem::path path = filesystem::current_path() / ".donut_runtime/presets";
	
	for (const auto & entry : filesystem::directory_iterator(path)) {
		string filename = entry.path().filename().string();
		
		if(filename == name + ".donutpreset") {
			ifstream file(entry.path().c_str());
			nlohmann::json filecontents;
			file >> filecontents;
			
			// Parse to Preset
			Preset preset;
			preset.name = name;
			
			for(auto& p : filecontents["parameters"]) {
				preset.parameters.push_back(ParameterPreset {
					p["key"], p["voice"], p["base_value"]
				});
			}
			
			if(modmatrix->clearAll()) {
				for (auto &m: filecontents["mod_links"]) {
					modmatrix->link(
					  pool->get(pool->translate((string) m["destination"]), m["voice"]),
					  modmatrix->get((string) m["source"], m["voice"]),
					  m["voice"]);
				}
			}
			
			for(auto& s : filecontents["sample_lib"]) {
				if(!library->load(s["name"])) {
					verbose("Failed to load sample");
					exit(1);
				}
			}
			
			sampler->reset();
			for(auto& r : filecontents["sampler_regions"]) {
				sampler->addRegion(r["sample"], r["key_start"], r["key_end"], r["key_root"], r["smp_start"], r["smp_end"]);
			}
			
			if(!filecontents["particles_setup"].empty()) {
				particles->swap(filecontents["particles_setup"]["sample"], filecontents["particles_setup"]["key_root"]);
			}
			
			// Write to ParameterPool
			pool->load(&preset.parameters);
			return;
		}
	}
	
	verbose("A preset with that name couldn't be found");
}

void PresetEngine::store(string name) {
	filesystem::path path = filesystem::current_path() / ".donut_runtime/presets";
	ofstream out(path.append(name + ".donutpreset"));

	nlohmann::json filecontents;
	filecontents["parameters"] = {};
	filecontents["mod_links"] = {};
	filecontents["sample_lib"] = {};
	filecontents["sampler_regions"] = {};
	
	for(auto& param : *pool->getAll()) {
		string key = pool->translate(param.pid);
		filecontents["parameters"].push_back({
		 {"key", key},
		 {"voice", param.voice_id},
		 {"value", param.value},
		 {"base_value", param.base_value}
		});
	}
	
	for(auto& link : *modmatrix->get()) {
		filecontents["mod_links"].push_back({
		  {"source", link->modulator->getName()},
		  {"destination", pool->translate(link->parameter->pid)},
		  {"voice", link->voice_id},
		  {"amount", link->amount}
		});
	}
	
	// TODO: Store sample library (aka which sample paths are loaded) and Sampler regions
	for(auto& smp : *library->getSamples()) {
		filecontents["sample_lib"].push_back({
		   {"name", smp->name}
		});
	}
	
	for(auto& r : *sampler->getRegions()) {
		filecontents["sampler_regions"].push_back({
			{"sample", r->sample->name},
			{"smp_start", r->smp_start},
			{"smp_end", r->smp_end},
			{"key_start", r->key_start},
			{"key_end", r->key_end},
			{"key_root", r->key_root},
			{"mode", r->mode}
		});
	}
	
	out << setw(4) << filecontents << endl;
	out.close();
}

void PresetEngine::autosave() {
	store("_autosave");
}
