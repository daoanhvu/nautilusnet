import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
import torch
import torch.nn as nn
import torch.optim as optim
import random
from nstock_model import NautilusStockModel


def prepare_dataset(dataset, lookback):
    X, y = [], []
    for i in range(len(dataset) - lookback):
        feature = dataset[i:i + lookback, 0]
        target = dataset[(i + lookback), 0]
        X.append(feature)
        y.append(target)
    return np.array(X), np.array(y)


def test_everything():
    data_size = 3268
    data = [random.uniform(0, 1) for _ in range(data_size)]
    data = np.array(data).reshape(-1, 1)
    # data of shape (3268, 1)
    print(f"data type: {type(data)}, data shape: {data.shape}")

    # df = pd.read_csv('acb1.csv')
    # scaler = MinMaxScaler(feature_range=(0, 1))
    # timeseries = df[["close"]].values.astype('float32')
    # timeseries = scaler.fit_transform(timeseries)
    # data_size = len(timeseries)
    # print(f"data type: {type(timeseries)}, timeseries shape: {timeseries.shape}")

    # LSTM models
    input_size = 1
    hidden_size = 512
    batch_size = 16
    # this is sequence length
    lookback = 7
    batch_first = False
    lstm1 = nn.LSTM(input_size=1, hidden_size=128,
                    num_layers=1, batch_first=batch_first)
    lstm2 = nn.LSTM(input_size=128, hidden_size=64,
                    num_layers=1, batch_first=batch_first)
    fc1 = nn.Linear(64, 25)
    fc2 = nn.Linear(25, 1)

    # prepare time series data
    X, Y = prepare_dataset(data, lookback=lookback)
    # X is in shape of (3261, 7)
    print(f"X type: {type(X)}, X shape: {X.shape}")
    # Y is in shape of (3261, 7)
    print(f"Y type: {type(Y)}, Y shape: {Y.shape}")

    X = X.reshape(-1, 7, 1)
    # X = X.reshape(-1, 7, 1).transpose((1, 0, 2))

    print(f"After reshape X shape: {X.shape}")
    print(f"After reshape Y shape: {Y.shape}")
    # X is in shape of (7, 3261, 1)
    X = torch.tensor(X, dtype=torch.float32)
    Y = torch.tensor(Y, dtype=torch.float32).view(-1, 1)

    # In case batch_first = True, then input tensor shape will be
    # {batch_size, sequence_length, input_size}

    # model = NautilusStockModel()
    output, (hn, cn) = lstm1(X)
    output2, (hn, cn) = lstm2(output)
    print(f"lstm1 Output size: {output.size()}")
    print(f"lstm2 Output2 size: {output2.size()}")
    out = fc1(output2[:, -1, :])
    out = fc2(out)
    print(f"FC out size: {out.size()}")

    loss_fn = nn.MSELoss()
    loss = loss_fn(out, Y)

    # tensor_input = torch.tensor(values, dtype=torch.float32)
    # print(tensor_values)

    # viewed_tensor = tensor_values.view(-1, 1)
    # print(viewed_tensor)


def test_preparing_data():
    data_size = 10
    data = [random.uniform(0, 1) for _ in range(data_size)]
    data = np.array(data).reshape(-1, 1)
    # data of shape (data_size, 1)
    print(f"data type: {type(data)}, data shape: {data.shape}")
    print(data)

    lookback = 4
    X, Y = prepare_dataset(data, lookback=lookback)
    print(f"X shape {X.shape}")
    print(X)
    print(f"Y shape {Y.shape}")
    print(Y)


def test_torch_cat():
    values = torch.randn(5).view(-1, 1).reshape(-1, 5, 1)
    print(values)
    replaced = 1.0
    values = torch.cat((values[:, 1:, :], torch.tensor([[[replaced]]])), dim=1)
    print(values)


def test_prepare_predict_data():
    input_data = torch.randn(61)
    print(input_data.shape)
    input_data = input_data.view(-1, 1).reshape(-1, 61, 1)
    print(input_data.shape)
    print(input_data)
    model = NautilusStockModel()
    checkpoint = torch.load(
        "D:\\projects\\data\\acb_NautilusStockModel_lookback_61_5001.weights", weights_only=True)
    model.load_state_dict(checkpoint['model_state_dict'])

    pred = model(input_data)
    print(f"Predition's shape: {pred.shape}, value: {pred}")
    
    input_data = torch.cat((input_data[:, 1:, :], pred.reshape(-1, 1, 1)), dim=1)
    print(input_data)


if __name__ == '__main__':
    # test_everything()
    # test_preparing_data()
    # test_torch_cat()
    test_prepare_predict_data()
