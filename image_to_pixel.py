import sys
from PIL import Image

# usage: python3 image_to_pixel.py [image]   (default original_image.png)
# the quadtree program expects a 256x256 greyscale image, so convert anything else to that
im = Image.open(sys.argv[1] if len(sys.argv) > 1 else "original_image.png").convert("L").resize((256, 256))
if len(sys.argv) > 1:
    im.save("original_image.png")  # a new image becomes the original the other steps compare against

with open("image.txt", "w") as file1:
    for i in im.getdata():
        file1.write(str(i))
        file1.write('\n')
