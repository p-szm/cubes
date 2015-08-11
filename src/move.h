//
//  move.h
//  Cubes
//
//  Created by Patrick on 10/08/2015.
//  Copyright (c) 2015 Patrick. All rights reserved.
//

#ifndef __Cubes__move__
#define __Cubes__move__

#include <stdio.h>
#include "vec.h"

class Move
{
public:
    Move(Vec3 dir, int limit);
    int x, y, z;
    int limit;
};

#endif /* defined(__Cubes__move__) */
