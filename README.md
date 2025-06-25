# OptViz
Visualization for optimization algorithms.

Ultimate goal (WIP): visualize progression of stochastic optimization algorithms across the loss
surface. Namely, if we abstractly define $\mathcal{L(\theta)}$ as the loss surface given some parameter
$\theta$ for a parametrized model, we want to visualize the evolution of $\mathcal{L}$ as $\theta$ is updated
by say SGD or Adam. One fun visualization we want to target is as if you were an ego actor at each
iteration time `t` with some view of the loss surface you're exploring, and follow the evolution
of the loss surface, moving as you go.

To start of with, just work with a simple LR example with two weight parameters (no bias term) on
2D mock data. Then go for models with more parameters using different projection schemes.

## Simple Logistic Regression Example
Current capability: Not the exact viz we want - just an animation of weight parameters over time
along with decision boundary over time.

We demonstrate viz functionality on a simple logistic regression example.

We generate mock 2D dimensional data with two classes that are nearly linearly
separable by a line around the origin (we inject noise so that not all samples can be, so a LR model
will inherently accrue some bias).

We then fit a LR model with only weights (no bias terms), namely two weight terms:
```math
\vec{w} = \begin{bmatrix} w_1 \\ w_2 \end{bmatrix}
```

Finally, we visualize the evolution of $\vec{w}$ over time with two visualizations side by side:
1. Plot of $w_1$ and $w_2$ over time.
2. Plot of the fitted model on an $x_2$ vs $x_1$ plan over time.

To run training, run the bash script `launch_train.sh` which has pre-set arguments. Modify for your
own purposes if necessary.

To run viz, run the bash script `launch_viz.sh` which has pre-set arguments. This will generate
an mp4 animation under `lr/assets/animation.mp4`.
