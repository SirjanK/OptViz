import torch
import torch.utils.data as data


# fixed parameters for data generation
CLASS_ZERO_GAUSSIAN_MEAN = torch.tensor([0.6, 0.2])
CLASS_ONE_GAUSSIAN_MEAN = torch.tensor([-0.4, -0.9])
CLASS_ZERO_GAUSSIAN_COV = torch.tensor([[0.1, 0.05], [0.05, 0.1]])
CLASS_ONE_GAUSSIAN_COV = torch.tensor([[0.2, 0.08], [0.08, 0.2]])


def generate_dataloader(n_samples: int, batch_size: int) -> data.DataLoader:
    """
    Generate a dataloader for mock data. See `generate_dataset` for more details
    on the dataset.

    :param n_samples: number of samples to generate
    :param batch_size: batch size
    :return: torch.utils.data.DataLoader
    """

    dataset = generate_dataset(n_samples)

    return data.DataLoader(dataset, batch_size=batch_size, shuffle=True)


def generate_dataset(n_samples: int) -> data.Dataset:
    """
    Generate mock 2D data with two classes that are nearly linearly separable by a line around
    the origin with some noise injected so they are not perfectly separable.

    :param n_samples: number of samples to generate
    :return: torch.utils.data.Dataset
    """

    # define two 2D gaussian distributions
    class_zero_gaussian = torch.distributions.MultivariateNormal(
        loc=CLASS_ZERO_GAUSSIAN_MEAN,
        covariance_matrix=CLASS_ZERO_GAUSSIAN_COV
    )
    class_one_gaussian = torch.distributions.MultivariateNormal(
        loc=CLASS_ONE_GAUSSIAN_MEAN,
        covariance_matrix=CLASS_ONE_GAUSSIAN_COV
    )

    # generate features
    class_zero_data = class_zero_gaussian.sample((n_samples // 2,))
    class_one_data = class_one_gaussian.sample((n_samples // 2,))

    # generate labels
    class_zero_labels = torch.zeros(n_samples // 2)
    class_one_labels = torch.ones(n_samples // 2)

    # combine data
    data = torch.cat((class_zero_data, class_one_data), dim=0)
    labels = torch.cat((class_zero_labels, class_one_labels), dim=0)

    # create dataset
    return data.TensorDataset(data, labels)
