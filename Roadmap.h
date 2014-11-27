#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <map>
#include <vector>
#include "RVO.h"


void setupScenario(RVO::RVOSimulator *sim);


void setupScenario2(RVO::RVOSimulator *sim);

void setupScenario3(RVO::RVOSimulator *sim);

void buildRoadmap(RVO::RVOSimulator *sim);

void updateVisualization(RVO::RVOSimulator *sim);

void setPreferredVelocities(RVO::RVOSimulator *sim);

bool reachedGoal(RVO::RVOSimulator *sim);