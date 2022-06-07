#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
//define something for Windows (32-bit and 64-bit, this part is common)
#define METADATA_GENERATION_N_THREADS 1
#else //macos

#define METADATA_GENERATION_N_THREADS 1
#endif

#define RHYTHM_FILTER_N_IMAGES 5

#include "ofMain.h"
#include "ofApp.h"
#include <atomic>
#include "ofAppData.h"
#include "opencv2/highgui.hpp"
#include "opencv2/features2d.hpp"

using namespace cv;
using namespace ofxCv;
using cv::Mat;
using cv::Point2f;
using cv::KeyPoint;
using cv::Scalar;
using cv::BFMatcher;
using cv::FastFeatureDetector;
using cv::SimpleBlobDetector;

// This is a simple example of a ThreadedObject created by extending ofThread.
// It contains data (count) that will be accessed from within and outside the
// thread and demonstrates several of the data protection mechanisms (aka
// mutexes).
class GenerateMetadata : public ofThread {
    
    public:
        // On destruction wait for the thread to finish
        // so we don't destroy the pixels while they are
        // being used. Otherwise we would crash
        ~GenerateMetadata() {
            stop();
            waitForThread(false);
        }

        ofAppData* appData;

        ofImage objectImage;

        void setup(ofAppData* appData) {
            this->appData = appData;
            objectImage.load("images/object_image.jpg");
            /*pixels.allocate(640, 480, OF_PIXELS_GRAY);
             tex.allocate(pixels);*/
            start();
        }

        // Start the thread.
        void start() {
            startThread();
        }

        // Signal the thread to stop.  After calling this method,
        // isThreadRunning() will return false and the while loop will stop
        // next time it has the chance to.
        // In order for the thread to actually go out of the while loop
        // we need to notify the condition, otherwise the thread will
        // sleep there forever.
        // We also lock the mutex so the notify_all call only happens
        // once the thread is waiting. We lock the mutex during the
        // whole while loop but when we call condition.wait, that
        // unlocks the mutex which ensures that we'll only call
        // stop and notify here once the condition is waiting
        void stop() {
            std::unique_lock<std::mutex> lck(mutex);
            stopThread();
            condition.notify_all();
        }

        // Everything in this function will happen in a different
        // thread which leaves the main thread completelty free for
        // other tasks.
        void threadedFunction() {
            //while (isThreadRunning()) {
                //since we are only writting to the frame number from one thread
                // and there's no calculations that depend on it we can just write to
                // it without locking
                //threadFrameNum++;

                // Lock the mutex until the end of the block, until the closing }
                // in which this variable is contained or we unlock it explicitly
                //std::unique_lock<std::mutex> lock(mutex);

                // The mutex is now locked so we can modify
                // the shared memory without problem
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

                // Now we wait for the main thread to finish
                // with this frame until we can generate a new one
                // This sleeps the thread until the condition is signaled
                // and unlocks the mutex so the main thread can lock it
                //condition.wait(lock);
            startMetadataGeneration();
            //}
        }

        void update() {
            // if we didn't lock here we would see
            // tearing as the thread would be updating
            // the pixels while we upload them to the texture
            //std::unique_lock<std::mutex> lock(mutex);
            //tex.loadData(pixels);
            //condition.notify_all();
        }

        void updateNoLock() {
            // we don't lock here so we will see
            // tearing as the thread will update
            // the pixels while we upload them to the texture
            //tex.loadData(pixels);
            //condition.notify_all();
        }

        // This drawing function cannot be called from the thread itself because
        // it includes OpenGL calls
        void draw() {
            //tex.draw(0, 0);
        }

