import sys
import re
import pathlib


def singlify(filename, cache=set()):
    if filename in cache:
        return

    cache.add(filename)

    with filename.open() as f:
        for line in f.readlines():
            # Strip off newline
            line = line[:-1]

            m = re.match('.*#include.*"([^"]*)".*', line)

            if not m:
                yield line
                continue

            include_file = pathlib.Path(m.group(1))

            for x in singlify(include_file, cache):
                yield x


def main():
    filename = pathlib.Path(sys.argv[1])
    for line in singlify(filename):
        print(line)

if __name__ == '__main__':
    main()
