#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetWindowTitle("Presidentes de Portugal - David Pereira e Francisco Mateus");

    frameByframe = false;

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

    if (dir.size() > 0){
        ofSetColor(ofColor::white);

        switch (mediaTypes[currentMedia])
        {
            case IMAGE_MEDIA_TYPE:
            {
               
                drawPresident();
                
                break;
            }
            case VIDEO_MEDIA_TYPE:
            {
                ofVideoPlayer* vid = (ofVideoPlayer*) mediaFiles[currentMedia];

                drawVideo(vid);

                break;
            }
        }
        ofSetColor(ofColor::gray);
        /*for (int i = 0; i < (int)dir.size(); i++) {
            if (i == currentMedia) {
                ofSetColor(ofColor::red);
            }
            else {
                ofSetColor(ofColor::black);
            }
            string fileInfo = "file " + ofToString(i + 1) + " = " + dir.getName(i);
            ofDrawBitmapString(fileInfo, 50, i * 20 + 50);
        }*/
    }
}

void ofApp::drawPresident()
{
    int windowXCenter = ofGetWidth() / 2;
    int centerPresidentImgXPos = windowXCenter - centerPresidentImgWidth/2;

    ofImage* centerPresidentImg = (ofImage*)mediaFiles[currentMedia];
    centerPresidentImg->draw(centerPresidentImgXPos, presidentCarrouselYPos, centerPresidentImgWidth, centerPresidentImgHeight);
    
    auto presidentName = mainXml.getValue("president:name", "", currentMedia);

    for (int prevImageIdx = currentMedia - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
    {        
        int previousPresidentImgXPos = centerPresidentImgXPos - (neighbourPresidentImgWidth)*times - spaceBetween * times;

        //off window limits check
        if (previousPresidentImgXPos + neighbourPresidentImgWidth <= 0) break;

        ofImage* prevPresidentImg = (ofImage*)mediaFiles[prevImageIdx];
        prevPresidentImg->draw(previousPresidentImgXPos, presidentCarrouselYPos, neighbourPresidentImgWidth, neighbourPresidentImgHeight);
    }
   
    for (int nextImageIdx = currentMedia + 1, times = 1; nextImageIdx < mediaFiles.size(); nextImageIdx++, times++)
    {
        int nextPresidentImgXPos = (centerPresidentImgXPos + centerPresidentImgWidth) + neighbourPresidentImgWidth*(times - 1) + spaceBetween * times;

        //off window limits check
        if (nextPresidentImgXPos  >= ofGetWidth()) break;

        ofImage* nextPresidentImg = (ofImage*)mediaFiles[nextImageIdx];
        nextPresidentImg->draw(nextPresidentImgXPos, presidentCarrouselYPos, neighbourPresidentImgWidth, neighbourPresidentImgHeight);
    }

    ofSetColor(ofColor::black);

    ofDrawBitmapString(presidentName, centerPresidentImgXPos, presidentCarrouselYPos + centerPresidentImgHeight + 25);

    ofSetColor(ofColor::gray);
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

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}