        void startMetadataGeneration() {
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
                threads[t].join();

        auto xml = appData->presidentsMetadataXml;

        xml.popTag();
        xml.removeTag("presidentsMetadata", 0);
        xml.clear();
        xml.save("data_xml/presidents_metadata.xml");

        xml.load("data_xml/presidents_metadata.xml");

        int presidentsMetadataNTags = xml.getNumTags("presidentsMetadata");
        assert(presidentsMetadataNTags == 0);

        xml.addTag("presidentsMetadata");
        xml.pushTag("presidentsMetadata");


        for (int w = 0; w < appData->presidentsMedias.size(); w++)
        {
            PresidentMetadata* metadata = appData->presidentsMedias[w]->metadata;

                if (metadata == NULL) {
                    assert(false);
                    continue;
                }
           
                xml.addTag("president");
                xml.pushTag("president", w);
            
                xml.setValue("id", this->appData->presidentsMedias[w]->name, w);
                xml.setValue("luminance", metadata->luminance, w);
                xml.setValue("color", metadata->color, w);
                xml.setValue("texture", metadata->texturePath, w);
                xml.setValue("faces", metadata->faces, w);
                xml.setValue("edges", metadata->edgesPath, w);
                xml.setValue("rhythm", metadata->rhythm, w);
                xml.setValue("objectTimes", metadata->objectTimes, w);

                xml.popTag();

                xml.saveFile();
            }

            xml.popTag();

            appData->isGeneratingMetadata = false;
            appData->metadataGenerated = true;

            stop();
        }
    
        void generateMetadataThread(int startPres, int endPres) {
            ofxCvHaarFinder finder;
            finder.setup("data_xml/haarcascade_frontalface_default.xml");

            for (int i = startPres; i <= endPres; i++)
                generateMetadata(appData->presidentsMedias[i], &finder);
        }

        void generateMetadata(President* president, ofxCvHaarFinder* finder) {
            if (president == NULL) {
                assert(false);
                return;
            }

            PresidentMetadata* metadata = appData->presidentsMedias[president->pres_id]->metadata;

            if (metadata == NULL)
                metadata = new PresidentMetadata();

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

            // edges - filter2D
            string edges = edgesFilter(president);
            
            metadata->edgesPath = edges;

            // textures
            string texture = textureFilter(president);
            
            metadata->texturePath = texture;

            // rhythm
            metadata->rhythm = rhythmFilter(president);

            //haarFinderMutex.lock();

            // number of faces
            int faces = finder->findHaarObjects(*president->profilePicture);

            //presidentsMetadataXml.setValue("faces", faces, president->pres_id);
            metadata->faces = faces;

            int objectTimes = objectTimesFilter(president);

            metadata->objectTimes = objectTimes;
        
            appData->presidentsMedias[president->pres_id]->metadata = metadata;

            //haarFinderMutex.unlock();
        }

        string edgesFilter(President* president) {
            double kernel_size;
            Mat kernel;
            kernel_size = 31;
            kernel = Mat::ones(kernel_size, kernel_size, CV_32F) / (kernel_size * kernel_size);

            Mat src = toCv(*president->profilePicture); // Load image
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

            string path = "textures_edges/" + std::to_string(president->pres_id) + "-edges.jpg";
            saveEdges.save(path);

            return path;
        }

        string textureFilter(President* president) {
            Mat src, dst;
            int kernel_size = 31;

            ofImage tmpImg = *president->profilePicture;

            tmpImg.setImageType(OF_IMAGE_GRAYSCALE);

            src = toCv(tmpImg); // Load an image

            double lambda = 1.0;
            double theta = 0;
            double psi = 0;
            double gamma = 0.02;
            double sigma = 0.56 * lambda;
            cv::Size matSize = cv::Size(kernel_size, kernel_size);

            while (theta <= 360.0) {
                Mat kernel = getGaborKernel(matSize, sigma, DegreesToRadians(theta), lambda, gamma, psi);
                filter2D(src, dst, CV_32F, kernel);

                //auto zeros = dst.

                theta += 45.0;
            }

            ofImage saveTexture;
            toOf(dst, saveTexture);

            string path = "textures_edges/" + std::to_string(president->pres_id) + "-texture.jpg";
            saveTexture.save(path);

            return path;
        }

        float DegreesToRadians(float degrees) {
            return degrees * (PI / 180);
        }

