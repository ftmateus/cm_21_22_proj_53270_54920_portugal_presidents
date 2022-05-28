#pragma once

#include "ofMain.h"

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
