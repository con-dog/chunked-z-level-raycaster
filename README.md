# A 2.5D Raycasting Game Engine

A brand-new, old-school game-engine inspired by the likes of [Wolfenstein 3D (1992)](https://en.wikipedia.org/wiki/Wolfenstein_3D) and [Ultima Underworld: The Stygian Abyss (1992)](https://en.wikipedia.org/wiki/Ultima_Underworld:_The_Stygian_Abyss). Uses simple raycasting techniques and a uniform cell grid to render 3D.

## Currently in Phase 6:
Vertical walls, chunks, representing a wall with 1 bit, and 2 bytes representing a vertical stack of 16 walls. Huge performance boosts (memory and speed) - But, having issues with rendering logic for verticals

![Almost, still have pathing issue](https://github.com/con-dog/chunked-z-level-raycaster/blob/40ce926e33ba22ae65f028dcd8b1114dda974217/_media/phase-6/2.png)
![Drawing but not raycasting the full length](https://github.com/con-dog/chunked-z-level-raycaster/blob/40ce926e33ba22ae65f028dcd8b1114dda974217/_media/phase-6/3.png)
![Drawing in wrong order](https://github.com/con-dog/chunked-z-level-raycaster/blob/40ce926e33ba22ae65f028dcd8b1114dda974217/_media/phase-6/6.png)

### Optimisations in Phase 6
Example of a 16x16x16 chunk of a map, where a bit '1' in binary represents a wall, where '0' represents no wall

```c
uint16_t map_chunk[CHUNK_X][CHUNK_Y] = { // 0x000F means z [0-3] has walls, z [3-15] has no walls
    {0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0002, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0007, 0x0000, 0x0000, 0x0000, 0x0004, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0001, 0x0001, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x000F},
    {0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F, 0x000F}};
```

This is as memory efficient as I thought I could make a map chunk.

```plaintext
map_chunk_memory = 16x16x16 Bits
             = 512 bytes
```

The value 16 for a chunk dimension works nicely, as each actual wall (non-empty cells) will have an associated struct entry, and we can pack its x-y-z coordinate (0->15) into 4 bits per-axis, which means we can use a single uint16_t to represent the coordinate of the wall within a chunk, and have room to spare:

```c
typedef struct Wall
{
  uint16_t coord;      // coords in chunk - [4 flags/unused][4 bits x][4 bits y][4 bits z]
  uint16_t texture_id; // Texture ID -> Value of 0 means this isn't a wall, its empty
} Wall;
```

And for every wall there are 4 unused bits in the coord variable, that we can use as flags later on (collision modes, interactive wall etc). Could even use those 4 bits as part of the texture id, and drop the texture_id type to a uint8_t to save more memory if needed.

This is the memory footprint of 1 Wall:

```plaintext
wall_memory = 4 bytes
```

That means that a chunk completely full of walls takes:
```plaintext
full_chunk = 4 bytes x 512 bytes
           = 2048 bytes
           = 2.048 Kilobytes
```

So a full map of 100x100x10 full chunks is ~204.8mB without compression. 

Realistically however, most chunks ~80-90% will be sky (completely empty) and myabe each chunk on average utilises about 1-10% of its total space with walls. So that will look more like:
100x100x10x(0.01 | 0.1) =~ 2.048 <-> 20.48 mB uncompressed.



## Future Goals
- 100x100x10 chunks maps
- Texture Atlas, different tetxures per side of wall
- Lighting system
- Thin walls
- Destructible walls/buildable walls
- Remake / Demake a Pokemon town in this "engine"
- Add 8bit audio, music blocks/textures that make a sounds on collision
- Procedurally generated worlds
- Texture transparency (ray pass-through to next texture, for things like windows etc)
- Sprites are all square/rectangular like minecraft and always face the player, but can "change direction" by changing the texture currently displayed
- Separate drawing/raycasting logic from window width and height

## Previous phases
### Phase 5

Animated textures, floor/wall collisions based on surface manifest, walk through doors, and just for fun - sprites - fishing.
![Enter doorways](https://github.com/con-dog/2.5D-raycasting-engine/blob/bbd244dfad4aa5922a7ad20163d3f4f63874540f/_media/phase-5/doors.gif)
![Phase 5 fishing gif](https://github.com/con-dog/2.5D-raycasting-engine/blob/35efbffc349788171c625aecd9ae6a6f2db17518/_media/phase-5/fishing-time.gif)
![Phase 5 gif](https://github.com/con-dog/2.5D-raycasting-engine/blob/3e8615c0fbabc73b51672c1551a10ede91257603/_media/phase-5/phase-5.gif)
![Phase 5 image](https://github.com/con-dog/2.5D-raycasting-engine/blob/6f3023fa86f8d4a3338c96a77d64b92d55bdabc6/_media/phase-5/fishing-time.png)


### Phase 4 

![Phase 4 gif](https://github.com/con-dog/2.5D-raycasting-engine/blob/1401433f57d4c0c732b924adf9c13507f07d32c8/_media/phase-4/phase-4.gif)
![Phase 4 image](https://github.com/con-dog/2.5D-raycasting-engine/blob/1401433f57d4c0c732b924adf9c13507f07d32c8/_media/phase-4/phase-4.png)
![Example level](https://github.com/con-dog/2.5D-raycasting-engine/blob/5c857ba532ab42b13a76408c7c08f4a9628c7d98/_media/phase-4/example-level.png)

- Levels are represented in memory as Jagged Arrays (to save memory)
- Wall and Floor maps are just CSV's (So I can quickly make levels in Excel/LibreOffice)
- Wall and Floor textures are represented by strings so I can very quickly iterate
- Walls/Floors are separate CSV's (this needs runtime tests (todo) so memory access doesn't corrupt)
- Textures/Assets are declared via a manifest.json

### Phase 3

![Phase 3 gif](https://github.com/con-dog/2.5D-raycasting-engine/blob/5c857ba532ab42b13a76408c7c08f4a9628c7d98/_media/phase-3/Screen%20Recording%202025-01-06%20at%207.53.50%E2%80%AFPM.gif)
![Phase 3 image](https://github.com/con-dog/2.5D-raycasting-engine/blob/5c857ba532ab42b13a76408c7c08f4a9628c7d98/_media/phase-3/phase-3.png)

- Fixed visual artifacts and pixel densities

### Phase 2

![Phase 2 gif](https://github.com/con-dog/2.5D-raycasting-engine/blob/5c857ba532ab42b13a76408c7c08f4a9628c7d98/_media/phase-2/phase-2.gif)

- Wall collision detection
- Basic texture mapping for walls
- Very warped tetures with many artifacts

### Phase 1

![Phase 1 gif](https://github.com/con-dog/2.5D-raycasting-engine/blob/5c857ba532ab42b13a76408c7c08f4a9628c7d98/_media/phase-1/phase-1.gif)
![Phase 1 image](https://github.com/con-dog/2.5D-raycasting-engine/blob/5c857ba532ab42b13a76408c7c08f4a9628c7d98/_media/phase-1/phase-1.png)

- Very basic ray-casting
- No collision detection
