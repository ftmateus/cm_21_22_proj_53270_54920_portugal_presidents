#include "Metadata.h"
#include "ofApp.h"
#include "President.h"
#include "GenerateMetadataThreadWork.h"

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
#define METADATA_GENERATION_N_THREADS 1
#else //macos

#define METADATA_GENERATION_N_THREADS 1
#endif

namespace Metadata
{
    void startMetadataGeneration(ofApp* app)
    {
        vector<thread> threads;

        int n_presidents = app->presidentsMedias.size();

        int presidents_per_thread = n_presidents / METADATA_GENERATION_N_THREADS;

        app->isGeneratingMetadata = true;

        for (int i = 0; i < n_presidents && METADATA_GENERATION_N_THREADS > 1; i += presidents_per_thread) {
            int endPres = i + presidents_per_thread > n_presidents - 1 ? n_presidents - 1 : i + presidents_per_thread;

            std::thread _thread(generateMetadataThread, app, i, endPres);

            //_thread.join();

            threads.push_back(std::move(_thread));
        }

        if (METADATA_GENERATION_N_THREADS == 1) generateMetadataThread(app, 0, n_presidents - 1);

        for (int t = 0; t < threads.size(); t++)
        {
            threads[t].join();
        }

        for (int w = 0; w < app->threadsWork.size(); w++)
        {
            GenerateMetadataThreadWork* work = app->threadsWork[w];

            app->presidentsMetadataXml.addTag("president");
            app->presidentsMetadataXml.pushTag("president", w); // mudar para numberOfItems + 1 depois de testar
            // saves the id - itemName
            app->presidentsMetadataXml.setValue("id", work->president->name, w);

            app->presidentsMetadataXml.setValue("luminance", work->luminance, w);
            app->presidentsMetadataXml.setValue("color", work->color, w);
            app->presidentsMetadataXml.setValue("texture", work->texture, w);
            app->presidentsMetadataXml.setValue("faces", work->faces, w);
            app->presidentsMetadataXml.setValue("edges", work->edges, w);
            app->presidentsMetadataXml.setValue("rhythm", work->rhythm, w);

            app->presidentsMetadataXml.popTag(); // item

            app->presidentsMetadataXml.saveFile();
        }

        app->isGeneratingMetadata = false;
    }


    void generateMetadataThread(ofApp* app, int startPres, int endPres) {
        ofxCvHaarFinder finder;
        finder.setup("data_xml/haarcascade_frontalface_default.xml");

        for (int i = startPres; i <= endPres; i++)
            generateMetadata(app, app->presidentsMedias[i], &finder);

    }

    void generateMetadata(ofApp* app, President* president, ofxCvHaarFinder* finder) {
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

        //// texture
        work->texture = textureFilter(president);

        // rhythm
        if (president->biographyVideo != NULL) {
            work->rhythm = rhythmFilter(president->biographyVideo);
        }

        int faces = finder->findHaarObjects(*president->profilePicture);

        //presidentsMetadataXml.setValue("faces", faces, president->pres_id);
        work->faces = faces;

        app->threadsWork[president->pres_id] = work;

        //haarFinderMutex.unlock();

    }


