#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);
    
    ofSetWindowTitle("🇵🇹 Presidentes de Portugal - David Pereira e Francisco Mateus 🇵🇹");

    frameByframe = false;

    //gui.setup(); // most of the time you don't need a name

    pauseBtn.setup("Pause", 25, 25);

    pauseBtn.setBackgroundColor(ofColor::white);

    //gui.add();
    
    //load images
    {
        imagesDir.listDir("images");
        imagesDir.allowExt("jpg");
        imagesDir.allowExt("png");
        imagesDir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

        //allocate the vector to have as many ofImages as files
        if (imagesDir.size())
            presidentsMedias.reserve(imagesDir.size());
        else
            assert(false);
    }

    if (!mainXml.load("data_xml/presidents.xml")) {
        ofLogError() << "Couldn't load file";
        assert(false);
    }
    mainXml.pushTag("presidents");


    int n_presidents = mainXml.getNumTags("president");

    for (int i = 0; i < n_presidents; i++) {

        PresidentMedia* presMedia = new PresidentMedia();

        //string name = mainXml.getValue("president:name", "", i);

        presMedia->name         = string(mainXml.getValue("president:name", "", i));
        presMedia->startDate    = string(mainXml.getValue("president:startDate", "", i));
        presMedia->endDate      = string(mainXml.getValue("president:endDate", "", i));
        presMedia->birthDate    = string(mainXml.getValue("president:birthDate", "", i));
        presMedia->deathDate    = string(mainXml.getValue("president:deathDate", "", i));

        string profilePicture = mainXml.getValue("president:profilePicture", "", i);
        presMedia->profilePicture = new ofImage();
        presMedia->profilePicture->load("images/" + profilePicture);

        string biographyVideo = mainXml.getValue("president:biographyVideo", "", i);
        if (biographyVideo != "")
        {
            presMedia->biographyVideo = new ofVideoPlayer();
            presMedia->biographyVideo->load("videos/" + biographyVideo);
        }

        presidentsMedias.push_back(presMedia);
    }

    currentPresidentIdx = 0;

    ofBackground(ofColor::white);
}

//--------------------------------------------------------------
void ofApp::update() {

    ofVideoPlayer* vid = presidentsMedias[currentPresidentIdx]->biographyVideo;

    if (vid == NULL) return;

    vid->update();
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

    pauseBtn.setPosition(windowXCenter, 600);

    pauseBtn.draw();

    //gui.draw();

    drawPresidents(); 

    drawPresidentDescription();

    drawBiographyVideo();
}

void ofApp::drawPresidentDescription()
{
    string presidentDescription = mainXml.getValue("president:description", "", currentPresidentIdx);
}

