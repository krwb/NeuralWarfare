#pragma once
#include "NeuralNetwork.h"
#include <cmath>

// Add Function
// Behavior: Simply returns the sum of the nodes inputs
// Use case: debugging and testing;
class AddFunction : public ActivationFunction
{
public:
	AddFunction() {
		name = "basicAdd";
	};
	~AddFunction() {};
	double operator()(double in) override {
		return in;
	};
private:

};

// Step Function
// Behavior: The Step Function activates with 1 if the sum of inputs exceeds a specified threshold, otherwise activates with 0. It's a binary decision maker.
// Use case: Useful for binary choices, like simple classifiers or rule - based systems where a clear threshold determines activation.
class StepFunction : public ActivationFunction
{
public:
	double threshold = 0;
	StepFunction() {
		name = "step";
	};
	~StepFunction() {};
	double operator()(double in) override {
		if (in > threshold) {
			return 1;
		}
		else {
			return 0;
		}
	};
private:

};

// Sigmoid Function
// Behavior: The Sigmoid function squashes input values to a range between 0 and 1, making it suitable for binary classification problems.It is smooth and differentiable, making it useful for gradient - based optimization algorithms.
// Use case: Binary classification problems, output layer of a neural network for probability estimation.
class SigmoidFunction : public ActivationFunction
{
public:
	SigmoidFunction() {
		name = "sigmoid";
	}

	~SigmoidFunction() {}

	double operator()(double in) override {
		return 1.0 / (1.0 + std::exp(-in));
	}
};

// Hyperbolic Tangent (Tanh) Function
// Behavior: Similar to the sigmoid function, the Tanh function squashes input values, but it ranges between - 1 and 1. It is often preferred in hidden layers for mitigating the vanishing gradient problem.
// Use case: Hidden layers of a neural network, especially in recurrent neural networks(RNNs).
class TanhFunction : public ActivationFunction
{
public:
	TanhFunction() {
		name = "tanh";
	}

	~TanhFunction() {}

	double operator()(double in) override {
		return tanh(in);
	}
};

// Rectified Linear Unit (ReLU) Function
// Behavior: The ReLU function outputs the input as - is if positive, and zero otherwise. It is widely used in hidden layers and has been shown to accelerate convergence during training.
// Use case: Hidden layers of a neural network, especially in deep neural networks.
class ReLUFunction : public ActivationFunction
{
public:
	ReLUFunction() {
		name = "relu";
	}

	~ReLUFunction() {}

	double operator()(double in) override {
		return std::max(0.0, in);
	}
};

// Leaky ReLU Function
// Behavior: The Leaky ReLU function allows a small, non - zero gradient when the input is negative, addressing the "dying ReLU" problem where neurons could become inactive during training.
// Use case: Hidden layers of neural networks, particularly in situations where traditional ReLU may result in dead neurons.
class LeakyReLUFunction : public ActivationFunction
{
public:
	double alpha = 0.01; // small positive slope for negative inputs

	LeakyReLUFunction() {
		name = "leaky_relu";
	}

	~LeakyReLUFunction() {}

	double operator()(double in) override {
		return in > 0 ? in : alpha * in;
	}
};
