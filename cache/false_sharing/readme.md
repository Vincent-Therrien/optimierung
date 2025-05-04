# False Sharing

False sharing is a loss in performance that occurs when multiple threads write into the same cache
line at the same time, which leads to cache evictions.


## Build

```
> mkdir build
> cd build
> cmake ..
> cmake --build . --config Release
```
