/* Refer to the README.md in the example's root folder for more information on usage */

#pragma once

#include "ofMain.h"
#include <ofxXmlSettings.h>

#define IMAGE_MEDIA_TYPE 0

#define VIDEO_MEDIA_TYPE 1

class ofApp : public ofBaseApp {

	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
		void drawPresident();
		void drawVideo(ofVideoPlayer* vid);
        void edgesFilter(string itemName, ofImage image);
        void textureFilter(string itemName, ofImage image);
        void generateMetadata(string itemName, string path, ofImage image, bool isVideo);
        void importMetadata();
        void exportMetadata();
    
		ofTrueTypeFont myfont;
		
		// we will have a dynamic number of images, based on the content of a directory:
		ofDirectory dir;
		//vector<ofImage> images;
		vector<ofBaseDraws*> mediaFiles;

		vector<int> mediaTypes;

		bool frameByframe;

		int currentMedia;

		ofxXmlSettings mainXml;


		const double aspectRatio = 4.0 / 5.0;
		const int spaceBetween = 25;

		const int presidentCarrouselYPos = 75;
		const int centerPresidentImgHeight = 300;
		const int centerPresidentImgWidth = centerPresidentImgHeight * aspectRatio;
		const int neighbourPresidentImgHeight = 200;
		const int neighbourPresidentImgWidth = neighbourPresidentImgHeight * aspectRatio;
		
};

