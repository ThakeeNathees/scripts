from PIL import Image

## USER DATA ----------------------------
GIF_NAME = 'idle'
## --------------------------------------

im = Image.open(GIF_NAME + '.gif')

## size of each frame.
_w, _h = im.size

frames = im.n_frames

## size of the sprite sheet.
w, h = _w * frames, _h

sprite_sheet = Image.new('RGBA', (w, h), color = (0, 0, 0, 0))

for frame in range(frames):
    im.seek(frame)

    rgba_im = im.convert('RGBA')
    
    for i in range(_w):
        for j in range(_h):
            sprite_sheet.putpixel((i + _w * frame, j), rgba_im.getpixel((i, j)))

sprite_sheet.save(GIF_NAME + '.png')
