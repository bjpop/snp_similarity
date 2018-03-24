import csv
import sys
from collections import defaultdict 

reader = csv.reader(sys.stdin)

header = next(reader)

print("Num samples: {}".format(len(header) - 3))

gt_counts = defaultdict(int)

num_snps = 0
for row in reader:
    num_snps += 1
    gts = row[3:]
    for gt in gts:
        gt_counts[gt] += 1

print("Num SNPs: {}".format(num_snps)) 

for gt,count in gt_counts.items():
    print("{}: {}".format(gt, count))
