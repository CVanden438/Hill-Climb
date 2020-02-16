// Hillclimb
// Uses hillclimb method to find optimal position for a distribution centre in Britain
// Added in : Weighs each city/town by population --- Finds the closest city/town to the position of the centre --- Reduces step size once it can't find a better position
// Author: Chris Vanden

//-------------------------------------------------------Includes------------------------------------------------------
#include <iostream>
#include <fstream>
#include <cmath>
#include <math.h>
#include <ctime>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
//---------------------------------------------------------------------------------------------------------------------

// Namespace Shortcut
using namespace std;

//---------------------------------------------------- Definitions-----------------------------------------------------
#define PI 3.14159265358979
// R used in equation later as radius of earth
#define R 3958.75 
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------------------Declaring Variables-------------------------------------------------
// Vectors to store the GBPlaces data
vector <double> temp;
vector< vector<double> > GBData;
vector <string> GBNames;
// Vectors to store specifically the lat, long and pop data
vector <double> latData, lonData;
vector <int> popData;
// The difference in lat and long between 2 points
double diffLat, diffLon;
// Used in Haversine formula
double a, c;
// Total distance between position and all cities/towns
double distTot = 0;
// Distance between 2 points and its inverse
double dist, invDist;
// Variable to skip first line of file
double z = 0;
// Stores the current position
double lon, lat;
// How far the position moves when chacking new positions
double step = 0.001;
// Stores the hill climb values
double value, oldValue, newValue;
// The final position after looping over hill climb
double lonFinal = 0.0, latFinal = 0.0;
// What direction the position will move after checking the points around it
double dirlon = 0, dirlat = 0;
// The best vlaue obtained after looping over hill climb
double globalMax = 0;
// Variabled using in reading in the data
double length, j, CommaPos, FirstCommaPos, k, tsize;
string Num, line, FirstNum;
string closest = "1";
// Used in changing the step size to prevent infinite loop
double x = 1;
//---------------------------------------------------------------------------------------------------------------------


//------------------------------------------------------Functions------------------------------------------------------
double fitness(double lon, double lat) {
	// Calculates the Great-Circle distance between 2 points using the Haversine formula
	for (double i = 0; i < GBData.size(); i++) {
		// Loops over every point to find the total distance between the current position and every city/town
		diffLat = lat - latData[i];
		diffLon = lon - lonData[i];
		a = pow(sin(diffLat / 2), 2) + cos(lat) * cos(latData[i]) * pow(sin(diffLon / 2), 2);
		c = 2 * atan2(sqrt(a), sqrt(1 - a));
		// Weights the distances by population
		dist = R * c * popData[i];
		distTot += dist;
	}
	// Inverts the distance as want to minimise it which is the same as maximising 1/distance
	invDist = 1 / distTot;
	return invDist;
}

