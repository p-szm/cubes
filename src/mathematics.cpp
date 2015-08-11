//
//  mathematics.cpp
//  Cubes
//
//  Created by Patrick on 10/08/2015.
//  Copyright (c) 2015 Patrick. All rights reserved.
//

#include "mathematics.h"

double abs_d(double x)
{
    if (x >= 0)
        return x;
    return -x;
}

double max(double x, double y)
{
    if (x > y)
        return x;
    return y;
}