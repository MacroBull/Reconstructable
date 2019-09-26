# Reconstructable
C++11 Reconstructable &amp; ClonePtr template for non-copyable classes

Define **RECONSTRUCT COPY** as **call constructor with arguments it is constructed with** and **RECONSTRUCT** as **call constructor with arguments it is constructed with in-place**.

`ClonePtr<T>` manages a non-copyable class instance and assign it the capability of `reconstruct-copy`.

`Reconstructable<T>` makes an inherit class and assign it the capability of `reconstruct`.

```C++
ClonePtr<NonCopyable> x(1, 2);
x.a = 3;
x.b = 4;
auto                  y = x; // with {1, 2}
auto                  z = y; // with {1, 2}

Reconstructable<NonCopyable> x(1, 2);
x.a = 3;
x.b = 4;
x.reconstruct(); // re-inited with {1, 2}
```
