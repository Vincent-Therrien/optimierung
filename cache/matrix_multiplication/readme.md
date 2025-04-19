# Cache-Optimized Matrix Multiplication

The program performs matrix multiplication:

```math
\begin{bmatrix}
a & b \\
c & d
\end{bmatrix}
\cdot
\begin{bmatrix}
e & f \\
g & h
\end{bmatrix}
=
\begin{bmatrix}
ae + bg & af + bh \\
ce + dg & cf + dh
\end{bmatrix}
```

**four algorithms** are implemented:

- `1_vanilla` (naïve implementation): implemented with three nested loops. Accessing the columns of
  the right-hand matrix result in numerous cache misses.
- `2_vanilla_transposition`: implemented with three nester loops, but the right-hand matrix is
  naïvely transposed before multiplication.
- `3_cache_optimized_transposition`: Same as the previous implementation, but transposition is
  performed recursively in a cache-oblivious way.
- `4_cache_oblivious`: Matrix multiplication performed recursively in a cache-obvious way.


## Build

```
> mkdir build
> cd build
> cmake ..
> cmake --build . --config Release
```

You can then execute the program (`Release/main` on Linux and `Release\main.exe` on W*ndows).
