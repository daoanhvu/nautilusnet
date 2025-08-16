import vnstock


stock_symbol = 'POW'
start_year = 2012
end_year = 2025
start_date = f'{start_year}-01-01'
end_date = f'{end_year}-12-31'
df = vnstock.stock_historical_data(symbol=stock_symbol, start_date=start_date, end_date=end_date, resolution='1D',
                                   type='stock', beautify=True, decor=False, source='TCBS')

df.to_csv(f'./{stock_symbol}_{start_year}_to_{end_year}.csv')

print(f'Done downloading {stock_symbol} stock in [{start_year}, {end_year}]')
