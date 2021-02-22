extends Node

## sprite sheet link:
## http://gaurav.munjal.us/Universal-LPC-Spritesheet-Character-Generator/

var TEXTURE = "res://download.png"

var sheet:Dictionary = {
	"spell":7,
	"thrust":8,
	"walk":9,
	"slash":6,
	"shoot":13,
	"die":6,
}

func _ready():
	var sprite := Sprite.new()
	sprite.hframes = 13
	sprite.vframes = 21
	sprite.texture = load(TEXTURE)
	var animation_player := AnimationPlayer.new()
	add_child(sprite)
	sprite.add_child(animation_player)
	
	var row:int = 0
	for anim_name in sheet:
		for dir in [ "up", "left", "down", "right" ]:
			var animation = Animation.new()
			animation.loop = true
			var track_index = animation.add_track(Animation.TYPE_VALUE)
			animation.track_set_path(track_index, ".:frame")
			var time:float = 0
			for i in range(sheet[anim_name]):
				animation.track_insert_key(track_index, time, row*13 + i)
				time += .1
			animation.length = time - .1
			row += 1
			if anim_name == "die":
				animation_player.add_animation(anim_name,animation)
				animation.loop = false
				break
			animation_player.add_animation(anim_name+"_"+dir,animation)
	animation_player.play("walk_up")

func _process(delta):
	pass # Replace with function body.