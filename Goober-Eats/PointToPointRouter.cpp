#include "provided.h"
#include <list>
#include <queue>
#include <map>
#include <stack>
#include "ExpandableHashMap.h"
#include <unordered_map>
using namespace std;

class PointToPointRouterImpl
{
public:
    PointToPointRouterImpl(const StreetMap* sm);
    ~PointToPointRouterImpl();
    DeliveryResult generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const;
private:
    const StreetMap *mapdata;
    struct associationsMade
    {
        GeoCoord start;
        GeoCoord end;
        string name;
        associationsMade(GeoCoord g, GeoCoord h, string a) :name(a), start(g), end(h) { ; }
        associationsMade() { ; }
    };
	//structure made for easy linking of a certain start coordinate to its respective end geoocordinate and name
	//usefuk for tracing back path to the point after finding a path
};

PointToPointRouterImpl::PointToPointRouterImpl(const StreetMap* sm)
{
    mapdata = sm;
}

PointToPointRouterImpl::~PointToPointRouterImpl()
{
}

DeliveryResult PointToPointRouterImpl::generatePointToPointRoute(
    const GeoCoord& start,
    const GeoCoord& end,
    list<StreetSegment>& route,
    double& totalDistanceTravelled) const
{
    vector <StreetSegment> checker;
	//in case there are no street segments associated with the given start or ending positions
    if (mapdata->getSegmentsThatStartWith(start,checker) == false || mapdata->getSegmentsThatStartWith(end,checker) == false)
        return BAD_COORD;
	//special case wherein no travel is required
	if (start == end)
	{
		route.clear();
		totalDistanceTravelled = 0;
		return DELIVERY_SUCCESS;
	}

    GeoCoord current;
    queue <GeoCoord> coordQ;
    coordQ.push(start);
    ExpandableHashMap <GeoCoord, GeoCoord> iVisited; //to keep a track of all visited geoCoordinates
    iVisited.associate(start,start); //since start is the very first visited geoCoordinate
    bool isFound = false;
	ExpandableHashMap <GeoCoord, associationsMade> toFindAssociationsEasily; //stores all the associations betwwen a certain 
																			//geoocordinate and its associated name and end points
    while (!coordQ.empty())
    {
        current = coordQ.front();
        coordQ.pop();
        if (current == end)
        {
            isFound = true;
            break;
        }
        vector <StreetSegment> v;
        mapdata->getSegmentsThatStartWith(current, v);
        for (int i = 0; i < v.size(); i++)
        {
            if (iVisited.find(v[i].end)==nullptr) //evaluate a position only if it hasn't already been visited
            {
                coordQ.push(v[i].end);
                iVisited.associate(v[i].end,v[i].end);
                associationsMade a(v[i].end, current, v[i].name);//creating an object of type associationsMade 
																//so that it can stored in the vector
				toFindAssociationsEasily.associate(v[i].end, a); //making the association
            }
        }
    }
    if (isFound)
    {
        route.clear(); //clearing any junk value that may exist in the route
        stack <associationsMade> toMakeList; //using a stack to build the route back, since the last in streetSegment has to be first one in the route
		                                    //(LIFO)
        bool doneMakingStack = false;
        GeoCoord endSeg = end;
		associationsMade temp;
        while (!doneMakingStack)
        {
			temp = *toFindAssociationsEasily.find(endSeg);//finding the element associated with the endSeg position
			toMakeList.push(temp);
			endSeg = temp.end; //to trace the path back
			if (endSeg == start) //if we trace back and reach the start, then we are done!
			{
				doneMakingStack = true;
				break;
			}
        }
        double sum = 0;
        while (!toMakeList.empty()) //while the stack is not empty
        {
			//reversing the end and start in street segment constrcutor, since we have moved from the end position to the start posiiton
            StreetSegment s((toMakeList.top()).end, (toMakeList.top()).start, (toMakeList.top()).name);
            sum += distanceEarthMiles((toMakeList.top()).end, (toMakeList.top()).start);//adding up individual street segments' lengthss
            toMakeList.pop();//popping the element we just working with in the stack
            route.push_back(s);//building route...
        }
        totalDistanceTravelled = sum;
		
        return DELIVERY_SUCCESS;
		
    }
	
    return NO_ROUTE; //code only gets here if no route is generated
}




















   /* map<geoEndAndName, GeoCoord> m;
    queue <GeoCoord> placesToExplore;
    bool found = false;
    bool outOfPlaces = false;
    placesToExplore.push(start);
    while (!placesToExplore.empty())
    {
        if (placesToExplore.front() == end)
        {
            found = true;
            break;
        }
        vector <StreetSegment> v;
        mapdata->getSegmentsThatStartWith(placesToExplore.front(), v);
        for (int i = 0; i < v.size(); i++)
        {
            if (placesToExplore.front() == v[i].end)
                placesToExplore.pop();
            else
            {
                placesToExplore.push(v[i].end);

            }
        }
    }
     
    if (found)
    {

    }*/



//******************** PointToPointRouter functions ***************************

// These functions simply delegate to PointToPointRouterImpl's functions.
// You probably don't want to change any of this code.

PointToPointRouter::PointToPointRouter(const StreetMap* sm)
{
    m_impl = new PointToPointRouterImpl(sm);
}

PointToPointRouter::~PointToPointRouter()
{
    delete m_impl;
}

DeliveryResult PointToPointRouter::generatePointToPointRoute(
        const GeoCoord& start,
        const GeoCoord& end,
        list<StreetSegment>& route,
        double& totalDistanceTravelled) const
{
    return m_impl->generatePointToPointRoute(start, end, route, totalDistanceTravelled);
}
