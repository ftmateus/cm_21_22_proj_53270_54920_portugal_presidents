#pragma once
#include "President.h"

class ofAppData
{

	public :

		ofAppData() {
			isGeneratingMetadata = false;
			metadataGenerated = false;
			showingSearchPresidents = false;

			currentPresidentIdx = 0;
		};
		bool isGeneratingMetadata;

		bool metadataGenerated;

		map<string, vector<President *>> presidentsSearchIndex;


		map<int, President*> presidentsMedias;

		bool showingSearchPresidents;

		string currentSearchTerm;

		vector<President*> currentSearchResult;

		int currentPresidentIdx;

		std::mutex mutex;

		ofxXmlSettings presidentsXml;

		ofxXmlSettings presidentsMetadataXml;

		President* getPresidentByCarrouselPosition(int index)
		{
			if (showingSearchPresidents)
			{
				assert(&currentSearchTerm != NULL);
				assert(currentSearchTerm.length() > 0);
				assert(&currentSearchResult != NULL);
				return currentSearchResult[index];
			}
			else
				return presidentsMedias[index];
		}

		President* getPresidentByCurrentCarrouselPosition()
		{
			return getPresidentByCarrouselPosition(currentPresidentIdx);
		}

		int getCarrouselCurrentSize()
		{
			if (showingSearchPresidents)
			{
				assert(&currentSearchTerm != NULL);
				assert(currentSearchTerm.length() > 0);
				assert(&currentSearchResult != NULL);
				return currentSearchResult.size();
			}
			else
				return presidentsMedias.size();
		}
};

