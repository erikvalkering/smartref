import sys
import re
import pathlib


def search_filename(filename, search_paths):
    for path in search_paths:
        if (path / filename).exists():
            return path / filename

    return None


def singlify(filename, search_paths, cache=set()):
    search_paths = [filename.parent] + search_paths

    with filename.open() as f:
        for line in f.readlines():
            # Strip off newline
            line = line[:-1]

            if '#pragma once' in line:
                continue

            m = re.match('.*#include.*[<"]([^>"]*)[>"].*', line)

            if not m:
                yield line
                continue

            include_file = search_filename(m.group(1), search_paths)

            # If include file not on search path, it might be a system header
            if include_file is None:
                yield line
            elif include_file in cache:
                yield '// skipping ' + line
            else:
                cache.add(include_file)
                yield '// ' + line
                for x in singlify(include_file, search_paths, cache):
                    yield x


def main():
    filename = pathlib.Path(sys.argv[1])
    search_paths = [pathlib.Path(sys.argv[2] if len(sys.argv) >= 3 else '')]

    for line in singlify(filename, search_paths):
        print(line)

if __name__ == '__main__':
    main()
