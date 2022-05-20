#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    
    ofSetWindowTitle("🇵🇹 Presidentes de Portugal - David Pereira e Francisco Mateus 🇵🇹");

    frameByframe = false;

    gui.setup(); // most of the time you don't need a name

    dir.listDir("images");
    dir.allowExt("jpg");
    dir.allowExt("png");
    //dir.allowExt("mov");

    if (!mainXml.load("data_xml/presidents.xml")) {
        ofLogError() << "Couldn't load file";
        assert(FALSE);
    }
    mainXml.pushTag("presidents");

    dir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

    //allocate the vector to have as many ofImages as files
    if( dir.size() ){
        mediaFiles.reserve(dir.size());
        mediaTypes.reserve(dir.size());
    }

    int n_presidents = mainXml.getNumTags("president");

    for (int i = 0; i < n_presidents; i++) {
        string profilePicture = mainXml.getValue("president:profilePicture", "", i);

        ofImage* img = new ofImage();
        img->load("images/" + profilePicture);
        mediaFiles.push_back(img);
        mediaTypes.push_back(IMAGE_MEDIA_TYPE);

        cout << profilePicture;
    }
    currentMedia = 0;

    ofBackground(ofColor::white);
}

//--------------------------------------------------------------
void ofApp::update() {
    if (mediaTypes[currentMedia] == VIDEO_MEDIA_TYPE)
    {
        ofVideoPlayer* vid = (ofVideoPlayer*)mediaFiles[currentMedia];

        vid->update();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    {
        myfont.load("arial.ttf", 34);
        ofRectangle titleBox = myfont.getStringBoundingBox("Presidentes de Portugal", 50, 50);
        int windowXCenter = ofGetWidth() / 2;

        myfont.drawString("Presidentes de Portugal", windowXCenter - titleBox.width / 2, 50);
        titleBox.~ofRectangle();
    }

    drawPresidents(); 

    drawPresidentDescription();
}

void ofApp::drawPresidentDescription()
{
    string presidentDescription = mainXml.getValue("president:description", "", currentMedia);
}

void ofApp::drawPresidents()
{    
    ofSetColor(ofColor::white);
    ofImage* centerPresidentImg = (ofImage*)mediaFiles[currentMedia];
    centerPresidentImg->draw(centerPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, CENTER_PRESIDENT_IMG_WIDTH, CENTER_PRESIDENT_IMG_HEIGHT);
    
    for (int prevImageIdx = currentMedia - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
    {        
        int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

        ofImage* prevPresidentImg = (ofImage*)mediaFiles[prevImageIdx];
        prevPresidentImg->draw(previousPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }
   
    for (int nextImageIdx = currentMedia + 1, times = 1; nextImageIdx < mediaFiles.size(); nextImageIdx++, times++)
    {
        int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH*(times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (nextPresidentImgXPos  >= ofGetWidth()) break;

        ofImage* nextPresidentImg = (ofImage*)mediaFiles[nextImageIdx];
        nextPresidentImg->draw(nextPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }

    myfont.load("arial.ttf", 20);
    ofSetColor(ofColor::black);

    {
        string presidentName = mainXml.getValue("president:name", "", currentMedia);

        drawStringCentered(presidentName, windowXCenter, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT + 25);
    }

    {
        string startDate = mainXml.getValue("president:startDate", "", currentMedia);

        drawStringRight(startDate, centerPresidentImgXPos - 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string endDate = mainXml.getValue("president:endDate", "", currentMedia);
        myfont.drawString(endDate, (windowXCenter + CENTER_PRESIDENT_IMG_WIDTH / 2) + 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string birthDate = mainXml.getValue("president:birthDate", "", currentMedia);
        string deathDate = mainXml.getValue("president:deathDate", "", currentMedia);

        string presidentLifePeriod = birthDate + "->" + deathDate;

        drawStringCentered(presidentLifePeriod, windowXCenter, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT + 50);
    }

}

void ofApp::drawStringCentered(const std::string& c, float x, float y) 
{
    ofRectangle stringBox = myfont.getStringBoundingBox(c, 0, 0);
    myfont.drawString(c, x - (stringBox.width / 2), y);
    stringBox.~ofRectangle();
}

void ofApp::drawStringRight(const std::string& c, float x, float y)
{
    ofRectangle stringBox = myfont.getStringBoundingBox(c, 0, 0);
    myfont.drawString(c, x - (stringBox.width), y);
    stringBox.~ofRectangle();
}


void ofApp::drawVideo(ofVideoPlayer *vid) {

    vid->setLoopState(OF_LOOP_NORMAL);

    vid->play();

    ofSetHexColor(0xFFFFFF);

    vid->draw(380, 175);
    ofSetHexColor(0x000000);
    ofPixels& pixels = vid->getPixels();

    /*ofSetHexColor(0x000000);
    ofDrawBitmapString("press f to change", 300, vid->getHeight() + 80);
    if (frameByframe) ofSetHexColor(0xCCCCCC);
    ofDrawBitmapString("mouse speed position", 300, vid->getHeight() + 100);
    if (!frameByframe) ofSetHexColor(0xCCCCCC); else ofSetHexColor(0x000000);
    ofDrawBitmapString("keys <- -> frame by frame ", 300, vid->getHeight() + 120);
    ofSetHexColor(0x000000);

    ofDrawBitmapString("frame: " + ofToString(vid->getCurrentFrame()) + "/" + ofToString(vid->getTotalNumFrames()), 300, vid->getHeight() + 140);
    ofDrawBitmapString("duration: " + ofToString(vid->getPosition() * vid->getDuration(), 2) + "/" + ofToString(vid->getDuration(), 2), 300, vid->getHeight() + 160);
    ofDrawBitmapString("speed: " + ofToString(vid->getSpeed(), 2), 300, vid->getHeight() + 180);*/

    if (vid->getIsMovieDone()) {
        ofSetHexColor(0xFF0000);
        ofDrawBitmapString("end of movie", 20, 440);
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (dir.size() > 0){
        int oldMedia = -1;
        if (key == OF_KEY_RIGHT && currentMedia < mediaFiles.size() - 1)
            oldMedia = currentMedia++;
        else if (key == OF_KEY_LEFT && currentMedia > 0)
            oldMedia = currentMedia--;
        
        if (oldMedia != -1 && mediaTypes[oldMedia] == VIDEO_MEDIA_TYPE)
        {
            ofVideoPlayer* vid = (ofVideoPlayer*) mediaFiles[oldMedia];

            vid->stop();
        }

       /* if (currentMedia < 0)
            currentMedia = (int)mediaFiles.size() - 1;
        else
            currentMedia %= mediaFiles.size();*/

    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

bool ofApp::isMousePtrInCarrousel(int x, int y)
{
    /* if (y < presidentCarrouselYPos || y > presidentCarrouselYPos + centerPresidentImgHeight)
         return false;*/

         //if(x <= centerPresidentImgXPos || x >= centerPresidentImgXPos + centerPresidentImgWidth)

    return y >= PRESIDENTS_CARROUSEL_Y_POS && y <= PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT;
}

bool ofApp::isMousePtrInsideCenterPresident(int x, int y)
{
    return x >= centerPresidentImgXPos && x <= centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH;
}

bool ofApp::isMousePtrOnCenterPresidentLeft(int x, int y)
{
    return x < centerPresidentImgXPos;
}

bool ofApp::isMousePtrOnCenterPresidentRight(int x, int y)
{
    return x > centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    /* OF_MOUSE_BUTTON_LEFT
    OF_MOUSE_BUTTON_RIGHT
    OF_MOUSE_BUTTON_MIDDLE */

    //int centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;

    //if it's outside of carrousel, skip
    if (!isMousePtrInCarrousel(x, y)) return;

    if (isMousePtrInsideCenterPresident(x, y)) return;

    if (isMousePtrOnCenterPresidentLeft(x, y))
    {
        for (int prevImageIdx = currentMedia - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
        {
            int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

            if (x >= previousPresidentImgXPos && x <= previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
            {
                if (y <= CENTER_PRESIDENT_IMG_HEIGHT + NEIGHBOUR_PRESIDENT_IMG_HEIGHT)
                {
                    currentMedia = prevImageIdx;
                }
            }

        }
    }
    else if (isMousePtrOnCenterPresidentRight(x, y))
    {
        for (int nextImageIdx = currentMedia + 1, times = 1; nextImageIdx < mediaFiles.size(); nextImageIdx++, times++)
        {
            int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH * (times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (nextPresidentImgXPos >= ofGetWidth()) break;

            if (x >= nextPresidentImgXPos && x <= nextPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
            {
                if (y <= CENTER_PRESIDENT_IMG_HEIGHT + NEIGHBOUR_PRESIDENT_IMG_HEIGHT)
                {
                    currentMedia = nextImageIdx;
                }
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) 
{
    if (isMousePtrInCarrousel(x, y))
    {
        if (scrollY >= 1.0 && currentMedia < mediaFiles.size() - 1)
            currentMedia++;
        else if (scrollY <= -1.0 && currentMedia > 0)
            currentMedia--;
    }
}


//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(ofResizeEventArgs& resize){
    windowXCenter = ofGetWidth() / 2;
    centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
