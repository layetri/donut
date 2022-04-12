//
// Created by Daniël Kamp on 13/03/2022.
//

#ifndef DONUT_TABLES_H
#define DONUT_TABLES_H

#include "../Global.h"
#include "../DSP/Buffer.h"
#include <vector>
#include <math.h>
#include <filesystem>
#include <fstream>

#define TWO_PI (2 * M_PI)

using namespace std;

class Tables {
	public:
		Tables();
		~Tables();

		void generateWaveforms();

		Buffer* getSquare();
		Buffer* getSine();
		Buffer* getTriangle();
		Buffer* getNoise();

	private:
		Buffer* square;
		Buffer* sine;
		Buffer* triangle;
		Buffer* noise;
};


#endif //DONUT_TABLES_H
