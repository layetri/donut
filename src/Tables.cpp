//
// Created by Daniël Kamp on 13/03/2022.
//

#include <System/Tables.h>

Tables::Tables() {

}

Tables::~Tables() {
	delete sine;
	delete square;
	delete triangle;
	delete noise;
}

void Tables::generateWaveforms() {
	uint16_t max_t = (uint16_t) (samplerate / TABLE_FREQUENCY);

	square = new Buffer(max_t);
	sine = new Buffer(max_t);
	triangle = new Buffer(max_t);
	noise = new Buffer(max_t);
	
	filesystem::path path = filesystem::current_path() / ".donut_runtime/wavetable";
	for (const auto & entry : filesystem::directory_iterator(path)) {
		ifstream file(entry.path().c_str());
		string line;
		string filename = entry.path().filename().string();
		
		if(filename.find(to_string(samplerate)) != string::npos) {
			while (getline(file, line)) {
				istringstream iss(line);
				sample_t val;
				
				while (iss >> val) {
					if (filename.find("square") != string::npos) {
						square->write(val);
						square->tick();
					} else if (filename.find("sine") != string::npos) {
						sine->write(val);
						sine->tick();
					} else if (filename.find("triangle") != string::npos) {
						triangle->write(val);
						triangle->tick();
					} else if (filename.find("noise") != string::npos) {
						noise->write(val);
						noise->tick();
					}
				}
			}
		}
	}
}

Buffer* Tables::getSquare() {
	return square;
}

Buffer* Tables::getSine() {
	return sine;
}

Buffer* Tables::getTriangle() {
	return triangle;
}

Buffer* Tables::getNoise() {
	return noise;
}