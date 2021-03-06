//
// Created by Daniël Kamp on 12/04/2022.
//

#ifndef DONUT_SAMPLELIBRARY_H
#define DONUT_SAMPLELIBRARY_H

#include <string>
#include <vector>
#include <filesystem>

#include <Global.h>
#include <DSP/Buffer.h>
#include <System/GUI.h>
#include <ext/AudioFile.h>

using namespace std;

struct Sample {
	string name;
	Buffer* sample;
};

class SampleLibrary {
	public:
		SampleLibrary (GUI* gui);
		~SampleLibrary ();
		
		bool load(const string&);
		void listLoaded();
		void list();
		
		Sample* get(const string&);
		Sample* getDefaultSample();
		vector<Sample*>* getSamples();
	private:
		vector<Sample*> samples;
		GUI* gui;
		
		void findAllSampleFiles(vector<SampleGUIItem>*, filesystem::path);
};


#endif //DONUT_SAMPLELIBRARY_H
