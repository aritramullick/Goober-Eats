#include "provided.h"
#include <vector>
#include <list>
using namespace std;

class DeliveryPlannerImpl
{
public:
	DeliveryPlannerImpl(const StreetMap* sm);
	~DeliveryPlannerImpl();
	DeliveryResult generateDeliveryPlan(
		const GeoCoord& depot,
		const vector<DeliveryRequest>& deliveries,
		vector<DeliveryCommand>& commands,
		double& totalDistanceTravelled) const;
private:
	const StreetMap* mapdata;
	PointToPointRouter p2p;
	string direction(double angle) const //function to convert angles into a string of directions
	{
		if (angle >= 0 && angle < 22.5)
		{
			return "east";
		}
		if (angle >= 22.5 && angle < 67.5)
		{
			return "northeast";
		}
		if (angle >= 67.5 && angle < 112.5)
		{
			return "north";
		}
		if (angle >= 112.5 && angle < 157.5)
		{
			return "northwest";
		}
		if (angle >= 157.5 && angle < 202.5)
		{
			return "west";
		}
		if (angle >= 202.5 && angle < 247.5)
		{
			return "southwest";
		}
		if (angle >= 247.5 && angle < 292.5)
		{
			return "south";
		}
		if (angle >= 292.5 && angle < 337.5)
		{
			return "southeast";
		}
		if (angle >= 337.5)
		{
			return "east";
		}
		return "";
	}
};

DeliveryPlannerImpl::DeliveryPlannerImpl(const StreetMap* sm) : p2p(sm)
{
	mapdata = sm;
}

DeliveryPlannerImpl::~DeliveryPlannerImpl()
{
}

