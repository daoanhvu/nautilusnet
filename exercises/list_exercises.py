

def calculate_median(list):
    list.sort()
    n = len(list)
    mid = n // 2
    if n % 2 == 0:
        m1 = list[mid - 1]
        m2 = list[mid]
        return (m1 + m2) / 2.0
    else:
        return list[mid]

if __name__=='__main__':
    list_data = [34, 5, 10 , 21, 109, 3, 4, 55]
    # 3, 4, 5, 10, 21, 34, 55, 109
    m = calculate_median(list_data)
    print(f"Median: {m}\n")