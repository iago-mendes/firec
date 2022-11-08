#include "grid.h"
using namespace std;

// Grid

Grid::Grid(int N_theta_, int N_phi_) {
	N_theta = N_theta_;
	N_phi = N_phi_;

	delta_theta = M_PI / N_theta;
	delta_phi = 2 * M_PI / N_phi;
}

double Grid::theta(int i) {
	return M_PI / N_theta * (i + (double) 1 / 2);
}

double Grid::phi(int j) {
	return 2 * M_PI / N_theta * j;
}

// GridFunction

GridFunction::GridFunction(Grid grid_, double (*function)(int i, int j)) {
	grid = grid_;

	vector<double> base_vector(grid.N_phi, 0);
	points.resize(grid.N_theta, base_vector);

	for (int i = 0; i < grid.N_theta; i++) {
		for (int j = 0; j < grid.N_phi; j++) {
			points[i][j] = function(i, j);
		}
	}
}

double GridFunction::partial_theta(int i, int j) {
	if (i == 0) {
		return (points[1][j] - points[0][j]) / grid.delta_theta;
	}
	else if (i == grid.N_theta - 1) {
		return (points[grid.N_theta - 1][j] - points[grid.N_theta - 2][j]) / grid.delta_theta;
	}
	else {
		return (points[i + 1][j] - points[i - 1][j]) / (2 * grid.delta_theta);
	}
}

double GridFunction::partial_phi(int i, int j) {
	double df;

	if (j == 0) {
		df = points[i][1] - points[i][grid.N_phi - 1];
	}
	else if (j == grid.N_phi - 1) {
		df = points[i][0] - points[i][grid.N_phi - 2];
	}
	else {
		df = points[i][j + 1] - points[i][j - 1];
	}

	return df / (2 * grid.delta_phi);
}

// Grid3DFunction

Grid3DFunction::Grid3DFunction(Grid grid_, double (*function)(int i, int j, char coordinate)) {
	grid = grid_;

	vector<double> base_vector(grid.N_phi, 0);
	x_values.resize(grid.N_theta, base_vector);
	y_values.resize(grid.N_theta, base_vector);
	z_values.resize(grid.N_theta, base_vector);

	for (int i = 0; i < grid.N_theta; i++) {
		for (int j = 0; j < grid.N_phi; j++) {
			x_values[i][j] = function(i, j, 'x');
			y_values[i][j] = function(i, j, 'y');
			z_values[i][j] = function(i, j, 'z');
		}
	}
}

double Grid3DFunction::rms() {
	double x_rms = get_rms(&x_values);
	double y_rms = get_rms(&y_values);
	double z_rms = get_rms(&z_values);

	double rms = (x_rms + y_rms + z_rms) / 3;
	return rms;
}

Grid3DFunction *Grid3DFunction::partial_theta() {
	Grid3DFunction *new_function = multiplied_by(1);

	for (int i = 0; i < grid.N_theta; i++) {
		for (int j = 0; j < grid.N_phi; j++) {
			if (i == 0) {
				(*new_function).x_values[i][j] = (x_values[1][j] - x_values[0][j]) / grid.delta_theta;
				(*new_function).y_values[i][j] = (y_values[1][j] - y_values[0][j]) / grid.delta_theta;
				(*new_function).z_values[i][j] = (z_values[1][j] - z_values[0][j]) / grid.delta_theta;
			}
			else if (i == grid.N_theta - 1) {
				(*new_function).x_values[i][j] = (x_values[grid.N_theta - 1][j] - x_values[grid.N_theta - 2][j]) / grid.delta_theta;
				(*new_function).y_values[i][j] = (y_values[grid.N_theta - 1][j] - y_values[grid.N_theta - 2][j]) / grid.delta_theta;
				(*new_function).z_values[i][j] = (z_values[grid.N_theta - 1][j] - z_values[grid.N_theta - 2][j]) / grid.delta_theta;
			}
			else {
				(*new_function).x_values[i][j] = (x_values[i + 1][j] - x_values[i - 1][j]) / (2 * grid.delta_theta);
				(*new_function).y_values[i][j] = (y_values[i + 1][j] - y_values[i - 1][j]) / (2 * grid.delta_theta);
				(*new_function).z_values[i][j] = (z_values[i + 1][j] - z_values[i - 1][j]) / (2 * grid.delta_theta);
			}
		}
	}

	return new_function;
}

