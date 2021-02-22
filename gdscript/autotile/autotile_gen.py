from PIL import Image

## Auto tile generator.
## Author : http://github.com/ThakeeNathees/

## ----------------------------------------------------------------------------
##         USER DATA AREA (change the below values for your need)
## ----------------------------------------------------------------------------

TILE_SIZE = 16 ## 16 x 16 tiles.

## A png image of tiles including middle, top, left, right, bottom, and
## other 4 diagonal tiles. with dimention as TILE_SIZE with no spaces
## between tiles.
'''
  .----+ +----+ +----.   ^
  |    | |    | |    |   |  16 px * 16 px
  +----+ +----+ +----+   v
  +----+ +----+ +----+
  |    | |    | |    |
  +----+ +----+ +----+
  +----+ +----+ +----+
  |    | |    | |    |
  .____| |____| |____.
'''
TILE_PATH = 'tile.png'

## The image name to generate.
OUTPUT_PATH = 'autotile.gen.png'

## If COLOR_CORNERS is true, it'll color corners with CORNER_COLOR
## for a reference to user to paint out the corners.
COLOR_CORNERS = True

## Corner color (r, g, b, a). User should rewrite the corners
## once it generated.
CORNER_COLOR = (0, 0, 0, 255)

## ----------------------------------------------------------------------------
##                    INTERNAL DO NOT TOUCH ANYTHING BELOW
## ----------------------------------------------------------------------------

H_TILES_COUNT = 11 ## Generated horizontal tile count.
V_TILES_COUNT = 5  ## Generated verticle tile count.

CORNER_TL = 0b1
CORNER_TR = 0b10
CORNER_BL = 0b100
CORNER_BR = 0b1000

CORNER_BOX_SIZE = 2 ## n*n box.

## Check if the point (x, y) in the given corner. corner SHOULD be one of
## the above 4 and x, y should be 0..TILE_SIZE.
def in_corner(point, corner):
	px, py = point
	
	if corner == CORNER_TL:
		return px < CORNER_BOX_SIZE and py < CORNER_BOX_SIZE
	if corner == CORNER_TR:
		return TILE_SIZE - CORNER_BOX_SIZE <= px and \
			   py < CORNER_BOX_SIZE
	if corner == CORNER_BL:
		return px < CORNER_BOX_SIZE and \
			   TILE_SIZE - CORNER_BOX_SIZE <= py
	if corner == CORNER_BR:
		return TILE_SIZE - CORNER_BOX_SIZE <= px and \
			   TILE_SIZE - CORNER_BOX_SIZE <= py
	return False

## Copy a tile at tile coordinate source (x, y) to tile coordinate dst.
## tile_img is the image loaded from TILE_PATH and img is the current
## image buffer. if corner is not 0 it'll mark a tile connection corner.
## provide corner as bit mask value of corners (use CORNER_...).
'''
   CORNER_TL +------+ CORNER_TR
             |      |
             |      |
  CORNER_BL  +------+ CORNER_BR
'''
def copy_tile(dst, source, img, tile_img, corner = 0):
	dx, dy = dst
	sx, sy = source
	
	for i in range(TILE_SIZE):
		for j in range(TILE_SIZE):
			px = tile_img.getpixel((TILE_SIZE*sx + i, TILE_SIZE*sy + j))
			
			## check for corners.
			if COLOR_CORNERS:
				for shift in range(0, 4):
					if (corner & (1 << shift)) and in_corner((i, j), (1 << shift)):
						px = CORNER_COLOR
						break
			
			img.putpixel((TILE_SIZE*dx + i, TILE_SIZE*dy + j), px)
	
