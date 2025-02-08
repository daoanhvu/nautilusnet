import vnstock


stock_symbol = 'ACB'
year = 2025
start_date = f'{year}-01-01'
end_date = f'{year}-12-31'
df = vnstock.stock_historical_data(symbol=stock_symbol, start_date=start_date, end_date=end_date, resolution='1D',
                                   type='stock', beautify=True, decor=False, source='TCBS')

df.to_csv(f'./{stock_symbol}_{year}.csv', index=False)

print(f'Done downloading {stock_symbol} stock in {year}')
