//
// Created by Daniël Kamp on 04/03/2021.
//

#ifndef _LOCALENV_DRYWETMIXER_H
#define _LOCALENV_DRYWETMIXER_H

class DryWetMixer {
  public:
    DryWetMixer(float ratio);
    ~DryWetMixer();

    void update();

    void setRatio(float ratio);
  private:
    float ratio;
};


#endif //_LOCALENV_DRYWETMIXER_H
