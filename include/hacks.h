

#define UncheckedPtrInit(type, variable, init) \
  type variable = 0; \
  _Unchecked { variable = init; }

#define UncheckedBoundsInit(type, variable, bounds, init) \
  type variable : bounds = 0; \
  _Unchecked { variable = init; }
