# Runtime module header layout

Each module separates its consumer API from its implementation:

```text
<module>/
  include/polos/<module>/   Public headers, included as polos/<module>/...
  src/                     Implementation files and private headers
    <subsystem>/           Related .cpp and .hpp files live together
  test/                    Module tests
```

Public headers must compile without any module's `src/` include directory. Keep
data types needed by consumers public; use forward declarations and out-of-line
definitions when ownership of an internal type would otherwise expose its header.
Error types and dependencies required by public/header-only APIs remain public.

Within a module, include private headers relative to `src/`, for example
`#include "vulkan/render_context.hpp"`. Include public headers using their full
`polos/<module>/...` path, including from the module's own implementation. Do not
reach into another module's `src/` directory.

Include guards mirror the path: `POLOS_<MODULE>_<PATH>_HPP` for public headers and
`POLOS_<MODULE>_SRC_<PATH>_HPP` for private headers, with separators replaced by
underscores. Keep the opening directives and closing comment consistent.

`define_polos_module` exposes `include/` to consumers and adds `src/` only as a
PRIVATE include directory on compiled targets. It tracks both public and private
headers for IDEs and build-info dependencies, including when `rendering` and
`rendering_impl` share the same module directory. Tests may use their own module's
private headers; this does not make those headers part of the consumer API.
