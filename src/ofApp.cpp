#include "ofApp.h"
#include <thread>
#include <memory>

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
    presidentsXml.pushTag("presidents");


    int n_presidents = presidentsXml.getNumTags("president");

    for (int i = 0; i < n_presidents; i++) {

        President* presMedia = new President();

        //string name = presidentsXml.getValue("president:name", "", i);

        presMedia->name         = string(presidentsXml.getValue("president:name", "", i));
        presMedia->pres_id = i;
        presMedia->startDate    = string(presidentsXml.getValue("president:startDate", "", i));
        presMedia->endDate      = string(presidentsXml.getValue("president:endDate", "", i));
        presMedia->birthDate    = string(presidentsXml.getValue("president:birthDate", "", i));
        presMedia->deathDate    = string(presidentsXml.getValue("president:deathDate", "", i));

        string profilePicture = presidentsXml.getValue("president:profilePicture", "", i);
        presMedia->profilePicture = new ofImage();
        presMedia->profilePicture->load("images/" + profilePicture);

        string biographyVideo = presidentsXml.getValue("president:biographyVideo", "", i);
        if (biographyVideo != "")
        {
            presMedia->biographyVideo = new ofVideoPlayer();
            presMedia->biographyVideo->load("videos/" + biographyVideo);
        }

        generateMetadata(presMedia);

        presidentsMedias.push_back(presMedia);
    }

    //vector<thread> threads;

    //for (int i = 0; i < n_presidents; i+=4) {
    //    int endPres = i + 4 > n_presidents - 1? n_presidents  - 1: i + 4;

    //    std::thread _thread(&ofApp::generateMetadataThread, this, i, endPres);

    //    //_thread.join();

    //    threads.push_back(std::move(_thread));
    //}

    //for (int t = 0; t < threads.size(); t++)
    //    threads[t].join();

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

    drawBiographyVideo();
    
}

