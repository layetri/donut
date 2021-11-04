//
// Created by Daniël Kamp on 22/06/2021.
//

#include "Header/Analyzer.h"
#define TWO_PI (M_PI * 2)

Analyzer::Analyzer(int size, Buffer* input) {
  buffer = input;
  fft_size = size;
  fft2 = fft_size / 2;
  f_inc = 44100.0 / fft_size;

  x = 0;
  z = 0;
}

Analyzer::~Analyzer() {}

float *Analyzer::getFloatValues() {
  return xz;
}

// ========================================
//  Analysis process
// ========================================
void Analyzer::process() {
  CArray chunk(fft2);

  float val;
  double freq_avg = 0.0, amp_avg = 0.0;

  // Do FFT analysis
  for(int i = 0; i < fft_size; i++) {
    val = (buffer->readBack((fft2) - i) / 65536.0) - 1.0;

    amp_avg += abs(val);
    chunk[i] = val;
  }
  fft(chunk);


  // Calculate weighed average of spectrum and amplitude
  int c = 0, f = 0;
  for(int i = 0; i < 16000 / f_inc; i++) {
    f += f_inc;
    if (f > 100) {
      freq_avg += (abs(std::arg(chunk[i]) / M_PI) * std::abs(chunk[i]));
      c++;
    }
  }

  amp_avg = amp_avg / fft2;
  freq_avg = abs(freq_avg) / c;

  // Calculate variables for spatialisation
  // - Calculate Z value based on spectral information and amplitude
  // Z: [-1.0, 1.0]
  z = (freq_avg * amp_avg) + 0.5 * z;
  xz[1] = z;

  // - Calculate X value based on spectral information, LFO automation,
  //   and number of sources in given range
  // X: [0.0, 1.0]
  x = 0.15 * freq_avg + 0.85 * x;
  xz[0] = x;
}

// Cooley–Tukey FFT (but an optimized version)
// Borrowed from https://rosettacode.org/wiki/Fast_Fourier_transform#C.2B.2B
void Analyzer::fft(CArray &chunk) {
  // DFT
  unsigned int N = chunk.size(), k = N, n;
  double thetaT = 3.14159265358979323846264338328L / N;
  Complex phiT = Complex(cos(thetaT), -sin(thetaT)), T;
  while (k > 1) {
    n = k;
    k >>= 1;
    phiT = phiT * phiT;
    T = 1.0L;
    for (unsigned int l = 0; l < k; l++) {
      for (unsigned int a = l; a < N; a += n) {
        unsigned int b = a + k;
        Complex t = chunk[a] - chunk[b];
        chunk[a] += chunk[b];
        chunk[b] = t * T;
      }
      T *= phiT;
    }
  }
  // Decimate
  unsigned int m = (unsigned int)log2(N);
  for (unsigned int a = 0; a < N; a++) {
    unsigned int b = a;
    // Reverse bits
    b = (((b & 0xaaaaaaaa) >> 1) | ((b & 0x55555555) << 1));
    b = (((b & 0xcccccccc) >> 2) | ((b & 0x33333333) << 2));
    b = (((b & 0xf0f0f0f0) >> 4) | ((b & 0x0f0f0f0f) << 4));
    b = (((b & 0xff00ff00) >> 8) | ((b & 0x00ff00ff) << 8));
    b = ((b >> 16) | (b << 16)) >> (32 - m);

    if (b > a) {
      Complex t = chunk[a];
      chunk[a] = chunk[b];
      chunk[b] = t;
    }
  }
}