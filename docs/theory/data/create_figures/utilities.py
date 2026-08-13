''' Script to parse csv file produced by redback '''

import os, sys, csv
from os.path import expanduser
import numpy as np
import pylab as P
import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter
import matplotlib
import subprocess

def parseCsv(filename, column_keys = None):
  ''' Parse csv file created by moose and return dictionary of data '''
  print 'Parsing "{}"...'.format(filename)
  column_index = {} # mapping, key=column_key, value=corresponding column index
  data = {} # dict of data, key=column_key, value=data list (floats)
  with open(filename, 'rb') as csvfile:
    csvreader = csv.reader(csvfile)
    line_i = 0 # line index
    for row in csvreader:
      if line_i == 0:
        # Headers. Find interesting columns
        headers = row
        # prepare structure for all columns we want
        if column_keys is None:
          # grab all data in file
          column_keys_we_want = [elt.lower() for elt in headers]
        else:
          # grab only requested data from file
          assert type(column_keys)==type([])
          column_keys_we_want = column_keys
        for column_key in column_keys_we_want:
          data[column_key] = []
        for column_i, elt in enumerate(headers):
          elt_lower = elt.lower()
          if elt_lower in column_keys_we_want:
            column_index[elt_lower] = column_i
        column_indices = column_index.values()
        line_i += 1
        print 'Found columns {0}'.format(column_index)
        continue
      # Data line
      if len(row) < len(headers):
        break # finished reading all data
      for column_key in column_keys_we_want:
        data[column_key].append(float(row[column_index[column_key]]))
      line_i += 1
      continue # go to next data line
  print 'Finished parsing csv file'
  return data

def plotFigure(analytical_x_data, analytical_y_data,
               numerical_x_data, numerical_y_data,
               x_label='Time',
               y_label='Value XXX',
               figure_rootfilename=None, # Figure file name without extension
               do_show=False):
  ''' Function to plot figure of numerical vs analytical results
      and save picture to file.
  '''
  my_marker_size = 10

  fig = plt.figure()
  ax = fig.add_subplot(1,1,1)
  plt.xlabel(x_label, fontsize=20)
  plt.ylabel(y_label, fontsize=20)
#   if 0:
#     major_ticks_x = np.arange(0, 200, 20)
#     minor_ticks_x = np.arange(10, 200, 20)
#     major_ticks_y = np.arange(0, 0.01, 0.002)
#     minor_ticks_y = np.arange(0.001, 0.01, 0.002)
#     ax.set_xticks(major_ticks_x)
#     ax.set_xticks(minor_ticks_x, minor=True)
#     ax.set_yticks(major_ticks_y)
#     ax.set_yticks(minor_ticks_y, minor=True)
#     #ax.grid(which='both')
#     ax.grid(which='minor', color='lightgrey', linestyle='--')#, alpha=0.2)#http://matplotlib.org/examples/color/named_colors.html
#     ax.grid(which='major', color='darkgrey', linestyle='-')#, alpha=0.5) # Note: alpha props not exported to eps :-(
#     ax.set_axisbelow(True)
#     plt.xticks(major_ticks_x, fontsize = 16)
#     plt.yticks(major_ticks_y, fontsize = 16)

  line, = plt.plot(analytical_x_data, analytical_y_data, 'o',
                   markeredgecolor='red', markerfacecolor='white',
                   markersize=my_marker_size, label='Analytical solution')
  line, = plt.plot(numerical_x_data, numerical_y_data, '-x',
                   color='black',
                   markeredgecolor='black', markerfacecolor='black',
                   markersize=my_marker_size, label='Numerical results')

  plt.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3,
             ncol=2, mode="expand", borderaxespad=0.)
                   #'grey', linewidth=2, linestyle='-')