    string edgesFilter(President* president) {
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

    string textureFilter(President* president) {
        Mat src, dst;
        int kernel_size = 31;

        src = toCv(*president->profilePicture); // Load an image

        double lambda = 1.0;
        double theta = 0;
        double psi = 0;
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

    double rhythmFilter(ofVideoPlayer* video)
    {
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
    }

    void filterItems(ofApp* app, string filter) {
        // if filter is empty, items = auxItems
        remove(filter.begin(), filter.end(), ' '); // trim
        if (filter == "") {
            (void)ofLog(OF_LOG_NOTICE, "Items = AuxItems!");
            app->items.clear();

            int size = app->auxItems.size();
            app->items.resize(size);
            app->itemsSize = size;
            app->items = app->auxItems;

            return;
        }

        // filter items
        vector<Item*> filteredItems;
        int counter = 0;
        int numPresidents = app->presidentsXml.getNumTags("president");

        for (int i = 0; i < numPresidents; i++) {
            bool wasAdded = false;
            // tags
            app->presidentsXml.pushTag("president", i);
            app->presidentsXml.pushTag("tags");

            int numTags = app->presidentsXml.getNumTags("tag");
            for (int j = 0; j < numTags; j++) {
                string tag = app->presidentsXml.getValue("tag", "", j);

                if (tag.find(filter) != std::string::npos) { // add this item
                    filteredItems.push_back(app->auxItems[i]);
                    counter++;
                    wasAdded = true;
                    break;
                }
            }
            app->presidentsXml.popTag(); // tags
            // so the same item isnt added twice
            if (!wasAdded) {
                // times
                app->presidentsXml.pushTag("times");

                int numTimes = app->presidentsXml.getNumTags("time");
                for (int j = 0; j < numTimes; j++) {
                    app->presidentsXml.pushTag("time", j);
                    string name = app->presidentsXml.getValue("name", "");

                    if (name.find(filter) != std::string::npos) { // add this item
                        filteredItems.push_back(app->auxItems[i]);
                        counter++;
                        break;
                    }
                }
                app->presidentsXml.popTag(); // times
            }

            app->presidentsXml.popTag(); // item
        }
        // items = filteredItems
        app->items.clear();
        app->items.resize(counter);
        app->itemsSize = counter;
        app->items = filteredItems;
    }


    void filterByColor(ofApp* app, float hue) {
        vector<Item*> filteredItems;
        int counter = 0;
        int numPresidents = app->presidentsXml.getNumTags("president");

        for (int i = 0; i < numPresidents; i++) {
            app->presidentsXml.pushTag("president", i);
            float color = app->presidentsXml.getValue("color", 0);
            if (abs(color - hue) <= 10 || abs(color - hue) >= 245) {
                // this item will apear
                filteredItems.push_back(app->auxItems[i]);
                counter++;
            }
            app->presidentsXml.popTag(); // item
        }
        // items = filteredItems
        app->items.clear();
        app->items.resize(counter);
        app->itemsSize = counter;
        app->items = filteredItems;
    }

    void importMetadata(ofApp* app, int index) {
        //int index = e.target->getIndex();
        ofImage auxImg = app->items[index]->getImage();
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

        app->presidentsMetadataXml.pushTag("item", index);
        if (app->presidentsMetadataXml.getNumTags("tags") == 0)
            app->presidentsMetadataXml.addTag("tags");

        app->presidentsMetadataXml.pushTag("tags");
        int numExTags = app->presidentsMetadataXml.getNumTags("tag"); // number of existing tags

        for (int j = 0; j < numberOfTags; j++) {
            app->presidentsMetadataXml.addValue("tag", listTags[j]);
        }
        app->presidentsMetadataXml.popTag(); // tags

        if (app->presidentsMetadataXml.getNumTags("times") == 0)
            app->presidentsMetadataXml.addTag("times");

        app->presidentsMetadataXml.pushTag("times");
        int numExTimes = app->presidentsMetadataXml.getNumTags("time"); // number of existing times

        int j = numExTimes;
        for (map<string, int>::iterator itr = mapTimes.begin(); itr != mapTimes.end(); ++itr) {
            bool found = false;

            int numTimesTag = app->presidentsMetadataXml.getNumTags("time");
            for (int i = 0; i < numTimesTag; i++) {
                app->presidentsMetadataXml.pushTag("time", i);
                if (app->presidentsMetadataXml.getValue("name", "") == itr->first)
                    found = true;

                app->presidentsMetadataXml.popTag(); // time
            }

            if (!found) {
                app->presidentsMetadataXml.addTag("time");
                app->presidentsMetadataXml.pushTag("time", j);

                app->presidentsMetadataXml.addValue("name", itr->first);
                app->presidentsMetadataXml.addValue("numTime", itr->second);

                app->presidentsMetadataXml.popTag(); // time
            }
            j++;
        }
        app->presidentsMetadataXml.popTag(); // times

        app->presidentsMetadataXml.popTag(); // item

        // Saves file
        if (app->presidentsMetadataXml.saveFile())
            (void)ofLog(OF_LOG_NOTICE, "Saved!");
        else
            (void)ofLog(OF_LOG_NOTICE, "Didn't save!");
    }
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





