#include "Item.h"

Item::Item(string path, ofImage image, bool isVideo, bool isVideoPlaying)
{
    this->path = path;
    this->image = image;
    this->isVideo = isVideo;
    this->isVideoPlaying = isVideoPlaying;
}

string Item::getPath() {
    return path;
}

ofImage Item::getImage() {
    return image;
}

bool Item::getIsVideo() {
    return isVideo;
}

bool Item::getIsVideoPlaying() {
    return isVideoPlaying;
}

void Item::setVideoPlaying(bool isPlaying) {
    this->isVideoPlaying = isPlaying;
}

