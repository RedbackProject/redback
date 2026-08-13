import os, csv
import numpy as np
import pylab as P
import matplotlib.pyplot as plt
from scipy.optimize import curve_fit

csv_dir = os.path.join(os.path.expanduser('~'), 'projects/redback/tests/benchmark_1_T')

########################################################
# Read all data
filenames = {}
filenames['A'] = os.path.join(csv_dir, 'bench1_a_out.csv')
filenames['B'] = os.path.join(csv_dir, 'bench1_b_out.csv')
filenames['C'] = os.path.join(csv_dir, 'bench1_c_out.csv')
filenames['D'] = os.path.join(csv_dir, 'bench1_d_out.csv')
times = {}
temperatures = {}
for key in ['A', 'B', 'C', 'D']:
  times[key] = []
  temperatures[key] = []
for key in ['A', 'B', 'C', 'D']:
  with open(filenames[key]) as csvfile:
    myreader = csv.reader(csvfile, delimiter=' ', quotechar='|')
    index_line = 0
    for line in myreader:
      index_line += 1
      if index_line == 1:
        tmp = line[0].split(',')
        assert tmp[0].lower() == 'time'
        assert tmp[1].lower() == 'middle_temp'
        continue
      if not line:
        continue
      # read data
      tmp = line[0].split(',')
      times[key].append(float(tmp[0]))
      temperatures[key].append(float(tmp[1]))

# First figure, A and B (converging to lower branch)
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
# plotting point markers
m_thickness = 2    # marker edge thickness
m_colour = 'black' # marker edge colour
m_size = 5        # marker size

major_ticks_x = np.arange(0, 10, 2)
minor_ticks_x = np.arange(0, 10, 0.5)
major_ticks_y = np.arange(0, 0.25, 0.04)
minor_ticks_y = np.arange(0, 0.25, 0.01)
ax.set_xticks(major_ticks_x)
ax.set_xticks(minor_ticks_x, minor=True)
ax.set_yticks(major_ticks_y)
ax.set_yticks(minor_ticks_y, minor=True)
##ax.grid(which='both')
ax.grid(which='minor', color='lightgrey', linestyle='--')#, alpha=0.2)#http://matplotlib.org/examples/color/named_colors.html
ax.grid(which='major', color='darkgrey', linestyle='-')#, alpha=0.5) # Note: alpha props not exported to eps :-(
ax.set_axisbelow(True)
plt.xlabel('Time', fontsize=20)
plt.ylabel('Temperature', fontsize=20)
plt.xticks(major_ticks_x, fontsize = 16)
plt.yticks(major_ticks_y, fontsize = 16)
plot_A, = plt.plot(times['A'], temperatures['A'],'-o', markersize=m_size)#markerfacecolor='lightgrey')
plot_B, = plt.plot(times['B'], temperatures['B'],'-o', markersize=m_size)#, markeredgecolor='green',markerfacecolor='lightgrey')
plt.legend((plot_A, plot_B), ('case A', 'case B'))

#plt.plot(0.82, 1.73, 'o', markerfacecolor="None", markeredgecolor='{0}'.format(m_colour), markeredgewidth=m_thickness, markersize=m_size)
# annotations (see http://matplotlib.org/users/annotations_intro.html)

#P.xlim([0, 3.0])
P.ylim([0, 0.20])
##plt.axis('equal')

if 1:
    filename = 'benchmark_1_T_results_A_B.eps'
    plt.savefig(filename, format='eps', dpi=1000)
    print 'Figure saved as {0}'.format(os.path.realpath(filename))
else:
    plt.show()

########################################################
# Second figure, C and D (converging to upper branch)
fig = plt.figure()
ax = fig.add_subplot(1,1,1)
# plotting point markers
m_thickness = 2    # marker edge thickness
m_colour = 'black' # marker edge colour
m_size = 5        # marker size

major_ticks_x = np.arange(0, 100, 10)
minor_ticks_x = np.arange(0, 100, 5)
major_ticks_y = np.arange(0, 1200, 200)
minor_ticks_y = np.arange(0, 1200, 100)
ax.set_xticks(major_ticks_x)
ax.set_xticks(minor_ticks_x, minor=True)
ax.set_yticks(major_ticks_y)
ax.set_yticks(minor_ticks_y, minor=True)
##ax.grid(which='both')
ax.grid(which='minor', color='lightgrey', linestyle='--')#, alpha=0.2)#http://matplotlib.org/examples/color/named_colors.html
ax.grid(which='major', color='darkgrey', linestyle='-')#, alpha=0.5) # Note: alpha props not exported to eps :-(
ax.set_axisbelow(True)
plt.xlabel('Time', fontsize=20)
plt.ylabel('Temperature', fontsize=20)
plt.xticks(major_ticks_x, fontsize = 16)
plt.yticks(major_ticks_y, fontsize = 16)
plot_C, = plt.plot(times['C'], temperatures['C'],'-o', markersize=m_size)#markerfacecolor='lightgrey')
plot_D, = plt.plot(times['D'], temperatures['D'],'-o', markersize=m_size)#, markeredgecolor='green',markerfacecolor='lightgrey')
plt.legend((plot_C, plot_D), ('case C', 'case D'), loc='center')

#plt.plot(0.82, 1.73, 'o', markerfacecolor="None", markeredgecolor='{0}'.format(m_colour), markeredgewidth=m_thickness, markersize=m_size)
# annotations (see http://matplotlib.org/users/annotations_intro.html)

#P.xlim([0, 3.0])
P.ylim([0, 1200])
##plt.axis('equal')

if 1:
    filename = 'benchmark_1_T_results_C_D.eps'
    plt.savefig(filename, format='eps', dpi=1000)
    print 'Figure saved as {0}'.format(os.path.realpath(filename))
else:
    plt.show()
