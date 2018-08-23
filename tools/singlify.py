import sys


def singlify(filename):
    with open(filename) as f:
        for line in f.readlines():
            # Strip off newline
            line = line[:-1]

            if '#include' in line:
                # TODO
                continue

            yield line


def main():
    filename = sys.argv[1]
    for line in singlify(filename):
        print(line)

if __name__ == '__main__':
    main()
