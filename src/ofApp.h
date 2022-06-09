/* Refer to the README.md in the example's root folder for more information on usage */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxCvHaarFinder.h"
#include "ofxCv.h"
#include "ofxDatGui.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "GenerateMetadata.h"
#include "ofAppData.h"

#define IMAGE_MEDIA_TYPE 0

#define VIDEO_MEDIA_TYPE 1

using namespace cv;
using namespace ofxCv;
using cv::ORB;

class ofApp : public ofBaseApp {

	public :

		enum Filters {
			NO_FILTER, EDGES_FILTER, TEXTURE_FILTER
		};

		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void mouseReleased(int x, int y, int button);
		void windowResized(ofResizeEventArgs& resize);
		void mouseScrolled(int x, int y, float scrollX, float scrollY);
		void drawPresidents();
		void drawBiographyVideo();
		void getPresidentsInfo();
		void getPresidentsInfoThread(int startPres, int endPres);
		void getPresidentInfo(int xmlIndex);
		ofImage *getPresidentProfilePicture(President *president);

		void applyFilter(Filters filter);
		void pausePlayVideo();
		void generateMetadata();

		void drawStringCentered(const std::string& c, float x, float y);
		void drawStringRight(const std::string& c, float x, float y);
		bool isMousePtrInCarrousel(int x, int y);
		bool isMousePtrInsideCenterPresident(int x, int y);
		bool isMousePtrOnCenterPresidentLeft(int x, int y);
		bool isMousePtrOnCenterPresidentRight(int x, int y);
		bool isMousePtrBelowNeighbourPresidents(int x, int y);
		int getPresidentIndexWhereMouseIsPointing(int x, int y);
		void switchPresident(President* previousPresident);

		void indexPresidentForSearch(President *president);
		void indexStringForSearch(String str, President* president);
		void search();
		void cancelSearch();
		void setVideoFullScreen();

		void importMetadata();

		#define MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT -1

		ofxButton pauseBtn;
    
		ofTrueTypeFont myfont;

		bool fullScreen;
		
		// we will have a dynamic number of images, based on the content of a directory:
		ofDirectory imagesDir;
		ofDirectory videosDir;

		bool frameByframe;

		ofxPanel gui;
    
		std::mutex mutex;

		//computed
		int windowXCenter;
		int centerPresidentImgXPos;

		GenerateMetadata generateMetadataThread;

		ofAppData *appData;

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
