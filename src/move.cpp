//
//  move.cpp
//  Cubes
//
//  Created by Patrick on 10/08/2015.
//  Copyright (c) 2015 Patrick. All rights reserved.
//

#include "move.h"

Move::Move(Vec3 dir, int limit): limit(limit)
{
    x = dir.x;
    y = dir.y;
    z = dir.z;
}