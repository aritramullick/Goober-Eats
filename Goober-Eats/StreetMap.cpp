#include "provided.h"
#include <string>
#include <vector>
#include <functional>
#include <fstream>
#include <sstream>
#include "ExpandableHashMap.h"

using namespace std;

unsigned int hasher(const GeoCoord& g)
{
    return std::hash<string>()(g.latitudeText + g.longitudeText);
}

class StreetMapImpl
{
public:
    StreetMapImpl();
    ~StreetMapImpl();
    bool load(string mapFile);
    bool getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const;
private:
    ExpandableHashMap <GeoCoord,vector<StreetSegment>> map;
};

StreetMapImpl::StreetMapImpl()
{
}

StreetMapImpl::~StreetMapImpl()
{
}

bool StreetMapImpl::load(string mapFile)
{
    double numberOfSegments;
    string streetName;
    ifstream inf;
    int lineOfCoordinate=0;
    inf.open(mapFile);
    if (!inf)
        return false;
    string line;
    int lineNumber = 0; //initially at 0th line number 
	//opening the file and loading every line
    while (getline(inf, line))
    {
        istringstream iss(line);
        if (lineNumber == 0) //lineNumber of 0 correspons to the name of the street that we are on
        {
            streetName = line;
            lineNumber++;
            continue;
        }
        if (lineNumber == 1) //lineNumber of 1 coorespons to the number of streetsegments 
							//that are associated with a particular name of a street
        {
            iss >> numberOfSegments; //total number of segments for that street
            lineNumber++;
            lineOfCoordinate = 0; //the number of segments that we have succesfully evaluated till this point
            continue;
        }
        if (lineNumber > 1) //corresponds to receving only the streetsegments for the current street name
        {
            lineOfCoordinate++;//since we are evaluating a line of coordinates (a new street segment is evaluated)
            string start1, start2;
            string end1, end2;
            iss >> start1 >> start2 >> end1 >> end2; //getting the text that will help create the latitude and longitude texts to be 
													 //constructing geo coordinates for the starting and ending point of the street segment
            GeoCoord start(start1, start2);
            GeoCoord end(end1, end2);
            StreetSegment segmentStart(start, end, streetName); //street segment that moves from starting to end point
            StreetSegment segmentEnd(end, start, streetName); //street segment that moves from ending point to starting point

            if (map.find(start) == nullptr) //in the case wherein the start geoCoordinate doesn't exist as a key in the hashmap
            {
                vector <StreetSegment> v;
                v.push_back(segmentStart);
                map.associate(start, v); //new association is made
            }
            else if (map.find(start) != nullptr) //in the case wherein the end geocoordinate already exists as a key in the given hash map
            {
                vector <StreetSegment> v = *(map.find(start)); //finding that vector
                v.push_back(segmentStart); //adding the new street segment to that vector
                map.associate(start, v); //associating this new vector to the hash map
            }
			///////////////////////////////////////////////////////////////////////////////////////
			//REPEATING SAME THING AS ABOVE BUT WITH THE END COORDINATE AS THE KEY FOR THE HASHMAP
			///////////////////////////////////////////////////////////////////////////////////
            if (map.find(end) == nullptr)
            {
				vector <StreetSegment> v;
                v.push_back(segmentEnd);
                map.associate(end, v);
            }
            else if (map.find(end) != nullptr)
            {
                vector <StreetSegment> v = *(map.find(end));
                v.push_back(segmentEnd);
                map.associate(end, v);
            }
           
            if (lineOfCoordinate >= numberOfSegments) //if line of coordinate is greater than the total number of segments, 
														//it indicates that we are done with the current street name
													  //thus we reset lineNumber to 0 so that we can evaluate the name of the 
														//new street that is coming up
            {
               lineNumber = 0;   
            }
        }
        
    }
    return true;
}

bool StreetMapImpl::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
    if (map.find(gc) != nullptr)
    {
        segs = *(map.find(gc));
        return true;
    }
    else
    {
        return false;
    }
}

//******************** StreetMap functions ************************************

// These functions simply delegate to StreetMapImpl's functions.
// You probably don't want to change any of this code.

StreetMap::StreetMap()
{
    m_impl = new StreetMapImpl;
}

StreetMap::~StreetMap()
{
    delete m_impl;
}

bool StreetMap::load(string mapFile)
{
    return m_impl->load(mapFile);
}

bool StreetMap::getSegmentsThatStartWith(const GeoCoord& gc, vector<StreetSegment>& segs) const
{
   return m_impl->getSegmentsThatStartWith(gc, segs);
}
