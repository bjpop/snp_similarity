import csv
import sys

reader = csv.reader(sys.stdin)
header = next(reader)

samples = header[1:]

for sample1, row in enumerate(reader):
    for sample2, val in enumerate(row[1:]):
        val = float(val)
        if sample1 != sample2 and val >= 0.7:
            print("{}, {}, {:.2f}".format(samples[sample1], samples[sample2], val))
