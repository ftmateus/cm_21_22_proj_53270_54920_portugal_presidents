#include "ofApp.h"
#include "ofAppData.h"


using namespace cv;

//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);

    ofSetWindowTitle("🇵🇹 Presidentes de Portugal - David Pereira e Francisco Mateus 🇵🇹");

    frameByframe = false;

    appData = new ofAppData();

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


    if (!appData->presidentsXml.load("data_xml/presidents.xml")) {
        ofLogError() << "Couldn't load file";
        
    }
   
    getPresidentsInfo();

    if (appData->presidentsMetadataXml.load("data_xml/presidents_metadata.xml"))
    {
        importMetadata();
        //assert(appData->metadataGenerated == true);
    }
    else
    {
        appData->presidentsMetadataXml.clear();
        appData->presidentsMetadataXml.save("data_xml/presidents_metadata.xml");
        //appData->presidentsMetadataXml.pushTag("presidentsMetadata");
        //appData->presidentsMetadataXml.load("data_xml/presidents_metadata.xml");
        /*ofLogError() << "Couldn't load file";
        assert(false);*/
    }

    initButtons();
        
    ofBackground(ofColor::white);

    appData->latestPresidentsSelected.push(appData->getPresidentByCurrentCarrouselPosition());
}

//--------------------------------------------------------------
void ofApp::update() {

    int carrouselCurrentSize = appData->getCarrouselCurrentSize();

    if (carrouselCurrentSize == 0) return;

    President* president = appData->getPresidentByCurrentCarrouselPosition();

    ofVideoPlayer* vid = president->biographyVideo;

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

    /*pauseBtn.draw();

    importMetadataBtn->draw();
    generateMetadataBtn->draw();
    extractMetadataBtn->draw();*/

    int carrouselCurrentSize = appData->getCarrouselCurrentSize();
    if (carrouselCurrentSize > 0)
    {   
        drawPresidents();

        drawBiographyVideo();
    }

    if (appData->isGeneratingMetadata)
    {
        drawStringCentered("Generating Metadata...", windowXCenter, ofGetWindowHeight() - 50);
    }
    
}

ofImage *ofApp::getPresidentProfilePicture(President* president)
{
    ofImage* img;

    if (currentFilterApplied == EDGES_FILTER)
    {
        if (president->metadata->edgesProfilePicture == NULL)
        {
            string edgesFilename = president->metadata->edgesPath;
            president->metadata->edgesProfilePicture = new ofImage();
            president->metadata->edgesProfilePicture->load(edgesFilename);
        }
        img = president->metadata->edgesProfilePicture;
    }
    else if (currentFilterApplied == TEXTURE_FILTER)
    {
        if (president->metadata->textureProfilePicture == NULL)
        {
            string textureFilename = president->metadata->texturePath;
            president->metadata->textureProfilePicture = new ofImage();
            president->metadata->textureProfilePicture->load(textureFilename);
        }
        img = president->metadata->textureProfilePicture;
    }
    else
    {
        assert(currentFilterApplied == NO_FILTER);
        img = president->profilePicture;
    }
    assert(img != NULL);

    return img;
}

