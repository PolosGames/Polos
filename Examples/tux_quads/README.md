# Tux quads

500,000 Tux-textured quads at randomly sampled points on a sine wave, viewed
from directly above. The random seed keeps the layout repeatable. Mouse look
and movement have smooth transitions.

- Hold **right mouse button** to capture the cursor and look around.
- While holding it, use **W/A/S/D** to move, **Space** to rise, and **Left Ctrl** to descend.
- Release the button to restore the cursor and smoothly stop moving.
- Press **Escape** to close the example.

From the repository root:

```sh
cmake --preset linux-debug
cmake --build out/build/linux-debug --target tux_quads
./out/build/linux-debug/Examples/tux_quads/tux_quads
```
