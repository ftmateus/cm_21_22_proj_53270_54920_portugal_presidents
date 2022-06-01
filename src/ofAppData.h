#pragma once

class ofAppData
{

	public :

		ofAppData() {
			isGeneratingMetadata = false;
			metadataGenerated = false;
			currentPresidentIdx = 0;
		};
		bool isGeneratingMetadata;

		bool metadataGenerated;

		map<int, President*> presidentsMedias;

		int currentPresidentIdx;

		std::mutex mutex;

		ofxXmlSettings presidentsXml;

		ofxXmlSettings presidentsMetadataXml;

};

