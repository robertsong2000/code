from datetime import datetime

# 获取当前日期时间（也可以自定义日期，如 datetime(2025, 7, 21) ）
current_date = datetime.now()  
# %U 表示周数，周日作为一周的第一天；%W 表示周数，周一作为一周的第一天，根据需求选择
week_number = current_date.strftime("%U")  
print(f"当前日期 {current_date} 是今年的第 {week_number} 周")