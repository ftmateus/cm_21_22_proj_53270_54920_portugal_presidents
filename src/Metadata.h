#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "ofApp.h"
#include "President.h"

class Metadata : public ofBaseApp
{
    public :
	void startMetadataGeneration(ofApp* app);

	void generateMetadataThread(ofApp* app, int startPres, int endPres);

	void generateMetadata(ofApp* app, President* president, ofxCvHaarFinder* finder);

	void importMetadata(ofApp* app, int index);

	void extractMetadata(ofApp* app);

	string edgesFilter(President* president);

	string textureFilter(President* president);

	double rhythmFilter(ofVideoPlayer* video);

	void filterByColor(ofApp* app, float hue);

	void filterItems(ofApp* app, string filter);
};
