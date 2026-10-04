# Image processor (quadtree)

Stores a 256×256 greyscale image as a quadtree: uniform quadrants collapse into one leaf.
Supports view, cut (keep one quadrant) and zoom (blow a quadrant up to full size).

```
make run                              # image -> pixels -> quadtree menu -> edited_image.png
python3 image_to_pixel.py photo.jpg   # use your own image (converted to 256x256 grey)
make test                             # view is lossless, zoom matches a 2x crop
```
