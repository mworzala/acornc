I realized it was probably worth going through more thoroughly 
what I want to include eventually if I get to it.

The inspiration comes from a mix of Go and Zig (and maybe some Rust?).


### Random thoughts
- A way to generate a C header for an acorn project would be cool. 
  Would make it very simple to integrate with C, which would be convenient for working on the self-hosted compiler (eg could do small bits in acorn).
- Similarly, the ability to import a C header would be good. Same concept as zig @cImport.


### Rough plan
- Keep working on the parser until it can parse:
  - ✓ structs (c structs. just fields/types, no methods)
  - ✓ enums (c enums. just values, no methods or tagging)
  - ✓ field access
  - ✓ functions (name params type body, no methods)
  - ✓ calls
  - ✓ arithmetic / comparison / logic
  - ✓ if/else
  - ✓ while (just while, no for, no break/continue)
  - ✓ return
