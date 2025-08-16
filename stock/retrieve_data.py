import vnstock
# import yfinance as yf
import argparse


stock_symbol = 'POW'
start_year = 2012
end_year = 2025
start_date = f'{start_year}-01-01'
end_date = f'{end_year}-12-31'
df = vnstock.stock_historical_data(symbol=stock_symbol, start_date=start_date, end_date=end_date, resolution='1D',
                                   type='stock', beautify=True, decor=False, source='TCBS')

df.to_csv(f'./{stock_symbol}_{start_year}_to_{end_year}.csv')

print(f'Done downloading {stock_symbol} stock in [{start_year}, {end_year}]')
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
