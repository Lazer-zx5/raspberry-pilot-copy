#include <Arduino.h>
#include "carstate.hpp"

// put function declarations here:
int myFunction(int, int);

CarState CS;

void setup() {
	// put your setup code here, to run once:
	int result = myFunction(2, 3);
}

void loop() {
	// put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
	return x + y;
}
