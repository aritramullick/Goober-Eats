# Goober-Eats
A UCLA CS32 Project that involves developing a program that mimics something like Google Maps.
The program takes in the locations where a person must deliver food, and outputs the directions that the person must take to get to that point -- similar to Google Maps since it provides directions on how to get from point A to point B.
Makes use of the following concepts
1. Self-defined HashMaps
2. Loading streetmap data to `main.cpp` file
3. Using BFS to find path from point A to point B with given data
4. Simulated Annealing for optimisation of the delivery routes

## Requirements

1. g++ compiler   : To run Makefile on Unix based environment
2. mapdata.txt    : Saved streetmap data of Westwood neighborhood
3. deliveries.txt : Depot co-ordinates with list of deliveries (with co-ordinates) to be made 
   
## File Purposes

### `.cpp` Files

1. `DeliveryOptimizer.cpp` : Simulated Annealing for optimizing order of deliveries (Solving Travelling Salesman Problem)
2. `DeliveryPlanner.cpp`   : Converts path of street segments into string of commands to be understood by user
3. `StreetMap.cpp`         : Receives input from `mapdata.txt` to convert into arrays of street segments that can be used in C++ code
4. `PointToPointRouter.cpp`: BFS to find path from point A to point B
5. `main.cpp`              : Driver code

### `.h` Files

1. `ExpandableHashMap.h` : Self-defined hashmap for use 
2. `provided.h`          : Given miscellaneous helper functions to generate random numbers, find distances with given latitudes and longitutes of 2 points, etc.

### Other Files

1. `Makefile`: To generate object files and the `main` executable

## Usage

Use the below lines on CLI in order to run sample test case.

```
cd Goober-Eats
make all
./main  mapdata.txt deliveries.txt
```

In order to clean up the folder to have just the `.cpp` and `.h` files
```
make clean
```
