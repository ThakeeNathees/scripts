tool ## Godot 3.4.2
extends EditorScript

## -----------------------------------------------------------------------------
## USER INPUT
## -----------------------------------------------------------------------------

## If false, will export animation.
const EXPORT_MODEL = true

const MODEL_FBX_PATH   = "res://models/_temp/knight.fbx"
const MODEL_EXPORT_DIR = "res://models/knight/"

const ANIM_FBX_PATH    = "res://anims/_temp/idle.fbx"
const ANIM_EXPORT_DIR  = "res://anims/basics/"
const ANIM_LOOP        = true

## -----------------------------------------------------------------------------
## ENTRY POINT
## -----------------------------------------------------------------------------

func _run():
  if EXPORT_MODEL: _exportModel(MODEL_FBX_PATH, MODEL_EXPORT_DIR)
  else: _exportAnim(ANIM_FBX_PATH, ANIM_EXPORT_DIR, ANIM_LOOP)
  print('-- FINISHED -- ')

## -----------------------------------------------------------------------------
## INTERNAL
## -----------------------------------------------------------------------------

func _getBonePrefix(skeleton:Skeleton) -> String:
  for i in skeleton.get_bone_count():
    var bone_name = skeleton.get_bone_name(i)
    if 'Hips' in bone_name:
      return bone_name.substr(0, bone_name.find("Hips"))
  assert(false, "unreachable")
  return ""

func _renameBones(skeleton:Skeleton, prefix:String):
  if prefix == "": return
  for i in skeleton.get_bone_count():
    skeleton.set_bone_name(i,
      skeleton.get_bone_name(i).replace(prefix, ""))

func _renameSkinBinds(mesh:MeshInstance, prefix:String):
  for i in mesh.skin.get_bind_count():
    mesh.skin.set_bind_name(i, mesh.skin.get_bind_name(i).replace(prefix, ""))

func _renameAnimations(animation_player:AnimationPlayer, prefix:String):
  for anim_name in animation_player.get_animation_list():
    var anim := animation_player.get_animation(anim_name)
    for i in anim.get_track_count():
      anim.track_set_path(i,String(anim.track_get_path(i)).replace(prefix, ""))

## Set owner of all the children of the parent node to the [root_node]
func _changeOwner(root_node:Node, parent:Node):
  for node in parent.get_children():
    node.owner = root_node
    _changeOwner(root_node, node)

func _addRootMotionBone(skeleton:Skeleton):
  skeleton.add_bone("RootMotion")
  skeleton.set_bone_parent(skeleton.find_bone("Hips"),
                           skeleton.get_bone_count() - 1)

## Returns the index of the hips track.
func _getAnimHipsTrack(anim:Animation) -> int:
  for i in anim.get_track_count():
    if anim.track_get_type(i) == Animation.TYPE_TRANSFORM and \
    anim.track_get_path(i) == "Skeleton:Hips":
      return i
  assert(false, "Hips track not found")
  return -1

func _addRootMotionTrack(anim:Animation):
  var track_hips := _getAnimHipsTrack(anim)
  var track_root := anim.add_track(Animation.TYPE_TRANSFORM)
  anim.track_set_path(track_root, "Skeleton:RootMotion")
  
  for i in anim.track_get_key_count(track_hips):
    var time = anim.track_get_key_time(track_hips, i)
    var hips_transform = anim.track_get_key_value(track_hips, i)
    
    anim.track_insert_key(track_root, time, {
      "location" : Vector3(hips_transform["location"].x, 0, hips_transform["location"].z),
      "rotation" : Quat(),
      "scale"    : Vector3.ONE,
    })
    anim.track_set_key_value(track_hips, i, {
      "location" : Vector3(0, hips_transform["location"].y, 0),
      "rotation" : hips_transform["rotation"],
      "scale"    : Vector3.ONE,
    })

func _makeDirIfNotExists(var path:String):
  var dir = Directory.new()
  if not dir.dir_exists(path):
    dir.make_dir_recursive(path)

func _saveNodeAsScene(node:Node, path:String):
  var scene = PackedScene.new()
  if scene.pack(node) != OK:
    print("Error occured while packing the scene.")
    return
  _makeDirIfNotExists(path.get_base_dir())
  if ResourceSaver.save(path, scene, ResourceSaver.FLAG_COMPRESS) != OK:
    print("Error occured while saving the resource")
    return

func _saveAnimation(anim:Animation, export_dir:String, anim_name:String):
  var flags:int = 0
  _makeDirIfNotExists(export_dir)
  ResourceSaver.save(export_dir + anim_name + ".anim", anim, ResourceSaver.FLAG_COMPRESS)

func _copyMaterials(root_node:Node, export_dir:String):
  var already_saved = []
  
  for mesh_instance in root_node.get_children():
    if !(mesh_instance is MeshInstance): continue
    var mesh :Mesh = mesh_instance.mesh
    for i in mesh.get_surface_count():
      var material :Material = mesh.surface_get_material(i)
      var target_path = export_dir + material.resource_name + ".material"
      
      if not (material.resource_name in already_saved):
        _makeDirIfNotExists(target_path.get_base_dir())
        ResourceSaver.save(target_path, material, ResourceSaver.FLAG_COMPRESS)
        already_saved.append(material.resource_name)
      mesh.surface_set_material(i, load(target_path))


## -----------------------------------------------------------------------------
## MODEL EXPORTER
## -----------------------------------------------------------------------------

func _exportModel(model_fbx_path, model_export_dir):
  var scene :Spatial = load(model_fbx_path).instance()
  var root_node :Spatial = scene.get_node("RootNode")
  root_node.name = "Model"
  
  var skeleton :Skeleton = root_node.get_node("Skeleton")
  var prefix :String = _getBonePrefix(skeleton)
  
  ## Remove the prefix name.
  if prefix != "":
    _renameBones(skeleton, prefix)  
    for mesh in root_node.get_children():
      if !(mesh is MeshInstance): continue
      _renameSkinBinds(mesh, prefix)
  
  ## Remove animation node for the model.
  var animation := root_node.get_node("AnimationPlayer")
  animation.owner = null
  root_node.remove_child(animation)
  
  ## Remove spatial node points.
  var points :Spatial = root_node.get_node(prefix + "Hips")
  if points != null: root_node.remove_child(points)
  
  _addRootMotionBone(skeleton)  
  _changeOwner(root_node, root_node)
  _copyMaterials(root_node, model_export_dir)
  
  var model_name = model_fbx_path.get_file().get_basename()
  _saveNodeAsScene(root_node, model_export_dir + model_name + ".scn")
  
## -----------------------------------------------------------------------------
## ANIMATION EXPORTER
## -----------------------------------------------------------------------------
func _exportAnim(anim_fbx_path:String, export_dir:String, is_loop:bool):
  var scene :Spatial = load(anim_fbx_path).instance()
  var root_node :Spatial = scene.get_node("RootNode")
  var animation_player :AnimationPlayer = root_node.get_node("AnimationPlayer")
  
  var prefix :String = _getBonePrefix(root_node.get_node("Skeleton"))
  if prefix != "": _renameAnimations(animation_player, prefix)
  
  ## FIXME: animation name below.
  var animation :Animation = animation_player.get_animation("mixamo.com")
  animation.loop = is_loop
  _addRootMotionTrack(animation)
  
  var anim_name = anim_fbx_path.get_file().get_basename()
  _saveAnimation(animation, export_dir, anim_name)
  

  
  
