
#include "ofApp.h"
#include "Metadata.h"

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
        if (!imagesDir.size())
            assert(false);
    }

    if (!presidentsMetadataXml.load("data_xml/presidents_metadata.xml")) {
        ofLogError() << "Couldn't load file";
        assert(false);
    }

    presidentsMetadataXml.pushTag("presidentsMetadata");
    presidentsMetadataXml.clear();

    if (!presidentsXml.load("data_xml/presidents.xml")) {
        ofLogError() << "Couldn't load file";
        assert(false);
    }
   
    getPresidentsInfo();

    //std::thread _thread(&ofApp::startMetadataGeneration, this);

    Metadata::startMetadataGeneration(this);

    initButtons();
        
    currentPresidentIdx = 0;

    ofBackground(ofColor::white);
}

//--------------------------------------------------------------
void ofApp::update() {

    ofVideoPlayer* vid = presidentsMedias[currentPresidentIdx]->biographyVideo;

    if (vid == NULL) return;


    vid->update();

    generateMetadataBtn->update();

    //generateMetadata
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

    importMetadataBtn->setPosition(ofGetWidth() - importMetadataBtn->getWidth() - 50, 25);
    importMetadataBtn->draw();

    generateMetadataBtn->setPosition(ofGetWidth() - importMetadataBtn->getWidth() - 300, 25);
    generateMetadataBtn->draw();

    /*im2->draw();
    im3->draw();*/

    //gui.draw();

    drawPresidents(); 

    drawBiographyVideo();
    
}

void ofApp::drawPresidentDescription() {
    string presidentDescription = presidentsXml.getValue("president:description", "", currentPresidentIdx);
}

