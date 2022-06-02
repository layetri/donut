#pragma once

#include "../Global.h"

class FloatBuffer {
    public:
        FloatBuffer(int length);
        ~FloatBuffer();

        virtual void tick();
        void write(float sample);
        void purge();
		virtual float getSample();
		virtual float getSampleAt(uint n);
		
        void setPosition(int position);

        float* data;
	protected:
        int position;
        int length;
};