#   if 0:
#     P.xlim([0, 130])
#     P.ylim([0, 0.0075])
#
#     # http://stackoverflow.com/questions/3677368/matplotlib-format-axis-offset-values-to-whole-numbers-or-specific-number
#     def thousandths(x, pos):
#         'The two args are the value and tick position'
#         return '%1.0f' % (x*1e3)
#     formatter = FuncFormatter(thousandths)
#     ax.yaxis.set_major_formatter(formatter)
#     #plt.text(0.0, 1.01, '1e-3', fontsize=16, transform = plt.gca().transAxes)

  if do_show:
      plt.show()
  else:
    # save to file
    if figure_rootfilename is None:
      error_msg = 'You must provide a "figure_rootfilename" since do_show=False'
      raise Exception, error_msg
    figure_eps_filename = '{0}.eps'.format(figure_rootfilename)
    figure_pdf_filename = '{0}.pdf'.format(figure_rootfilename)
    this_fig_dir, short_eps_file_name = os.path.split(figure_eps_filename)
    if not os.path.isdir(this_fig_dir):
      os.makedirs(this_fig_dir)
      print 'Created subdirectory "{0}"'.format(this_fig_dir)
    plt.savefig(figure_eps_filename, format='eps', dpi=1000)
    # Convert eps to pdf
    try:
      command = "epstopdf {0}".format(figure_eps_filename)
      retcode = subprocess.call(command, shell=True)
      if retcode < 0:
        print >>sys.stderr, "Child was terminated by signal", -retcode
      else:
        print >>sys.stderr, "Child returned", retcode
    except OSError as e:
      print >>sys.stderr, "Execution failed:", e
    # Delete the eps
    try:
      os.remove(figure_eps_filename)
    except OSError as e:
      print >>sys.stderr, "Could not remove eps file"
    print 'Figure saved as {0}'.format(figure_pdf_filename)

def createFigsBenchmark(redback_dir,# redback directory
                        benchmark_dir_short_name, # benchmark subdirectory (e.g 'benchmark_7_HM')
                        csv_shortfilename, # short filename for csv file to read (note: same name in results and gold subdirs)
                        x_properties_list, # list of properties to plot in x (e.g ['time','time'])
                        y_properties_list, # list of properties to plot in y (e.g ['stress_xx','p0'])
                        do_show=False): # flag to show figure or save it to file
  ''' Generic function to produce all figures for a given benchmark '''
  results_dir = os.path.join(redback_dir,'tests',benchmark_dir_short_name)
  gold_dir = os.path.join(results_dir,'gold')
  results_csv_filename = os.path.join(results_dir,csv_shortfilename)
  gold_csv_filename = os.path.join(gold_dir,csv_shortfilename)
  figures_dir = os.path.join(redback_dir,'doc','theory','figures')

  csv_results_data = parseCsv(results_csv_filename)
  csv_gold_data = parseCsv(gold_csv_filename)

  labels = {
    'time':'Time',
    'stress_xx':'Stress xx',
    'stress_zz':'Stress zz',
    'p0':'Pore pressure',
  }

  nb_props = len(x_properties_list)
  assert len(y_properties_list)==nb_props
  for i_prop in range(nb_props):
    x_property_name = x_properties_list[i_prop]
    if x_property_name in labels:
      x_property_label = labels[x_property_name]
    else:
      error_msg = 'You must edit utilities.py and add a label for property "{0}"'.format(x_property_name)
      raise Exception, error_msg
    y_property_name = y_properties_list[i_prop]
    if y_property_name in labels:
      y_property_label = labels[y_property_name]
    else:
      error_msg = 'You must edit utilities.py and add a label for property "{0}"'.format(y_property_name)
      raise Exception, error_msg

    fig_rootshortfilename = '{0}_{1}_{2}'.format(benchmark_dir_short_name,os.path.splitext(csv_shortfilename)[0],y_property_name)
    plotFigure(analytical_x_data=csv_gold_data[x_property_name],
               analytical_y_data=csv_gold_data[y_property_name],
               numerical_x_data=csv_results_data[x_property_name],
               numerical_y_data=csv_results_data[y_property_name],
               figure_rootfilename=os.path.join(figures_dir,benchmark_dir_short_name,fig_rootshortfilename),
               x_label=x_property_label,
               y_label=y_property_label,
               do_show=do_show)

if __name__ == "__main__":
  # test that parser works
  home_dir = expanduser('~')
  input_csv_filename = os.path.join(home_dir,'projects','redback','tests','gravity','gold','gravity_poro_stress.csv')
  data_all = parseCsv(input_csv_filename)
  data_short = parseCsv(input_csv_filename, ['time','p0','zdisp'])
  assert sorted(data_all.keys()) == ['p0', 'stress_xx', 'stress_yy', 'stress_zz', 'time', 'zdisp']
  assert sorted(data_short.keys()) == ['p0', 'time', 'zdisp']
  assert data_all['time'] == [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0]
  assert data_short['time'] == [0.0, 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0, 10.0]
  print 'Parser works OK!'
