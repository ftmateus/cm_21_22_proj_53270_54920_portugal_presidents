/* Refer to the README.md in the example's root folder for more information on usage */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include <ofxCVHaarFinder.h>
#include "ofxGui.h"

#define IMAGE_MEDIA_TYPE 0

#define VIDEO_MEDIA_TYPE 1

using namespace cv;

class ofApp : public ofBaseApp {

	public:

		typedef struct
		{
			string name;
			string startDate;
			string endDate;
			string birthDate;
			string deathDate;
			ofImage* profilePicture;
			ofVideoPlayer* biographyVideo;
			vector<ofImage*> otherImages;
		} PresidentMedia;

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
        string edgesFilter(string presidentName, ofImage image);
        string textureFilter(string presidentName, ofImage image);
        //double rhythmFilter(string path);
        void generateMetadata(string presidentName, string path, ofImage image, bool isVideo);
        void importMetadata();
        void exportMetadata();
				void drawStringCentered(const std::string& c, float x, float y);
		void drawStringRight(const std::string& c, float x, float y);
		void drawPresidentDescription();
		bool isMousePtrInCarrousel(int x, int y);
		bool isMousePtrInsideCenterPresident(int x, int y);
		bool isMousePtrOnCenterPresidentLeft(int x, int y);
		bool isMousePtrOnCenterPresidentRight(int x, int y);
		bool isMousePtrBelowNeighbourPresidents(int x, int y);
		int getPresidentIndexWhereMouseIsPointing(int x, int y);
		void switchPresident(PresidentMedia* previousPresident);

		#define MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT -1

		ofxButton pauseBtn;
    
		ofTrueTypeFont myfont;
		
		// we will have a dynamic number of images, based on the content of a directory:
		ofDirectory imagesDir;
		ofDirectory videosDir;
		//vector<ofImage> images;
		/*vector<ofImage*> profilePictures;

		vector<ofVideoPlayer*> biographyVideos;*/

		vector<PresidentMedia*> presidentsMedias;

		bool frameByframe;

		int currentPresidentIdx;

		ofxPanel gui;

		

		//computed
		int windowXCenter;
		int centerPresidentImgXPos;

		ofxXmlSettings mainXml;
    
        ofxCvHaarFinder finder;

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

