//
// Created by Daniël Kamp on 12/02/2021.
//

#ifndef DONUT_AD_H
#define DONUT_AD_H


class AD {
    public:
        // Takes attack and decay in samples
        AD(int attack, int decay);

        virtual ~AD();

};


#endif //DONUT_AD_H
