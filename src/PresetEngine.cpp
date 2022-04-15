//
// Created by Daniël Kamp on 31/01/2022.
//

#include <System/PresetEngine.h>

PresetEngine::PresetEngine(ParameterPool* parameters, ModMatrix* mm, Sampler* sampler, SampleLibrary* library) {
	pool = parameters;
	modmatrix = mm;
	this->sampler = sampler;
	this->library = library;

	available_presets = new vector<string>;
	presets = new vector<Preset*>;

	filesystem::path path = filesystem::current_path() / ".donut_runtime/presets";
	for (const auto & entry : filesystem::directory_iterator(path)) {
		available_presets->push_back(entry.path().c_str());
	}
	selected = 0;
}

PresetEngine::~PresetEngine() {
	for (auto &pr : *presets) {
		delete pr;
	}
	delete available_presets;
}

void PresetEngine::log() {
	for (uint i = 0; i < available_presets->size(); i++) {
		printw("[%u] %s\n", i, available_presets->at(i).erase(0, 65).c_str());
		refresh();
	}
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
				preset.parameters.push_back(new ParameterPreset {
					p["key"], p["voice"], p["base_value"]
				});
			}
			
			if(modmatrix->clearAll()) {
				for (auto &m: filecontents["mod_links"]) {
					modmatrix->link(
					  pool->get(pool->translate((string) m["destination"]), m["voice"]),
					  modmatrix->get(m["source"], m["voice"]),
					  m["voice"]);
				}
			}
			
			for(auto& s : filecontents["sample_lib"]) {
				library->load(s["name"]);
			}
			
			for(auto& r : filecontents["sampler_regions"]) {
				sampler->addRegion(r["sample"]);
				sampler->setRoot(r["sample"], r["key_root"]);
			}
			
			// Store the preset in memory
			presets->push_back(&preset);
			
			// Write to ParameterPool
			pool->load(&preset.parameters);
			return;
		}
	}
	
	verbose("A preset with that name couldn't be found");
}

void PresetEngine::store(string name) {
	string path = "/Users/layetri/Development/_modules/donut/.donut_runtime/presets/";
	ofstream out(path.append(name).append(".donutpreset"));

	nlohmann::json filecontents;
	filecontents["parameters"] = {};
	filecontents["mod_links"] = {};
	filecontents["sample_lib"] = {};
	filecontents["sampler_regions"] = {};
	
	for(auto& param : *pool->getAll()) {
		string key = pool->translate(param->pid);
		filecontents["parameters"].push_back({
		 {"key", key},
		 {"voice", param->voice_id},
		 {"value", param->value},
		 {"base_value", param->base_value}
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
