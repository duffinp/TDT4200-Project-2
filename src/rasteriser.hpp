#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <algorithm>
#include "utilities/floats.hpp"
#include "utilities/geometry.hpp"

std::vector<unsigned char> rasterise( std::vector<Mesh> &meshs,
                                      unsigned int width,
                                      unsigned int height,
                                      unsigned int depthLimit = 1);