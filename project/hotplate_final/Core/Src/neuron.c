#include "neuron.h"
#include "cli.h"
#include "MAX6675.h"  // thermocouple
#include "math.h"     // Sigmoid activation function

extern uint8_t target; // Hotplate target temperature

double sigmoid(double x) { // Sigmoid activation function
    return 1.0 / (1.0 + exp(-x));
}

double heatDuty() {
    double diff = target - getMAX6675(); // Input = set - now
    double weight = 0.0838;
    double bias = -3.78;
    double net_input = weight * diff + bias; // Magic
    double output = sigmoid(net_input); // Activation function
    return output * 100.0; // Percent duty output
}
