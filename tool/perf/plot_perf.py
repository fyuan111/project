#!/usr/bin/env python3
"""
实时绘制串口性能数据
用法: sudo python3 plot_perf.py /dev/ttyACM0
"""

import serial
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from collections import deque
import re
import sys

# 配置
SERIAL_PORT = sys.argv[1] if len(sys.argv) > 1 else '/dev/ttyACM0'
BAUD_RATE = 115200
MAX_POINTS = 200

# 数据存储
data = {
    'count': deque(maxlen=MAX_POINTS),
    'time_us': deque(maxlen=MAX_POINTS),
    'max_us': deque(maxlen=MAX_POINTS),
    'min_us': deque(maxlen=MAX_POINTS),
    'avg_us': deque(maxlen=MAX_POINTS),
}

# 打开串口
try:
    ser = serial.Serial(SERIAL_PORT, BAUD_RATE, timeout=1)
    print(f"Connected to {SERIAL_PORT} at {BAUD_RATE} baud")
except Exception as e:
    print(f"Error opening serial port: {e}")
    print("Try running with sudo: sudo python3 plot_perf.py")
    sys.exit(1)

# 创建图表
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(12, 8))
fig.suptitle('Performance Monitor', fontsize=16)

def parse_line(line):
    """解析 Teleplot 格式的数据"""
    # 匹配: >perf|count:123 或 >count:123
    match = re.search(r'>(?:\w+\|)?(\w+):([\d.]+)', line)
    if match:
        name = match.group(1)
        value = float(match.group(2))
        print(f"Parsed: {name} = {value}")  # 调试信息
        if name in data:
            data[name].append(value)
            return True
        else:
            print(f"Warning: Unknown field '{name}'")
    return False

def animate(i):
    """动画更新函数"""
    # 读取所有可用数据
    while ser.in_waiting:
        try:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                parse_line(line)
                # 也打印到终端
                if line.startswith('>'):
                    print(line)
        except Exception as e:
            print(f"Error reading: {e}")

    # 更新图表
    ax1.clear()
    ax2.clear()

    # 图1: 当前时间
    if len(data['count']) > 0 and len(data['time_us']) > 0:
        ax1.plot(list(data['count']), list(data['time_us']), 'b-', linewidth=2)
        ax1.set_xlabel('Count', fontsize=12)
        ax1.set_ylabel('Time (us)', fontsize=12)
        ax1.set_title('Current Measurement Time', fontsize=14)
        ax1.grid(True, alpha=0.3)
        # 自动调整 Y 轴范围，显示微小变化
        if len(data['time_us']) > 0:
            times = list(data['time_us'])
            y_min, y_max = min(times), max(times)
            margin = (y_max - y_min) * 0.1 or 1  # 10% 边距
            ax1.set_ylim(y_min - margin, y_max + margin)

    # 图2: 统计数据
    if len(data['count']) > 0:
        count_list = list(data['count'])
        if len(data['max_us']) > 0:
            x = count_list[-len(data['max_us']):]
            ax2.plot(x, list(data['max_us']), 'r-', linewidth=2, label='Max')
            ax2.plot(x, list(data['min_us']), 'g-', linewidth=2, label='Min')
            ax2.plot(x, list(data['avg_us']), 'b-', linewidth=2, label='Avg')
            ax2.set_xlabel('Count', fontsize=12)
            ax2.set_ylabel('Time (us)', fontsize=12)
            ax2.set_title('Statistics (Max/Min/Avg)', fontsize=14)
            ax2.legend(loc='upper right')
            ax2.grid(True, alpha=0.3)
            # 自动调整 Y 轴范围
            all_values = list(data['max_us']) + list(data['min_us']) + list(data['avg_us'])
            if all_values:
                y_min, y_max = min(all_values), max(all_values)
                margin = (y_max - y_min) * 0.1 or 1
                ax2.set_ylim(y_min - margin, y_max + margin)

# 创建动画
ani = animation.FuncAnimation(fig, animate, interval=100, cache_frame_data=False)

plt.tight_layout()
print("\nStarting plot... Close the plot window to exit.\n")

try:
    plt.show()
except KeyboardInterrupt:
    print("\nStopped by user")
finally:
    ser.close()
    print("Serial port closed")