void ofApp::drawPresidentDescription()
{
    string presidentDescription = presidentsXml.getValue("president:description", "", currentPresidentIdx);
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


void ofApp::switchPresident(President* previousPresident)
{
    if (previousPresident == NULL) return;

    President* currentPresident = presidentsMedias[currentPresidentIdx];
 
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

//--------------------------------------------------------------
/*void ofApp::filterEdgeAndTexture() {
    int npresidents = presidentsXml.getNumTags("president");
    
    string id;
    
    for(int i = 0; i < npresidents; i++) {
        id = presidentsXml.getValue("president:id", "", i);
        string path = dir.getPath(i);
        ofImage img = ofImage(path);
        generateMetadata(id, path, img, false);
    }
    for(int i = 0; i < (int)dir.size(); i++) {
        string fileName = dir.getName(i);
        string presidentName = ofFilePath().getBaseName(fileName);
        string path = dir.getPath(i);
        ofImage img = ofImage(path);
        generateMetadata(presidentName, path, img, false);
    }
}*/


string ofApp::edgesFilter(President *president) {
    double kernel_size;
    Mat kernel;
    kernel_size = 31;
    kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (kernel_size * kernel_size);

    Mat src = toCv(*president->profilePicture);
    Mat dst;
    Mat gray_image;

    vector<Mat> channels;
    Mat hsv;

    /**
        Da erro em algumas imagens, como a do antonio salazar e francisco costa
    */
    cvtColor(src, hsv, CV_RGB2HSV);
    split(hsv, channels);
    gray_image = channels[0];

    Canny(*president->profilePicture, dst, 10, 350);

    //filter2D(src, dst, CV_32F, kernel);

    ofImage saveEdges;
    toOf(dst, saveEdges);

    string path = "textures_edges/" + president->name + "-edges.jpg";
    saveEdges.save(path);

    return path;
}

string ofApp::textureFilter(President *president) {
    Mat src, dst;
    int kernel_size = 31;

    src = toCv(*president->profilePicture); // Load an image

    double lambda = 1.0;
    double theta = 0;
    double psi =  0;
    double gamma = 0.02;
    double sigma = 0.56 * lambda;

    Mat kernel = getGaborKernel(cv::Size(kernel_size, kernel_size), sigma, theta, lambda, gamma, psi);
    filter2D(src, dst, CV_32F, kernel);

    ofImage saveTexture;
    toOf(dst, saveTexture);

    string path = "textures_edges/" + president->name + "-texture.jpg";
    saveTexture.save(path);

    return path;
}

/*double ofApp::rhythmFilter(string path)
{
    ofVideoPlayer videoAux;
    videoAux.load(path);

    Mat src_0 = toCv(video.getPixels());
    video.setPosition(0.25);
    Mat src_1 = toCv(video.getPixels());
    video.setPosition(0.5);
    Mat src_2 = toCv(video.getPixels());
    video.setPosition(0.75);
    Mat src_3 = toCv(video.getPixels());
    video.setPosition(1);
    Mat src_4 = toCv(video.getPixels());

    if (src_0.empty() || src_1.empty() || src_2.empty() || src_3.empty() || src_4.empty())
    {
        // something went wrong
        return -1;
    }

    Mat hsv_0, hsv_1, hsv_2, hsv_3, hsv_4;
    cvtColor(src_0, hsv_0, COLOR_BGR2HSV);
    cvtColor(src_1, hsv_1, COLOR_BGR2HSV);
    cvtColor(src_2, hsv_2, COLOR_BGR2HSV);
    cvtColor(src_3, hsv_3, COLOR_BGR2HSV);
    cvtColor(src_4, hsv_4, COLOR_BGR2HSV);

    Mat hsv_half_down = src_0(Range(hsv_0.rows / 2, hsv_0.rows), Range(0, hsv_0.cols));
    int h_bins = 50, s_bins = 60;
    int histSize[] = { h_bins, s_bins };

    // hue varies from 0 to 179, saturation from 0 to 255
    float h_ranges[] = { 0, 180 };
    float s_ranges[] = { 0, 256 };
    const float* ranges[] = { h_ranges, s_ranges };

    // Use the 0-th and 1-st channels
    int channels[] = { 0, 1 };

    Mat hist_0, hist_half_down, hist_1, hist_2, hist_3, hist_4;
    calcHist(&hsv_0, 1, channels, Mat(), hist_0, 2, histSize, ranges, true, false);
    normalize(hist_0, hist_0, 0, 1, NORM_MINMAX, -1, Mat());

    calcHist(&hsv_1, 1, channels, Mat(), hist_1, 2, histSize, ranges, true, false);
    normalize(hist_1, hist_1, 0, 1, NORM_MINMAX, -1, Mat());

    calcHist(&hsv_2, 1, channels, Mat(), hist_2, 2, histSize, ranges, true, false);
    normalize(hist_2, hist_2, 0, 1, NORM_MINMAX, -1, Mat());

    calcHist(&hsv_3, 1, channels, Mat(), hist_3, 2, histSize, ranges, true, false);
    normalize(hist_3, hist_3, 0, 1, NORM_MINMAX, -1, Mat());

    calcHist(&hsv_4, 1, channels, Mat(), hist_4, 2, histSize, ranges, true, false);
    normalize(hist_4, hist_4, 0, 1, NORM_MINMAX, -1, Mat());

    vector<Mat> histVector = { hist_0, hist_1, hist_2, hist_3, hist_4 };
    double rhythm = 0;
    for (int i = 1; i < histVector.size(); i++)
    {
        rhythm += compareHist(hist_0, histVector[i], i);
    }
    rhythm /= histVector.size();
    return rhythm;
}*/


void ofApp::generateMetadataThread(int startPres, int endPres) 
{
    for (int i = startPres; i <= endPres; i++)
        generateMetadata(presidentsMedias[i]);
}

void ofApp::generateMetadata(President *president) {
    //int numPresidents = presidentsXml.getNumTags("president");
    
    /*for(int i = 0; i < numPresidents; i++) {
        presidentsXml.pushTag("president", i);
        string id = presidentsXml.getValue("id", "");
        
        if(id == presidentName) {
            presidentsXml.popTag();
            return;
        }
        presidentsXml.popTag();
    }*/
    
    presidentsMetadataXml.addTag("president");
    presidentsMetadataXml.pushTag("president", president->pres_id); // mudar para numberOfItems + 1 depois de testar
    // saves the id - itemName
    presidentsMetadataXml.setValue("id", president->name, president->pres_id);

    // color && luminance
    ofPixels& pixels = president->profilePicture->getPixels();

    float avgRed = 0;
    float avgGreen = 0;
    float avgBlue = 0;
    float avgLuminance = 0;

    int pixelSize = pixels.getWidth() * pixels.getHeight();
    for (auto& pixel : pixels.getPixelsIter()) {
        avgRed += pixel.getColor().r;
        avgGreen += pixel.getColor().g;
        avgBlue += pixel.getColor().b;
        avgLuminance += pixel.getColor().getBrightness();
    }
    avgRed /= pixelSize;
    avgGreen /= pixelSize;
    avgBlue /= pixelSize;
    avgLuminance = avgLuminance / pixelSize;

    ofColor newColor;
    newColor.r = avgRed;
    newColor.g = avgGreen;
    newColor.b = avgBlue;

    presidentsMetadataXml.setValue("luminance", avgLuminance, president->pres_id);
    presidentsMetadataXml.setValue("color", newColor.getHue(), president->pres_id);

    // faces
    // finder faces
    finder.setup("data_xml/haarcascade_frontalface_default.xml");
    /*if (isVideo) {
        ofVideoPlayer auxVideo;
        auxVideo.load(path);

        for (int i = 0; i <= 5; i++) {
            ofImage auxImg;

            auxImg.setFromPixels(auxVideo.getPixels());
            faces += finder.findHaarObjects(auxImg);

            auxVideo.setPosition(i * 0.25);
        }
        faces /= 5;
    }
    else {
        
    }*/

    int faces = finder.findHaarObjects(*president->profilePicture);

    presidentsMetadataXml.setValue("faces", faces, president->pres_id);
    // edges - filter2D
    /*string edges = edgesFilter(president);

    if (edges != "")
        presidentsMetadataXml.setValue("edges", edges, president->pres_id);*/
    //// texture
    string texture = textureFilter(president);
    if (texture != "")
        presidentsMetadataXml.setValue("texture", texture, president->pres_id);
    // rhythm
    /*if (isVideo) {
        double rhythm = rhythmFilter(path);
        presidentsXml.setValue("rhythm", rhythm, numPresidents);
    }*/

    presidentsMetadataXml.popTag(); // item

    presidentsMetadataXml.saveFile();
    
}

void ofApp::filterItems(string filter) {
    // if filter is empty, items = auxItems
    remove(filter.begin(), filter.end(), ' '); // trim
    if (filter == "") {
        (void)ofLog(OF_LOG_NOTICE, "Items = AuxItems!");
        items.clear();

        int size = auxItems.size();
        items.resize(size);
        itemsSize = size;
        items = auxItems;

        return;
    }

    // filter items
    vector<Item*> filteredItems;
    int counter = 0;
    int numPresidents = presidentsXml.getNumTags("president");

    for (int i = 0; i < numPresidents; i++) {
        bool wasAdded = false;
        // tags
        presidentsXml.pushTag("president", i);
        presidentsXml.pushTag("tags");

        int numTags = presidentsXml.getNumTags("tag");
        for (int j = 0; j < numTags; j++) {
            string tag = presidentsXml.getValue("tag", "", j);

            if (tag.find(filter) != std::string::npos) { // add this item
                filteredItems.push_back(auxItems[i]);
                counter++;
                wasAdded = true;
                break;
            }
        }
        presidentsXml.popTag(); // tags
        // so the same item isnt added twice
        if (!wasAdded) {
            // times
            presidentsXml.pushTag("times");

            int numTimes = presidentsXml.getNumTags("time");
            for (int j = 0; j < numTimes; j++) {
                presidentsXml.pushTag("time", j);
                string name = presidentsXml.getValue("name", "");

                if (name.find(filter) != std::string::npos) { // add this item
                    filteredItems.push_back(auxItems[i]);
                    counter++;
                    break;
                }
            }
            presidentsXml.popTag(); // times
        }

        presidentsXml.popTag(); // item
    }
    // items = filteredItems
    items.clear();
    items.resize(counter);
    itemsSize = counter;
    items = filteredItems;
}

void ofApp::filterByColor(float hue) {
    vector<Item*> filteredItems;
    int counter = 0;
    int numPresidents = presidentsXml.getNumTags("president");

    for (int i = 0; i < numPresidents; i++) {
        presidentsXml.pushTag("president", i);
        float color = presidentsXml.getValue("color", 0);
        if (abs(color - hue) <= 10 || abs(color - hue) >= 245) {
            // this item will apear
            filteredItems.push_back(auxItems[i]);
            counter++;
        }
        presidentsXml.popTag(); // item
    }
    // items = filteredItems
    items.clear();
    items.resize(counter);
    itemsSize = counter;
    items = filteredItems;
}

/*void ofApp::handleUserItems(int userId, vector<Item*> items_input, bool useItemsInput) {
    if (!useItemsInput) {
        int numberOfUsers = user_presidentsXml.getNumTags("user_items");

        int numberOfItems = 0;
        vector<string> user_items;

        for (int i = 0; i < numberOfUsers; i++) {
            user_presidentsXml.pushTag("user_items", i);

            if (user_presidentsXml.getValue("user", 0) == userId) {
                // get items
                user_presidentsXml.pushTag("items", i);
                numberOfItems = user_presidentsXml.getNumTags("item");

                user_items.assign(numberOfItems, string());

                for (int j = 0; j < numberOfItems; j++) {
                    user_presidentsXml.pushTag("item", j);
                    string itemId = user_presidentsXml.getValue("id", "");
                    // add to vector
                    user_items.push_back(itemId);

                    user_presidentsXml.popTag(); // item
                }
            }
            user_presidentsXml.popTag(); // items
            user_presidentsXml.popTag(); // user_items
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

void ofApp::importMetadata(ofxDatGuiButtonEvent e) {
    int index = e.target->getIndex();
    ofImage auxImg = items[index]->getImage();
    ofImage object = ofImage();

    string tags = ofSystemTextBoxDialog("Number of tags", "1");
    int numberOfTags = stoi(tags);

    vector<string> listTags;
    listTags.assign(numberOfTags, string());

    for (int i = 0; i < numberOfTags; i++) {
        string tag = ofSystemTextBoxDialog("Tag " + ofToString(i + 1), "");
        listTags[i] = tag;
    }

    string times = ofSystemTextBoxDialog("Number of objects to process (times a specific object (input as an image) appears in the item):", "1");
    int numberTimes = stoi(times);
    map<string, int> mapTimes;

    for (int i = 0; i < numberTimes; i++) {
        ofFileDialogResult result = ofSystemLoadDialog("Load file", false, "object_items/");

        if (result.bSuccess) {
            string path = result.getPath();
            object.load(path);
            // Number of times the object appears
            /*int numberOfTimes = objectTimesFilter(auxImg, object);
            if(numberOfTimes!=0)
                mapTimes.insert({ ofFilePath().getBaseName(result.filePath), numberOfTimes });*/
        }
        else {
            ofSystemTextBoxDialog("Error loading file...");
        }
    }
    
    (void)ofLog(OF_LOG_NOTICE, "index: " + ofToString(index));

    presidentsMetadataXml.pushTag("item", index);
    if (presidentsMetadataXml.getNumTags("tags") == 0)
        presidentsMetadataXml.addTag("tags");

        presidentsMetadataXml.pushTag("tags");
        int numExTags = presidentsMetadataXml.getNumTags("tag"); // number of existing tags

        for (int j = 0; j < numberOfTags; j++) {
            presidentsMetadataXml.addValue("tag", listTags[j]);
        }
        presidentsMetadataXml.popTag(); // tags

        if (presidentsMetadataXml.getNumTags("times") == 0)
            presidentsMetadataXml.addTag("times");

        presidentsMetadataXml.pushTag("times");
        int numExTimes = presidentsMetadataXml.getNumTags("time"); // number of existing times

        int j = numExTimes;
        for (map<string, int>::iterator itr = mapTimes.begin(); itr != mapTimes.end(); ++itr) {
            bool found = false;

            int numTimesTag = presidentsMetadataXml.getNumTags("time");
            for (int i = 0; i < numTimesTag; i++) {
                presidentsMetadataXml.pushTag("time", i);
                if (presidentsMetadataXml.getValue("name", "") == itr->first)
                    found = true;
                
                presidentsMetadataXml.popTag(); // time
            }

            if (!found) {
                presidentsMetadataXml.addTag("time");
                presidentsMetadataXml.pushTag("time", j);

                presidentsMetadataXml.addValue("name", itr->first);
                presidentsMetadataXml.addValue("numTime", itr->second);

                presidentsMetadataXml.popTag(); // time
            }
            j++;
        }
        presidentsMetadataXml.popTag(); // times

        presidentsMetadataXml.popTag(); // item

        // Saves file
        if (presidentsMetadataXml.saveFile())
            (void)ofLog(OF_LOG_NOTICE, "Saved!");
        else
            (void)ofLog(OF_LOG_NOTICE, "Didn't save!");
}

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
    im1->onButtonEvent(this, &ofApp::importMetadata);

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


