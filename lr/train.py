# train LR model with SGD

import argparse
import os
import shutil
from typing import Callable, Dict

import pandas as pd
import torch
import torch.nn as nn
import torch.optim as optim
import torch.utils.data as data
from torch.utils.tensorboard import SummaryWriter

from data_gen import generate_dataloader
from model import LogisticRegression


# for determinism
torch.manual_seed(12)


LOSS_KEY = "loss"
ACCURACY_KEY = "accuracy"


def train(train_dataloader: data.DataLoader, val_dataloader: data.DataLoader, epochs: int, lr: float, val_interval: int, log_dir: str) -> pd.DataFrame:
    """
    Train LR model with SGD and return a dataframe containing the iteration step, w1, w2.
    Also, log loss and accuracy on validation set at specified intervals along with the batch
    train loss to tensorboard.
    
    :param train_dataloader: DataLoader for training set
    :param val_dataloader: DataLoader for validation set
    :param epochs: Number of epochs to train
    :param lr: Learning rate
    :param val_interval: Interval at which to evaluate the model
    :param log_dir: Directory to save tensorboard logs
    :return: DataFrame containing the iteration step, w1, w2
    """

    # Create tensorboard writer
    if os.path.exists(log_dir):
        shutil.rmtree(log_dir)
    os.makedirs(log_dir, exist_ok=True)
    writer = SummaryWriter(log_dir)
    
    # Configure custom colors for better visualization
    # Train metrics will be orange, validation metrics will be blue
    writer.add_custom_scalars({
        'Loss': {
            'Loss': ['multiline', ['Loss/Train', 'Loss/Validation']]
        },
        'Accuracy': {
            'Accuracy': ['multiline', ['Accuracy/Train', 'Accuracy/Validation']]
        }
    })
    
    model = LogisticRegression()
    model.train()
    optimizer = optim.SGD(model.parameters(), lr=lr)
    criterion = nn.BCELoss()
    # map from metric name to function that calculates the metric
    metric_calculators = {
        ACCURACY_KEY: lambda preds, labels: (preds.round() == labels).float().mean(),
    }

    param_df = pd.DataFrame(columns=["step", "w1", "w2"])

    global_step = 0
    
    for epoch_idx in range(epochs):
        writer.add_scalar('Training/Epoch', epoch_idx, global_step)
        
        for batch_idx, (data, labels) in enumerate(train_dataloader):
            optimizer.zero_grad()
            preds = model(data)
            train_loss = criterion(preds, labels)
            train_loss.backward()
            optimizer.step()
            
            # Calculate batch train accuracy
            train_accuracy = metric_calculators[ACCURACY_KEY](preds, labels)
            
            # Log batch train metrics
            writer.add_scalar('Loss/Train', train_loss.item(), global_step)
            writer.add_scalar('Accuracy/Train', train_accuracy.item(), global_step)

            if batch_idx % val_interval == 0:
                val_metrics = evaluate(model, val_dataloader, criterion, metric_calculators)
                
                # Log validation metrics (will overlay with train metrics)
                writer.add_scalar('Loss/Validation', val_metrics[LOSS_KEY], global_step)
                writer.add_scalar('Accuracy/Validation', val_metrics[ACCURACY_KEY].item(), global_step)
                
                # Log learning rate
                writer.add_scalar('Training/Learning_Rate', lr, global_step)
            
            param_df.loc[len(param_df)] = [
                global_step,
                model.weights[0].item(),
                model.weights[1].item()
            ]

            global_step += 1

    
    writer.close()
    return param_df


def evaluate(model: LogisticRegression, dataloader: data.DataLoader, criterion: nn.Module, metric_calculators: Dict[str, Callable[[torch.Tensor, torch.Tensor], float]]) -> Dict[str, float]:
    """
    Evaluate the model on the validation set and return the loss.
    ASSUMPTION: dataloader contains one batch for evaluation.

    :param model: LogisticRegression model
    :param dataloader: DataLoader for validation set
    :param criterion: Loss function
    :param metric_calculators: Dictionary mapping metric name to function that calculates the metric
    :return: Dictionary mapping metric name to metric value (includes loss)
    """

    model.eval()
    with torch.no_grad():
        data, labels = next(iter(dataloader))
        preds = model(data)
        loss_value = criterion(preds, labels).item()

        metrics = {
            LOSS_KEY: loss_value,
        }

        for metric_name, metric_calculator in metric_calculators.items():
            metrics[metric_name] = metric_calculator(preds, labels)
        
    # restore train state
    model.train()
    return metrics


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--n_train", type=int, required=True,
                       help="Number of training samples to generate")
    parser.add_argument("--n_val", type=int, required=True,
                       help="Number of validation samples to generate") 
    parser.add_argument("--batch_size", type=int, required=True,
                       help="Training batch size")
    parser.add_argument("--epochs", type=int, required=True,
                       help="Number of epochs to train for")
    parser.add_argument("--lr", type=float, required=True,
                       help="Learning rate for optimizer")
    parser.add_argument("--val_interval", type=int, required=True,
                       help="Number of steps between validation evaluations")
    parser.add_argument("--log_dir", type=str, required=True,
                       help="Directory to save tensorboard logs")
    parser.add_argument("--params_path", type=str, required=True,
                       help="Path to save model parameters over time")
    args = parser.parse_args()

    # create dataloaders
    train_dataloader = generate_dataloader(args.n_train, args.batch_size)
    # have val dataloader load entire batch
    val_dataloader = generate_dataloader(args.n_val, batch_size=args.n_val)

    # train
    param_df = train(train_dataloader, val_dataloader, args.epochs, args.lr, args.val_interval, args.log_dir)

    # save params to params path
    if os.path.exists(args.params_path):
        os.remove(args.params_path)
    os.makedirs(os.path.dirname(args.params_path), exist_ok=True)
    param_df['step'] = param_df['step'].astype(int)
    param_df.to_csv(args.params_path, index=False)

    print(f"Training successful; Saved params to {args.params_path}")
