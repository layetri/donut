//
// Created by Daniël Kamp on 31/01/2022.
//

#include "Header/PresetEngine.h"

PresetEngine::PresetEngine(ParameterPool* parampool) {
	pool = parampool;

	available_presets = new vector<string>;
	presets = new vector<Preset*>;

	filesystem::path path = filesystem::current_path() / ".donut_runtime/presets";
	cout << path << endl;
	for (const auto & entry : filesystem::directory_iterator(path)) {
		printw("%s\n", entry.path().c_str());
		refresh();
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

void PresetEngine::load(uint n) {
	auto path = available_presets->at(n);
	// Read preset
	ifstream file(path.c_str());

	// Parse to struct
	Preset p;
	p.name = path.erase(0, 65);

	string line;
	while(getline(file, line)) {
		istringstream iss(line);
		string key;
		if(getline(iss, key, '=')) {
			string value;
			if(line[0] == '#')
				continue;

			if(getline(iss, value)) {
				p.parameters.insert(pair<string, float> {key, stof(value)});
				printw("%s: %s\n", key.c_str(), value.c_str());
			}
		}
	}

	presets->push_back(&p);

	// Write to ParameterPool
	pool->load(p.parameters);
}

void PresetEngine::store(string name) {
	string path = "/Users/layetri/Development/_modules/donut/.donut_runtime/presets/";
	ofstream out(path.append(name).append(".donutpreset"));

	for(auto& param : *pool->getAll()) {
		out << param->key << "=" << to_string(param->value) << endl;
	}

	out.close();
}

