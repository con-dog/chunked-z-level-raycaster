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
      "name": "brick-a",
      "path": "assets/textures/64x64/bricks/brick-a.png",
      "category": "bricks",
      "surface_type": "0b110",
      "expected_pixel_width": 64,
      "expected_pixel_height": 64,
      "use_scale_mode_nearest": true,
      "is_collision_enabled": true
    }
  ],
  "external_manifests": [
    {"name": "...", "version": "", "path": "..."}
  ]
}
```

### Fields

surface_type is a binary string bit field

eg: `"0b110"` means `Ceiling & Wall`

See the following table:
```markdown
| Ceiling | Wall | Floor | Surface Type           |
|---------|------|-------|------------------------|
| 0       | 0    | 0     | None                   |
| 0       | 0    | 1     | Floor                  |
| 0       | 1    | 0     | Wall                   |
| 0       | 1    | 1     | Wall & Floor           |
| 1       | 0    | 0     | Ceiling                |
| 1       | 0    | 1     | Ceiling & Floor        |
| 1       | 1    | 0     | Ceiling & Wall         |
| 1       | 1    | 1     | Ceiling & Wall & Floor |
```
