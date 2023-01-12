import matplotlib.pyplot as plt
import pandas as pd

# Read in the CSV file
df = pd.read_csv("readings.csv")

# Create a 2D figure
plt.figure()

# Plot the gyro data
plt.plot(df["gyro_x"], df["gyro_y"], 'gray',label='gyro')

# Plot the accel data
plt.plot(df["accel_x"], df["accel_y"], 'blue',label='accel')

# Plot the fused data
plt.plot(df["fused_x"], df["fused_y"], 'green',label='fused')

# Add labels and legend
plt.xlabel('X-axis')
plt.ylabel('Y-axis')
plt.legend()

# Show the plot
plt.show()
