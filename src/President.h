#pragma once

#include <iostream>
#include "ofMain.h"

using namespace std;

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

};
