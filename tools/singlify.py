import sys
import re


def singlify(filename, cache=set()):
    if filename in cache:
        return

    cache.add(filename)

    with open(filename) as f:
        for line in f.readlines():
            # Strip off newline
            line = line[:-1]

            if '#include' in line:
                m = re.match('.*"([^"]*)".*', line)

                if not m:
                    yield line
                    continue

                include_file = m.group(1)

                for x in singlify(include_file, cache):
                    yield x
            else:
                yield line


def main():
    filename = sys.argv[1]
    for line in singlify(filename):
        print(line)

if __name__ == '__main__':
    main()
