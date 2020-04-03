#include "provided.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include <utility>

using namespace std;



class DeliveryOptimizerImpl
{
public:
	DeliveryOptimizerImpl(const StreetMap* sm);
	~DeliveryOptimizerImpl();
	void optimizeDeliveryOrder(
		const GeoCoord& depot,
		vector<DeliveryRequest>& deliveries,
		double& oldCrowDistance,
		double& newCrowDistance) const;
private:
	//"inspired" by code from project 3 to generate seemingly random numbers
	inline
	int randInt(int min, int max) const
	{
		if (max < min)
			std::swap(max, min);
		static std::random_device rd;
		static std::default_random_engine generator(rd());
		std::uniform_int_distribution<> distro(min, max);
		return distro(generator);
	}

	void calcCrow(const GeoCoord depot, double &dist, const vector <DeliveryRequest> &deliveries) const
	{
		double totalSum = 0;
			for (int i = 0; i < deliveries.size() - 1; i++)
			{
				totalSum += distanceEarthMiles(deliveries[i].location, deliveries[i + 1].location);
			}
		totalSum += distanceEarthMiles(depot, deliveries[0].location);
		totalSum += distanceEarthMiles(deliveries[deliveries.size() - 1].location, depot);
		dist = totalSum;
	}

	void generateNewOrder( vector <DeliveryRequest>& oldOrder) const
	{
		
		int randIndex1 = randInt(0, oldOrder.size() - 1);
		int randIndex2 = randInt(0, oldOrder.size() - 1);
		//cerr << "\nMy random indices are : ";
		//cerr<< randIndex1 << " " << randIndex2 << endl;
		DeliveryRequest temp (oldOrder[randIndex1].item,oldOrder[randIndex1].location);
		oldOrder[randIndex1].item = oldOrder[randIndex2].item;
		oldOrder[randIndex1].location = oldOrder[randIndex2].location;
		oldOrder[randIndex2].item = temp.item;
		oldOrder[randIndex2].location = temp.location;
		//cerr << "\nTrying to generate a new order\n";

	}
};



DeliveryOptimizerImpl::DeliveryOptimizerImpl(const StreetMap* sm)
{
}

DeliveryOptimizerImpl::~DeliveryOptimizerImpl()
{
}

void DeliveryOptimizerImpl::optimizeDeliveryOrder(
	const GeoCoord& depot,
	vector<DeliveryRequest>& deliveries,
	double& oldCrowDistance,
	double& newCrowDistance) const
{
	vector <DeliveryRequest> temp;
	for (int j = 0; j < deliveries.size(); j++)
	{
		temp.push_back(deliveries[j]);
	}
	double temperature = 100000;
	double deltaDistance = 0;
	double coolingRate = 0.99999;
	double absoluteTemperature = 0.00001;
	double distance=0;
	calcCrow(depot, distance, deliveries);
	oldCrowDistance = distance;
	while (temperature > absoluteTemperature)

	{
		double randomNum = (double)randInt(0,1000);
		randomNum = ((double)randomNum / 1000.00);
		generateNewOrder(temp);
		double newDistance = 0;
		calcCrow(depot, newDistance, temp);
		deltaDistance = newDistance - distance;
		if (deltaDistance < 0 || ((distance > 0) &&
			(double)exp(-deltaDistance / temperature) > randomNum))
		{
			for (int i = 0;i<temp.size();i++)
				deliveries[i] = temp[i];
			distance = distance + deltaDistance;
		}
		else
			for (int i = 0; i < temp.size(); i++)
			{
				temp[i] = deliveries[i];
			}
			//cooling down
		temperature *= coolingRate;
	}

	calcCrow(depot, newCrowDistance, deliveries);
	cerr << "\nOld Crow Distance : " << oldCrowDistance;
	cerr << "\nNew crow distance : " << newCrowDistance;
	for (int i = 0; i < deliveries.size(); i++)
	{
		cerr << deliveries[i].item << endl;
	}
}
   // Delete these lines and implement this function correctly
    

//******************** DeliveryOptimizer functions ****************************

// These functions simply delegate to DeliveryOptimizerImpl's functions.
// You probably don't want to change any of this code.

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm)
{
    m_impl = new DeliveryOptimizerImpl(sm);
}

DeliveryOptimizer::~DeliveryOptimizer()
{
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(
        const GeoCoord& depot,
        vector<DeliveryRequest>& deliveries,
        double& oldCrowDistance,
        double& newCrowDistance) const
{
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