DeliveryResult DeliveryPlannerImpl::generateDeliveryPlan(
	const GeoCoord& depot,
	const vector<DeliveryRequest>& deliveries,
	vector<DeliveryCommand>& commands,
	double& totalDistanceTravelled) const
{
	double crow1 = 0, crow2 = 0;
	DeliveryOptimizer optimize (mapdata);
	vector <DeliveryRequest> optimisedDelivery;//a new vector with the deliveries in the correct optimised order
	for (int i = 0; i < deliveries.size(); i++)
		optimisedDelivery.push_back(deliveries[i]);//copying all values of deliveries to optimisedDelivery

	optimize.optimizeDeliveryOrder(depot, optimisedDelivery, crow1, crow2);//optimizing...

	vector <bool> deliverable;//to ensure that only goods that are deliverable are delivered
	GeoCoord current = depot;
	list <StreetSegment> totalRoute;
	list <StreetSegment> route;
	double distance;
	bool Isdelivery;
	double mySum = 0; //for the sum of all distances
	for (int i = 0; i < optimisedDelivery.size(); i++)
	{
		DeliveryResult res = p2p.generatePointToPointRoute(current, optimisedDelivery[i].location, route, distance);
		if (res == BAD_COORD || res == NO_ROUTE)
		{
			//since delivery is not possible, pushing a false value into the boolean vector deliverable
			Isdelivery = false;
			deliverable.push_back(Isdelivery);
			continue;
		}
		mySum += distance;
		//adding all street segments to the totalRoute list
		for (auto p = route.begin(); p != route.end(); p++)
			totalRoute.push_back(*p);
		//since delivery IS possible, pushing a true value into the boolean vector deliverable
		Isdelivery = true;
		deliverable.push_back(Isdelivery);
		current = optimisedDelivery[i].location;
	}

	//finding a vector of street segments that are linked with the depot for use later
	//////////////////////////////////////////////////////////////
	vector <StreetSegment> segs;
	mapdata->getSegmentsThatStartWith(depot, segs);
	///////////////////////////////////////////////////////////////

	//SPECIAL CASE WHEREIN ALL DELIVERIES ARE AT THE DEPOT ITSELF
	DeliveryCommand d1, d2;
	if (totalRoute.empty())
	{
		for (int i = 0; i<optimisedDelivery.size(); i++)
			if (optimisedDelivery[i].location == depot)
			{
				d1.initAsDeliverCommand(optimisedDelivery[i].item);
				commands.push_back(d1);
			}
		return DELIVERY_SUCCESS;
	}
	///////////////////////////////////////////////////////
	//BRINGING BACK THE USER TO THE DEPOT AND ADDING THE ROUTE FOR THE SAME
	route.clear();
	p2p.generatePointToPointRoute(current, depot, route, distance);
	mySum += distance;
	totalDistanceTravelled = mySum;
	for (auto p = route.begin(); p != route.end(); p++)
		totalRoute.push_back(*p);
	///////////////////////////////////////////////////////

	//adding an extra segment that starts with the depot onto the totalRoute list, 
	//so that the loop can break out when the start position is equal to the depot location
	totalRoute.push_back(segs[0]);
	double distanceInOneStreet = 0;
	/////////////////////////////////////////////////////

	int j = 0;
	string streetName;
	StreetSegment s1, s2, s3;
	list <StreetSegment>::iterator it;
	
	for (it = totalRoute.begin(); it != totalRoute.end(); it++)
	{
		//initially, street name is set to the very first street segment's name
		if (it == totalRoute.begin())
		{
			streetName = (*it).name;
			s1 = (*it);
		}
		//adding the distance of the current street segment to
		//the distance travelled on our current street
		distanceInOneStreet += distanceEarthMiles((*it).start, (*it).end);
		while (!deliverable[j])//ensuring we do not deliver something that cannot be delivered
			j++;
		auto x = it;
		x++;
		// for delivery                                   or if we are at the depot to end journey
		if ((*it).start == optimisedDelivery[j].location ||(it->start==depot && x == totalRoute.end()))
		{
			double theta;
			theta = angleOfLine(s1); //angle of first street segment of the street

	 //subtracting the current street segment's path since we do not cover this distance before delivering
			distanceInOneStreet -= distanceEarthMiles((*it).start, (*it).end); 
			if (distanceInOneStreet != 0)
			{
				d1.initAsProceedCommand(direction(theta), s1.name, distanceInOneStreet);
				commands.push_back(d1);
			}
	//reinitaliasing the value of distanceInOneStreet to be the current street segment's distance 
	//which would otherwise be skipped if we reset the distnce to 0
			distanceInOneStreet = distanceEarthMiles((*it).start, (*it).end);

			if (j < optimisedDelivery.size())
			{
				while (optimisedDelivery[j].location == (*it).start && deliverable[j])
				{
						
						d1.initAsDeliverCommand(optimisedDelivery[j].item);
						commands.push_back(d1);
						j++;
						
					if (j >= optimisedDelivery.size())
						break;
				}
			}
			s1 = *it;//we are now on a new street (or rather, same street but we treat it as a new one since we just 
					//delivered at a location, and it is as if we restart the journey fron this point) 
					//and we would need to provide directions accordingly
			streetName = s1.name;
			continue;
		}

		//when we encounter a new street name, we have a proceed command and a turn command

		if ((*it).name != streetName)
		{
			//subtracting current street segment
			distanceInOneStreet = distanceInOneStreet - distanceEarthMiles((*it).start, (*it).end);

			streetName = (*it).name;
			it--;
			s2 = (*it); //the preceding street
			it++;
			s3 = (*it); //the current street

			double theta = angleOfLine(s1);
			//proceed
			d2.initAsProceedCommand(direction(theta), s1.name, distanceInOneStreet);
			commands.push_back(d2);
			
			s1 = s3;

			//reinitalising the distance
			distanceInOneStreet = distanceEarthMiles((*it).start, (*it).end);

			//turn
				double angle = angleBetween2Lines(s2, s3); //angle between current and previous segments

				if (angle > 359 || angle < 1)
				{
					continue;
				}
				if (angle >= 1 && angle < 180)
				{
					d1.initAsTurnCommand("left", s3.name);
					commands.push_back(d1);
				}
				if (angle >= 180 && angle <= 359)
				{
					d1.initAsTurnCommand("right", s3.name);
					commands.push_back(d1);
				}
		}

		
	}

	return DELIVERY_SUCCESS;
}

//******************** DeliveryPlanner functions ******************************

// These functions simply delegate to DeliveryPlannerImpl's functions.
// You probably don't want to change any of this code.

DeliveryPlanner::DeliveryPlanner(const StreetMap* sm)
{
	m_impl = new DeliveryPlannerImpl(sm);
}

DeliveryPlanner::~DeliveryPlanner()
{
	delete m_impl;
}

DeliveryResult DeliveryPlanner::generateDeliveryPlan(
	const GeoCoord& depot,
	const vector<DeliveryRequest>& deliveries,
	vector<DeliveryCommand>& commands,
	double& totalDistanceTravelled) const
{
	return m_impl->generateDeliveryPlan(depot, deliveries, commands, totalDistanceTravelled);
}
