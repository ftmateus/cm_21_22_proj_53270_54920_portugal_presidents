/* Refer to the README.md in the example's root folder for more information on usage */

#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"
#include "ofxCvHaarFinder.h"
#include "ofxCv.h"
#include "ofxGui.h"
#include "Item.h"

#define IMAGE_MEDIA_TYPE 0

#define VIDEO_MEDIA_TYPE 1

using namespace cv;
using namespace ofxCv;

class Gallery : public ofBaseApp {

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
        void windowResized(ofResizeEventArgs& resize);
        void dragEvent(ofDragInfo dragInfo);
        void gotMessage(ofMessage msg);
        void mouseScrolled(int x, int y, float scrollX, float scrollY);
        void drawPresidents();
        void drawVideo(ofVideoPlayer* vid);
        string edgesFilter(string presidentName, ofImage image);
        string textureFilter(string presidentName, ofImage image);
        //double rhythmFilter(string path);
        void generateMetadata(string presidentName, string path, ofImage image, bool isVideo);
        /*void importMetadata(ofxButton );
        void extractMetadata(ofxButton );*/
        bool isMousePtrInCarrousel(int x, int y);
        void drawStringCentered(const std::string& c, float x, float y);
        void drawStringRight(const std::string& c, float x, float y);
        void drawPresidentDescription();
        void initXmlObjects();
        void filterByColor(float hue);
    
        ofTrueTypeFont myfont;
        
        // we will have a dynamic number of images, based on the content of a directory:
        ofDirectory dir;
        //vector<ofImage> images;
        vector<ofBaseDraws*> mediaFiles;

        vector<int> mediaTypes;

        bool frameByframe;

        int currentMedia;

        //computed
        int windowXCenter;
        int centerPresidentImgXPos;

        ofxXmlSettings mainXml;
    
        ofxCvHaarFinder finder;
    
        ofxButton* nextButton;
        ofVideoPlayer video;
        int videoPlaying;
        float currentFrame;

        const double PRESIDENT_PORTRAIT_ASPECT_RATIO = 4.0 / 5.0;
        const int SPACE_BETWEEN_PRESIDENTS = 25;

        const int PRESIDENTS_CARROUSEL_Y_POS = 75;

        const int CENTER_PRESIDENT_IMG_HEIGHT = 300;
        const int CENTER_PRESIDENT_IMG_WIDTH = CENTER_PRESIDENT_IMG_HEIGHT * PRESIDENT_PORTRAIT_ASPECT_RATIO;

        const int NEIGHBOUR_PRESIDENT_IMG_HEIGHT = 200;
        const int NEIGHBOUR_PRESIDENT_IMG_WIDTH = NEIGHBOUR_PRESIDENT_IMG_HEIGHT * PRESIDENT_PORTRAIT_ASPECT_RATIO;
        
};