double random_number(double upper, double lower, int n) {
	// Function to generate a random number between the 2 given bounds, split into n amounts
	double r;
	r = lower + (rand() % (n + 1) * (1. / n) * (upper - lower));
	return r;
}
//----------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------Main Program--------------------------------------------------------
int main() {

	// Seeds the random number generator so it doesn't give same set of numbers every time
	srand(static_cast<unsigned int>(time(NULL)));

	//--------------------------------------------Reading in GBPlaces---------------------------------------------------
	ifstream theFile("GBplaces.csv");
	// Opens the file
	if (theFile.is_open()) {
		// Loops through each line until the end of the file
		while (!theFile.eof()) {
			// Gets each line of the file and sets it to variable 'line'
			getline(theFile, line);
			if (z == 1) {
				// Adds the first number from each line to the 1D temp vector, this is done as each line doesn't start with a comma
				FirstCommaPos = line.find(',', 0);
				FirstNum = line.substr(0, FirstCommaPos);
				GBNames.push_back(FirstNum);
				// Loops through every character in each line
				for (int i = 0; i < line.length(); i++) {
					if (line[i] == ',') {
						// Gets here if character is a ','
						// Finds the next comma after the comma i is currently on
						CommaPos = line.find(',', i + 1);
						// Finds the length of the number between the 2 commas
						length = CommaPos - i + 1;
						// Sets the number to the variable Num
						Num = line.substr(i + 1, length);
						// Adds the number to the end of the 1D vctor
						temp.push_back(atof(Num.c_str()));
					}
				}
				// Gets here once it reaches the end of each line
				// Adds the 1D vector (which has all the numbers of the line sepereated into its elements) to the end of the 2D vector
				GBData.push_back(temp);
				// Finds how many elements are in each line (aka how many numbers in each line)
				tsize = temp.size();
				// Clears the 1D vector so the next line can now be stored in it without the previous line already being there
				temp.clear();
			}
			z = 1;
		}
		// Closes te file
		theFile.close();
	}
	else {
		// Gets here if it can't open the file
		cout << "Error\n";
		exit(1);
	}
	//------------------------------------------------------------------------------------------------------------------


	//----------------------------------- Adding Lat, Long and Pop to their own Matrix----------------------------------
	for (size_t i = 0; i < GBData.size(); i++) {
		// Loops over lattitude, longitude and population rows of matrix and adds them to seperate matrix
		latData.push_back(GBData[i][2]);
		lonData.push_back(GBData[i][3]);
		popData.push_back(GBData[i][1]);
		// Converts the lat and long values to radians for use in distance formula
		latData[i] = (PI / 180) * latData[i];
		lonData[i] = (PI / 180) * lonData[i];
	}
	//------------------------------------------------------------------------------------------------------------------


	//---------------------------------------------------Hill Climb-----------------------------------------------------
	// Does the hill climb method multiple times to account for it finding local maximums to give a value closer to the global max
	for (int k = 0; k < 5000; k++) {
		step = 0.001;
		// Assigns random starting point which is within britain
		lon = random_number(0.1, -0.1, 100);
		lat = random_number(1, 0.8, 100);

		// Finds the total distance to all towns/cities from that point
		value = fitness(lon, lat);
		// Sets the distance to 0 for the next time the function is called
		distTot = 0;

		// Loop to check every point around the current position to see if it can find a better one
		do {
			// Stores the current value
			oldValue = value;
			// Looks at a square of 8 points around the current position
			for (int i = -1; i <= 1; i++) {
				for (int j = -1; j <= 1; j++) {
					if (i == 0 && j == 0) {
						// Stops program from checking the point it is currently at to save it doing unnecessary calculations
					}
					else {
						// Calculates the total distance at each of the 8 points
						newValue = fitness(lon + step * i, lat + step * j);
						if (newValue >= value) {
							// If 1/total distance is greater than current point then stores which of the 8 points that is
							dirlon = i;
							dirlat = j;
							// Sets the new best value to the variable value so that if it finds a better position, it will now compare the other points to that new 
							// positions value
							value = newValue;
						}
					}
					// For each value of k, will only get here once
					// If it can't find a better value using the 0.001 step then it sets step to be 10x smaller
					if (value == oldValue) {
						if (x == 1) {
							step = 0.0001;
							i = -1;
							j = -1;
							x = 0;
						}
					}
				}
				// Changes the current position to the new one it just found
				lon += step * dirlon;
				lat += step * dirlat;


			}
			x = 1;
			// Will repeat the process until it reaches a position where no better position can be found
		} while (value > oldValue);

		// While looping over the k loop, stores the position and value of the local max obtained if it is better than the previous one
		if (value > globalMax) {
			globalMax = value;
			// Converts the latitudes and longitudes back into degrees
			lonFinal = lon;
			latFinal = lat;
		}
	}
	//------------------------------------------------------------------------------------------------------------------


	//-----------------------------------------------Finding closest city/town------------------------------------------
	dist = 0;
	double olddist = 0;
	for (double i = 0; i < GBData.size(); i++) {
		// Loops over every city/town and finds the dstance between the position and them
		diffLat = latFinal - latData[i];
		diffLon = lonFinal - lonData[i];
		a = pow(sin(diffLat / 2), 2) + cos(latFinal) * cos(latData[i]) * pow(sin(diffLon / 2), 2);
		c = 2 * atan2(sqrt(a), sqrt(1 - a));
		dist = R * c;
		invDist = 1 / dist;
		if (invDist > olddist) {
			olddist = invDist;
			// Whenever it finds a closer place it sets that as the current closest place
			closest = GBNames[i];
		}
	}
	// Converting lat and long back to degrees
	lonFinal = lonFinal * (180 / PI);
	latFinal = latFinal * (180 / PI);
	//------------------------------------------------------------------------------------------------------------------


	//---------------------------------------------Writing output to console--------------------------------------------
	cout << "After being weighted with the population of each town/city, the optimum location for a distribution centre is at a:\n";
	cout << "Lattitude of " << latFinal << " and a longitude of " << lonFinal << "\n";
	cout << "The closest city/town to this is " << closest << "\n";
	//------------------------------------------------------------------------------------------------------------------

	return 0;
}