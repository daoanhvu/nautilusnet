import numpy as np
import pandas as pd
import yfinance as yf
import matplotlib.pyplot as plt
from sklearn.preprocessing import MinMaxScaler
import torch
import torch.nn as nn
import torch.optim as optim
import argparse
from nstock_model import NautilusStockModel
from nstock_model import prepare_dataset
import os


def save_weights(epochs, folder, ticker, lookback, loss, parameters, optimizer_params):
    weight_file_name = f"NautilusStockModel_{ticker}_{lookback}_{epochs}.weights"
    weight_file_path = os.path.join(folder, weight_file_name)
    print(f"Going to save model's parameters to {weight_file_path}.")
    torch.save({
        'epoch': epochs,
        'model_state_dict': parameters,
        'optimizer_state_dict': optimizer_params,
        'loss': loss
    }, weight_file_path)


device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
print(f"Going to train the model using device {device}.")

# python ./train.py --ticker=POW --data_file= --epochs=10 --cp=D:\projects\data\acb_NautilusStockModel_100.weights --lookback=45
# python ./train.py --ticker=POW --data_file=./POW_20120101_20251231.csv --epochs=2200 --lookback=45
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cp', type=str, required=False,
                        help='Checkpoint file')
    parser.add_argument('--ticker', type=str, required=True,
                        help='Checkpoint file')
    parser.add_argument('--lookback', type=int, required=False,
                        help='The time step to lookback', default=61)
    parser.add_argument('--epochs', type=int,
                        required=True, help='Number of epoch')
    parser.add_argument('--data_file', type=str,
                        required=True, help='Data file')
    args = parser.parse_args()

    df = pd.read_csv(args.data_file)
    scaler = MinMaxScaler(feature_range=(0, 1))
    timeseries = df[["close"]].values.astype('float32')
    timeseries = scaler.fit_transform(timeseries)
    data_size = len(timeseries)

    print(f"Data length: {data_size}")
    print(f"Lookback : {args.lookback}")
    ticker = args.ticker
    lookback = args.lookback
    train_size = int(data_size * 0.75)
    train, test = timeseries[:train_size], timeseries[train_size:]
    # Prepare and split data in train and test sets
    X, Y = prepare_dataset(timeseries, lookback=lookback)
    X_train, X_test = X[:train_size], X[train_size:]
    y_train, y_test = Y[:train_size], Y[train_size:]

    # Reshape input
    X_train = X_train.reshape(-1, lookback, 1)
    X_test = X_test.reshape(-1, lookback, 1)

    # Convert data to pytorch tensors
    X_train = torch.tensor(X_train, dtype=torch.float32).to(device=device)
    y_train = torch.tensor(
        y_train, dtype=torch.float32).view(-1, 1).to(device=device)
    X_test = torch.tensor(X_test, dtype=torch.float32).to(device=device)
    y_test = torch.tensor(
        y_test, dtype=torch.float32).view(-1, 1).to(device=device)

    # print(f"X_train shape: {X_train.shape}, y_train shape {y_train.shape}")

    model = NautilusStockModel()
    model.to(device=device)
    optimizer = optim.Adam(model.parameters(), lr=0.001)
    loss_fn = nn.MSELoss()
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
        optimizer.zero_grad()
        y_pred = model(X_train)
        loss = loss_fn(y_pred, y_train)
        loss.backward()
        optimizer.step()
        if (epoch > 0) and ((epoch % 500) == 0):
            save_weights(epoch, 'D:\\projects\\data', ticker, lookback=lookback, loss=loss,
                         parameters=model.state_dict(), optimizer_params=optimizer.state_dict())
        if epoch % 100 == 0:
            print("Epoch %d/%d, Loss: %.4f" %
                  (epoch, n_epoch, loss.item()))

    save_weights(n_epoch, 'D:\\projects\\data', ticker, lookback=lookback, loss=loss,
                         parameters=model.state_dict(), optimizer_params=optimizer.state_dict())

    # Make prediction
    model.eval()
    y_pred = model(X_test).cpu().detach().numpy()
    y_pred = scaler.inverse_transform(y_pred)
    y_actual = scaler.inverse_transform(y_test.cpu().numpy())

    plt.figure(figsize=(12, 6))
    plt.plot(y_actual, label='Actual prices')
    plt.plot(y_pred, label='Predicted prices')
    plt.legend()
    plt.show()
