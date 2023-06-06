#include <iostream>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include "gnuplot-iostream.h" // Include the gnuplot-iostream header

using namespace std;

const int NUM_FISH = 100; // number of fish in the school
const int NUM_ITERATIONS = 1000; // number of algorithm iterations
const double MIN_POSITION = -5.0; // minimum position
const double MAX_POSITION = 10.0; // maximum position
const double MIN_VELOCITY = -1.0; // minimum velocity
const double MAX_VELOCITY = 1.0; // maximum velocity
const double COEFF_INFLUENCE = 0.01; // coefficient of influence
const double COEFF_SOCIAL = 0.1; // social coefficient
const double FISH_WEIGHT_SCALE = 1.0; // fish weight scale factor
const double FISH_WEIGHT_INCREMENT = 0.01; // fish weight increment
const double FISH_VELOCITY_DECREMENT = 0.05; // fish velocity decrement
const double SCHOOL_RADIUS = 2.0; // school radius

struct Fish {
  double position[2]; // fish position in 2 dimensions
  double velocity[2]; // fish velocity in 2 dimensions
  double weight; // fish weight
  double prev_fitness; // previous fitness
};

double fitness(double x, double y) {
  // fitness function (in this example, the Rosenbrock benchmark function)
  return pow(1 - x, 2) + 100 * pow(y - x * x, 2);
}

double randomDouble(double min, double max) {
  // generate a random double number between min and max
  return min + (max - min) * rand() / RAND_MAX;
}

void feeding(Fish& fish) {
  double delta_weight = FISH_WEIGHT_INCREMENT * (fitness(fish.position[0], fish.position[1]) - fish.prev_fitness);
  fish.weight = max(FISH_WEIGHT_SCALE, fish.weight + delta_weight);
  fish.prev_fitness = fitness(fish.position[0], fish.position[1]);
}

Fish updateFish(Fish fish) {
  // update fish position and velocity
  Fish newFish = fish;
  newFish.position[0] = fish.position[0] + fish.velocity[0];
  newFish.position[1] = fish.position[1] + fish.velocity[1];
  newFish.velocity[0] = fish.velocity[0] + randomDouble(-COEFF_INFLUENCE, COEFF_INFLUENCE);
  newFish.velocity[1] = fish.velocity[1] + randomDouble(-COEFF_INFLUENCE, COEFF_INFLUENCE);
  
  // check if the new position is outside the bounds
  if (newFish.position[0] < MIN_POSITION || newFish.position[0] > MAX_POSITION
      || newFish.position[1] < MIN_POSITION || newFish.position[1] > MAX_POSITION) {
    newFish.velocity[0] *= -1; // reverse velocity if out of bounds
    newFish.velocity[1] *= -1;
  }
  
  // decrement fish velocity
  newFish.velocity[0] *= (1 - FISH_VELOCITY_DECREMENT);
  newFish.velocity[1] *= (1 - FISH_VELOCITY_DECREMENT);
  
  feeding(newFish); // feed the fish
  
  return newFish;
}

Fish updateSchool(Fish school[NUM_FISH]) {
  // update the fish school
  for (int i = 0; i < NUM_FISH; i++) {
    for (int j = 0; j < NUM_FISH; j++) {
      if (i == j) {
        continue;
      }
      // calculate the social influence of the fish
      double distance = sqrt(pow(school[i].position[0] - school[j].position[0], 2) + pow(school[i].position[1] - school[j].position[1], 2));
      if (distance < SCHOOL_RADIUS) { // within the school radius
        school[i].velocity[0] += COEFF_SOCIAL * (school[j].position[0] - school[i].position[0]) / school[j].weight;
        school[i].velocity[1] += COEFF_SOCIAL * (school[j].position[1] - school[i].position[1]) / school[j].weight;
      }
    }
    school[i] = updateFish(school[i]);
  }
  return *school;
}

int main() {
  // Seed the random number generator
  srand(time(NULL));

  Fish school[NUM_FISH];

  // Initialize the fish school
  for (int i = 0; i < NUM_FISH; i++) {
    school[i].position[0] = randomDouble(MIN_POSITION, MAX_POSITION);
    school[i].position[1] = randomDouble(MIN_POSITION, MAX_POSITION);
    school[i].velocity[0] = randomDouble(MIN_VELOCITY, MAX_VELOCITY);
    school[i].velocity[1] = randomDouble(MIN_VELOCITY, MAX_VELOCITY);
    school[i].weight = randomDouble(0.1, FISH_WEIGHT_SCALE);
    school[i].prev_fitness = fitness(school[i].position[0], school[i].position[1]);
  }

  Gnuplot gp("gnuplot-persist"); // Create the Gnuplot object

  //Set the output format to PNG or JPG
  gp << "set terminal pngcairo enhanced\n"; // for PNG format
  gp << "set output 'output.png'\n"; //Output file name for PNG

  // gp << "termnial jpeg enchanced\n"; // For JPG format
  // gp << "set output 'output.jpg'\n"; // Output file for JPG


  // Main loop
  for (int iter = 0; iter < NUM_ITERATIONS; iter++) {
    // Update the fish school
    school = updateSchool(school);

    // Plot the fish positions
    gp << "plot '-' with points pointtype 7 pointsize 1.5 lc variable\n";
    for (int i = 0; i < NUM_FISH; i++) {
      gp << school[i].position[0] << " " << school[i].position[1] << " " << school[i].weight << "\n";
    }
    gp << "e\n";
    gp.flush();

    // Delay for visualization
    usleep(100000);
  }

  return 0;
}
