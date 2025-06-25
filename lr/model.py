import torch
import torch.nn as nn


class LogisticRegression(nn.Module):
    """
    Simple logistic regression model with two features and two classes.
    """

    def __init__(self):
        super().__init__()
        self.weights = nn.Parameter(torch.randn(2))

    def forward(self, x: torch.Tensor) -> torch.Tensor:
        return torch.sigmoid(x @ self.weights)

    def get_weights(self) -> torch.Tensor:
        return self.weights.data
    
    @classmethod
    def from_weights(cls, weights: torch.Tensor) -> "LogisticRegression":
        model = cls()
        model.weights.data = weights
        return model 
