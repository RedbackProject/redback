''' Script to produce documentation figures related to benchmarks 8 (THM).
    Pictures are generated in the "figures" subdirectory directly, ready for LaTeX compilation
    Requirements:
      * you MUST run the tests first as the script reads csv files from the tests results
'''


import os, sys, csv
from os.path import expanduser

from utilities import parseCsv, plotFigure, createFigsBenchmark

def createFigsBench8Elastic(redback_dir=os.path.join(expanduser('~'),'projects','redback'),
                            do_show=False):
  ''' Create figures for benchmark 8 elastic '''
  benchmark_dir_short_name = 'benchmark_8_THM'

  csv_shortfilename = 'bench_THM_elastic.csv'
  x_properties_list = ['time',     'time',     'time']
  y_properties_list = ['stress_xx','stress_zz','p0']
  createFigsBenchmark(redback_dir, benchmark_dir_short_name, csv_shortfilename,
                      x_properties_list, y_properties_list, do_show=do_show)



if __name__ == '__main__':
  createFigsBench8Elastic(do_show=True)
  print 'Finished'
