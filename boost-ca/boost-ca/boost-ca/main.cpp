//
//  main.cpp
//  boost-ca
//
//  Created by Кирилл Романовский on 13.03.13.
//  Copyright (c) 2013 mstu. All rights reserved.
//
#include <algorithm>
#include <string>
#include <iostream>
#include <ctime>

#include "CellGraph.h"

#include <boost/iostreams/device/mapped_file.hpp>

using namespace std;

const std::string kFilePath("/Users/lunatik/opencl/ca-crypto/boost-ca/boost-ca/graph.ca");

int main(int argc, const char * argv[])
{
    srand ((unsigned int)time(NULL));
    
    const int V = 230;
    CellGraph g(V);
    g.loadFromFile(kFilePath, kFilePath);
    
    int iter = 0;
    cout << "start" << endl;
    clock_t begin_time = clock();
    const size_t iterationCount = 1000000;
    while (++iter < iterationCount)
    {
        g.encrypt();
    }
    cout << "finish1: " << float( clock () - begin_time ) /  CLOCKS_PER_SEC << endl;
    g.printState();
    cout.flush();
    return 0;
}

