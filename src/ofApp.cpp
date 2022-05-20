#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
    
    ofSetWindowTitle("🇵🇹 Presidentes de Portugal - David Pereira e Francisco Mateus 🇵🇹");

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
    
    //filterEdgeAndTexture();

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

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    /* OF_MOUSE_BUTTON_LEFT
    OF_MOUSE_BUTTON_RIGHT
    OF_MOUSE_BUTTON_MIDDLE */

    int centerPresidentImgXPos = windowXCenter - CENTER_PRESIDENT_IMG_WIDTH / 2;

    if (isMousePtrInCarrousel(x, y))
    {
        if (x <= centerPresidentImgXPos || x >= centerPresidentImgXPos + CENTER_PRESIDENT_IMG_WIDTH)
        {

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

//--------------------------------------------------------------
/*void ofApp::filterEdgeAndTexture() {
    int npresidents = mainXml.getNumTags("president");
    
    string id;
    
    for(int i = 0; i < npresidents; i++) {
        id = mainXml.getValue("president:id", "", i);
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

string ofApp::edgesFilter(string presidentName, ofImage image)
{
    double kernel_size;
    Mat kernel;
    kernel_size = 31;
    kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (kernel_size * kernel_size);

    Mat src = toCv(image);
    Mat dst;
    Mat gray_image;

    vector<Mat> channels;
    Mat hsv;
    cvtColor(src, hsv, CV_RGB2HSV);
    split(hsv, channels);
    gray_image = channels[0];

    Canny(image, dst, 10, 350);

    //filter2D(src, dst, CV_32F, kernel);

    ofImage saveEdges;
    toOf(dst, saveEdges);

    string path = "textures_edges/" + presidentName + "-edges.jpg";
    saveEdges.save(path);

    return path;
}

string ofApp::textureFilter(string presidentName, ofImage image)
{
    Mat src, dst;
    int kernel_size = 31;

    src = toCv(image); // Load an image

    double lambda = 1.0;
    double theta = 0;
    double psi =  0;
    double gamma = 0.02;
    double sigma = 0.56 * lambda;

    Mat kernel = getGaborKernel(cv::Size(kernel_size, kernel_size), sigma, theta, lambda, gamma, psi);
    filter2D(src, dst, CV_32F, kernel);

    ofImage saveTexture;
    toOf(dst, saveTexture);

    string path = "textures_edges/" + presidentName + "-texture.jpg";
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

/*void ofApp::generateMetadata(string presidentName, string path, ofImage image, bool isVideo) {
    int numPresidents = mainXml.getNumTags("president");
    
    for(int i = 0; i < numPresidents; i++) {
        mainXml.pushTag("president", i);
        string id = mainXml.getValue("id", "");
        
        if(id == presidentName) {
            mainXml.popTag();
            return;
        }
        mainXml.popTag();
    }
    
    mainXml.addTag("president");
    mainXml.pushTag("president", numPresidents); // mudar para numberOfItems + 1 depois de testar
    // saves the id - itemName
    mainXml.setValue("id", presidentName, numPresidents);

    // color && luminance
    ofPixels& pixels = image.getPixels();

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

    mainXml.setValue("luminance", avgLuminance, numPresidents);
    mainXml.setValue("color", newColor.getHue(), numPresidents);

    // faces
    // finder faces
    finder.setup("data_xml/haarcascade_frontalface_default.xml");
    int faces = 0;
    if (isVideo) {
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
        faces = finder.findHaarObjects(image);
    }

    mainXml.setValue("faces", faces, numPresidents);
    // edges - filter2D
    string edges = edgesFilter(presidentName, image);

    if (edges != "")
        mainXml.setValue("edges", edges, numPresidents);
    // texture
    string texture = textureFilter(presidentName, image);
    if (texture != "")
        mainXml.setValue("texture", texture, numPresidents);
    // rhythm
    if (isVideo) {
        double rhythm = rhythmFilter(path);
        mainXml.setValue("rhythm", rhythm, numPresidents);
    }

    mainXml.popTag(); // item

    mainXml.saveFile();
    
}*/

/*void ofApp::filterItems(string filter)
{
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
    int numItems = itemsXML.getNumTags("item");

    for (int i = 0; i < numItems; i++) {
        bool wasAdded = false;
        // tags
        itemsXML.pushTag("item", i);
        itemsXML.pushTag("tags");

        int numTags = itemsXML.getNumTags("tag");
        for (int j = 0; j < numTags; j++) {
            string tag = itemsXML.getValue("tag", "", j);

            if (tag.find(filter) != std::string::npos) { // add this item
                filteredItems.push_back(auxItems[i]);
                counter++;
                wasAdded = true;
                break;
            }
        }
        itemsXML.popTag(); // tags
        // so the same item isnt added twice
        if (!wasAdded) {
            // times
            itemsXML.pushTag("times");

            int numTimes = itemsXML.getNumTags("time");
            for (int j = 0; j < numTimes; j++) {
                itemsXML.pushTag("time", j);
                string name = itemsXML.getValue("name", "");

                if (name.find(filter) != std::string::npos) { // add this item
                    filteredItems.push_back(auxItems[i]);
                    counter++;
                    break;
                }
            }
            itemsXML.popTag(); // times
        }

        itemsXML.popTag(); // item
    }
    // items = filteredItems
    items.clear();
    items.resize(counter);
    itemsSize = counter;
    items = filteredItems;
}*/

/*void Gallery::filterByColor(float hue)
{
    vector<Item*> filteredItems;
    int counter = 0;
    int numberOfItems = itemsXML.getNumTags("item");

    for (int i = 0; i < numberOfItems; i++) {
        itemsXML.pushTag("item", i);
        float color = itemsXML.getValue("color", 0);
        if (abs(color - hue) <= 10 || abs(color - hue) >= 245) {
            // this item will apear
            filteredItems.push_back(auxItems[i]);
            counter++;
        }
        itemsXML.popTag(); // item
    }
    // items = filteredItems
    items.clear();
    items.resize(counter);
    itemsSize = counter;
    items = filteredItems;
}*/

/*void ofApp::handleUserItems(int userId, vector<Item*> items_input, bool useItemsInput) {
    if (!useItemsInput) {
        int numberOfUsers = user_itemsXML.getNumTags("user_items");

        int numberOfItems = 0;
        vector<string> user_items;

        for (int i = 0; i < numberOfUsers; i++) {
            user_itemsXML.pushTag("user_items", i);

            if (user_itemsXML.getValue("user", 0) == userId) {
                // get items
                user_itemsXML.pushTag("items", i);
                numberOfItems = user_itemsXML.getNumTags("item");

                user_items.assign(numberOfItems, string());

                for (int j = 0; j < numberOfItems; j++) {
                    user_itemsXML.pushTag("item", j);
                    string itemId = user_itemsXML.getValue("id", "");
                    // add to vector
                    user_items.push_back(itemId);

                    user_itemsXML.popTag(); // item
                }
            }
            user_itemsXML.popTag(); // items
            user_itemsXML.popTag(); // user_items
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
