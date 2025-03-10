# Work in Progress

Nothing to see here, yet; move along!

## Conventions

- All definitions inside `MY` namespace
  - Macros have a `MY_` prefix

- Implementations aim to be safe(r) by default
    - Use of assertions
    - Undefined behavior is avoided
    - Safe-guards can be bypassed if needed

- Pointers are preferred over references
  - `nullptr` typically indicates failure

- Avoid exceptions

- Avoid dynamic initialization

- Marginal use of the C++ standard library
  - Primarily `<type_traits>` and `<memory>` utilities
  - Avoid RTTI

- Internal functions and members have a trailing underscore `_`
  - They are not fully encapsulated

- Type and function names loosely resembles C++ standard library naming
  - `size` refers to element count rather than the actual size in bytes
  - `last` refers to an end iterator pointing past the last element
  - `Vector` refers to an array with dynamic element count