void ofApp::drawPresidents() {
    ofSetColor(ofColor::white);
    ofImage* centerPresidentImg = presidentsMedias[currentPresidentIdx]->profilePicture;
    centerPresidentImg->draw(centerPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, CENTER_PRESIDENT_IMG_WIDTH, CENTER_PRESIDENT_IMG_HEIGHT);
    
    for (int prevImageIdx = currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++) {
        int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

        ofImage* prevPresidentImg = presidentsMedias[prevImageIdx]->profilePicture;
        prevPresidentImg->draw(previousPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }
   
    for (int nextImageIdx = currentPresidentIdx + 1, times = 1; nextImageIdx < presidentsMedias.size(); nextImageIdx++, times++) {
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

void ofApp::drawStringCentered(const std::string& c, float x, float y) {
    ofRectangle stringBox = myfont.getStringBoundingBox(c, 0, 0);
    myfont.drawString(c, x - (stringBox.width / 2), y);
    stringBox.~ofRectangle();
}

void ofApp::drawStringRight(const std::string& c, float x, float y) {
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
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (imagesDir.size() == 0) return;

    int previousPresidentIdx = -1;
    if (key == OF_KEY_RIGHT && currentPresidentIdx < presidentsMedias.size() - 1)
        previousPresidentIdx = currentPresidentIdx++;
    else if (key == OF_KEY_LEFT && currentPresidentIdx > 0)
        previousPresidentIdx = currentPresidentIdx--;
    else if (key == ' ') {
        ofVideoPlayer* vid = presidentsMedias[currentPresidentIdx]->biographyVideo;

        if (vid != NULL) {
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


void ofApp::switchPresident(President* previousPresident) {
    if (previousPresident == NULL) return;

    President* currentPresident = presidentsMedias[currentPresidentIdx];
 
    if (previousPresident->biographyVideo != NULL)
        previousPresident->biographyVideo->stop();

    if (currentPresident->biographyVideo != NULL) {
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

bool ofApp::isMousePtrInCarrousel(int x, int y) {
    return y >= PRESIDENTS_CARROUSEL_Y_POS && y <= PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT;
}

bool ofApp::isMousePtrInsideCenterPresident(int x, int y) {
    return x >= centerPresidentImgXPos && x <= centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH;
}

bool ofApp::isMousePtrOnCenterPresidentLeft(int x, int y) {
    return x < centerPresidentImgXPos;
}

bool ofApp::isMousePtrOnCenterPresidentRight(int x, int y) {
    return x > centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH;
}

bool ofApp::isMousePtrBelowNeighbourPresidents(int x, int y) {
    return y > PRESIDENTS_CARROUSEL_Y_POS + NEIGHBOUR_PRESIDENT_IMG_HEIGHT;
}

int ofApp::getPresidentIndexWhereMouseIsPointing(int x, int y) {
    //if it's outside of carrousel, skip
    if (!isMousePtrInCarrousel(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrInsideCenterPresident(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrBelowNeighbourPresidents(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrOnCenterPresidentLeft(x, y)) {
        for (int prevImageIdx = currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++) {
            int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

            if (x >= previousPresidentImgXPos && x <= previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
                return prevImageIdx;
        }
    } else if (isMousePtrOnCenterPresidentRight(x, y)) {
        for (int nextImageIdx = currentPresidentIdx + 1, times = 1; nextImageIdx < presidentsMedias.size(); nextImageIdx++, times++) {
            int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH * (times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (nextPresidentImgXPos >= ofGetWidth()) break;

            if (x >= nextPresidentImgXPos && x <= nextPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
                return nextImageIdx;
        }
    }
    
    return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;
}

void ofApp::getPresidentsInfo() {
    presidentsXml.pushTag("presidents");

    vector<thread> threads;

    const int n_threads = 1;

    int n_presidents = presidentsXml.getNumTags("president");

    int presidents_per_thread = n_presidents / n_threads;

    //does not make cycle if n_threads == 1
    for (int i = 0; i < n_presidents && n_threads > 1; i+=presidents_per_thread) {
        int endPres = i + presidents_per_thread > n_presidents - 1 ? n_presidents - 1 : i + presidents_per_thread;

        threads.push_back(std::thread(&ofApp::getPresidentsInfoThread, this, i, endPres));

        //getPresidentsInfoThread(i, endPres);
    }

    if(n_threads == 1) getPresidentsInfoThread(0, n_presidents - 1);

    for (int t = 0; t < threads.size(); t++)
        threads[t].join();

    //for (int w = 0; w < threadsWork.size(); w++)
    //{
    //    GenerateMetadataThreadWork* work = threadsWork[w];

    //    presidentsMetadataXml.addTag("president");
    //    presidentsMetadataXml.pushTag("president", w); // mudar para numberOfItems + 1 depois de testar
    //    // saves the id - itemName
    //    presidentsMetadataXml.setValue("id", work->president->name, w);

    //    presidentsMetadataXml.setValue("luminance", work->luminance, w);
    //    presidentsMetadataXml.setValue("color", work->color, w);
    //    presidentsMetadataXml.setValue("texture", work->texture, w);
    //    presidentsMetadataXml.setValue("faces", work->faces, w);
    //    presidentsMetadataXml.setValue("edges", work->edges, w);


    //    presidentsMetadataXml.popTag(); // item

    //    presidentsMetadataXml.saveFile();
    //}
}

void ofApp::getPresidentsInfoThread(int startPres, int endPres) {
    for (int i = startPres; i <= endPres; i++)
        getPresidentInfo(i);
}

void ofApp::getPresidentInfo(int xmlIndex) {
    President* presMedia = new President();

    //string name = presidentsXml.getValue("president:name", "", i);

    presMedia->pres_id = xmlIndex;
    presMedia->name = string(presidentsXml.getValue("president:name", "", xmlIndex));

    if (presMedia->name == "") return;

    presMedia->startDate = string(presidentsXml.getValue("president:startDate", "", xmlIndex));
    presMedia->endDate = string(presidentsXml.getValue("president:endDate", "", xmlIndex));
    presMedia->birthDate = string(presidentsXml.getValue("president:birthDate", "", xmlIndex));
    presMedia->deathDate = string(presidentsXml.getValue("president:deathDate", "", xmlIndex));

    presMedia->profilePicturePath = presidentsXml.getValue("president:profilePicture", "", xmlIndex);
    presMedia->profilePicture = new ofImage();
    
    if (!presMedia->profilePicture->load("images/" + presMedia->profilePicturePath))
        assert(false);

    presMedia->biographyVideoPath = presidentsXml.getValue("president:biographyVideo", "", xmlIndex);
    if (presMedia->biographyVideoPath != "") {
        presMedia->biographyVideo = new ofVideoPlayer();
        if(!presMedia->biographyVideo->load("videos/" + presMedia->biographyVideoPath))
            assert(false);
    }

    mutex.lock();

    presidentsMedias[presMedia->pres_id] = presMedia;

    mutex.unlock();

    //generateMetadata(presMedia);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
    /* OF_MOUSE_BUTTON_LEFT
    OF_MOUSE_BUTTON_RIGHT
    OF_MOUSE_BUTTON_MIDDLE */

    //int centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;
    if (button == OF_MOUSE_BUTTON_LEFT) {
        int president = getPresidentIndexWhereMouseIsPointing(x, y);
        if (president != MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT) {
            int previousPresidentIdx = currentPresidentIdx;
            currentPresidentIdx = president;
            switchPresident(presidentsMedias[previousPresidentIdx]);
            return;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (isMousePtrInCarrousel(x, y)) {
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
void ofApp::windowResized(ofResizeEventArgs& resize) {
    windowXCenter = ofGetWidth() / 2;
    centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;

    if (resize.width <= CENTER_PRESIDENT_IMG_WIDTH + 2 * (NEIGHBOUR_PRESIDENT_IMG_WIDTH)+SPACE_BETWEEN_PRESIDENTS * 2)
        ofSetWindowShape(CENTER_PRESIDENT_IMG_WIDTH + 2 * (NEIGHBOUR_PRESIDENT_IMG_WIDTH)+SPACE_BETWEEN_PRESIDENTS * 2, ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

/*void Gallery::extractMetadata() {
    
}*/

/*void Gallery::initXmlObjects() {
    
}*/

void ofApp::initButtons()
{
    //---------Import
    importMetadataBtn = new ofxDatGuiButton("Import Metadata");
    
    importMetadataBtn->setIndex(0);
    importMetadataBtn->setWidth(100);
    importMetadataBtn->onButtonEvent(this, &ofApp::onButtonEvent);

    importMetadataBtn->setEnabled(true);

    generateMetadataBtn = new ofxDatGuiButton("Generate Metadata");

    generateMetadataBtn->setIndex(1);
    generateMetadataBtn->setWidth(100);
    generateMetadataBtn->onButtonEvent(this, &ofApp::onButtonEvent);

    generateMetadataBtn->setEnabled(true);
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {

}

/*int ofApp::objectTimesFilter(ofImage image, ofImage objImage) {
    int numberOfMatches = 0;
    ofImage  tempImg = image;
    tempImg.setImageType(OF_IMAGE_GRAYSCALE);
    Mat img1 = toCv(tempImg);
    objImage.setImageType(OF_IMAGE_GRAYSCALE);
    Mat img2 = toCv(objImage);

    if (!img1.empty() && !img2.empty())
    {
        if (img1.channels() != 1) {
            cvtColor(img1, img1, cv::COLOR_RGB2GRAY);
        }

        if (img2.channels() != 1) {
            cvtColor(img2, img2, cv::COLOR_RGB2GRAY);
        }

        vector<KeyPoint> keyP1;
        vector<KeyPoint> keyP2;
        Mat desc1;
        Mat desc2;
        vector<cv::DMatch> matches;

        Ptr<ORB> detector = ORB::create();
        detector->detectAndCompute(img1, Mat(), keyP1, desc1);
        detector->detectAndCompute(img2, Mat(), keyP2, desc2);
        matches.clear();

        BFMatcher bruteMatcher(cv::NORM_L2, true);
        bruteMatcher.match(desc1, desc2, matches, Mat());

        int k1s = keyP1.size();
        int k2s = keyP2.size();
        int ms = matches.size();
        float distances = 0;
        for (int j = 0; j < matches.size(); j++) {
            distances += matches[j].distance;
        }
        float distanceAvg = distances / ms;
        if (distanceAvg < 340) {
            numberOfMatches = 1;
        }
    }
    return numberOfMatches;
}*/
