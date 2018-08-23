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
                    continue

                for x in singlify(m.group(1), cache):
                    yield x
            else:
                yield line


def main():
    filename = sys.argv[1]
    for line in singlify(filename):
        print(line)

if __name__ == '__main__':
    main()
