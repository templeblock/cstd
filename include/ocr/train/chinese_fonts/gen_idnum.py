#! /usr/bin/env python
# -*- coding: utf-8 -*-
from gen_utils import *
from allcharset import *


chars = '0123456789函数则是相当于'
chars = '01'

if 1:
    #random.shuffle(fontfiles)
    #fontfiles = fontfiles[:200] + ['huawenxihei.ttf', 'fz-v4.0.ttf']
    fontfiles = listdir('D:/fonts/ttf_han/')
    fontfiles = listdir('D:/fonts/ttf/')
    fontfiles = fullpath('D:/fonts/ttf_num/', ['ocrb10bt.ttf'])
    outpath = 'D:/OCR_Line/lines/en'
    chars = '0123456789X'

if 1:
    #random.shuffle(fontfiles)
    #fontfiles = fontfiles[:200] + ['huawenxihei.ttf', 'fz-v4.0.ttf']
    fontfiles = listdir('D:/fonts/ttf/')
    fontfiles = fullpath('D:/fonts/ttf_num/', ['ocrb10bt.ttf'])
    outpath = 'E:/OCR_Line/lines/num50w'
    chars = numxs
    other_chars = other+fuhao

root = outpath + '/'
infos_fn = root+'infos.txt'

imgoutpath = outpath+'/img'
mkdir(outpath)
mkdir(imgoutpath)
fonts = []

fontsize = 32

for fontfile in fontfiles:
    font = ImageFont.truetype(fontfile, fontsize)
    fonts.append(font)


if 1:
    print(len(chars))
    labels = ['blank']

    for i in chars:
        labels.append(i)

    savelines(labels, root+'label.txt')

infos = []

def gen_char():
    if rand() < 0.9:
        return chars[randint(0, len(chars))]
    return ' '

beginid = 0

if 0:
    for root, dirs, files in os.walk(imgoutpath):  # path 为根目录
        for t in files:
            a = t.split('.', 1)[0]
            a = a.split('_')[0]
            i = int(a)
            if i>=beginid:
                beginid = i+1

infos = []
if 0:
    if os.path.exists(infos_fn):
        with open(infos_fn, 'r') as file_to_read:
            for line in file_to_read.readlines(): # 整行读取数据
                line=line.strip('\n')
                if len(line)>0:
                    infos.append(line)

base = 10000
num = 50*base

for i in range(beginid, num):
    a = 1.1+rand()*0.5
    b = 1.1+rand()*0.5
    h = int(32*a)
    w = int(280*b)

    while 1:
        b = randint(0, 255)
        f = randint(0, 255)
        if abs(b-f) > 50:
            break
    bgColor = (b, b, b)
    fillColor = (f, f, f)

    img_OpenCV2 = np.ones((h, w, 3), dtype=np.uint8)
    cv2.rectangle(img_OpenCV2, (0, 0), (w, h), bgColor, -1)
    img_OpenCV2 = add_line2(img_OpenCV2, 150, (b+f)//2)
    img_OpenCV2 = cv2.cvtColor(img_OpenCV2, cv2.COLOR_BGR2RGB)
    img_PIL = Image.fromarray(img_OpenCV2)
    draw = ImageDraw.Draw(img_PIL)
    pos_x = randint(-5, 5)
    pos_y0 = randint(0, h-fontsize-4)
    ss = []
    fontindex = randint(0, len(fonts))
    font = fonts[fontindex]
    jbeg = randint(0, 3)
    jend = randint(10, 18)
    for j in range(18):
        pos_y = pos_y0 + randint(-2, 2)
        ch = ' '
        idx = 0
        t = rand()
        if t<0.9 and j>=jbeg and j<=jend:
            idx1 = randint(0, 10)
            
            ch = chars[idx1]
            size = font.getsize(ch)
            if size[0]+pos_x < w:
                idx = idx1 + 1
                draw.text((pos_x, pos_y), ch, font=font, fill=fillColor)
                pos_x = pos_x + size[0] + randint(-5, 5)
        else:
            if 1:
                if (fontsize/2)+pos_x < w:
                    idx = 0
                    pos_x = pos_x + fontsize/2
            else:
                idx1 = randint(0, len(other_chars))
                ch = other_chars[idx1]
                size = font.getsize(ch)
                if size[0]+pos_x < w:
                    idx = 0
                    draw.text((pos_x, pos_y), ch, font=font, fill=fillColor)
                    pos_x = pos_x + size[0] + randint(-5, 5)
                

        ss.append(idx)

    im = np.asarray(img_PIL)
    im = cv2.cvtColor(im, cv2.COLOR_RGB2GRAY)

    k = 3
    if rand() > 0.5:
        if rand() > 0.5:
            if b > f:
                im = add_erode(im, randint(1, k))
            else:
                im = add_dilate(im, randint(1, k))
        else:
            if b > f:
                im = add_erode(im, randint(1, k))
            else:
                im = add_dilate(im, randint(1, k))

    im = add_noise(im, 50)

    #if rand() > 0.5:        im = add_line(im, 10)

    if rand() > 0.5:
        im1 = add_shader(im)
        k = 0.4
        im = im1*k + im*(1-k)
    # im = HSVequalizeHist(im)
    if rand() > 0.5:
        im = motion_blur(im, randint(2, 5), randint(0, 360))

    if rand() > 0.5:
        im = gauss_blur(im, 5, randint(5, 10))

    if 0:
        j = i//base
        fn = '%s/%d' % (imgoutpath, j)
        if not os.path.exists(fn):
            mkdir(fn)
        fn = '%s/%d/%d.jpg' % (imgoutpath, j, i)
        im = cv2.resize(im, (280, 32))
        la = ' '.join(map(str, ss))
        s = '%s %s' % (fn, la)
        infos.append(s)
        #print(h, w, fn)
        # cv2.imwrite(fn, im)
        cv2.imencode('.jpg', im)[1].tofile(fn)

    if 1:
        j = i//base
        if i%base==0:
            print(j)
        fn = '%s/%d' % (imgoutpath, j)
        if not os.path.exists(fn):
            mkdir(fn)
        la = '_'.join(map(str, ss))
        fn = '%s/%d/%d_%s.jpg' % (imgoutpath, j, i, la)
        im = cv2.resize(im, (280, 32))
        la = ' '.join(map(str, ss))
        s = '%s %s' % (fn, la)
        infos.append(s)
        #print(h, w, fn)
        # cv2.imwrite(fn, im)
        cv2.imencode('.jpg', im)[1].tofile(fn)


if 0:
    count = len(infos)
    count_train = int(count*0.8)

    outtxt = root+'list.txt'
    savelines(infos, root+'infos.txt')
    savelines(infos[:count_train], root+'train.txt')
    savelines(infos[count_train:], root+'test.txt')
