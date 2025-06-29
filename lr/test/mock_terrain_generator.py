# mock terrain generator for testing purposes
# we generate three files:
#  1. mock_terrain_metadata.json: contains metadata about the terrain like
# grid min and max bounds and delta.
#  2. mock_terrain.bin: binary file containing the z values of the terrain grid
#  3. mock_trajectory.csv: contains the trajectory of the ego actor
# move all these to lr/ego_viz/godot/assets/


import numpy as np
import json
import pandas as pd
from typing import Tuple, Dict, Any

# grid parameters
X_MIN = -5
X_MAX = 5
Y_MIN = -5
Y_MAX = 5
DELTA = 0.05

# sinusoidal terrain parameters
AMPLITUDE = 5
OSCILLATION_RADIUS = 1

# trajectory parameters
NUM_SPIRALS = 4
MAX_TIME = 100


def generate_mock_terrain(x_min: float, x_max: float, y_min: float, y_max: float, delta: float) -> Tuple[Dict[str, Any], np.ndarray]:
    """
    Generate mock terrain data

    :param x_min: minimum x coordinate
    :param x_max: maximum x coordinate
    :param y_min: minimum y coordinate
    :param y_max: maximum y coordinate
    :param delta: spacing between points
    :return: 1) metadata about the terrain, 2) 2D numpy array of z for (x, y) grid
    """
    x = np.arange(x_min, x_max, delta)
    y = np.arange(y_min, y_max, delta)
    x, y = np.meshgrid(x, y)
    radius = np.sqrt(x**2 + y**2)
    z = AMPLITUDE * np.cos(2 * np.pi * radius / OSCILLATION_RADIUS)

    # create metadata
    metadata = {
        "x_min": x_min,
        "width": len(x),
        "y_min": y_min,
        "height": len(y),
        "delta": delta,
    }

    return metadata, z


def generate_mock_trajectory(max_radius: float, num_spirals: int, max_time: int) -> pd.DataFrame:
    """
    Generate mock trajectory as a numpy array of (t, x, y) points.

    :param max_radius: maximum radius of the trajectory
    :param num_spirals: number of spirals to make
    :param max_time: maximum time of the trajectory
    :return: numpy array of (t, x, y) points
    """
    t = np.arange(0, max_time, 1, dtype=np.int32)  # frame index

    # frequency of the spiral
    spiral_freq = 1 / (2 * np.pi * num_spirals)

    # create theta array as a normalized version of time
    theta_norm = max_time * spiral_freq
    theta = t / theta_norm

    # create radius array as a normalized version of time
    radius_scale_factor = max_radius * spiral_freq
    # spiral in polar coordinates
    radius = radius_scale_factor * theta

    # create x and y arrays
    x = radius * np.cos(theta)
    y = radius * np.sin(theta)
    return pd.DataFrame({"t": t, "x": x, "y": y})


def save_terrain_binary(terrain_z, filename):
    """
    Save terrain z values as a flattened binary array of float32.
    """
    # Write height data as flattened float32 array
    terrain_z.flatten().astype(np.float32).tofile(filename)


def main():
    terrain_metadata, terrain_z = generate_mock_terrain(
        x_min=X_MIN, 
        x_max=X_MAX, 
        y_min=Y_MIN, 
        y_max=Y_MAX, 
        delta=DELTA,
    )
    save_terrain_binary(terrain_z, filename="mock_terrain.bin")
    with open("mock_terrain_metadata.json", "w") as f:
        json.dump(terrain_metadata, f)

    max_radius = min(abs(X_MIN), abs(X_MAX), abs(Y_MIN), abs(Y_MAX))
    trajectory = generate_mock_trajectory(
        max_radius=max_radius,
        num_spirals=NUM_SPIRALS,
        max_time=MAX_TIME,
    )
    # save trajectory["t"] as integer, trajectory["x"] and trajectory["y"] as float
    trajectory["t"] = trajectory["t"].astype(np.int32)
    trajectory.to_csv("mock_trajectory.csv", index=False, header=True)


if __name__ == "__main__":
    main()
