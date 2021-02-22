tool
extends EditorScript

## -- README --
## First create a tile set using the editor, which will in later needed to
## extract the auto tile bit mask from. Now update the below vaues for your
## tile set and run the script and it'll print some code (see below).

var tile_size = 16
var h_tiles_count = 11  ## how many tiles are there horizontaly?
var v_tiles_count =  5  ## how many tiles are there vertically
var TEXTURE = "res://autotile.gen.png"

## Once you run the code copy all the values paste below.
## Replace "## -- AUTO GENERATED DO NOT EDIT --" part below. Once everything is
## done set the print_bit_mask_flag to false, and run the scritpt for different 
## TEXTURE it'll make and add TileMap node to the current scene.
var print_bitmask_flags = true

func _run():
	var scene:Node = get_scene()
	
	## ------------------------------------------------------------------------
	if print_bitmask_flags:
		var bit_mask_code = 'tile_set.autotile_set_bitmask(0, Vector2(%s, %s), %s)'
		var ref :TileMap = scene.get_node('RefTileMap')
		print('## -- AUTO GENERATED DO NOT EDIT --')
		for w in range(0, h_tiles_count):
			for h in range(0, v_tiles_count):
				var flag = ref.tile_set.autotile_get_bitmask(0, Vector2(w, h))
				print(bit_mask_code % [w, h, flag])
		return
	
	## ------------------------------------------------------------------------
	
	var tile_map :TileMap = TileMap.new()
	scene.add_child(tile_map)
	tile_map.set_owner(scene)
	tile_map.cell_size = Vector2(tile_size, tile_size)

	var tile_set := TileSet.new()
	tile_map.tile_set = tile_set
	
	var img := load(TEXTURE)
	tile_set.create_tile(0)
	
	tile_set.tile_set_texture(0, img)
	tile_set.tile_set_tile_mode(0, TileSet.AUTO_TILE)
	tile_set.tile_set_region(0, Rect2(Vector2.ZERO, img.get_size()))
	tile_set.autotile_set_size(0, Vector2(tile_size, tile_size))
	tile_set.autotile_set_bitmask_mode(0, TileSet.BITMASK_3X3_MINIMAL)
	
	
	## -- AUTO GENERATED DO NOT EDIT --
	tile_set.autotile_set_bitmask(0, Vector2(0, 0), 432)
	tile_set.autotile_set_bitmask(0, Vector2(0, 1), 438)
	tile_set.autotile_set_bitmask(0, Vector2(0, 2), 54)
	tile_set.autotile_set_bitmask(0, Vector2(0, 3), 48)
	tile_set.autotile_set_bitmask(0, Vector2(0, 4), 0)
	tile_set.autotile_set_bitmask(0, Vector2(1, 0), 504)
	tile_set.autotile_set_bitmask(0, Vector2(1, 1), 511)
	tile_set.autotile_set_bitmask(0, Vector2(1, 2), 63)
	tile_set.autotile_set_bitmask(0, Vector2(1, 3), 56)
	tile_set.autotile_set_bitmask(0, Vector2(1, 4), 0)
	tile_set.autotile_set_bitmask(0, Vector2(2, 0), 216)
	tile_set.autotile_set_bitmask(0, Vector2(2, 1), 219)
	tile_set.autotile_set_bitmask(0, Vector2(2, 2), 27)
	tile_set.autotile_set_bitmask(0, Vector2(2, 3), 24)
	tile_set.autotile_set_bitmask(0, Vector2(2, 4), 0)
	tile_set.autotile_set_bitmask(0, Vector2(3, 0), 144)
	tile_set.autotile_set_bitmask(0, Vector2(3, 1), 146)
	tile_set.autotile_set_bitmask(0, Vector2(3, 2), 18)
	tile_set.autotile_set_bitmask(0, Vector2(3, 3), 16)
	tile_set.autotile_set_bitmask(0, Vector2(3, 4), 0)
	tile_set.autotile_set_bitmask(0, Vector2(4, 0), 176)
	tile_set.autotile_set_bitmask(0, Vector2(4, 1), 182)
	tile_set.autotile_set_bitmask(0, Vector2(4, 2), 434)
	tile_set.autotile_set_bitmask(0, Vector2(4, 3), 50)
	tile_set.autotile_set_bitmask(0, Vector2(4, 4), 178)
	tile_set.autotile_set_bitmask(0, Vector2(5, 0), 248)
	tile_set.autotile_set_bitmask(0, Vector2(5, 1), 255)
	tile_set.autotile_set_bitmask(0, Vector2(5, 2), 507)
	tile_set.autotile_set_bitmask(0, Vector2(5, 3), 59)
	tile_set.autotile_set_bitmask(0, Vector2(5, 4), 251)
	tile_set.autotile_set_bitmask(0, Vector2(6, 0), 440)
	tile_set.autotile_set_bitmask(0, Vector2(6, 1), 447)
	tile_set.autotile_set_bitmask(0, Vector2(6, 2), 510)
	tile_set.autotile_set_bitmask(0, Vector2(6, 3), 62)
	tile_set.autotile_set_bitmask(0, Vector2(6, 4), 446)
	tile_set.autotile_set_bitmask(0, Vector2(7, 0), 152)
	tile_set.autotile_set_bitmask(0, Vector2(7, 1), 155)
	tile_set.autotile_set_bitmask(0, Vector2(7, 2), 218)
	tile_set.autotile_set_bitmask(0, Vector2(7, 3), 26)
	tile_set.autotile_set_bitmask(0, Vector2(7, 4), 154)
	tile_set.autotile_set_bitmask(0, Vector2(8, 0), 184)
	tile_set.autotile_set_bitmask(0, Vector2(8, 1), 191)
	tile_set.autotile_set_bitmask(0, Vector2(8, 2), 506)
	tile_set.autotile_set_bitmask(0, Vector2(8, 3), 58)
	tile_set.autotile_set_bitmask(0, Vector2(8, 4), 186)
	tile_set.autotile_set_bitmask(0, Vector2(9, 0), 443)
	tile_set.autotile_set_bitmask(0, Vector2(9, 1), 0)
	tile_set.autotile_set_bitmask(0, Vector2(9, 2), 442)
	tile_set.autotile_set_bitmask(0, Vector2(9, 3), 190)
	tile_set.autotile_set_bitmask(0, Vector2(9, 4), 0)
	tile_set.autotile_set_bitmask(0, Vector2(10, 0), 254)
	tile_set.autotile_set_bitmask(0, Vector2(10, 1), 0)
	tile_set.autotile_set_bitmask(0, Vector2(10, 2), 250)
	tile_set.autotile_set_bitmask(0, Vector2(10, 3), 187)
	tile_set.autotile_set_bitmask(0, Vector2(10, 4), 0)

