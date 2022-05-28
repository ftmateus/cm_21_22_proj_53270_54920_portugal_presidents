/* Refer to the README.md in the example's root folder for more information on usage */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxCvHaarFinder.h"
#include "ofxCv.h"
#include "ofxDatGui.h"
#include "Item.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "Metadata.h"
#include "GenerateMetadataThreadWork.h"
#include "President.h"

#define IMAGE_MEDIA_TYPE 0

#define VIDEO_MEDIA_TYPE 1

using namespace cv;
using namespace ofxCv;


class ofApp : public ofBaseApp {

	public :
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
		void windowResized(ofResizeEventArgs& resize);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);		
		void mouseScrolled(int x, int y, float scrollX, float scrollY);
		void drawPresidents();
		void drawBiographyVideo();
        //void filterEdgeAndTexture();
		void getPresidentsInfo();
		void getPresidentsInfoThread(int startPres, int endPres);
		void getPresidentInfo(int xmlIndex);
		void onButtonEvent(ofxDatGuiButtonEvent e);


		//void generateMetadata(string presidentName, string path, ofImage* image, bool isVideo);
		void drawPresidentDescription();
        void initButtons();
        //int objectTimesFilter(ofImage image, ofImage objImage);
    
		void drawStringCentered(const std::string& c, float x, float y);
		void drawStringRight(const std::string& c, float x, float y);
		bool isMousePtrInCarrousel(int x, int y);
		bool isMousePtrInsideCenterPresident(int x, int y);
		bool isMousePtrOnCenterPresidentLeft(int x, int y);
		bool isMousePtrOnCenterPresidentRight(int x, int y);
		bool isMousePtrBelowNeighbourPresidents(int x, int y);
		int getPresidentIndexWhereMouseIsPointing(int x, int y);
		void switchPresident(President* previousPresident);
		void ofApponButtonEvent(ofxDatGuiButtonEvent e);

		#define MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT -1

		ofxButton pauseBtn;
    
		ofTrueTypeFont myfont;

		bool isGeneratingMetadata;
		
		// we will have a dynamic number of images, based on the content of a directory:
		ofDirectory imagesDir;
		ofDirectory videosDir;
		//vector<ofImage> images;
		/*vector<ofImage*> profilePictures;

		vector<ofVideoPlayer*> biographyVideos;*/

		map<int, President*> presidentsMedias;

		bool frameByframe;

		int currentPresidentIdx;

		ofxPanel gui;

		vector<Item*> items; // contains the filtered items
		vector<Item*> auxItems; // contains all the items

		int itemsSize;

		ofxDatGuiButton* importMetadataBtn;
		ofxDatGuiButton* generateMetadataBtn;


		
		map<int, GenerateMetadataThreadWork*> threadsWork;
		

		std::mutex mutex;

		//computed
		int windowXCenter;
		int centerPresidentImgXPos;

		ofxXmlSettings presidentsXml;

		ofxXmlSettings presidentsMetadataXml;

		const double PRESIDENT_PORTRAIT_ASPECT_RATIO = 4.0 / 5.0;
		const int SPACE_BETWEEN_PRESIDENTS = 25;

		const int BIOGRAPHY_VIDEO_WIDTH = 640;
		const int BIOGRAPHY_VIDEO_HEIGH = 360;

		const int PRESIDENTS_CARROUSEL_Y_POS = 75;

		const int CENTER_PRESIDENT_IMG_HEIGHT = 300;
		const int CENTER_PRESIDENT_IMG_WIDTH = CENTER_PRESIDENT_IMG_HEIGHT * PRESIDENT_PORTRAIT_ASPECT_RATIO;

		const int NEIGHBOUR_PRESIDENT_IMG_HEIGHT = 200;
		const int NEIGHBOUR_PRESIDENT_IMG_WIDTH = NEIGHBOUR_PRESIDENT_IMG_HEIGHT * PRESIDENT_PORTRAIT_ASPECT_RATIO;

};

