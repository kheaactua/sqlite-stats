Simple C functions to provide SQLite3 with a couple handle math/stat functions.

Functions include:
- Root Mean Squared: *e.g.* SELECT rms(height) FROM people;

## Building

On Linux, build with GCC:

```
gcc -fPIC -shared stats.c -o stats.so $(pkg-config --cflags sqlite3.pc) -lm
```

Only include the `pkg-config` if you're using a custom installed version of
SQLite.  If you omit it, it should default to a system install.

## Use:

In Sqlite3, load the functions with:

```
SELECT load_extension('stats');
```

Note: `load_extension` takes the full path (minus the suffix) to find the shared library
