# OptViz
Visualization for optimization algorithms.

## Simple Logistic Regression Example
We demonstrate viz functionality on a simple logistic regression example.

We generate mock 2D dimensional data with two classes that are nearly linearly
separable by a line around the origin (we inject noise so that not all samples can be, so a LR model
will inherently accrue some bias).

We then fit a LR model with only weights (no bias terms), namely two weight terms
$\vec{w} = \begin{bmatrix} w_1 \\ w_2 \end{bmatrix}$.

Finally, we visualize the evolution of $\vec{w}$ over time with two visualizations side by side:
1. Plot of $w_1$ and $w_2$ over time.
2. Plot of the fitted model on an $x_2$ vs $x_1$ plan over time.

To run this, run the bash script `launch_train.sh` which has pre-set arguments. Modify for your
own purposes if necessary.
