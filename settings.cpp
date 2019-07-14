/*
 *  settings.cpp
 *  Selection_Recombination
 *
 *  Created by Joshua Schraiber on 4/30/13.
 *  Copyright 2013 UC Berkeley. All rights reserved.
 *
 */

#include "math.h"
#include "settings.h"
#include "param.h"
#include "MbRandom.h"
#include "popsize.h"

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

settings::settings(int argc, char* const argv[]) {

    //define defaults
    max_dt = 0.001;
    min_grid = 10;
    bridge = 0;
    mcmc = 0;
    linked_sites = 0;
    num_gen = 100;
    p = 0;
    num_test = 1000;
    rescale = -INFINITY;
    printFreq = 1000;
    sampleFreq = 1000;
    fracOfPath = 20;
    min_update = 10;
    baseName = "out";
    fOrigin = 0.0;
    infer_age = 0;
    popFile = "";
    inputFile = "";
    mySeed = time(0);
    output_tsv = 0;
    a1prop = 5.0;
    a2prop = 5.0;
    ageprop = 20.0;
    endprop = 2.0;
    timeprop = 0.1;
    pathprop = 20.0;
    a1start = 25.0;
    a2start = 50.0;
    set_gen = 0;
    set_N0 = 0;
    gen_time = 1;
    N0 = 0.5;
    h = 0.5;
    fix_h = false;
    min_freq = 0;
    ascertain = false;

    //read the parameters
    int ac = 1;
    while (ac < argc) {
        switch(argv[ac][1]) {
            case 'd':
                max_dt = atof(argv[ac+1]);
                ac += 2;
                break;
            case 'g':
                min_grid = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'n':
                num_gen = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'b':
                bridge = 1;
                bridge_pars = argv[ac+1];
                ac += 2;
                break;
            case 'R':
                output_tsv = 1;
                ac += 1;
                break;
            case 'p':
                p = 1;
                ac += 1;
                break;
            case 'T':
                num_test = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'r':
                rescale = atof(argv[ac+1]);
                ac += 2;
                break;
            case 'f':
                printFreq = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'o':
                baseName = std::string(argv[ac+1]);
                ac += 2;
                break;
            case 's':
                sampleFreq = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'F':
                fracOfPath = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'M':
                min_update = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'O':
                fOrigin = atof(argv[ac+1]);
                ac += 2;
                break;
            case 'e':
                mySeed = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'a':
                infer_age = 1;
                ac += 1;
                break;
            case 'P':
                popFile = argv[ac+1];
                ac += 2;
                break;
            case 'G':
                set_gen = 1;
                gen_time = atoi(argv[ac+1]);
                ac += 2;
                break;
            case 'N':
                set_N0 = 1;
                N0 = atoi(argv[ac+1]);
                ac += 2;
                break;
            case '0':
                a1prop = 0.0;
                a2prop = 0.0;
                a1start = 0.0;
                a2start = 0.0;
                ac += 1;
                break;
            case 'h':
                h = atof(argv[ac+1]);
                a1prop = 0.0;
                a1start = a2start*h;
                fix_h = true;
                ac += 2;
                break;
            case 'D':
                mcmc = 1;
                inputFile = argv[ac+1];
                ac += 2;
                break;
            case 'A':
                ascertain = true;
                min_freq = atof(argv[ac+1]);
                ac += 2;
                break;
        }
    }
}

std::vector<double> settings::parse_bridge_pars() {
    std::vector<double> pars(0);
    std::string string_pars(bridge_pars);
    std::istringstream stringstream_pars(string_pars);
    std::string cur_par;

    while (std::getline(stringstream_pars,cur_par,',')) {
        pars.push_back(atof(cur_par.c_str()));
    }
    if (pars.size() < 4) {
        std::cerr << "ERROR: Not enough bridge parameters" << std::endl;
        std::cerr << "Only " << pars.size() << " specified; 4 are required" << std::endl;
        exit(1);
    }
    return pars;
}

void settings::print() {
    std::cout << "max_dt\t" << max_dt << std::endl;
    std::cout << "min_grid\t" << min_grid << std::endl;
    if (bridge) {
        std::cout << "num_test\t" << num_test << std::endl;
        std::cout << "rescale\t" << rescale << std::endl;
        std::vector<double> pars = parse_bridge_pars();
        std::cout << "x0\t" << pars[0] << std::endl;
        std::cout << "xt\t" << pars[1] << std::endl;
        std::cout << "gamma\t" << pars[2] << std::endl;
        std::cout << "t\t" << pars[3] << std::endl;
    } else if (mcmc) {
        std::cerr << "num_gen\t" << num_gen << std::endl;
        if (linked_sites) {
            //file destinations
        } else {
            //parameters of the path
        }
    }
}

popsize* settings::parse_popsize_file() {
    if (popFile == "") {
        std::cerr << "ERROR: No population size history specified! Use -P option" << std::endl;
        exit(1);
    }
    
    
    if (set_gen && !set_N0) {
        std::cout << "Specified a generation time but not a base population size. Unless your times are measured in units of 2N0 years, this is likely an error" << std::endl;
    } else if (!set_gen && set_N0) {
        std::cout << "Specified base population size but not a generation time. Assuming times are measured in generations, converting all units to 2N0 generations" << std::endl;
    } else if (set_gen && set_N0) {
        std::cout << "Specified both generation time and base population size. Assuming times are measured in years, converting all units to 2N0 generations" << std::endl;
    } else {
        std::cout << "Did not specify either generation time or base population size. Assuming times are in units of 2N0 generations" << std::endl;
    }
    
    //make pop sizes
    return new popsize(*this);
}

bool comparePtrToSampleTime(sample_time* a, sample_time* b) { return (*a < *b); }

std::vector<sample_time*> settings::parse_input_file(MbRandom* r) {
    std::cout << "Parsing input" << std::endl;
    std::ifstream inFile(inputFile.c_str());
    std::string curLineString;
    int curCount;
    int curSS;
    double curLowTime;
    double curHighTime;
    double curTime;
    
    std::vector<sample_time*> sample_time_vec;
    
    while (getline(inFile, curLineString)) {
        std::istringstream curLine(curLineString);
        curLine >> curCount >> curSS >> curLowTime >> curHighTime;
        if (curCount < 0 || curCount > curSS) {
            std::cerr << "ERROR: Allele count is not between 0 and sample size: X = " << curCount << ", SS = " << curSS << std::endl;
            exit(1);
        }
        if (curLowTime > curHighTime) {
            std::cerr << "ERROR: Low end of time range higher than high end: t_low = " << curLowTime << ", t_high = " << curHighTime << std::endl;
            exit(1);
        }
        //Convert time units
        curLowTime /= (gen_time*2*N0);
        curHighTime /= (gen_time*2*N0);
        //Set the sample time randomly to initialize
        curTime = r->uniformRv(curLowTime, curHighTime);
        
        sample_time* cur_sample_time = new sample_time(curTime, curLowTime, curHighTime, curSS, curCount, r);
        sample_time_vec.push_back(cur_sample_time);
    }
    
    //Sort so in order by current value
    std::sort(sample_time_vec.begin(), sample_time_vec.end(),comparePtrToSampleTime);
    
    
    //check that most recent time point is fixed
    int num_sam = sample_time_vec.size();
    if (sample_time_vec[num_sam-1]->get_oldest()<sample_time_vec[num_sam-1]->get_youngest()) {
        std::cerr << "ERROR: most recent time point must have no uncertainty" << std::endl;
        exit(1);
    }

    return sample_time_vec;
}


