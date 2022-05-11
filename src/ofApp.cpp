#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

    frameByframe = false;

    dir.listDir("/");
    dir.allowExt("jpg");
    dir.allowExt("png");
    dir.allowExt("mov");

    dir.sort(); // in linux the file system doesn't return file lists ordered in alphabetical order

    //allocate the vector to have as many ofImages as files
    if( dir.size() ){
        mediaFiles.reserve(dir.size());
        mediaTypes.reserve(dir.size());
    }

    // you can now iterate through the files and load them into the ofImage vector
    for(int i = 0; i < (int)dir.size(); i++){
        string mediaPath = dir.getPath(i);
        size_t extDotLocation = mediaPath.find_last_of(".");
        if (extDotLocation)
        {
            string mediaExt = mediaPath.substr(extDotLocation);
            if (mediaExt == ".jpg" || mediaExt == ".png")
            {
                ofImage *img = new ofImage();
                img->load(mediaPath);
                mediaTypes.push_back(IMAGE_MEDIA_TYPE);
                mediaFiles.push_back(img);
            }
             else if (mediaExt == ".mov")
            {
                ofVideoPlayer *vid = new ofVideoPlayer();
                vid->load(mediaPath);
                mediaTypes.push_back(VIDEO_MEDIA_TYPE);
                mediaFiles.push_back(vid);
            }
        }
    }
    currentMedia = 0;

    ofBackground(ofColor::white);
}

//--------------------------------------------------------------
void ofApp::update()
{
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
                ofImage* img = (ofImage*) mediaFiles[currentMedia];

                drawImage(img);
                
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

void ofApp::drawImage(ofImage* img)
{
    img->draw(380, 175);

    ofSetColor(ofColor::gray);
    /*string pathInfo = dir.getName(currentMedia) + " " + dir.getPath(currentMedia) + "\n\n" +
        "press any key to advance current image\n\n" +
        "many thanks to hikaru furuhashi for the OFs";
    ofDrawBitmapString(pathInfo, 300, img->getHeight() + 80);*/

    
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
        if (key == OF_KEY_RIGHT)
            oldMedia = currentMedia++;
        else if (key == OF_KEY_LEFT)
            oldMedia = currentMedia--;
        
        if (oldMedia != -1 && mediaTypes[oldMedia] == VIDEO_MEDIA_TYPE)
        {
            ofVideoPlayer* vid = (ofVideoPlayer*) mediaFiles[oldMedia];

            vid->stop();
        }

        if (currentMedia < 0)
            currentMedia = (int)mediaFiles.size() - 1;
        else
            currentMedia %= mediaFiles.size();
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
