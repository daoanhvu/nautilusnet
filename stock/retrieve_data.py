import vnstock
# import yfinance as yf
import argparse


# def get_stock_data(ticker, start, end):
#     df = yf.download(ticker, start=start, end=end)
#     return df['Close'].values.reshape(-1, 1)


def retrive_vnstock_prices(ticker, start_date, end_date, source):
    stock = vnstock.Vnstock().stock(symbol=ticker, source=source)
    df = stock.quote.history(start=start_date, end=end_date)
    df.to_csv(f'./{ticker}_{start_date}_{end_date}.csv', index=False)
    print(f'Done downloading {ticker} stock in [{start_date}-{end_date}]')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--ticker', type=str, required=True,
                        help='Stock symbol')
    args = parser.parse_args()

    start_date = f'2012-01-01'
    end_date = f'2025-12-31'
    retrive_vnstock_prices(args.ticker, start_date, end_date, source='TCBS')
