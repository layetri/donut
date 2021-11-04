//
// Created by Daniël Kamp on 12/02/2021.
//

#ifndef SNOWSTORM_ENVELOPE_H
#define SNOWSTORM_ENVELOPE_H


class Envelope {
public:
    // Takes attack and decay in samples
    Envelope(int attack, int decay);

    virtual ~Envelope();

};


#endif //SNOWSTORM_ENVELOPE_H
