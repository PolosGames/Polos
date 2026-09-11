#!/usr/bin/env python3
"""Flag modules that include another module's headers without declaring it.

Merging the engine into one shared library means a missing dependency no longer
fails at link time, only at compile time and only if the include path happens not
to be inherited from somewhere else. This catches the gap directly instead.
"""

import re
import sys
from pathlib import Path

MODULES_DIR = Path(__file__).resolve().parent.parent / "Source" / "Runtime" / "polos"

DEP_RE = re.compile(r"polos::(\w+)")
# the trailing slash keeps polos/polos_config.hpp and polos/polos_api.hpp out
INCLUDE_RE = re.compile(r'#\s*include\s*[<"]polos/(\w+)/')


def declared_deps(module_dir):
    """Module names named in the CMakeLists, whatever the DEPS keyword."""
    cmakelists = module_dir / "CMakeLists.txt"
    if not cmakelists.is_file():
        return set()

    names = set(DEP_RE.findall(cmakelists.read_text(encoding="utf-8")))
    # the _INTERFACE aliases point at the same module
    return {name.removesuffix("_INTERFACE") for name in names}


def included_modules(module_dir):
    """Map each other module included here to the files reaching for it."""
    found = {}
    for source in module_dir.rglob("*"):
        if source.suffix not in (".hpp", ".cpp") or not source.is_file():
            continue
        for name in set(INCLUDE_RE.findall(source.read_text(encoding="utf-8"))):
            found.setdefault(name, []).append(source)
    return found


def violations_for(module_dir, known_modules):
    name = module_dir.name
    declared = declared_deps(module_dir) | {name}

    found = []
    for included, sources in sorted(included_modules(module_dir).items()):
        if included in declared or included not in known_modules:
            continue
        found.append((name, included, sorted(sources)))
    return found


def main():
    modules = sorted(d for d in MODULES_DIR.iterdir() if (d / "CMakeLists.txt").is_file())
    known = {d.name for d in modules}

    failures = []
    for module_dir in modules:
        failures.extend(violations_for(module_dir, known))

    for name, included, sources in failures:
        rels = ", ".join(str(s.relative_to(MODULES_DIR.parent.parent.parent)) for s in sources)
        print(f"{name}: includes polos/{included}/ but does not declare polos::{included}")
        print(f"    {rels}")

    if failures:
        print(f"\n{len(failures)} undeclared module dependencies", file=sys.stderr)
        return 1

    print(f"checked {len(modules)} modules, no undeclared dependencies")
    return 0


if __name__ == "__main__":
    sys.exit(main())
