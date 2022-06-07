#pragma once

#include <iostream>
#include "ofMain.h"

using namespace std;

class TextureProperties {
	public:
		int zeroes;
};

class PresidentMetadata {
	public:
		//President* president;
		float luminance;
		float color;
		int faces;
		int objectTimes;
		double rhythm;
		string texturePath;
		string edgesPath;
		ofImage* edgesProfilePicture;
		ofImage* textureProfilePicture;
};

class President {
	public: 
		string name;
		int pres_id;
		string startDate;
		string endDate;
		string birthDate;
		string deathDate;
		ofImage* profilePicture;
		string profilePicturePath;
		ofVideoPlayer* biographyVideo;
		string biographyVideoPath;
		vector<ofImage*> otherImages;
		PresidentMetadata *metadata;
		vector<string> tags;
};