void ofApp::drawPresidents()
{    
    ofSetColor(ofColor::white);
    ofImage* centerPresidentImg = presidentsMedias[currentPresidentIdx]->profilePicture;
    centerPresidentImg->draw(centerPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, CENTER_PRESIDENT_IMG_WIDTH, CENTER_PRESIDENT_IMG_HEIGHT);
    
    for (int prevImageIdx = currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
    {        
        int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

        ofImage* prevPresidentImg = presidentsMedias[prevImageIdx]->profilePicture;
        prevPresidentImg->draw(previousPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }
   
    for (int nextImageIdx = currentPresidentIdx + 1, times = 1; nextImageIdx < presidentsMedias.size(); nextImageIdx++, times++)
    {
        int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH*(times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (nextPresidentImgXPos  >= ofGetWidth()) break;

        ofImage* nextPresidentImg = presidentsMedias[nextImageIdx]->profilePicture;
        nextPresidentImg->draw(nextPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }

    myfont.load("arial.ttf", 20);
    ofSetColor(ofColor::black);

    {
        string presidentName = presidentsMedias[currentPresidentIdx]->name;

        drawStringCentered(presidentName, windowXCenter, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT + 25);
    }

    {
        string startDate = presidentsMedias[currentPresidentIdx]->startDate;

        drawStringRight(startDate, centerPresidentImgXPos - 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string endDate = presidentsMedias[currentPresidentIdx]->endDate;
        myfont.drawString(endDate, (windowXCenter + CENTER_PRESIDENT_IMG_WIDTH / 2) + 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string birthDate = presidentsMedias[currentPresidentIdx]->birthDate;
        string deathDate = presidentsMedias[currentPresidentIdx]->deathDate;

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


void ofApp::drawBiographyVideo() {

    ofVideoPlayer* vid = presidentsMedias[currentPresidentIdx]->biographyVideo;

    if (vid == NULL) return;

    ofSetHexColor(0xFFFFFF);

    vid->draw(windowXCenter - BIOGRAPHY_VIDEO_WIDTH/2, 512, BIOGRAPHY_VIDEO_WIDTH, BIOGRAPHY_VIDEO_HEIGH);
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (imagesDir.size() == 0) return;

    int previousPresidentIdx = -1;
    if (key == OF_KEY_RIGHT && currentPresidentIdx < presidentsMedias.size() - 1)
        previousPresidentIdx = currentPresidentIdx++;
    else if (key == OF_KEY_LEFT && currentPresidentIdx > 0)
        previousPresidentIdx = currentPresidentIdx--;
    else if (key == ' ')
    {
        ofVideoPlayer* vid = presidentsMedias[currentPresidentIdx]->biographyVideo;

        if (vid != NULL)
        {
            vid->setPaused(!vid->isPaused());
            //vid->closeMovie();
        }
    }
        
    if (previousPresidentIdx != -1)
        switchPresident(presidentsMedias[previousPresidentIdx]);

    
       /* if (currentMedia < 0)
            currentMedia = (int)mediaFiles.size() - 1;
        else
            currentMedia %= mediaFiles.size();*/
}


void ofApp::switchPresident(PresidentMedia* previousPresident)
{
    if (previousPresident == NULL) return;

    PresidentMedia* currentPresident = presidentsMedias[currentPresidentIdx];
 
    if (previousPresident->biographyVideo != NULL)
    {
        previousPresident->biographyVideo->stop();
    }

    if (currentPresident->biographyVideo != NULL)
    {
        currentPresident->biographyVideo->setLoopState(OF_LOOP_NORMAL);

        currentPresident->biographyVideo->play();
    }

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y )
{

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

bool ofApp::isMousePtrInCarrousel(int x, int y)
{
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

bool ofApp::isMousePtrBelowNeighbourPresidents(int x, int y)
{
    return y > PRESIDENTS_CARROUSEL_Y_POS + NEIGHBOUR_PRESIDENT_IMG_HEIGHT;
}

int ofApp::getPresidentIndexWhereMouseIsPointing(int x, int y)
{
    //if it's outside of carrousel, skip
    if (!isMousePtrInCarrousel(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrInsideCenterPresident(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrBelowNeighbourPresidents(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrOnCenterPresidentLeft(x, y))
    {
        for (int prevImageIdx = currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
        {
            int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

            if (x >= previousPresidentImgXPos && x <= previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
            {
                return prevImageIdx;
            }

        }
    }
    else if (isMousePtrOnCenterPresidentRight(x, y))
    {
        for (int nextImageIdx = currentPresidentIdx + 1, times = 1; nextImageIdx < presidentsMedias.size(); nextImageIdx++, times++)
        {
            int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH * (times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (nextPresidentImgXPos >= ofGetWidth()) break;

            if (x >= nextPresidentImgXPos && x <= nextPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
            {
                return nextImageIdx;
            }
        }
    }
    return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    /* OF_MOUSE_BUTTON_LEFT
    OF_MOUSE_BUTTON_RIGHT
    OF_MOUSE_BUTTON_MIDDLE */

    //int centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;
    if (button == OF_MOUSE_BUTTON_LEFT)
    {
        int president = getPresidentIndexWhereMouseIsPointing(x, y);
        if (president != MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT)
        {
            int previousPresidentIdx = currentPresidentIdx;
            currentPresidentIdx = president;
            switchPresident(presidentsMedias[previousPresidentIdx]);
            return;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) 
{
    if (isMousePtrInCarrousel(x, y))
    {
        int previousPresidentIdx = -1;
        if (scrollY >= 1.0 && currentPresidentIdx < presidentsMedias.size() - 1)
            previousPresidentIdx = currentPresidentIdx++;
        else if (scrollY <= -1.0 && currentPresidentIdx > 0)
            previousPresidentIdx = currentPresidentIdx--;

        if (previousPresidentIdx != -1)
            switchPresident(presidentsMedias[previousPresidentIdx]);

    }

    
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(ofResizeEventArgs& resize) 
{
    windowXCenter = ofGetWidth() / 2;
    centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;

    if (resize.width <= CENTER_PRESIDENT_IMG_WIDTH + 2 * (NEIGHBOUR_PRESIDENT_IMG_WIDTH)+SPACE_BETWEEN_PRESIDENTS * 2)
    {
        ofSetWindowShape(CENTER_PRESIDENT_IMG_WIDTH + 2 * (NEIGHBOUR_PRESIDENT_IMG_WIDTH)+SPACE_BETWEEN_PRESIDENTS * 2, ofGetHeight());
    }
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
