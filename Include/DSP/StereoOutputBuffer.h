//
// Created by Daniel Kamp on 2-6-22.
//

#ifndef DONUT_STEREOOUTPUTBUFFER_H
#define DONUT_STEREOOUTPUTBUFFER_H

#include <DSP/FloatBuffer.h>

class StereoOutputBuffer {
	public:
		StereoOutputBuffer();
		~StereoOutputBuffer();
		
		void writeSamplePair(float, float);
		void recycle();
		void setBlockSize(uint);
		void tick();
		void tickReadHead();
		
		void initAsReadyToRead();
		void resetReadCount() {samples_read = 0;};
		
		float getLeftSample();
		float getRightSample();
		
		uint getBlockSize() const {return block_size;};
		
		bool hasReachedBlockEnd(uint pos) const;
		bool isDoneReading();
		bool isReadyToWrite();
		bool isReadyToRead();
	
		
		float* getl() {return left_channel.data;};
		float* getr() {return right_channel.data;};
	private:
		FloatBuffer left_channel, right_channel;
		uint samples_written = 0, samples_read = 0, block_size = 0;
		bool _ready_to_write = true;
		bool _ready_to_read = false;
};


#endif //DONUT_STEREOOUTPUTBUFFER_H
