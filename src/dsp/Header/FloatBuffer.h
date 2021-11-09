class FloatBuffer {
    public:
        FloatBuffer(int length);
        ~FloatBuffer();

        void tick();
        void write(float sample);
        float getSample();
    private:
        float* data;
        int position;
        int length;
};