#include "provided.h"
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <random>
#include <utility>

using namespace std;

class DeliveryOptimizerImpl {
public:
	/**
	* Sets up deliveries array in the most optimal ordering (where we travel as little as possible)
	* @param depot Starting location of the deliveries to be made
	* @param deliveries Deliveries array in the ordering of the deliveries to be made
	* @param oldCrowDistance Initial distance of the ordering of the distances
	* @param newCrowDistance Final distance (based on ordering of the distances)
	*/
	void optimizeDeliveryOrder(const GeoCoord& depot, vector<DeliveryRequest>& deliveries, double& oldCrowDistance, double& newCrowDistance) const;
private:
	/**
	* Sets the dist variable to return an estimate of total distance of the trip with given ordering of deliveries
	*/
	void calcCrow(const GeoCoord depot, double &dist, const vector <DeliveryRequest> &deliveries) const {
		double totalSum = 0;
		int deliveriesLength = deliveries.size();
		for (int i = 0; i < deliveriesLength - 1; i++) 
			totalSum += distanceEarthMiles(deliveries[i].location, deliveries[i + 1].location);
		totalSum += distanceEarthMiles(depot, deliveries[0].location);
		totalSum += distanceEarthMiles(deliveries[deliveriesLength - 1].location, depot);
		dist = totalSum;
	}
	/**
	* Creates a new randomly generated ordering of deliveries by creating two random indices to swap the deliveries
	*/
	void generateNewOrder(vector <DeliveryRequest>& oldOrder) const {
		int randIndex1 = randInt(0, oldOrder.size() - 1);
		int randIndex2 = randInt(0, oldOrder.size() - 1);
		DeliveryRequest temp (oldOrder[randIndex1].item,oldOrder[randIndex1].location);
		oldOrder[randIndex1].item = oldOrder[randIndex2].item;
		oldOrder[randIndex1].location = oldOrder[randIndex2].location;
		oldOrder[randIndex2].item = temp.item;
		oldOrder[randIndex2].location = temp.location;
	}
};

void DeliveryOptimizerImpl::optimizeDeliveryOrder(const GeoCoord& depot, vector<DeliveryRequest>& deliveries, double& oldCrowDistance, double& newCrowDistance) const {
	vector <DeliveryRequest> currOrder;
	double distance=0;
	for (int j = 0; j < deliveries.size(); j++)
		currOrder.push_back(deliveries[j]);
	calcCrow(depot, distance, deliveries);
	oldCrowDistance = distance;
	// Implementing Simulated Annealing (based on concept from https://www.geeksforgeeks.org/simulated-annealing/)
	double temperature = 100000;
	double deltaDistance = 0;
	double coolingRate = 0.99999;
	double absoluteTemperature = 0.0001;
	while (temperature > absoluteTemperature) {
		// random number generation
		double randomNum = (double)randInt(0,1000);
		randomNum = ((double)randomNum / 1000.00);
		generateNewOrder(currOrder);
		double newDistance = 0;
		calcCrow(depot, newDistance, currOrder);
		deltaDistance = newDistance - distance;
		// either the newDistance is lower OR 
		// we accept a slightly worse solution in order to not get stuck in a local minima
		if (deltaDistance < 0 || ((distance > 0) &&
		(double)exp(-deltaDistance / temperature) > randomNum)) {
			for (int i = 0;i<currOrder.size();i++)
				deliveries[i] = currOrder[i];
			distance = distance + deltaDistance;
		}
		else
			for (int i = 0; i < currOrder.size(); i++)
				currOrder[i] = deliveries[i];
		//cooling down
		temperature *= coolingRate;
	}

	calcCrow(depot, newCrowDistance, deliveries);
	cerr<<"New Distance: "<<newCrowDistance<<endl;
	for (int i = 0; i < deliveries.size(); i++)
		cerr << deliveries[i].item << endl;
}

DeliveryOptimizer::DeliveryOptimizer(const StreetMap* sm) {
    m_impl = new DeliveryOptimizerImpl();
}

DeliveryOptimizer::~DeliveryOptimizer() {
    delete m_impl;
}

void DeliveryOptimizer::optimizeDeliveryOrder(const GeoCoord& depot, vector<DeliveryRequest>& deliveries, double& oldCrowDistance, double& newCrowDistance) const {
    return m_impl->optimizeDeliveryOrder(depot, deliveries, oldCrowDistance, newCrowDistance);
}
