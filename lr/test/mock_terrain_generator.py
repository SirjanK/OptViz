# mock terrain generator for testing purposes
# we generate a CSV of (x, y, z) points that will then be used
# by ego_viz for testing visualization.
# run this script to generate the mock_terrain.csv file and move
# it to ego_viz/assets directory.


import numpy as np


# sinusoidal terrain parameters
AMPLITUDE = 5
OSCILLATION_RADIUS = 1


def generate_mock_terrain(x_min: float, x_max: float, y_min: float, y_max: float, delta: float) -> np.ndarray:
    """
    Generate mock terrain as a numpy array of (x, y, z) points.

    :param x_min: minimum x coordinate
    :param x_max: maximum x coordinate
    :param y_min: minimum y coordinate
    :param y_max: maximum y coordinate
    :param delta: spacing between points
    :return: numpy array of (x, y, z) points
    """
    x = np.arange(x_min, x_max, delta)
    y = np.arange(y_min, y_max, delta)
    x, y = np.meshgrid(x, y)
    radius = np.sqrt(x**2 + y**2)
    z = AMPLITUDE * np.cos(2 * np.pi * radius / OSCILLATION_RADIUS)
    return np.column_stack((x.flatten(), y.flatten(), z.flatten()))


def main():
    terrain = generate_mock_terrain(
        x_min=-5, 
        x_max=5, 
        y_min=-5, 
        y_max=5, 
        delta=0.05,
    )
    np.savetxt("mock_terrain.csv", terrain, delimiter=",", header="x,y,z")


if __name__ == "__main__":
    main()