        double rhythmFilter(President* president) {
            ofVideoPlayer* video = president->biographyVideo;

            if (video == NULL) return 0.0;

            Mat src[RHYTHM_FILTER_N_IMAGES];
            Mat hsv[RHYTHM_FILTER_N_IMAGES];

            const float pct_offset = 1 / (RHYTHM_FILTER_N_IMAGES - 1);
            float pct = 0.0f;
            for (int i = 0; i < RHYTHM_FILTER_N_IMAGES; i++) {
                assert(pct <= 1);

                video->setPosition(pct);
                src[i] = toCv(video->getPixels());

                if (src[i].empty()) return -1;

                cvtColor(src[i], hsv[i], COLOR_BGR2HSV);

                pct += pct_offset;
            }

            Mat hsv_half_down = src[0](Range(hsv[0].rows / 2, hsv[0].rows), Range(0, hsv[0].cols));
            int h_bins = 50, s_bins = 60;
            int histSize[] = { h_bins, s_bins };

            // hue varies from 0 to 179, saturation from 0 to 255
            float h_ranges[] = { 0, 180 };
            float s_ranges[] = { 0, 256 };
            const float* ranges[] = { h_ranges, s_ranges };

            // Use the 0-th and 1-st channels
            int channels[] = { 0, 1 };

            Mat hist[RHYTHM_FILTER_N_IMAGES];

            for (int i = 0; i < RHYTHM_FILTER_N_IMAGES; i++) {
                calcHist(&hsv[i], 1, channels, Mat(), hist[i], 2, histSize, ranges, true, false);
                normalize(hist[i], hist[i], 0, 1, NORM_MINMAX, -1, Mat());
            }

            double rhythm = 0;
            for (int i = 1; i < RHYTHM_FILTER_N_IMAGES; i++)
                rhythm += compareHist(hist[0], hist[i], i);

            rhythm /= RHYTHM_FILTER_N_IMAGES;
            return rhythm;
        }

    int _objectTimesFilter(ofImage img)
    {
        int numberOfMatches = 0;
        img.setImageType(OF_IMAGE_GRAYSCALE);
        Mat img1 = toCv(img);
        objectImage.setImageType(OF_IMAGE_GRAYSCALE);
        Mat img2 = toCv(objectImage);

            if (!img1.empty() && !img2.empty()) {
                if (img1.channels() != 1)
                    cvtColor(img1, img1, cv::COLOR_RGB2GRAY);

                if (img2.channels() != 1)
                    cvtColor(img2, img2, cv::COLOR_RGB2GRAY);

                vector<KeyPoint> keyP1;
                vector<KeyPoint> keyP2;
                Mat desc1;
                Mat desc2;
                vector<cv::DMatch> matches;

                cv::Ptr<ORB> detector = ORB::create();
                detector->detectAndCompute(img1, Mat(), keyP1, desc1);
                detector->detectAndCompute(img2, Mat(), keyP2, desc2);
                matches.clear();

                BFMatcher bruteMatcher(cv::NORM_L1, true);
                bruteMatcher.match(desc1, desc2, matches, Mat());

            int k1s = keyP1.size();
            int k2s = keyP2.size();
            int ms = matches.size();
            float distances = 0;
            for (int j = 0; j < matches.size(); j++) {
                distances += matches[j].distance;
            }
            float distanceAvg = distances / ms;
            for (int j = 0; j < matches.size(); j++) {
                if (matches[j].distance < (distances / (ms * 2)))
                    numberOfMatches++;
            }

            /*if (distanceAvg < (distances / ms*2)) {
                numberOfMatches = 1;
            }*/
        }
        return numberOfMatches;
    }

    int objectTimesFilter(President *president) {

        int numberOfMatches = 0;

        numberOfMatches += _objectTimesFilter(*(president->profilePicture));

        /*if (president->biographyVideo != NULL)
        {
            assert(president->biographyVideoPath.length() > 0);
            assert(president->biographyVideo->getMoviePath() == "videos/" + president->biographyVideoPath);
            ofVideoPlayer tempVid = *(president->biographyVideo);
            int total_frames = tempVid.getTotalNumFrames();

            for (int f = 0; f < total_frames; f++)
            {
                tempVid.setFrame(f);
                assert(tempVid.getCurrentFrame() <= f);
                assert(tempVid.getCurrentFrame() >= f - 1);
                numberOfMatches += _objectTimesFilter(tempVid.getPixels()); 
            }
        }*/

        return numberOfMatches;
    }

    protected:
        // pixels represents shared data that we aim to always access from both the
        // main thread AND this threaded object and at least from one of them for
        // writing. Therefore, we need to protect it with the mutex.
        // Otherwise it wouldn't make sense to lock.
        //ofFloatPixels pixels;

        //ofTexture tex;
        std::condition_variable condition;
        //int threadFrameNum = 0;
};
