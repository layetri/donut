import numpy as np
import math
import matplotlib.pyplot as plt

TABLE_FREQUENCY = 2.0
SAMPLE_MAX_VALUE = 32767
NEG_SAMPLE_MAX_VALUE = -32768

samplerates = [44100, 48000, 96000]
odd = list(range(100))[1::2]

# Generate tables
for samplerate in samplerates:
    phase_step = TABLE_FREQUENCY / samplerate
    phase = 0.0

    sine = np.zeros(samplerate)
    square = np.zeros(samplerate)
    triangle = np.zeros(samplerate)
    adsr = np.zeros(samplerate)

    for i in range(samplerate):
        tri_tmp = 0
        sqr_tmp = 0
        for j in odd:
            tri_tmp += (math.cos(math.pi * 2 * (j * phase + 1.0)) / (j*j)) * SAMPLE_MAX_VALUE
        for j in range(1, 50):
            n = (2 * j) - 1
            sqr_tmp += (math.sin(math.pi * 2 * n * phase) / n) * SAMPLE_MAX_VALUE

        # Clip values
        sqr_tmp = (NEG_SAMPLE_MAX_VALUE < sqr_tmp < SAMPLE_MAX_VALUE) * sqr_tmp + (sqr_tmp > SAMPLE_MAX_VALUE) * SAMPLE_MAX_VALUE + (sqr_tmp < NEG_SAMPLE_MAX_VALUE) * NEG_SAMPLE_MAX_VALUE
        tri_tmp = (NEG_SAMPLE_MAX_VALUE < tri_tmp < SAMPLE_MAX_VALUE) * tri_tmp + (tri_tmp > SAMPLE_MAX_VALUE) * SAMPLE_MAX_VALUE + (tri_tmp < NEG_SAMPLE_MAX_VALUE) * NEG_SAMPLE_MAX_VALUE
        sqr_tmp = 0.005 * sqr_tmp + 0.995 * square[i-1]

        sine[i] = math.sin(math.pi * 2 * phase) * SAMPLE_MAX_VALUE
        square[i] = sqr_tmp
        triangle[i] = tri_tmp

        phase = (phase + phase_step < 1.0) * (phase + phase_step)

    # Dump to CSV
    plt.plot(square, "b")
    plt.plot(triangle, "r")
    plt.plot(sine, "g")
    plt.show()

    with open(".donut_runtime/wavetable/square_" + str(samplerate) + ".txt", "w") as sq:
        for val in square:
            sq.write(str(math.trunc(val)) + " ")

    with open(".donut_runtime/wavetable/sine_" + str(samplerate) + ".txt", "w") as si:
        for val in sine:
            si.write(str(math.trunc(val)) + " ")

    with open(".donut_runtime/wavetable/triangle_" + str(samplerate) + ".txt", "w") as tr:
        for val in triangle:
            tr.write(str(math.trunc(val)) + " ")

    with open(".donut_runtime/wavetable/adsr_" + str(samplerate) + ".txt", "w") as env:
        for val in adsr:
            env.write(str(val) + " ")
