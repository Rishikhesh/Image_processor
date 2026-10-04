from PIL import Image

with open("op_pixel.txt") as file2:
    pix_val = [int(i) for i in file2]

# the C++ program always writes a 256x256 greyscale image, row by row
image_out = Image.new("L", (256, 256))
image_out.putdata(pix_val)
image_out.save("edited_image.png")
print("saved edited_image.png")
