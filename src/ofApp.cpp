#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
    #define METADATA_GENERATION_N_THREADS 1
#else //macos

   #define METADATA_GENERATION_N_THREADS 1
#endif

#include "ofApp.h"

#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"

using namespace cv;
using cv::Mat;
using cv::Point2f;
using cv::KeyPoint;
using cv::Scalar;
using cv::BFMatcher;
using cv::FastFeatureDetector;
using cv::SimpleBlobDetector;

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

    startMetadataGeneration();

    initButtons();
        
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

    importMetadataBtn->draw();
    generateMetadataBtn->draw();
    extractMetadataBtn->draw();
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
void ofApp::mouseReleased(int x, int y, int button){
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

void ofApp::startMetadataGeneration() {
    vector<thread> threads;

    const int n_threads = METADATA_GENERATION_N_THREADS;
    int n_presidents = presidentsMedias.size();

    int presidents_per_thread = n_presidents / n_threads;

    isGeneratingMetadata = true;

    for (int i = 0; i < n_presidents && n_threads > 1; i += presidents_per_thread) {
        int endPres = i + presidents_per_thread > n_presidents - 1 ? n_presidents - 1 : i + presidents_per_thread;

        std::thread _thread(&ofApp::generateMetadataThread, this, i, endPres);

        //_thread.join();

        threads.push_back(std::move(_thread));
    }

    if (n_threads == 1) generateMetadataThread(0, n_presidents - 1);

    for (int t = 0; t < threads.size(); t++)
        threads[t].join();

    for (int w = 0; w < threadsWork.size(); w++) {
        GenerateMetadataThreadWork* work = threadsWork[w];

        presidentsMetadataXml.addTag("president");
        presidentsMetadataXml.pushTag("president", w); // mudar para numberOfItems + 1 depois de testar
        // saves the id - itemName
        presidentsMetadataXml.setValue("id", work->president->name, w);

        presidentsMetadataXml.setValue("luminance", work->luminance, w);
        presidentsMetadataXml.setValue("color", work->color, w);
        presidentsMetadataXml.setValue("texture", work->texture, w);
        presidentsMetadataXml.setValue("faces", work->faces, w);
        presidentsMetadataXml.setValue("edges", work->edges, w);
        presidentsMetadataXml.setValue("rhythm", work->rhythm, w);

        presidentsMetadataXml.popTag(); // item

        presidentsMetadataXml.saveFile();
    }

    isGeneratingMetadata = false;
}

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

    cvtColor(src, hsv, CV_RGB2HSV);
    split(hsv, channels);
    gray_image = channels[0];

    Canny(*president->profilePicture, dst, 10, 350);

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

double ofApp::rhythmFilter(ofVideoPlayer* video) {
    Mat src_0 = toCv(video->getPixels());
    video->setPosition(0.25);
    Mat src_1 = toCv(video->getPixels());
    video->setPosition(0.5);
    Mat src_2 = toCv(video->getPixels());
    video->setPosition(0.75);
    Mat src_3 = toCv(video->getPixels());
    video->setPosition(1);
    Mat src_4 = toCv(video->getPixels());

    if (src_0.empty() || src_1.empty() || src_2.empty() || src_3.empty() || src_4.empty())
        // something went wrong
        return -1;

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
        rhythm += compareHist(hist_0, histVector[i], i);
    rhythm /= histVector.size();
    return rhythm;
}

void ofApp::generateMetadataThread(int startPres, int endPres) {
    ofxCvHaarFinder finder;
    finder.setup("data_xml/haarcascade_frontalface_default.xml");

    for (int i = startPres; i <= endPres; i++)
        generateMetadata(presidentsMedias[i], &finder);
}

void ofApp::generateMetadata(President *president, ofxCvHaarFinder *finder) {
    if (president == NULL) return;
    
    GenerateMetadataThreadWork* work = new GenerateMetadataThreadWork();
    work->president = president;

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
    
    work->luminance = avgLuminance;
    work->color = newColor.getHue();

    // faces
    // finder faces
    //
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

    // edges - filter2D
    work->edges = edgesFilter(president);

    // texture
    work->texture = textureFilter(president);
    
    // rhythm
    if (president->biographyVideo != NULL)
        work->rhythm = rhythmFilter(president->biographyVideo);

    //haarFinderMutex.lock();

    int faces = finder->findHaarObjects(*president->profilePicture);

    //presidentsMetadataXml.setValue("faces", faces, president->pres_id);
    work->faces = faces;

    threadsWork[president->pres_id] = work;

    //haarFinderMutex.unlock();
 
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
            int numberOfTimes = objectTimesFilter(auxImg, object);
            if(numberOfTimes!=0)
                mapTimes.insert({ ofFilePath().getBaseName(result.filePath), numberOfTimes });
        } else
            ofSystemTextBoxDialog("Error loading file...");
    }
    
    (void)ofLog(OF_LOG_NOTICE, "index: " + ofToString(index));

    presidentsMetadataXml.pushTag("item", index);
    if (presidentsMetadataXml.getNumTags("tags") == 0)
        presidentsMetadataXml.addTag("tags");

        presidentsMetadataXml.pushTag("tags");
        int numExTags = presidentsMetadataXml.getNumTags("tag"); // number of existing tags

        for (int j = 0; j < numberOfTags; j++)
            presidentsMetadataXml.addValue("tag", listTags[j]);
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