Grid3DFunction *Grid3DFunction::partial_phi() {
	Grid3DFunction *new_function = multiplied_by(1);

	for (int i = 0; i < grid.N_theta; i++) {
		for (int j = 0; j < grid.N_phi; j++) {
			if (j == 0) {
				(*new_function).x_values[i][j] = (x_values[i][1] - x_values[i][grid.N_phi - 1]) / (2 * grid.delta_phi);
				(*new_function).y_values[i][j] = (y_values[i][1] - y_values[i][grid.N_phi - 1]) / (2 * grid.delta_phi);
				(*new_function).z_values[i][j] = (z_values[i][1] - z_values[i][grid.N_phi - 1]) / (2 * grid.delta_phi);
			}
			else if (j == grid.N_phi - 1) {
				(*new_function).x_values[i][j] = (x_values[i][0] - x_values[i][grid.N_phi - 2]) / (2 * grid.delta_phi);
				(*new_function).y_values[i][j] = (y_values[i][0] - y_values[i][grid.N_phi - 2]) / (2 * grid.delta_phi);
				(*new_function).z_values[i][j] = (z_values[i][0] - z_values[i][grid.N_phi - 2]) / (2 * grid.delta_phi);
			}
			else {
				(*new_function).x_values[i][j] = (x_values[i][j + 1] - x_values[i][j - 1]) / (2 * grid.delta_phi);
				(*new_function).y_values[i][j] = (y_values[i][j + 1] - y_values[i][j - 1]) / (2 * grid.delta_phi);
				(*new_function).z_values[i][j] = (z_values[i][j + 1] - z_values[i][j - 1]) / (2 * grid.delta_phi);
			}
		}
	}

	return new_function;
}

Grid3DFunction *Grid3DFunction::multiplied_by(double (*multiplier)(double theta, double phi, char coordinate)) {
	Grid3DFunction *new_function;
	new_function = new Grid3DFunction;
	(*new_function) = Grid3DFunction();

	(*new_function).grid = grid;
	(*new_function).x_values = x_values;
	(*new_function).y_values = y_values;
	(*new_function).z_values = z_values;

	for (int i = 0; i < grid.N_theta; i++) {
		double theta = grid.theta(i);

		for (int j = 0; j < grid.N_phi; j++) {
			double phi = grid.phi(j);
			
			(*new_function).x_values[i][j] *= multiplier(theta, phi, 'x');
			(*new_function).y_values[i][j] *= multiplier(theta, phi, 'y');
			(*new_function).z_values[i][j] *= multiplier(theta, phi, 'z');
		}
	}

	return new_function;
}

Grid3DFunction *Grid3DFunction::multiplied_by(double multiplier) {
	Grid3DFunction *new_function;
	new_function = new Grid3DFunction;
	(*new_function) = Grid3DFunction();

	(*new_function).grid = grid;
	(*new_function).x_values = x_values;
	(*new_function).y_values = y_values;
	(*new_function).z_values = z_values;

	for (int i = 0; i < grid.N_theta; i++) {
		for (int j = 0; j < grid.N_phi; j++) {
			(*new_function).x_values[i][j] *= multiplier;
			(*new_function).y_values[i][j] *= multiplier;
			(*new_function).z_values[i][j] *= multiplier;
		}
	}

	return new_function;
}

Grid3DFunction *Grid3DFunction::added_with(
	Grid3DFunction *function,
	double (*multiplier)(double theta, double phi, char coordinate)
) {
	Grid3DFunction *new_function = multiplied_by(1);

	for (int i = 0; i < grid.N_theta; i++) {
		double theta = grid.theta(i);

		for (int j = 0; j < grid.N_phi; j++) {
			double phi = grid.phi(j);

			(*new_function).x_values[i][j] += multiplier(theta, phi, 'x') * (*function).x_values[i][j];
			(*new_function).y_values[i][j] += multiplier(theta, phi, 'y') * (*function).y_values[i][j];
			(*new_function).z_values[i][j] += multiplier(theta, phi, 'z') * (*function).z_values[i][j];
		}
	}

	return new_function;
}

Grid3DFunction *Grid3DFunction::added_with(
	Grid3DFunction *function,
	double multiplier
) {
	Grid3DFunction *new_function = multiplied_by(1);

	for (int i = 0; i < grid.N_theta; i++) {
		for (int j = 0; j < grid.N_phi; j++) {
			(*new_function).x_values[i][j] += multiplier * (*function).x_values[i][j];
			(*new_function).y_values[i][j] += multiplier * (*function).y_values[i][j];
			(*new_function).z_values[i][j] += multiplier * (*function).z_values[i][j];
		}
	}

	return new_function;
}
