#pragma once

#include "Global.h"

class FloatBuffer {
    public:
        FloatBuffer(int length);
        ~FloatBuffer();

        virtual void tick();
        void write(float sample);
        virtual float getSample();

        void setPosition(int position);

	protected:
        float* data;
        int position;
        int length;
};