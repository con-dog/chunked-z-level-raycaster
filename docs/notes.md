## Goal

## Issues
### Window size relationship to FPS
- My current FPS is tied to screen size, and at full Macbook Pro M1 screen size, I get ~50-60 FPS
    - Not good! thats just for a single z-level
    - Can optimize this by doing ray calculations in a smaller window, and then scaling up the results to the screen size

### Raycasting multiple z-levels
- I want multiple Z levels (like minecraft, 100's)
  - Without optimizations, this means casting multiple vertical rays for every single horizontal ray
  - I currently cast 160 rays for a single z-level
  - This is a shitload of rays (160 x N), will cause a massive FPS drop
  - Can optimize this by having a vertical FOV half that of the horizontal FOV
    - Also if a wall has been hit on this z-level, we can start casting rays above the wall only (and similarly below)
    - This will reduce the number of rays casted by a lot
    - This will also allow for more z-levels to be rendered at once
  - An even better optimization is to use run-length encoding to store the walls in a z-level
    - This will allow for a constant time lookup of walls in a z-level
    - This will also allow for more z-levels to be rendered at once
    - This will also allow for more complex levels to be rendered


Actually an even better solution is to use bit packing to represent the wall map:

```c
map [16][16] = {
  0b1110000011100001, ...
}

// coord 1,1,1
{1, 1, 1} = 0b 1011 0001 0001 0001; // [0-3] = 0000, [4-7] = 0001, [8-11] = 0001, [12-15] = 0001
// [0-3] =
// 0 -> unused
// 1 -> texture-type ... regular vs special (animated, interactable, etc)
// 2 -> solid wall / passable wall
// 3 -> solid floor / passable floor

// [4-7] = x pos in chunk
// [8-11] = y pos in chunk
// [12-15] = z pos in chunk

// Then we use a hashmap to store entries
struct Entry {
    uint16_t coords;    // [4 unused][4 bits x][4 bits y][4 bits z]
    uint16_t texture;   // Texture ID
}

// Not siure about hashmap implementation yet.

Then we have Chunk structures:
struct Chunk {
    uint16_t coords;    // [8 unused][8 bits x][8 bits y][8 bits z]
    uint16_t hash_table[512]; // 512 entries
    Entry *entries;      // Array of entries
    size_t length;    // Number of entries
}

// Hash function
uint16_t hash_coords(uint8_t x, uint8_t y, uint8_t z) {
    return (x * 7 + y * 13 + z) & 0x1FF;
}

// Insertion
uint16_t idx = hash_coords(x,y,z);
while(hash_table[idx] != 0) {  // 0 means empty
    idx = (idx + 1) & 0x3FF;    // Wrap around
}
hash_table[idx] = wall_index;

// Lookup
uint16_t idx = hash_coords(x,y,z);
while(hash_table[idx] != 0) {
    if(walls[hash_table[idx]].coords == target_coords) {
        return &walls[hash_table[idx]];
    }
    idx = (idx + 1) & 0x3FF;
}

// Then we have a world structure
struct World {
    struct Chunk *chunks;  // Array of chunks, only store chunks with entries
    size_t length;         // Number of chunks
}

```

For now, for simplicity, maybe do store