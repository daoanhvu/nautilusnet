import numpy as np
import pandas as pd
import yfinance as yf
import matplotlib.pyplot as plt
import torch
import torch.nn as nn


class NautilusStockModel(nn.Module):
    def __init__(self, input_size=1, hidden_size1=128, hidden_size2=64, num_layers=2, output_size=1):
        super(NautilusStockModel, self).__init__()
        batch_first = False
        # parameters:
        # input_size: the size of input's feature, in this case we just have one feature, the stock price
        self.lstm1 = nn.LSTM(input_size, hidden_size1,
                             num_layers=1, batch_first=batch_first)
        self.lstm2 = nn.LSTM(hidden_size1, hidden_size2,
                             num_layers=1, batch_first=batch_first)
        # This is the fully connected layer
        self.fc1 = nn.Linear(hidden_size2, 25)
        self.fc2 = nn.Linear(25, output_size)

    def forward(self, x):
        out, _ = self.lstm1(x)
        out, _ = self.lstm2(out)
        out = self.fc1(out[:, -1, :])
        return self.fc2(out)


def prepare_dataset(dataset, lookback):
    X, y = [], []
    for i in range(len(dataset) - lookback):
        feature = dataset[i:i + lookback, 0]
        target = dataset[(i + lookback), 0]
        X.append(feature)
        y.append(target)
    return np.array(X), np.array(y)


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
