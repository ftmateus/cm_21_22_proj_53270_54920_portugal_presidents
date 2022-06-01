#include "ofApp.h"
//--------------------------------------------------------------
void ofApp::setup() {

    ofSetVerticalSync(true);

    ofSetWindowTitle("🇵🇹 Presidentes de Portugal - David Pereira e Francisco Mateus 🇵🇹");

    frameByframe = false;

    data = new ofAppData();

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


    if (!data->presidentsXml.load("data_xml/presidents.xml")) {
        ofLogError() << "Couldn't load file";
        
    }
   
    getPresidentsInfo();

    if (data->presidentsMetadataXml.load("data_xml/presidents_metadata.xml"))
    {
        importMetadata();
        //assert(data->metadataGenerated == true);
    }
    else
    {
        data->presidentsMetadataXml.clear();
        data->presidentsMetadataXml.addTag("presidentsMetadata");
        data->presidentsMetadataXml.save("data_xml/presidents_metadata.xml");
        //data->presidentsMetadataXml.pushTag("presidentsMetadata");
        //data->presidentsMetadataXml.load("data_xml/presidents_metadata.xml");
        /*ofLogError() << "Couldn't load file";
        assert(false);*/
    }

    initButtons();
        
    ofBackground(ofColor::white);
}

//--------------------------------------------------------------
void ofApp::update() {

    ofVideoPlayer* vid = data->presidentsMedias[data->currentPresidentIdx]->biographyVideo;

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

    im1->draw();
    /*im2->draw();
    im3->draw();*/

    //gui.draw();

    drawPresidents(); 

    drawBiographyVideo();

    if (data->isGeneratingMetadata)
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
        if (president->metadata->edgesProfilePicture == NULL)
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

void ofApp::drawPresidents()
{    
    ofSetColor(ofColor::white);
    ofImage* centerPresidentImg = getPresidentProfilePicture(data->presidentsMedias[data->currentPresidentIdx]);
    centerPresidentImg->draw(centerPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, CENTER_PRESIDENT_IMG_WIDTH, CENTER_PRESIDENT_IMG_HEIGHT);
    
    for (int prevImageIdx = data->currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
    {        
        int previousPresidentImgXPos = centerPresidentImgXPos - (NEIGHBOUR_PRESIDENT_IMG_WIDTH)*times - SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (previousPresidentImgXPos + NEIGHBOUR_PRESIDENT_IMG_WIDTH <= 0) break;

        ofImage* prevPresidentImg = getPresidentProfilePicture(data->presidentsMedias[prevImageIdx]);
        prevPresidentImg->draw(previousPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }
   
    for (int nextImageIdx = data->currentPresidentIdx + 1, times = 1; nextImageIdx < data->presidentsMedias.size(); nextImageIdx++, times++)
    {
        int nextPresidentImgXPos = (centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH) + NEIGHBOUR_PRESIDENT_IMG_WIDTH*(times - 1) + SPACE_BETWEEN_PRESIDENTS * times;

        //off window limits check
        if (nextPresidentImgXPos  >= ofGetWidth()) break;

        ofImage* nextPresidentImg = getPresidentProfilePicture(data->presidentsMedias[nextImageIdx]);
        nextPresidentImg->draw(nextPresidentImgXPos, PRESIDENTS_CARROUSEL_Y_POS, NEIGHBOUR_PRESIDENT_IMG_WIDTH, NEIGHBOUR_PRESIDENT_IMG_HEIGHT);
    }

    myfont.load("arial.ttf", 20);
    ofSetColor(ofColor::black);

    {
        string presidentName = data->presidentsMedias[data->currentPresidentIdx]->name;

        drawStringCentered(presidentName, windowXCenter, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT + 25);
    }

    {
        string startDate = data->presidentsMedias[data->currentPresidentIdx]->startDate;

        drawStringRight(startDate, centerPresidentImgXPos - 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string endDate = data->presidentsMedias[data->currentPresidentIdx]->endDate;
        myfont.drawString(endDate, (windowXCenter + CENTER_PRESIDENT_IMG_WIDTH / 2) + 10, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT - 75);
    }

    {
        string birthDate = data->presidentsMedias[data->currentPresidentIdx]->birthDate;
        string deathDate = data->presidentsMedias[data->currentPresidentIdx]->deathDate;

        string presidentLifePeriod = birthDate + "->" + deathDate;

        drawStringCentered(presidentLifePeriod, windowXCenter, PRESIDENTS_CARROUSEL_Y_POS + CENTER_PRESIDENT_IMG_HEIGHT + 50);
    }

}

void ofApp::importMetadata()
{    auto xml = data->presidentsMetadataXml;

    xml.pushTag("presidentsMetadata");

    int n_presidents = xml.getNumTags("president");

    if (n_presidents != data->presidentsMedias.size())
    {
        assert(false);
        return;
    }

    for (int p = 0; p < n_presidents; p++)
    {
        PresidentMetadata* metadata = new PresidentMetadata();

        metadata->luminance = stof(string(xml.getValue("president:luminance", "", p)));
        metadata->faces     = stoi(string(xml.getValue("president:faces", "", p)));
        metadata->rhythm    = stod(string(xml.getValue("president:rhythm", "", p)));
        metadata->color     = stof(string(xml.getValue("president:color", "", p)));
        metadata->texturePath   = string(xml.getValue("president:texture", "", p));
        metadata->edgesPath     = string(xml.getValue("president:edges", "", p));


        President* president = data->presidentsMedias[p];
        president->metadata = metadata;
    }

    data->metadataGenerated = true;

    xml.popTag();
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

    ofVideoPlayer* vid = data->presidentsMedias[data->currentPresidentIdx]->biographyVideo;

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

    switch (key)
    {
        case OF_KEY_RIGHT:
            if(data->currentPresidentIdx < data->presidentsMedias.size() - 1)
                previousPresidentIdx = data->currentPresidentIdx++;
            break;
            
        case OF_KEY_LEFT:
            if (data->currentPresidentIdx > 0)
                previousPresidentIdx = data->currentPresidentIdx--;
            break;
        case ' ':
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
            string tags = ofSystemTextBoxDialog("Number of tags", "1");
            int numberOfTags = stoi(tags);
            break;
    }        
    if (previousPresidentIdx != -1)
        switchPresident(data->presidentsMedias[previousPresidentIdx]);

    
       /* if (currentMedia < 0)
            currentMedia = (int)mediaFiles.size() - 1;
        else
            currentMedia %= mediaFiles.size();*/
}

void ofApp::applyFilter(Filters filter)
{
    if (!data->metadataGenerated) return;
    currentFilterApplied = currentFilterApplied == filter ? NO_FILTER : filter;
}

void ofApp::pausePlayVideo()
{
    ofVideoPlayer* vid = data->presidentsMedias[data->currentPresidentIdx]->biographyVideo;

    if (vid == NULL) return;

    vid->setPaused(!vid->isPaused());
    //vid->closeMovie();
}

void ofApp::generateMetadata()
{
    data->isGeneratingMetadata = true;
    data->metadataGenerated = false;
    //generateMetadataThread = new GenerateMetadata(data);
    generateMetadataThread.setup(data);
    currentFilterApplied = NO_FILTER;
    //std::thread _thread(&ofApp::startMetadataGeneration, this);
}

void ofApp::searchPresidents()
{

}

void ofApp::switchPresident(President* previousPresident)
{
    if (previousPresident == NULL) return;

    President* currentPresident = data->presidentsMedias[data->currentPresidentIdx];
 
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
        for (int prevImageIdx = data->currentPresidentIdx - 1, times = 1; prevImageIdx >= 0; prevImageIdx--, times++)
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
        for (int nextImageIdx = data->currentPresidentIdx + 1, times = 1; nextImageIdx < data->presidentsMedias.size(); nextImageIdx++, times++)
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

void ofApp::getPresidentsInfo()
{
    data->presidentsXml.pushTag("presidents");

    vector<thread> threads;

    const int n_threads = 1;

    int n_presidents = data->presidentsXml.getNumTags("president");

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
    //    PresidentMetadata* work = threadsWork[w];

    //    data->presidentsMetadataXml.addTag("president");
    //    data->presidentsMetadataXml.pushTag("president", w); // mudar para numberOfItems + 1 depois de testar
    //    // saves the id - itemName
    //    data->presidentsMetadataXml.setValue("id", work->president->name, w);

    //    data->presidentsMetadataXml.setValue("luminance", work->luminance, w);
    //    data->presidentsMetadataXml.setValue("color", work->color, w);
    //    data->presidentsMetadataXml.setValue("texture", work->texture, w);
    //    data->presidentsMetadataXml.setValue("faces", work->faces, w);
    //    data->presidentsMetadataXml.setValue("edges", work->edges, w);


    //    data->presidentsMetadataXml.popTag(); // item

    //    data->presidentsMetadataXml.saveFile();
    //}
}

void ofApp::getPresidentsInfoThread(int startPres, int endPres)
{
    for (int i = startPres; i <= endPres; i++)
    {
        getPresidentInfo(i);
    }
}

void ofApp::getPresidentInfo(int xmlIndex)
{
    President* presMedia = new President();

    //string name = data->presidentsXml.getValue("president:name", "", i);

    presMedia->pres_id = xmlIndex;
    presMedia->name = string(data->presidentsXml.getValue("president:name", "", xmlIndex));

    if (presMedia->name == "") return;

    presMedia->startDate = string(data->presidentsXml.getValue("president:startDate", "", xmlIndex));
    presMedia->endDate = string(data->presidentsXml.getValue("president:endDate", "", xmlIndex));
    presMedia->birthDate = string(data->presidentsXml.getValue("president:birthDate", "", xmlIndex));
    presMedia->deathDate = string(data->presidentsXml.getValue("president:deathDate", "", xmlIndex));

    presMedia->profilePicturePath = data->presidentsXml.getValue("president:profilePicture", "", xmlIndex);
    presMedia->profilePicture = new ofImage();
    
    if (!presMedia->profilePicture->load("images/" + presMedia->profilePicturePath))
        assert(false);

    presMedia->biographyVideoPath = data->presidentsXml.getValue("president:biographyVideo", "", xmlIndex);
    if (presMedia->biographyVideoPath != "")
    {
        presMedia->biographyVideo = new ofVideoPlayer();
        if(!presMedia->biographyVideo->load("videos/" + presMedia->biographyVideoPath))
            assert(false);
    }

    data->mutex.lock();

    data->presidentsMedias[presMedia->pres_id] = presMedia;

    data->mutex.unlock();

    //generateMetadata(presMedia);
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
            int previousPresidentIdx = data->currentPresidentIdx;
            data->currentPresidentIdx = president;
            switchPresident(data->presidentsMedias[previousPresidentIdx]);
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
        if (scrollY >= 1.0 && data->currentPresidentIdx < data->presidentsMedias.size() - 1)
            previousPresidentIdx = data->currentPresidentIdx++;
        else if (scrollY <= -1.0 && data->currentPresidentIdx > 0)
            previousPresidentIdx = data->currentPresidentIdx--;

        if (previousPresidentIdx != -1)
            switchPresident(data->presidentsMedias[previousPresidentIdx]);

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
//    data->presidentsMetadataXml.pushTag("item", index);
//    if (data->presidentsMetadataXml.getNumTags("tags") == 0)
//        data->presidentsMetadataXml.addTag("tags");
//
//        data->presidentsMetadataXml.pushTag("tags");
//        int numExTags = data->presidentsMetadataXml.getNumTags("tag"); // number of existing tags
//
//        for (int j = 0; j < numberOfTags; j++) {
//            data->presidentsMetadataXml.addValue("tag", listTags[j]);
//        }
//        data->presidentsMetadataXml.popTag(); // tags
//
//        if (data->presidentsMetadataXml.getNumTags("times") == 0)
//            data->presidentsMetadataXml.addTag("times");
//
//        data->presidentsMetadataXml.pushTag("times");
//        int numExTimes = data->presidentsMetadataXml.getNumTags("time"); // number of existing times
//
//        int j = numExTimes;
//        for (map<string, int>::iterator itr = mapTimes.begin(); itr != mapTimes.end(); ++itr) {
//            bool found = false;
//
//            int numTimesTag = data->presidentsMetadataXml.getNumTags("time");
//            for (int i = 0; i < numTimesTag; i++) {
//                data->presidentsMetadataXml.pushTag("time", i);
//                if (data->presidentsMetadataXml.getValue("name", "") == itr->first)
//                    found = true;
//                
//                data->presidentsMetadataXml.popTag(); // time
//            }
//
//            if (!found) {
//                data->presidentsMetadataXml.addTag("time");
//                data->presidentsMetadataXml.pushTag("time", j);
//
//                data->presidentsMetadataXml.addValue("name", itr->first);
//                data->presidentsMetadataXml.addValue("numTime", itr->second);
//
//                data->presidentsMetadataXml.popTag(); // time
//            }
//            j++;
//        }
//        data->presidentsMetadataXml.popTag(); // times
//
//        data->presidentsMetadataXml.popTag(); // item
//
//        // Saves file
//        if (data->presidentsMetadataXml.saveFile())
//            (void)ofLog(OF_LOG_NOTICE, "Saved!");
//        else
//            (void)ofLog(OF_LOG_NOTICE, "Didn't save!");
//}

/*void Gallery::extractMetadata() {
    
}*/

/*void Gallery::initXmlObjects() {
    
}*/

void ofApp::initButtons()
{
    //---------Import
    im1 = new ofxDatGuiButton("Import Metadata");
    im1->setPosition(450, 550);
    im1->setIndex(0);
    im1->setWidth(100);
    //im1->onButtonEvent(this, &ofApp::importMetadata);

    /*im2 = new ofxDatGuiButton("Import Metadata");
    im2->setPosition(200 + imageSize, imageSize + 100);
    im2->setIndex(1);
    im2->setWidth(100);
    im2->onButtonEvent(this, &ofApp::importMetadata);

    im3 = new ofxDatGuiButton("Import Metadata");
    im3->setPosition(250 + imageSize * 2, imageSize + 100);
    im3->setIndex(2);
    im3->setWidth(100);
    im3->onButtonEvent(this, &ofApp::importMetadata);*/
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
