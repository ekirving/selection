/*
 *  popsize.cpp
 *  Selection_Recombination
 *
 *  Created by Joshua Schraiber on 11/22/13.
 *  Copyright 2013 UC Berkeley. All rights reserved.
 *
 */

#include "popsize.h"
#include "settings.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <sstream>



popsize::popsize(settings& s) {
    sizes.resize(0);
    sizes.push_back(0);
    rates.resize(0);
    rates.push_back(0);
    times.resize(0);
    times.push_back(0);
    std::string pop_size_file = s.get_popFile();
    std::ifstream popFile(pop_size_file.c_str());
    std::string curLineString;
    double curSize;
    double curRate;
    std::string curTime;
    double g = s.get_gen_time();
    double N0 = s.get_N0();
    while (getline(popFile, curLineString)) {
        std::istringstream curLine(curLineString);
        curLine >> curSize >> curRate >> curTime;
        if (s.get_set_N0()) {
            curSize /= N0;
        }
        sizes.push_back(curSize);
        rates.push_back(curRate*2*N0);
        if (curTime == "-Inf") {
            times.push_back(-INFINITY);
        } else {
            times.push_back(atof(curTime.c_str())/(g*2*N0));
        }
    }
    //check some things
    for (int i = 0; i < sizes.size()-1; i++) {
        //the times better be in decreasing order!
        if (times[i] <= times[i+1]) {
            std::cerr << "ERROR: Times are not in decreasing order!" << std::endl;
            std::cerr << "Time " << i << " <= Time " << i+1 << std::endl;
            std::cerr << times[i] << " <= " << times[i+1] << std::endl;
            exit(1);
        }
    }
    //the last thing better be infinity, and not change
    if (times[times.size()-1] !=-INFINITY || rates[rates.size()-1] != 0) {
        std::cerr << "ERROR: Final time point is not infinity OR final epoch not constant" << std::endl;
        std::cerr << "Size Rate Time" << std::endl;
        std::cerr << sizes[sizes.size()-1] << " " << rates[rates.size()-1] << " " << times[times.size()-1] << std::endl;
        exit(1);
    }
    computeT();
}

void popsize::computeT() {
    T.resize(0);
    T.push_back(0);
    for (int j = 1; j < times.size()-1; j++) {
        if (rates[j]!=0) {
            T.push_back(1.0/(sizes[j]*rates[j])*(1-exp(-rates[j]*(times[j-1]-times[j]))));
        } else {
            T.push_back((times[j-1]-times[j])/sizes[j]);
        }
    }
}

double popsize::getSize (double t, bool leftLim) {
    int J;
    double size;
    for (int i = 0; i < times.size(); i++) {
        if (times[i] >= t) {
            J = i;
        } else {
            break;
        }
    }
    if (leftLim || t != times[J]) {
        if (times[J+1] != -INFINITY) {
            size = sizes[J+1]*exp(rates[J+1]*(t-times[J+1]));
        } else {
            size = sizes[J+1];
        }
    } else {
        size = sizes[J];
    }
    return size;
}

double popsize::getDeriv(double t, bool leftLim) {
    int J;
    double d;
    for (int i = 0; i < times.size(); i++) {
        if (times[i] >= t) {
            J = i;
        } else {
            break;
        }
    }
    if (leftLim || t != times[J]) {
        if (times[J+1] != -INFINITY) {
            d = rates[J+1]*sizes[J+1]*exp(rates[J+1]*(t-times[J+1]));
        } else {
            d = 0;
        }
    } else {
        d = rates[J]*sizes[J];
    }
    return d;
}

double popsize::getTau(double t) {
    double tau = 0;
    int J;
    for (int i = 0; i < times.size(); i++) {
        if (times[i] >= t) {
            tau += T[i];
            J = i;
        } else {
            break;
        }
    }
    if (times[J+1] != -INFINITY) {
        //it's not in the very last interval
        if (rates[J+1]!=0) {
            tau += 1.0/(sizes[J+1]*rates[J+1])*(exp(-rates[J+1]*(t-times[J+1]))-exp(-rates[J+1]*(times[J]-times[J+1])));
        } else {
            tau += (times[J]-t)/sizes[J+1];
        }
    } else {
        //the last one is constant!
        tau += (times[J]-t)/sizes[J+1];
    }

    return -tau;
}

std::vector<double> popsize::getTau(const std::vector<double>& t_vec) {
    std::vector<double> tau_vec(t_vec.size());
    for (int i = 0; i < t_vec.size(); i++) {
        tau_vec[i] = getTau(t_vec[i]);
    }
    return tau_vec;
}

std::vector<double> popsize::getBreakTimes(double t0, double t) {
    int first_ind;
    int last_ind;
    int J = 0;
    for (int i = 0; i < times.size(); i++) {
        if (times[i] >= t0) {
            J = i;
        } else {
            break;
        }
    }
    first_ind = J+1;
    for (int i = 0; i < times.size(); i++) {
        if (times[i] >= t) {
            J = i;
        } else {
            break;
        }
    }
    last_ind = J;

    std::vector<double> to_return;
    if (last_ind == first_ind - 1) {
        //if they're both in the same interval, just return the two times
        to_return.push_back(t0);
        to_return.push_back(t);
    } else {
        //if they're not, need to find all the ones in between
        to_return.push_back(t0);
        int k = first_ind-1;
        if (times[k] == t0) {
            k--;
        }
        while (k > last_ind) {
            to_return.push_back(times[k]);
            k--;
        }
        to_return.push_back(t);
    }
    return to_return;
}