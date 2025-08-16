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


device = torch.device("cuda:0" if torch.cuda.is_available() else "cpu")
print(f"Using device {device}.")


def predict_next_week(model, last_lookback_prices, lookback, scaler):
    days = 7
    model.eval()
    predicted_prices = []

    input_price_seq = torch.tensor(
        last_lookback_prices, dtype=torch.float32).view(-1, 1).reshape(-1, lookback, 1).to(device=device)

    for day in range(days):
        with torch.no_grad():
            pred = model(input_price_seq)
            next_price = pred.item()
            predicted_prices.append(next_price)
            input_price_seq = torch.cat((input_price_seq[:, 1:, :], pred.reshape(-1, 1, 1)), dim=1)

    predicted_prices = scaler.inverse_transform(
        np.array(predicted_prices).reshape(-1, 1))
    return predicted_prices


# python ./nstock.py --cp=D:\projects\data\acb_NautilusStockModel_lookback_61_5001.weights --data_file=D:\projects\nautilusnet\stock\test_data_acb.csv
# python ./nstock.py --lookback=45 --cp=D:\projects\data\NautilusStockModel_POW_45_2200.weights --data_file=D:\projects\nautilusnet\stock\POW_20120101_20251231.csv
if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--cp', type=str, required=True,
                        help='Checkpoint file')
    parser.add_argument('--lookback', type=int, required=False,
                        help='The time step to lookback', default=61)
    parser.add_argument('--data_file', type=str, required=True,
                        help='The input data for prediction')
    args = parser.parse_args()

    df = pd.read_csv(args.data_file)
    scaler = MinMaxScaler(feature_range=(0, 1))
    timeseries = df[["close"]].values.astype('float32')
    timeseries = scaler.fit_transform(timeseries)
    data_size = len(timeseries)

    lookback = args.lookback

    # Get the last lookback prices
    timeseries = timeseries[-lookback:]

    model = NautilusStockModel()
    model.to(device=device)

    if args.cp == None:
        print("Parameter need to be provided.")
        exit()

    print("Going to load saved parameters")
    param_path = args.cp
    checkpoint = torch.load(param_path, weights_only=True)
    model.load_state_dict(checkpoint['model_state_dict'])
    start_epoch = checkpoint['epoch']
    start_loss = checkpoint['loss']

    # Make prediction
    model.eval()
    pred_prices = predict_next_week(model, timeseries, lookback, scaler)

    future_dates = [i for i in range(1, 8)]
    plt.figure(figsize=(12, 6))
    plt.plot(future_dates, pred_prices, marker='o', label="Predicted Prices")
    plt.title("Stock Price Prediction for Next Week")
    plt.xlabel("Date")
    plt.ylabel("Price")
    plt.legend()
    plt.show()
