CXXFLAGS = -Wall -Wextra -O2 -std=c++17

quadtree: FINALPACKAGE_ADS.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

image.txt: original_image.png
	python3 image_to_pixel.py

# interactive: build the tree, pick view/cut/zoom, then render the result
run: quadtree image.txt
	./quadtree
	python3 pixel_to_image.py

# view must give back exactly the original pixels (the merge is lossless),
# and zooming into a quadrant must match that quadrant scaled 2x
test: quadtree image.txt
	printf '1\nn\n' | ./quadtree > /dev/null
	cmp -s op_pixel.txt image.txt && echo "view round-trip OK"
	printf '3\n2\nn\nn\n' | ./quadtree > /dev/null
	python3 -c "from PIL import Image; \
	o=Image.open('original_image.png').crop((0,0,128,128)).resize((256,256),Image.NEAREST); \
	z=[int(l) for l in open('op_pixel.txt')]; \
	assert list(o.getdata())==z; print('zoom top-left OK')"

clean:
	rm -f quadtree

.PHONY: run test clean
