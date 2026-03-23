# Mixed-Version Runtime Compatibility Notes

## Summary

`netver` is the neth wire protocol version. It is **not** the same thing as the ProviewR release version, and it is **not** a class version number.

The runtime code says to increase `net_cVersion` only when an **incompatible protocol change** is made in `rt_net.x`. Cached classes are a separate mechanism used when two nodes have different class definitions.

Relevant code:

- `src/lib/rt/src/rt_net.x`
  - `net_cVersion = 8`
  - `net_cFirstCclassVersion = 8`
  - `net_sNode` exchanges `netver`, not full ProviewR release version
- `src/exe/rt_neth/src/rt_neth.c`
  - decides compatibility from `mp->node.netver`
- `src/lib/rt/src/rt_cmvolc.c`
  - fetches cached classes when classes are not equal
- `src/lib/rt/src/rt_ndc.c`
  - converts data between different class versions

## When To Increment `netver`

Increment `netver` when you change the **network protocol** in a way that old and new runtimes cannot safely talk to each other without explicit compatibility handling.

Typical reasons:

- changing message layouts in `rt_net.x`
- changing message semantics in an incompatible way
- adding protocol features that require negotiation and older nodes cannot safely ignore

Do **not** increment `netver` just because a class changed.

## What Cached Classes Actually Do

Cached classes are there so mixed-version nodes can still exchange object/subscription data even when the class definitions differ.

Important details from `rt_ndc.c`:

- attributes are matched by `aix`, not by name
- if an attribute does not exist on the remote side, the local attribute is zeroed
- if an attribute exists remotely but not locally, the extra remote data is ignored/zeroed on conversion
- type conversion uses `conv_GetIdx()`
- if no valid conversion exists, the value may be zeroed or conversion can fail depending on path

This means a class changing does **not** automatically require a `netver` bump.

## Safe Changes For Mixed-Version Runtime

These are the safest kinds of class changes when older and newer runtimes must coexist:

- add new attributes at the end of a class
- add new attributes without changing existing attribute `aix`
- keep existing attribute types and meanings unchanged
- add new nested/class attributes at the end, with the same caution for their internal attributes
- extend arrays only if zero/default values in the new elements are acceptable

Why these are safer:

- the conversion logic matches on `aix`
- missing attributes are handled by zero-filling
- unchanged existing attributes continue to map cleanly

## Risky Changes

These are the ones most likely to cause problems in mixed-version operation:

- reordering attributes if that changes `aix`
- inserting new attributes in the middle if that changes later `aix` values
- deleting and recreating attributes so they get new `aix`
- changing the type of an existing attribute
- changing an attribute from scalar to array, array to scalar, or changing element semantics
- changing an existing attribute's meaning while keeping the same name or position
- changing class-vs-non-class structure of an existing attribute
- changes involving pointers/private-pointer-sensitive data

Important: the runtime matches by `aix`, not by attribute name. So "same name" is not enough if the generated class metadata changed identity.

## Practical Rule Of Thumb

If you add something to `PnDevice` and leave the old attributes intact, mixed-version runtime communication should normally still work through cached-class conversion.

If you change existing attributes in incompatible ways, then you should expect mixed-version problems even if `netver` stays the same.

## Release Numbering

The runtime compatibility logic discussed here does not compare full ProviewR release strings such as `6.1.0`, `6.1.3`, or `7.0.0`.

For this path, the important version is `netver`.

The historical release encoding in `pwr_uPwrVersion` names the third component `Update`, while newer release semantics may refer to it as `Patch`, but that is separate from the neth protocol compatibility check.
