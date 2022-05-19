#pragma once
#include "ofMain.h"
#include <string>

class Item
{
private:
    string path;
    ofImage image;
    bool isVideo;
    bool isVideoPlaying;

public:
    Item(string path, ofImage image, bool isVideo, bool isVideoPlaying);
    string getPath();
    ofImage getImage();
    bool getIsVideo();
    bool getIsVideoPlaying();
    void setVideoPlaying(bool isPlaying);
};

