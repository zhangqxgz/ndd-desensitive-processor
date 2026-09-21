#!/usr/bin/env python3
import re
import sys


def main():
    if len(sys.argv) != 4:
        print("usage: make_def.py <dumpbin_exports.txt> <out.def> <libname>")
        return 1
    src, dst, libname = sys.argv[1], sys.argv[2], sys.argv[3]
    names = []
    pattern = re.compile(r"^\s+\d+\s+[0-9A-Fa-f]+\s+[0-9A-Fa-f]+\s+(\S+)")
    with open(src, "r", encoding="utf-8", errors="ignore") as fh:
        for line in fh:
            match = pattern.match(line)
            if not match:
                continue
            name = match.group(1)
            if name in ("name",):
                continue
            names.append(name)
    if not names:
        print("no exports parsed")
        return 2
    with open(dst, "w", encoding="ascii", newline="\r\n") as fh:
        fh.write("LIBRARY %s\n" % libname)
        fh.write("EXPORTS\n")
        for name in names:
            fh.write("    %s\n" % name)
    print("wrote %d exports to %s" % (len(names), dst))
    return 0


if __name__ == "__main__":
    sys.exit(main())
