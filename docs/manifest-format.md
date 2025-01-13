# Location

Keep all manifests in the manifests folder, in a flat structure.

## Format

```json
{
  "manifest_version": 1,
  "metadata": {
    "name": "",
    "description": "",
    "version": "1.0.0",
    "author": "",
    "contact": "",
    "homepage_url": ""
  },
  "data": [
    {
     "name": "water-a",
      "src_directory": "assets/textures/64x64/water/a",
      "frame_src_files": ["water-b.png", "water-c.png"],
      "is_animated": true,
      "frame_duration": "0.8",
      "is_looping": true,
      "category": "water",
      "collision_mode": "0b001",
      "surface_type": "0b001",
      "expected_pixel_width": 64,
      "expected_pixel_height": 64,
      "use_scale_mode_nearest": true
    }
  ],
  "external_manifests": [
    {"name": "...", "version": "", "path": "..."}
  ]
}
```



### Fields

surface_type and collision_mode are binary string bit fields

eg: `"0b110"` means `Ceiling & Wall`

See the following table:
```markdown
| Ceiling | Wall | Floor | Surface Type   / Collision Mode |
|---------|------|-------|---------------------------------|
| 0       | 0    | 0     | None                            |
| 0       | 0    | 1     | Floor                           |
| 0       | 1    | 0     | Wall                            |
| 0       | 1    | 1     | Wall & Floor                    |
| 1       | 0    | 0     | Ceiling                         |
| 1       | 0    | 1     | Ceiling & Floor                 |
| 1       | 1    | 0     | Ceiling & Wall                  |
| 1       | 1    | 1     | Ceiling & Wall & Floor          |
```
