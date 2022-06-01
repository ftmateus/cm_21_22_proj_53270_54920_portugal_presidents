#include "Metadata.h"
#include <iostream>
#include "ofMain.h"

//using namespace Metadata;

void Metadata::filterItems(string filter) {
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

void Metadata::filterByColor(float hue) {
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

