/* Refer to the README.md in the example's root folder for more information on usage */

#pragma once

#include "ofMain.h"
#include "President.h"
#include "ofxXmlSettings.h"
#include "ofxCvHaarFinder.h"
#include "ofxCv.h"
#include "ofxDatGui.h"
#include "Item.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "GenerateMetadata.h"
#include "ofAppData.h"
//#include "Metadata.h"

#define IMAGE_MEDIA_TYPE 0

#define VIDEO_MEDIA_TYPE 1

using namespace cv;
using namespace ofxCv;

//using namespace Metadata;


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
        string edgesFilter(President* president);
        string textureFilter(President* president);
        //double rhythmFilter(string path);
		void getPresidentsInfo();
		void getPresidentsInfoThread(int startPres, int endPres);
		void getPresidentInfo(int xmlIndex);
		/*void generateMetadataThread(int startPres, int endPres);
		void generateMetadata(President *president, ofxCvHaarFinder* finder);
		void startMetadataGeneration();*/
		//void generateMetadata(string presidentName, string path, ofImage* image, bool isVideo);
		void importMetadata(ofxDatGuiButtonEvent e);
        void extractMetadata(ofxDatGuiButtonEvent e);
		ofImage *getPresidentProfilePicture(President *president);
        void initButtons();
        //int objectTimesFilter(ofImage image, ofImage objImage);
    
        void importMetadata();
        void exportMetadata();
		void drawStringCentered(const std::string& c, float x, float y);
		void drawStringRight(const std::string& c, float x, float y);
		bool isMousePtrInCarrousel(int x, int y);
		bool isMousePtrInsideCenterPresident(int x, int y);
		bool isMousePtrOnCenterPresidentLeft(int x, int y);
		bool isMousePtrOnCenterPresidentRight(int x, int y);
		bool isMousePtrBelowNeighbourPresidents(int x, int y);
		int getPresidentIndexWhereMouseIsPointing(int x, int y);
		void switchPresident(President* previousPresident);

		#define MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT -1

		ofxButton pauseBtn;
    
		ofTrueTypeFont myfont;

		
		
		// we will have a dynamic number of images, based on the content of a directory:
		ofDirectory imagesDir;
		ofDirectory videosDir;
		//vector<ofImage> images;
		/*vector<ofImage*> profilePictures;

		vector<ofVideoPlayer*> biographyVideos;*/

		//map<int, PresidentMetadata*> presidentsMetadata;


		bool frameByframe;


		ofxPanel gui;

		vector<Item*> items; // contains the filtered items
		vector<Item*> auxItems; // contains all the items

		int itemsSize;

		ofxDatGuiButton* im1;
		ofxDatGuiButton* im2;
		ofxDatGuiButton* im3;

		
		ofAppData *data;

		//computed
		int windowXCenter;
		int centerPresidentImgXPos;

		
		GenerateMetadata generateMetadataThread;
    
		enum Filters
		{
			NONE, EDGES, TEXTURE
		};

		Filters currentFilterApplied;

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

