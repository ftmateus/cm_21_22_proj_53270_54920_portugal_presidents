#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
#define METADATA_GENERATION_N_THREADS 1
#else //macos

#define METADATA_GENERATION_N_THREADS 1
#endif


#include "ofMain.h"
#include "ofApp.h"
#include <atomic>
#include "ofAppData.h"
using namespace cv;
using namespace ofxCv;

/// This is a simple example of a ThreadedObject created by extending ofThread.
/// It contains data (count) that will be accessed from within and outside the
/// thread and demonstrates several of the data protection mechanisms (aka
/// mutexes).
class GenerateMetadata : public ofThread
{
public:
    /// On destruction wait for the thread to finish
    /// so we don't destroy the pixels while they are
    /// being used. Otherwise we would crash
    ~GenerateMetadata() {
        stop();
        waitForThread(false);
    }

    ofAppData* appData;

    
    void setup(ofAppData* appData) {
        this->appData = appData;
        /*pixels.allocate(640, 480, OF_PIXELS_GRAY);
        tex.allocate(pixels);*/
        start();
    }

    /// Start the thread.
    void start() {
        startThread();
    }

    /// Signal the thread to stop.  After calling this method,
    /// isThreadRunning() will return false and the while loop will stop
    /// next time it has the chance to.
    /// In order for the thread to actually go out of the while loop
    /// we need to notify the condition, otherwise the thread will
    /// sleep there forever.
    /// We also lock the mutex so the notify_all call only happens
    /// once the thread is waiting. We lock the mutex during the
    /// whole while loop but when we call condition.wait, that
    /// unlocks the mutex which ensures that we'll only call
    /// stop and notify here once the condition is waiting
    void stop() {
        std::unique_lock<std::mutex> lck(mutex);
        stopThread();
        condition.notify_all();
    }

    /// Everything in this function will happen in a different
    /// thread which leaves the main thread completelty free for
    /// other tasks.
    void threadedFunction() {
        //while (isThreadRunning()) {
            //// since we are only writting to the frame number from one thread
            //// and there's no calculations that depend on it we can just write to
            //// it without locking
            //threadFrameNum++;

            //// Lock the mutex until the end of the block, until the closing }
            //// in which this variable is contained or we unlock it explicitly
            //std::unique_lock<std::mutex> lock(mutex);

            //// The mutex is now locked so we can modify
            //// the shared memory without problem
            //auto t = ofGetElapsedTimef();
            //for (auto line : pixels.getLines()) {
            //    auto x = 0;
            //    for (auto pixel : line.getPixels()) {
            //        auto ux = x / float(pixels.getWidth());
            //        auto uy = line.getLineNum() / float(pixels.getHeight());
            //        pixel[0] = ofNoise(ux, uy, t);
            //        x++;
            //    }
            //}

            //// Now we wait for the main thread to finish
            //// with this frame until we can generate a new one
            //// This sleeps the thread until the condition is signaled
            //// and unlocks the mutex so the main thread can lock it
            //condition.wait(lock);

            startMetadataGeneration();
        //}
    }

    void update() {
        //// if we didn't lock here we would see
        //// tearing as the thread would be updating
        //// the pixels while we upload them to the texture
        //std::unique_lock<std::mutex> lock(mutex);
        //tex.loadData(pixels);
        //condition.notify_all();
    }

    void updateNoLock() {
        //// we don't lock here so we will see
        //// tearing as the thread will update
        //// the pixels while we upload them to the texture
        //tex.loadData(pixels);
        //condition.notify_all();
    }

    /// This drawing function cannot be called from the thread itself because
    /// it includes OpenGL calls
    void draw() {
        //tex.draw(0, 0);
    }


    void startMetadataGeneration()
    {
        vector<std::thread> threads;

        const int n_threads = METADATA_GENERATION_N_THREADS;
        int n_presidents = appData->presidentsMedias.size();


        int presidents_per_thread = n_presidents / n_threads;

        appData->isGeneratingMetadata = true;

        for (int i = 0; i < n_presidents && n_threads > 1; i += presidents_per_thread) {
            int endPres = i + presidents_per_thread > n_presidents - 1 ? n_presidents - 1 : i + presidents_per_thread;

            std::thread _thread(&GenerateMetadata::generateMetadataThread, this, i, endPres);

            //_thread.join();

            threads.push_back(std::move(_thread));
        }

        if (n_threads == 1) generateMetadataThread(0, n_presidents - 1);

        for (int t = 0; t < threads.size(); t++)
        {
            threads[t].join();
        }

        for (int w = 0; w < appData->presidentsMedias.size(); w++)
        {
            PresidentMetadata* metadata = appData->presidentsMedias[w]->metadata;

            if (metadata == NULL)
            {
                assert(false);
                continue;
            }
            
            auto xml = appData->presidentsMetadataXml;

            xml.addTag("president");
            xml.pushTag("president", w); // mudar para numberOfItems + 1 depois de testar
            // saves the id - itemName
            xml.setValue("id", this->appData->presidentsMedias[w]->name, w);

            xml.setValue("luminance", metadata->luminance, w);
            xml.setValue("color", metadata->color, w);
            xml.setValue("texture", metadata->texture, w);
            xml.setValue("faces", metadata->faces, w);
            xml.setValue("edges", metadata->edges, w);


            xml.popTag(); // item

            xml.saveFile();
        }

        this->appData->isGeneratingMetadata = false;
        this->appData->metadataGenerated = true;

        stop();

    }
    
    void generateMetadataThread(int startPres, int endPres) {
        ofxCvHaarFinder finder;
        finder.setup("data_xml/haarcascade_frontalface_default.xml");

        for (int i = startPres; i <= endPres; i++)
            generateMetadata(appData->presidentsMedias[i], &finder);

    }

    void generateMetadata(President* president, ofxCvHaarFinder* finder) {
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

        if (president == NULL)
        {
            assert(false);
            return;
        }

        PresidentMetadata* metadata = new PresidentMetadata();
        //metadata->president = president;

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


        metadata->luminance = avgLuminance;
        metadata->color = newColor.getHue();


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
        string edges = edgesFilter(president);

        if (edges != "")
            metadata->edges = edges;
        //presidentsMetadataXml.setValue("edges", edges, president->pres_id);
    //// texture
        string texture = textureFilter(president);

        if (texture != "")
        {
            //presidentsMetadataXml.setValue("texture", texture, president->pres_id);
            metadata->texture = texture;
        }

        // rhythm
        /*if (isVideo) {
            double rhythm = rhythmFilter(path);
            presidentsXml.setValue("rhythm", rhythm, numPresidents);
        }*/

        //haarFinderMutex.lock();

        int faces = finder->findHaarObjects(*president->profilePicture);

        //presidentsMetadataXml.setValue("faces", faces, president->pres_id);
        metadata->faces = faces;

        appData->presidentsMedias[president->pres_id]->metadata = metadata;

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



protected:
    //// pixels represents shared data that we aim to always access from both the
    //// main thread AND this threaded object and at least from one of them for
    //// writing. Therefore, we need to protect it with the mutex.
    //// Otherwise it wouldn't make sense to lock.
    //ofFloatPixels pixels;

    //ofTexture tex;
    std::condition_variable condition;
    //int threadFrameNum = 0;
};
