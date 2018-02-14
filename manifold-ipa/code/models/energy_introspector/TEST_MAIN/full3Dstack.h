/*
 * full3Dstack.h
 *
 *  Created on: Jul 25, 2017
 *      Author: karthik
 */

#ifndef FULL3DSTACK_H_
#define FULL3DSTACK_H_

#include <iostream>
#include <assert.h>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <numeric> // for accumulate
#include <algorithm> // for max_element

#include "../energy_introspector.h"
#define NUM_LAYERS 6
#define NUM_CORES 16
#define TIME_STEP 0.002

template<typename Out>
void split(const std::string &s, char delim, Out result) {
    std::stringstream ss;
    ss.str(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        *(result++) = item;
    }
}

std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, std::back_inserter(elems));
    return elems;

}

EI::power_t stackpower[NUM_LAYERS][NUM_CORES];

bool flag = true;

void print_temperatures(EI::energy_introspector_t *energy_introspector, int iter);
void input_power(EI::energy_introspector_t *energy_introspector, int iter);

#endif /* FULL3DSTACK_H_ */