def main():
	w = TILE_SIZE * H_TILES_COUNT
	h = TILE_SIZE * V_TILES_COUNT
	
	img = Image.new('RGBA', (w, h), color = (0, 0, 0, 0))
	tile_img = Image.open(TILE_PATH).convert('RGBA')
	
	## Write the first 3*3 tiles on the image.
	for i in range(TILE_SIZE * 3):
		for j in range(TILE_SIZE * 3):
			img.putpixel((i, j), tile_img.getpixel((i, j)))
	
	## Now the verticle 1*3 tiles next to it.
	for i in range(TILE_SIZE):
		for j in range(TILE_SIZE*3):
			_i = i if (i < TILE_SIZE/2) else (TILE_SIZE*2) + i
			px = tile_img.getpixel((_i, j))
			dst = (TILE_SIZE*3 + i, j)
			img.putpixel(dst, px)
	
	## Now the horizontal 3*1 tiles below.
	for i in range(TILE_SIZE*3):
		for j in range(TILE_SIZE):
			_j = j if (j < TILE_SIZE/2) else (TILE_SIZE*2) + j
			px = tile_img.getpixel((i, _j))
			dst = (i, TILE_SIZE*3 + j)
			img.putpixel(dst, px)
	
	## The single tile at (3, 3)
	copy_tile((3, 3), (1, 0), img, tile_img)
	
	## -- Next squire. --
	
	## It's left.
	copy_tile((4, 0), (0, 0), img, tile_img, CORNER_BR)
	copy_tile((4, 1), (0, 1), img, tile_img, CORNER_BR)
	copy_tile((4, 2), (0, 1), img, tile_img, CORNER_TR)
	copy_tile((4, 3), (0, 2), img, tile_img, CORNER_TR)
	
	## It's right.
	copy_tile((7, 0), (2, 0), img, tile_img, CORNER_BL)
	copy_tile((7, 1), (2, 1), img, tile_img, CORNER_BL)
	copy_tile((7, 2), (2, 1), img, tile_img, CORNER_TL)
	copy_tile((7, 3), (2, 2), img, tile_img, CORNER_TL)
	
	## It's top.
	copy_tile((5, 0), (1, 0), img, tile_img, CORNER_BR)
	copy_tile((6, 0), (1, 0), img, tile_img, CORNER_BL)
	
	## It's bottom.
	copy_tile((5, 3), (1, 2), img, tile_img, CORNER_TR)
	copy_tile((6, 3), (1, 2), img, tile_img, CORNER_TL)
	
	## It's middle.
	copy_tile((5, 1), (1, 1), img, tile_img, CORNER_BR)
	copy_tile((6, 1), (1, 1), img, tile_img, CORNER_BL)
	copy_tile((5, 2), (1, 1), img, tile_img, CORNER_TR)
	copy_tile((6, 2), (1, 1), img, tile_img, CORNER_TL)
	
	## -- Next verticle and horizontal T tiles --
	
	## It's left verticle T tile.
	copy_tile((8, 0), (1, 0), img, tile_img, CORNER_BL | CORNER_BR)
	copy_tile((8, 1), (1, 1), img, tile_img, CORNER_BL | CORNER_BR)
	copy_tile((8, 2), (1, 1), img, tile_img, CORNER_TL | CORNER_TR)
	copy_tile((8, 3), (1, 2), img, tile_img, CORNER_TL | CORNER_TR)
	
	## It's bottom horizontal |- tile.
	copy_tile((4, 4), (0, 1), img, tile_img, CORNER_TR | CORNER_BR)
	copy_tile((5, 4), (1, 1), img, tile_img, CORNER_TR | CORNER_BR)
	copy_tile((6, 4), (1, 1), img, tile_img, CORNER_TL | CORNER_BL)
	copy_tile((7, 4), (2, 1), img, tile_img, CORNER_TL | CORNER_BL)
	
	## The single insided tile.
	copy_tile((8, 4), (1, 1), img, tile_img, CORNER_TL | CORNER_TR | \
	                                         CORNER_BL | CORNER_BR)
	
	## -- Next 2 diagonal tiles.
	
	## '\' diagonal tile.
	copy_tile((9, 0), (1, 1), img, tile_img, CORNER_TR | CORNER_BL)
	
	## '/' diagonal tile.
	copy_tile((10, 0), (1, 1), img, tile_img, CORNER_TL | CORNER_BR)
	
	## -- Finally the last 4 3 cornered tiles.
	
	copy_tile(( 9, 2), (1, 1), img, tile_img, CORNER_TL | CORNER_TR | CORNER_BL)
	copy_tile((10, 2), (1, 1), img, tile_img, CORNER_TL | CORNER_TR | CORNER_BR)
	copy_tile(( 9, 3), (1, 1), img, tile_img, CORNER_TL | CORNER_BL | CORNER_BR)
	copy_tile((10, 3), (1, 1), img, tile_img, CORNER_TR | CORNER_BL | CORNER_BR)
	
	## -- DONE SAVE THE IMAGE BUFFER -- 
	img.save(OUTPUT_PATH)
	print('[autotile_gen.py] "%s" generated!' % OUTPUT_PATH)

if __name__ == '__main__':
	main()
