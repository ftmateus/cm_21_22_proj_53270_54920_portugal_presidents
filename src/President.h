#pragma once

#include <iostream>
#include "ofMain.h"

using namespace std;

class PresidentMetadata
{
	public:
		//President* president;
		float luminance;
		float color;
		int faces;
		double rhythm;
		string texture;
		string edges;
		ofImage* edgesProfilePicture;
		ofImage* textureProfilePicture;

};


class President
{
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
};

