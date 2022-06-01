#pragma once

#include <iostream>
#include "ofMain.h"
#include "ofApp.h"

using namespace std;

namespace Metadata {
    ofApp* app;

    void filterByColor(float hue);
    void filterItems(string filter);
};
