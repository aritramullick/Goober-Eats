all: main

main: main.o DeliveryOptimizer.o DeliveryPlanner.o PointToPointRouter.o StreetMap.o ExpandableHashMap.h provided.h 
	g++ -o main main.o DeliveryOptimizer.o DeliveryPlanner.o PointToPointRouter.o StreetMap.o

main.o: main.cpp provided.h ExpandableHashMap.h
	g++ -c main.cpp

DeliveryOptimizer.o: DeliveryOptimizer.cpp provided.h ExpandableHashMap.h
	g++ -c DeliveryOptimizer.cpp

DeliveryPlanner.o: DeliveryPlanner.cpp provided.h ExpandableHashMap.h
	g++ -c DeliveryPlanner.cpp

PointToPointRouter.o: PointToPointRouter.cpp provided.h ExpandableHashMap.h
	g++ -c PointToPointRouter.cpp

StreetMap.o: StreetMap.cpp provided.h ExpandableHashMap.h
	g++ -c StreetMap.cpp

clean:
	rm *.o main