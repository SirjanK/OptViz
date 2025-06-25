import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation, FFMpegWriter
from data_gen import generate_dataset
import argparse
import torch
import os


# for determinism
torch.manual_seed(12)


def viz(param_df: pd.DataFrame, n_samples: int, fps: int, save_path: str, frame_step: int = 1) -> None:
    """
    Visualize the trajectory of the model parameters over time along with the decision boundary.

    :param param_df: DataFrame containing the model parameters over time
    :param n_samples: Number of samples to generate for viz to contextualize the decision boundary
    :param fps: Frames per second for animation
    :param save_path: Path to save animation
    :param frame_step: Step size for sampling frames (1 = every frame, 10 = every 10th frame, etc.)
    """

    # Configs
    interval = 1000 / fps  # ms 

    # Setup figure
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(10, 5))

    # --- Plot 1: w2 vs w1 over time (moving point)
    # derive xlim and ylim from param_df
    xlim = (param_df['w1'].min(), param_df['w1'].max())
    ylim = (param_df['w2'].min(), param_df['w2'].max())
    # adjust to add buffer
    xlim = (xlim[0] - 0.1 * (xlim[1] - xlim[0]), xlim[1] + 0.1 * (xlim[1] - xlim[0]))
    ylim = (ylim[0] - 0.1 * (ylim[1] - ylim[0]), ylim[1] + 0.1 * (ylim[1] - ylim[0]))
    ax1.set_xlim(*xlim)
    ax1.set_ylim(*ylim)
    ax1.set_title("w2 vs w1 Trajectory")
    ax1.set_xlabel("w1")
    ax1.set_ylabel("w2")
    ax1.grid(True, alpha=0.3)
    point_plot, = ax1.plot([], [], 'ro', label="Model Parameters")
    ax1.legend()

    # --- Plot 2: Static scatter + moving decision boundary
    # first generate samples
    dataset = generate_dataset(n_samples)
    X, y = dataset.tensors
    # derive xlim and ylim from X
    xlim = (X[:, 0].min(), X[:, 0].max())
    ylim = (X[:, 1].min(), X[:, 1].max())
    # adjust to add buffer
    xlim = (xlim[0] - 0.1 * (xlim[1] - xlim[0]), xlim[1] + 0.1 * (xlim[1] - xlim[0]))
    ylim = (ylim[0] - 0.1 * (ylim[1] - ylim[0]), ylim[1] + 0.1 * (ylim[1] - ylim[0]))
    ax2.set_xlim(*xlim)
    ax2.set_ylim(*ylim)
    ax2.set_title("Decision Boundary Over Time")
    ax2.set_xlabel("x1")
    ax2.set_ylabel("x2")
    ax2.grid(True, alpha=0.3)

    # scatter each class with different colors
    X_class_zero = X[y == 0]
    X_class_one = X[y == 1]
    ax2.scatter(X_class_zero[:, 0], X_class_zero[:, 1], s=10, alpha=0.5, color='orange', label='Class 0')
    ax2.scatter(X_class_one[:, 0], X_class_one[:, 1], s=10, alpha=0.5, color='purple', label='Class 1')
    line_plot, = ax2.plot([], [], 'b-', lw=2, label="Decision Boundary")
    ax2.legend()

    # X range for decision boundary line
    x_vals = np.linspace(*xlim, 100)

    def update(frame):
        w1 = param_df.at[frame, 'w1']
        w2 = param_df.at[frame, 'w2']

        # Update point plot
        point_plot.set_data([w1], [w2])

        # Update decision boundary: w1*x + w2*y = 0 → y = -w1/w2 * x
        if w2 != 0:
            y_vals = -w1 / w2 * x_vals
        else:
            y_vals = np.full_like(x_vals, np.nan)  # invisible

        line_plot.set_data(x_vals, y_vals)
        return point_plot, line_plot

    # Animation
    frames = range(0, len(param_df), frame_step)
    print(f"Creating animation with {len(frames)} frames (sampling every {frame_step}th frame)...")
    ani = FuncAnimation(fig, update, frames=frames, interval=interval, blit=True)

    # Save
    writer = FFMpegWriter(fps=fps, bitrate=1800)
    ani.save(save_path, writer=writer)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--params_path", type=str, required=True, help="Path to params csv")
    parser.add_argument("--save_path", type=str, required=True, help="Path to save animation")
    parser.add_argument("--n_samples", type=int, required=True, help="Number of samples to generate for viz")
    parser.add_argument("--fps", type=int, required=True, help="Frames per second for animation")
    parser.add_argument("--frame_step", type=int, default=1, help="Step size for sampling frames")
    args = parser.parse_args()

    param_df = pd.read_csv(args.params_path)

    # remove save_path if it exists
    if os.path.exists(args.save_path):
        os.remove(args.save_path)
    os.makedirs(os.path.dirname(args.save_path), exist_ok=True)

    viz(param_df, args.n_samples, fps=args.fps, save_path=args.save_path, frame_step=args.frame_step)
