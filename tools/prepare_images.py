"""Key near-black pixels to transparent for Pebble GCompOpSet sprites."""
import os
from PIL import Image

ROOT = os.path.join(os.path.dirname(__file__), '..', 'resources', 'images')


def key_black(path, threshold=18):
    im = Image.open(path).convert('RGBA')
    px = im.load()
    w, h = im.size
    for y in range(h):
        for x in range(w):
            r, g, b, a = px[x, y]
            if a == 0:
                continue
            if r <= threshold and g <= threshold and b <= threshold:
                px[x, y] = (0, 0, 0, 0)
    im.save(path)
    print('keyed', os.path.basename(path), im.size, im.mode)


def main():
    for name in ('star.png', 'moon.png', 'nebula.png', 'star_node.png'):
        path = os.path.join(ROOT, name)
        if os.path.exists(path):
            key_black(path)


if __name__ == '__main__':
    main()
