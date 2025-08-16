import matplotlib.pyplot as plt
import pandas as pd
import numpy as np
import torch
import torch.nn as nn
import torch.optim as optim
import torch.utils.data as data
import argparse

# https://machinelearningmastery.com/lstm-for-time-series-prediction-in-pytorch/


def create_dataset(dataset, lookback, device):
    X, y = [], []
    for i in range(len(dataset)-lookback):
        feature = dataset[i:i+lookback]
        target = dataset[i+1: i+lookback + 1]
        X.append(feature)
        y.append(target)
    return torch.tensor(X).to(device), torch.tensor(y).to(device)


class AirModel(nn.Module):
    def __init__(self):
        super().__init__()
        self.lstm = nn.LSTM(input_size=1,
                            hidden_size=64, num_layers=2,
                            batch_first=True)
        # This is the fully connected layer
        self.linear = nn.Linear(64, 1)

    def forward(self, x):
        x, _ = self.lstm(x)
        return self.linear(x)


device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
print(f"Going to train the model using device {device}.")


def plot_data(model, timeseries, train_size, X_train, X_test, y_pred, lookback):
    with torch.no_grad():
        # shift train predictions for plotting
        train_plot = np.ones_like(timeseries) * np.nan
        y_pred = model(X_train)
        y_pred = y_pred[:, -1, :]
        train_plot[lookback:train_size] = model(X_train).cpu()[:, -1, :]
        # shift test predictions for plotting
        test_plot = np.ones_like(timeseries) * np.nan
        test_plot[train_size+lookback:len(timeseries)
                  ] = model(X_test).cpu()[:, -1, :]
    # plot
    plt.plot(timeseries)
    plt.plot(train_plot, c='r')
    plt.plot(test_plot, c='g')
    plt.show()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cp', type=str, required=False,
                        help='Checkpoint file')
    parser.add_argument('--epochs', type=int,
                        required=True, help='Number of epoch')
    args = parser.parse_args()

    df = pd.read_csv('acb1.csv')
    timeseries = df[["close"]].values.astype('float32')

    data_size = len(timeseries)
    train_size = int(data_size * 0.67)
    test_size = data_size - train_size
    train, test = timeseries[:train_size], timeseries[train_size:]

    lookback = 16
    X_train, y_train = create_dataset(train, lookback=lookback, device=device)
    X_test, y_test = create_dataset(test, lookback=lookback, device=device)

    model = AirModel()
    model.to(device=device)
    optimizer = optim.Adam(model.parameters())
    loss_fn = nn.MSELoss()
    loader = data.DataLoader(data.TensorDataset(
        X_train, y_train), shuffle=True, batch_size=8)
    n_epoch = args.epochs

    if args.cp != None:
        print("Going to load saved parameters")
        param_path = args.cp
        checkpoint = torch.load(param_path, weights_only=True)
        model.load_state_dict(checkpoint['model_state_dict'])
        optimizer.load_state_dict(checkpoint['optimizer_state_dict'])
        start_epoch = checkpoint['epoch']
        start_loss = checkpoint['loss']
    else:
        print('No checkpoint file provided!')

    for epoch in range(n_epoch):
        model.train()
        for X_batch, y_batch in loader:
            y_pred = model(X_batch)
            loss = loss_fn(y_pred, y_batch)
            optimizer.zero_grad()
            loss.backward()
            optimizer.step()
        if (epoch > 0) and ((epoch % 1000) == 0):
            file_path = f'D:\\projects\\data\\acb_model_epoch_{epoch}.weights'
            print(f"Going to save model's parameters to {file_path}.")
            torch.save({
                'epoch': epoch,
                'model_state_dict': model.state_dict(),
                'optimizer_state_dict': optimizer.state_dict(),
                'loss': loss
            }, file_path)
        if epoch % 100 == 0:
            model.eval()
            with torch.no_grad():
                y_pred = model(X_train)
                train_rmse = np.sqrt(loss_fn(y_pred, y_train).cpu())
                y_pred = model(X_test)
                test_rmse = np.sqrt(loss_fn(y_pred, y_test).cpu())
            print("Epoch %d: Train RMSE: %.4f, Test RMSE %.4f" %
                  (epoch, train_rmse, test_rmse))

    plot_data(model, timeseries, train_size, X_train, X_test, y_pred, lookback)
