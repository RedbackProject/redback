
import pandas as pd 
import matplotlib.pyplot as plt
import numpy as np


# Read the data from the CSV file
df = pd.read_csv('test_DeBorst_hard.csv')

time = df['time']
tangential_force = df['tangential_force']
disp_x_top = df['disp_x_top']
initial_height = 60

plt.plot(disp_x_top/initial_height, tangential_force, label='Tangential Force', color='blue')
plt.savefig('tangential_force.png')
# plt.show()