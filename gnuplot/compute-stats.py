#!/usr/bin/env python3

import csv
import os
import statistics
import sys

def read_data(filename):
    data = {}

    fp = open(filename)
    reader = csv.reader(fp)
    header = next(reader)[1:]

    for row in reader:
        row = list(map(int, row))
        if row[0] not in data:
            data[row[0]] = dict((h, []) for h in header)
        for i, x in enumerate(row[1:]):
            data[row[0]][header[i]].append(x)

    fp.close()

    return header, data

def compute_stats(data):
    for n in data:
        for key in data[n]:
            if len(data[n][key]) == 1:
                data[n][key] = [data[n][key][0]]*5
                continue
            data[n][key] = [
                min(data[n][key]),
                *statistics.quantiles(data[n][key], method='inclusive'),
                max(data[n][key]),
            ]

def output(header, data, filename):
    fp = open(filename, 'w')

    out = csv.writer(fp, delimiter=' ')

    out_header = ['#', 'n']
    for h in data[list(data.keys())[0]]:
        out_header = [
            *out_header,
            f'{h}_min',
            f'{h}_first_quartile',
            f'{h}_median',
            f'{h}_third_quartile',
            f'{h}_max',
        ]
    out.writerow(out_header)

    for n in data:
        row = []
        for h in header:
            row = [*row, *data[n][h]]
        out.writerow([n, *row])

    fp.close()

def main(filename):
    header, data = read_data(filename)
    compute_stats(data)
    out_filename = os.path.join(
        os.path.dirname(filename),
        '.'.join([os.path.basename(filename).rsplit('.', 1)[0], 'dat'])
    )
    output(header, data, out_filename)

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print(f"Usage: {sys.argv[0]} [path to csv stats file]", file=sys.stderr)
        exit(1)
    main(sys.argv[1])
