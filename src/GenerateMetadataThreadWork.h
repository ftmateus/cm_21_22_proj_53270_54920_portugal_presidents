#pragma once

#include "ofMain.h"
#include "President.h"

class GenerateMetadataThreadWork
{
    public:
        President* president;
        float luminance;
        float color;
        int faces;
        string texture;
        string edges;
        double rhythm;
};
