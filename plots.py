import csv
import sys
import plotly as py
import plotly.graph_objs as go
from plotly.graph_objs import Scatter, Layout

reader = csv.reader(sys.stdin)
header = next(reader)

samples = header[1:]

data = []
non_zeros = []
for row in reader:
    data.append(row[1:])
    for val in row[1:]:
        val = float(val)
        if val > 0:
            non_zeros.append(val)

trace = go.Heatmap(z=data,
                   x=samples,
                   y=samples, colorscale='Greys')
data=[trace]
py.offline.plot(data, filename='fcctx_snp_similarity.html')

data = [go.Histogram(x=non_zeros)]
py.offline.plot(data, filename='fcctx_snp_histo.html')
