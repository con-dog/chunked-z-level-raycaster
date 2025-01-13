# A 2.5D Raycasting Game Engine

A brand-new, old-school game-engine inspired by the likes of [Wolfenstein 3D (1992)](https://en.wikipedia.org/wiki/Wolfenstein_3D) and [Ultima Underworld: The Stygian Abyss (1992)](https://en.wikipedia.org/wiki/Ultima_Underworld:_The_Stygian_Abyss). Uses simple raycasting techniques and a uniform cell grid to render 3D.

## Currently in Phase 5:
Animated textures, floor/wall collisions, and just for fun - sprites.
![Phase 5 image](https://github.com/con-dog/2.5D-raycasting-engine/blob/6f3023fa86f8d4a3338c96a77d64b92d55bdabc6/_media/phase-5/fishing-time.png)
![Phase 5 gif](https://github.com/con-dog/2.5D-raycasting-engine/blob/3e8615c0fbabc73b51672c1551a10ede91257603/_media/phase-5/phase-5.gif)

## Future Goals
- Remake / Demake a Pokemon town in this engine
- Texture transparency (ray pass-through to next texture)
- Texture collision enabled bitfield for floor wall or ceiling via 0bXXX (similar to texture surface type) so you can have hitbox detection for walls, floors, and ceilings
- Sprites are all square/rectangular like minecraft and always face the player, but can "change direction" by changing the texture currently displayed
- Separate drawing/raycasting logic from window width and height

## Previous phases
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