void ofApp::extractMetadata(ofxDatGuiButtonEvent e) {
    int index = e.target->getIndex();
    ofxXmlSettings saveFile;

    int numPresidents = presidentsMetadataXml.getNumTags("item");
    for (int i = 0; i < numPresidents; i++) {
        presidentsXml.pushTag("item", i);
        if (i == index) {
            saveFile.addValue("id", presidentsMetadataXml.getValue("id", ""));

            saveFile.addTag("tags");
            saveFile.pushTag("tags");

            presidentsXml.pushTag("tags");
            int numTags = presidentsMetadataXml.getNumTags("tag");
            for (int j = 0; j < numTags; j++)
                saveFile.addValue("tag", presidentsMetadataXml.getValue("tag", "", j));
            presidentsXml.popTag(); // tags
            saveFile.popTag(); // tags

            saveFile.addValue("luminance", presidentsMetadataXml.getValue("luminance", ""));
            saveFile.addValue("color", presidentsMetadataXml.getValue("color", ""));
            saveFile.addValue("faces", presidentsMetadataXml.getValue("faces", ""));
            saveFile.addValue("edges", presidentsMetadataXml.getValue("edges", ""));
            saveFile.addValue("texture", presidentsMetadataXml.getValue("texture", ""));

            saveFile.addTag("times");
            saveFile.pushTag("times");

            presidentsMetadataXml.pushTag("times");
            int numTimes = presidentsMetadataXml.getNumTags("time");
            for (int j = 0; j < numTimes; j++) {
                saveFile.addTag("time");
                saveFile.pushTag("time");
                presidentsMetadataXml.pushTag("time", j);

                saveFile.addValue("name", presidentsMetadataXml.getValue("name", "", j));
                saveFile.addValue("numTime", presidentsMetadataXml.getValue("numTime", "", j));

                presidentsMetadataXml.popTag(); // time
                saveFile.popTag(); // time
            }
            presidentsMetadataXml.popTag(); // times
            saveFile.popTag(); // times

            if (items[index]->getIsVideo())
                saveFile.addValue("rhythm", presidentsMetadataXml.getValue("rhythm", ""));

            presidentsMetadataXml.popTag(); // item
            break;
        }
        presidentsMetadataXml.popTag(); // item
    }
    // SAVES THE FILE
    /*windowFileSys = ofSystemSaveDialog("save.txt", "Save");
    if (windowFileSys.bSuccess) {
        string path = windowFileSys.getPath();
        saveFile.save(path);
    }*/
}

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

int ofApp::objectTimesFilter(ofImage image, ofImage objImage) {
    int numberOfMatches = 0;
    ofImage tempImg = image;
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

        cv::Ptr<ORB> detector = ORB::create();
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
}
