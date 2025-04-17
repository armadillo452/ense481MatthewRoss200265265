/**
 * @file neuron.h
 * @brief Neural computation functions for heater control.
 *
 * This header file provides declarations for functions that implement a simple
 * neuron model for controlling a heater. The model uses a sigmoid activation
 * function and computes a duty cycle value based on the difference between
 * a target temperature and the current measured temperature.
 */

#ifndef NEURON_H
#define NEURON_H

/**
 * @brief Computes the sigmoid activation function.
 *
 * This function calculates the sigmoid of the input value x. The sigmoid
 * function is defined as:
 *
 *     sigmoid(x) = 1 / (1 + exp(-x))
 *
 * It is used here as the activation function in the single-neuron model.
 *
 * @param x The input value to the sigmoid function.
 * @return The computed sigmoid result.
 */
double sigmoid(double x);

/**
 * @brief Calculates the heater duty cycle.
 *
 * This function implements a simple single-layer neural network using the
 * sigmoid function. It calculates the difference between the target temperature
 * and the current temperature (obtained via a thermocouple through getMAX6675()),
 * applies a weight and bias to this difference, and passes the result through
 * the sigmoid activation function. The final output is scaled to a percentage
 * value representing the heater duty cycle.
 *
 * @return The heater duty cycle as a percentage (0.0 to 100.0).
 */
double heatDuty(void);

#endif