void ofApp::drawPresidents() {
    ofSetColor(ofColor::white);

    int carrouselCurrentSize = appData->getCarrouselCurrentSize();
    assert(carrouselCurrentSize > 0);

    President* currentPresident = appData->getPresidentByCurrentCarrouselPosition();

    ofImage* centerPresidentImg = getPresidentProfilePicture(currentPresident);
    centerPresidentImg->draw(centerPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, CENTER_PRESIDENT_IMG_WIDTH, CENTER_PRESIDENT_IMG_HEIGHT);
    
    for (int prevImageIdx = appData->currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
    {        
        int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

        President* previousPresident = appData->getPresidentByCarrouselPosition(prevImageIdx);

        ofImage* prevPresidentImg = getPresidentProfilePicture(previousPresident);;
        prevPresidentImg->draw(previousPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }
   
    for (int nextImageIdx = appData->currentPresidentIdx + 1, times = 1; nextImageIdx < carrouselCurrentSize; nextImageIdx++, times++)
    {
        int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH*(times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (nextPresidentImgXPos  >= ofGetWidth()) break;

        President* nextPresident = appData->getPresidentByCarrouselPosition(nextImageIdx);

        ofImage* nextPresidentImg = getPresidentProfilePicture(nextPresident);;
        nextPresidentImg->draw(nextPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }

    myfont.load("arial.ttf", 20);
    ofSetColor(ofColor::black);

    {
        string presidentName = currentPresident->name;

        drawStringCentered(presidentName, windowXCenter, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT + 25);
    }

    {
        string startDate = currentPresident->startDate;

        drawStringRight(startDate, centerPresidentImgXPos - 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string endDate = currentPresident->endDate;
        myfont.drawString(endDate, (windowXCenter + CENTER_PRESIDENT_IMG_WIDTH / 2) + 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string birthDate = currentPresident->birthDate;
        string deathDate = currentPresident->deathDate;

        string presidentLifePeriod = birthDate + "->" + deathDate;

        drawStringCentered(presidentLifePeriod, windowXCenter, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT + 50);
    }

}

void ofApp::importMetadata()
{    
    auto xml = appData->presidentsMetadataXml;

    xml.pushTag("presidentsMetadata");

    int n_presidents = xml.getNumTags("president");

    if (n_presidents != appData->presidentsMedias.size())
    {
        assert(false);
        return;
    }

    for (int p = 0; p < n_presidents; p++)
    {
        PresidentMetadata* metadata = new PresidentMetadata();

        metadata->luminance     = stof(string(xml.getValue("president:luminance", "", p)));
        metadata->faces         = stoi(string(xml.getValue("president:faces", "", p)));
        metadata->rhythm        = stod(string(xml.getValue("president:rhythm", "", p)));
        metadata->color         = stof(string(xml.getValue("president:color", "", p)));
        metadata->objectTimes   = stoi(string(xml.getValue("president:objectTimes", "", p)));
        metadata->texturePath   = string(xml.getValue("president:texture", "", p));
        metadata->edgesPath     = string(xml.getValue("president:edges", "", p));


        President* president = appData->presidentsMedias[p];
        president->metadata = metadata;
    }

    appData->metadataGenerated = true;

    xml.popTag();
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

void ofApp::drawBiographyVideo() 
{
    President* president = appData->getPresidentByCurrentCarrouselPosition();

    ofVideoPlayer* vid = president->biographyVideo;

    if (vid == NULL) return;

    

    if (fullScreen)
    {
        ofSetHexColor(0xFFFFFF);
        vid->draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
        ofSetHexColor(0x000000);

    }
    else
    {
        ofSetHexColor(0xFFFFFF);
        vid->draw(windowXCenter - BIOGRAPHY_VIDEO_WIDTH / 2, 512, BIOGRAPHY_VIDEO_WIDTH, BIOGRAPHY_VIDEO_HEIGH);
        ofSetHexColor(0x000000);
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if (imagesDir.size() == 0) return;

    int previousPresidentIdx = -1;

    switch (key)
    {
        case OF_KEY_RIGHT:
            if (fullScreen) break;
            if (appData->currentPresidentIdx < appData->getCarrouselCurrentSize() - 1)
                previousPresidentIdx = appData->currentPresidentIdx++;
            break;

        case OF_KEY_LEFT:
            if (fullScreen) break;
            if (appData->currentPresidentIdx > 0)
                previousPresidentIdx = appData->currentPresidentIdx--;
            break;
        case ' ': //space key
            pausePlayVideo();
            break;
        case 'E': case 'e':
            applyFilter(EDGES_FILTER);
            break;
        case 'T': case 't':
            applyFilter(TEXTURE_FILTER);
            break;
        case 'G': case 'g':
            generateMetadata();
            break;
        case 'S': case 's':
            search();
            break;
        case 'C': case 'c':
            cancelSearch();
            break;
        case 'F': case 'f':
            setVideoFullScreen();
    }        
    if (previousPresidentIdx != -1)
        switchPresident(appData->presidentsMedias[previousPresidentIdx]);

    
       /* if (currentMedia < 0)
            currentMedia = (int)mediaFiles.size() - 1;
        else
            currentMedia %= mediaFiles.size();*/
}

void ofApp::search()
{
    string searchTerm = ofSystemTextBoxDialog("Search:", appData->currentSearchTerm);
    if (searchTerm.length() == 0)
    {
        cancelSearch();
        return;
    }
    appData->currentSearchResult = appData->presidentsSearchIndex[searchTerm];
    appData->currentPresidentIdx = 0;
    appData->currentSearchTerm = searchTerm;
    appData->showingSearchPresidents = true;
}

void ofApp::cancelSearch()
{
    if (appData->showingSearchPresidents)
        appData->currentPresidentIdx = 0;
    appData->currentSearchTerm = "";
    appData->showingSearchPresidents = false;
}

void ofApp::setVideoFullScreen()
{
    President* president = appData->getPresidentByCurrentCarrouselPosition();

    if (president->biographyVideo == NULL)
    {
        assert(!fullScreen);
        return;
    }

    fullScreen = !fullScreen;
    ofSetFullscreen(fullScreen);

}

void ofApp::applyFilter(Filters filter)
{
    if (!appData->metadataGenerated) return;
    currentFilterApplied = currentFilterApplied == filter ? NO_FILTER : filter;
}

void ofApp::pausePlayVideo()
{
    if (appData->getCarrouselCurrentSize() == 0) return;

    President* president = appData->getPresidentByCurrentCarrouselPosition();

    ofVideoPlayer* vid = president->biographyVideo;

    if (vid == NULL) return;

    vid->setPaused(!vid->isPaused());
    //vid->closeMovie();
}

void ofApp::generateMetadata()
{
    if (appData->isGeneratingMetadata) return;
    appData->isGeneratingMetadata = true;
    appData->metadataGenerated = false;
    //generateMetadataThread = new GenerateMetadata(appData);
    generateMetadataThread.setup(appData);
    currentFilterApplied = NO_FILTER;
    //std::thread _thread(&ofApp::startMetadataGeneration, this);
}

void ofApp::indexPresidentForSearch(President *president)
{
    indexStringForSearch(president->name, president);
    indexStringForSearch(president->startDate, president);
    indexStringForSearch(president->endDate, president);
    indexStringForSearch(president->birthDate, president);
    indexStringForSearch(president->deathDate, president);
    for (string tag : president->tags)
        indexStringForSearch(tag, president);
}

void ofApp::indexStringForSearch(string str, President *president)
{
    int strLength = str.length();
    subStrLoop:
    for (int c = 1; c <= strLength; c++)
    {
        string subStr = str.substr(0, c);
        vector<President*>* presList = &appData->presidentsSearchIndex[subStr];
        bool alreadyAdded = false;
        for (President* p : *presList)
        {
            if (p->pres_id == president->pres_id)
            {
                alreadyAdded = true;
                break;
            }
        }

        if(!alreadyAdded)
            presList->push_back(president);
    }

    int i = str.find(' ');

    if (i != string::npos)
    {
        string _str = str.substr(i + 1);
        indexStringForSearch(_str, president);
    }


}

void ofApp::searchPresidents()
{

}

void ofApp::switchPresident(President* previousPresident)
{
    if (previousPresident == NULL) return;


    President* currentPresident = appData->getPresidentByCurrentCarrouselPosition();

    int latestPresSelectedSize = appData->latestPresidentsSelected.size();

    assert(latestPresSelectedSize > 0 && latestPresSelectedSize <= MAX_LATEST_PRESIDENTS_SELECTED);
    
    if (latestPresSelectedSize == MAX_LATEST_PRESIDENTS_SELECTED)
    {
        President* popPresident = appData->latestPresidentsSelected.front();
        appData->latestPresidentsSelected.pop();

        if (popPresident->biographyVideo != NULL && popPresident != currentPresident)
        {
            assert(popPresident->biographyVideoPath.length() > 0);
            popPresident->biographyVideo->closeMovie();
        }
    }
    appData->latestPresidentsSelected.push(currentPresident);

    if (previousPresident->biographyVideo != NULL)
    {
        assert(previousPresident->biographyVideoPath.length() > 0);
        previousPresident->biographyVideo->stop();
    }
    if (currentPresident->biographyVideo != NULL) {
        assert(currentPresident->biographyVideoPath.length() > 0);

        if(!currentPresident->biographyVideo->isLoaded())
            currentPresident->biographyVideo->load("videos/" + currentPresident->biographyVideoPath);

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
    if (fullScreen) return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;
    //if it's outside of carrousel, skip
    if (!isMousePtrInCarrousel(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrInsideCenterPresident(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrBelowNeighbourPresidents(x, y)) 
        return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;

    if (isMousePtrOnCenterPresidentLeft(x, y))
    {
        for (int prevImageIdx = appData->currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
        {
            int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

            if (x >= previousPresidentImgXPos && x <= previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
                return prevImageIdx;
        }
    }
    else if (isMousePtrOnCenterPresidentRight(x, y))
    {
        for (int nextImageIdx = appData->currentPresidentIdx + 1, times = 1; nextImageIdx < appData->getCarrouselCurrentSize(); nextImageIdx++, times++)
        {
            int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH * (times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

            //off window limits check
            if (nextPresidentImgXPos >= ofGetWidth()) break;

            if (x >= nextPresidentImgXPos && x <= nextPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH)
                return nextImageIdx;
        }
    }
    return MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT;
}

void ofApp::getPresidentsInfo()
{
    appData->presidentsXml.pushTag("presidents");

    vector<thread> threads;

    const int n_threads = 1;

    int n_presidents = appData->presidentsXml.getNumTags("president");

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
}

void ofApp::getPresidentsInfoThread(int startPres, int endPres) {
    for (int i = startPres; i <= endPres; i++)
        getPresidentInfo(i);
}

void ofApp::getPresidentInfo(int xmlIndex) {
    auto xml = appData->presidentsXml;

    assert(xml.getPushLevel() == 1);
    xml.pushTag("president", xmlIndex);
    assert(xml.getPushLevel() == 2);

    President* presMedia = new President();

    //string name = appData->presidentsXml.getValue("president:name", "", i);

    presMedia->pres_id = xmlIndex;
    presMedia->name = string(xml.getValue("name", "", 0));

   
    if (presMedia->name == "")
    {  
        assert(false);
        return;
    }

    presMedia->startDate = string(xml.getValue("startDate", "", 0));
    presMedia->endDate = string(xml.getValue("endDate", "", 0));
    presMedia->birthDate = string(xml.getValue("birthDate", "", 0));
    presMedia->deathDate = string(xml.getValue("deathDate", "", 0));

    presMedia->profilePicturePath = xml.getValue("profilePicture", "", 0);
    presMedia->profilePicture = new ofImage();
    
    
    xml.pushTag("tags", 0);
    assert(xml.getPushLevel() == 3);
    {
        int n_tags = xml.getNumTags("tag");
        presMedia->tags.reserve(n_tags);
        for (int t = 0; t < n_tags; t++)
        {
            string tag = string(xml.getValue("tag", "", t));
            assert(tag != "");
            presMedia->tags.push_back(tag);
        }
    }
    xml.popTag();
    assert(xml.getPushLevel() == 2);
    

    if (!presMedia->profilePicture->load("images/" + presMedia->profilePicturePath))
        assert(false);

    presMedia->biographyVideoPath = xml.getValue("biographyVideo", "", 0);
    if (presMedia->biographyVideoPath != "")
    {
        presMedia->biographyVideo = new ofVideoPlayer();
        if(!presMedia->biographyVideo->load("videos/" + presMedia->biographyVideoPath))
            assert(false);
    }

    indexPresidentForSearch(presMedia);

    appData->mutex.lock();

    appData->presidentsMedias[presMedia->pres_id] = presMedia;

    appData->mutex.unlock();

    xml.popTag();

    assert(xml.getPushLevel() == 1);

    //generateMetadata(presMedia);
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    /* OF_MOUSE_BUTTON_LEFT
    OF_MOUSE_BUTTON_RIGHT
    OF_MOUSE_BUTTON_MIDDLE */

    //int centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;
    if (button == OF_MOUSE_BUTTON_LEFT) {
        int president = getPresidentIndexWhereMouseIsPointing(x, y);
        if (president != MOUSE_PTR_NOT_POINTING_TO_ANY_PRESIDENT)
        {
            int previousPresidentIdx = appData->currentPresidentIdx;
            appData->currentPresidentIdx = president;
            switchPresident(appData->presidentsMedias[previousPresidentIdx]);
            return;
        }
    }
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY) {
    if (fullScreen) return;
    if (isMousePtrInCarrousel(x, y)) {
        int previousPresidentIdx = -1;
        if (scrollY >= 1.0 && appData->currentPresidentIdx < appData->getCarrouselCurrentSize() - 1)
            previousPresidentIdx = appData->currentPresidentIdx++;
        else if (scrollY <= -1.0 && appData->currentPresidentIdx > 0)
            previousPresidentIdx = appData->currentPresidentIdx--;

        if (previousPresidentIdx != -1)
            switchPresident(appData->presidentsMedias[previousPresidentIdx]);

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


/*void ofApp::handleUserItems(int userId, vector<Item*> items_input, bool useItemsInput) {
    if (!useItemsInput) {
        int numberOfUsers = user_data->presidentsXml.getNumTags("user_items");

        int numberOfItems = 0;
        vector<string> user_items;

        for (int i = 0; i < numberOfUsers; i++) {
            user_data->presidentsXml.pushTag("user_items", i);

            if (user_data->presidentsXml.getValue("user", 0) == userId) {
                // get items
                user_data->presidentsXml.pushTag("items", i);
                numberOfItems = user_data->presidentsXml.getNumTags("item");

                user_items.assign(numberOfItems, string());

                for (int j = 0; j < numberOfItems; j++) {
                    user_data->presidentsXml.pushTag("item", j);
                    string itemId = user_data->presidentsXml.getValue("id", "");
                    // add to vector
                    user_items.push_back(itemId);

                    user_data->presidentsXml.popTag(); // item
                }
            }
            user_data->presidentsXml.popTag(); // items
            user_data->presidentsXml.popTag(); // user_items
        }
        //----------ofDirectory
        dir.listDir("items/");
        dir.sort();

        if (dir.size()) {
            items.assign(numberOfItems, &Item("", ofImage(), false, false));
            auxItems.assign(numberOfItems, &Item("", ofImage(), false, false));
        }

        int counter = 0;

        for (int i = 0; i < (int)dir.size(); i++) {
            // checks if user has the item
            string fileName = dir.getName(i);
            string presidentName = ofFilePath().getBaseName(fileName);

            if (find(user_items.begin(), user_items.end(), itemName) != user_items.end()) {
                string path = dir.getPath(i);
                ofImage img = ofImage(path);
                bool isVideo = false;
                if (!img.bAllocated()) {
                    isVideo = true;

                    video.load(path);
                    video.play();
                    video.setPaused(true);
                    video.setPosition(0.5);

                    img.setFromPixels(video.getPixels());
                }
                Item* item = new Item(path, img, isVideo, false);
                items[counter] = item;

                auxItems[counter++] = item;

                (void)ofLog(OF_LOG_NOTICE, "Generating metadata for : " + itemName);

                // generate metadata if not already generated
                generateMetadata(itemName, path, img, isVideo);
            }
        }
        itemsSize = counter;
    }
    else {
        int counter = items_input.size();
        items.clear();
        itemsSize = counter;
        items = items_input;
    }
    currentItem = 0;
}*/

//void ofApp::importMetadata(ofxDatGuiButtonEvent e) {
//    int index = e.target->getIndex();
//    ofImage auxImg = items[index]->getImage();
//    ofImage object = ofImage();
//
//    string tags = ofSystemTextBoxDialog("Number of tags", "1");
//    int numberOfTags = stoi(tags);
//
//    vector<string> listTags;
//    listTags.assign(numberOfTags, string());
//
//    for (int i = 0; i < numberOfTags; i++) {
//        string tag = ofSystemTextBoxDialog("Tag " + ofToString(i + 1), "");
//        listTags[i] = tag;
//    }
//
//    string times = ofSystemTextBoxDialog("Number of objects to process (times a specific object (input as an image) appears in the item):", "1");
//    int numberTimes = stoi(times);
//    map<string, int> mapTimes;
//
//    for (int i = 0; i < numberTimes; i++) {
//        ofFileDialogResult result = ofSystemLoadDialog("Load file", false, "object_items/");
//
//        if (result.bSuccess) {
//            string path = result.getPath();
//            object.load(path);
//            // Number of times the object appears
//            /*int numberOfTimes = objectTimesFilter(auxImg, object);
//            if(numberOfTimes!=0)
//                mapTimes.insert({ ofFilePath().getBaseName(result.filePath), numberOfTimes });*/
//        }
//        else {
//            ofSystemTextBoxDialog("Error loading file...");
//        }
//    }
//
//    
//    (void)ofLog(OF_LOG_NOTICE, "index: " + ofToString(index));
//
//    appData->presidentsMetadataXml.pushTag("item", index);
//    if (appData->presidentsMetadataXml.getNumTags("tags") == 0)
//        appData->presidentsMetadataXml.addTag("tags");
//
//        appData->presidentsMetadataXml.pushTag("tags");
//        int numExTags = appData->presidentsMetadataXml.getNumTags("tag"); // number of existing tags
//
//        for (int j = 0; j < numberOfTags; j++) {
//            appData->presidentsMetadataXml.addValue("tag", listTags[j]);
//        }
//        appData->presidentsMetadataXml.popTag(); // tags
//
//        if (appData->presidentsMetadataXml.getNumTags("times") == 0)
//            appData->presidentsMetadataXml.addTag("times");
//
//        appData->presidentsMetadataXml.pushTag("times");
//        int numExTimes = appData->presidentsMetadataXml.getNumTags("time"); // number of existing times
//
//        int j = numExTimes;
//        for (map<string, int>::iterator itr = mapTimes.begin(); itr != mapTimes.end(); ++itr) {
//            bool found = false;
//
//            int numTimesTag = appData->presidentsMetadataXml.getNumTags("time");
//            for (int i = 0; i < numTimesTag; i++) {
//                appData->presidentsMetadataXml.pushTag("time", i);
//                if (appData->presidentsMetadataXml.getValue("name", "") == itr->first)
//                    found = true;
//                
//                appData->presidentsMetadataXml.popTag(); // time
//            }
//
//            if (!found) {
//                appData->presidentsMetadataXml.addTag("time");
//                appData->presidentsMetadataXml.pushTag("time", j);
//
//                appData->presidentsMetadataXml.addValue("name", itr->first);
//                appData->presidentsMetadataXml.addValue("numTime", itr->second);
//
//                appData->presidentsMetadataXml.popTag(); // time
//            }
//            j++;
//        }
//        appData->presidentsMetadataXml.popTag(); // times
//
//        appData->presidentsMetadataXml.popTag(); // item
//
//        // Saves file
//        if (appData->presidentsMetadataXml.saveFile())
//            (void)ofLog(OF_LOG_NOTICE, "Saved!");
//        else
//            (void)ofLog(OF_LOG_NOTICE, "Didn't save!");
//}

/*void Gallery::extractMetadata() {
    
}*/

/*void Gallery::initXmlObjects() {
    
}*/

void ofApp::initButtons() {
    //---------Import
    importMetadataBtn = new ofxDatGuiButton("Import Metadata");
        
    importMetadataBtn->setPosition(50, 500);
    importMetadataBtn->setIndex(0);
    importMetadataBtn->setWidth(100);
    importMetadataBtn->onButtonEvent(this, &ofApp::onButtonEvent);

    importMetadataBtn->setEnabled(true);

    generateMetadataBtn = new ofxDatGuiButton("Generate Metadata");

    generateMetadataBtn->setPosition(50, 550);
    generateMetadataBtn->setIndex(1);
    generateMetadataBtn->setWidth(105);
    generateMetadataBtn->onButtonEvent(this, &ofApp::onButtonEvent);

    generateMetadataBtn->setEnabled(true);
    
    extractMetadataBtn = new ofxDatGuiButton("Extract Metadata");
    
    extractMetadataBtn->setPosition(50, 600);
    extractMetadataBtn->setIndex(2);
    extractMetadataBtn->setWidth(100);
    extractMetadataBtn->onButtonEvent(this, &ofApp::onButtonEvent);
    
    extractMetadataBtn->setEnabled(true);
}

void ofApp::onButtonEvent(ofxDatGuiButtonEvent e) {
    
}

