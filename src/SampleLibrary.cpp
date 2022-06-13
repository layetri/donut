//
// Created by Daniël Kamp on 12/04/2022.
//

#include "System/SampleLibrary.h"

SampleLibrary::SampleLibrary (GUI* gui) {
	this->gui = gui;
}

SampleLibrary::~SampleLibrary () = default;

bool SampleLibrary::load(const string& name) {
	bool s_exists = false;
	for(auto& s : samples) {
		s_exists = s->name == name;
		if(s_exists) {
			break;
		}
	}
	
	// Only load the sample if it hasn't been loaded yet
	if(!s_exists) {
		try {
			// This one's important: here we load the WAV file from storage and read it into a new buffer
			AudioFile<double> audioFile;
			audioFile.load((filesystem::current_path() / ".donut_runtime/samples" / (name + ".wav")).string());
			//	int file_sr = audioFile.getSampleRate();
			uint numSamples = audioFile.getNumSamplesPerChannel();
			auto buf = new Buffer(numSamples);
			
			// Load the sample (mono)
			for (int i = 0; i < numSamples; i++) {
				buf->write((sample_t) (audioFile.samples[0][i] * SAMPLE_MAX_VALUE));
				buf->tick();
			}
			
			auto *smp = new Sample{name, buf};
			samples.push_back(smp);
			
			return true;
		} catch (filesystem::filesystem_error const &ex) {
			verbose(ex.what());
			return false;
		}
	}
	return true;
}

void SampleLibrary::list() {
	filesystem::path path = filesystem::current_path() / ".donut_runtime/samples";
	#ifdef BUILD_GUI_IMGUI
		vector<SampleGUIItem> disp_list;
		findAllSampleFiles(&disp_list, path);
		gui->updateSampleList(disp_list);
	#else
		for (const auto & entry : filesystem::directory_iterator(path)) {
			gui->output(entry.path().string() + "\n", false);
		}
	#endif
	
}

void SampleLibrary::findAllSampleFiles(vector<SampleGUIItem>* returnList, filesystem::path base_path) {
	for (const auto & entry : filesystem::directory_iterator(base_path)) {
		auto p = entry.path().string();
		p = p.substr(p.find(".donut_runtime/samples") + 23);
		if(entry.is_directory()) {
			findAllSampleFiles(returnList, entry.path());
		} else if(entry.path().string().find(".wav") != string::npos) {
			returnList->push_back(SampleGUIItem {p});
		}
	}
}

void SampleLibrary::listLoaded() {
	for(auto& s : samples) {
		gui->output(s->name + "\n", false);
	}
}

Sample* SampleLibrary::get(const string& name) {
	for(auto& s : samples) {
		if(s->name == name) {
			return s;
		}
	}
	return samples[0];
}

Sample* SampleLibrary::getDefaultSample() {
	return get("default");
}

vector<Sample*>* SampleLibrary::getSamples() {
	return &samples;
}
