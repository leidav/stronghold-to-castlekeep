#!/usr/bin/env python3

import sys
import os.path
from PIL import Image
import json
from operator import itemgetter

def loadImages(images,animation,src_dir):
    loaded_images=[]
    for i in images:
        img=Image.open(src_dir+"/"+str(i["id"])+".png","r")
        if animation!= None:
            (left,top,right,bottom)=img.getbbox()
            i["xoffset"]=left;
            i["yoffset"]=top;
            i["width"]=right-left
            i["heigth"]=bottom-top
            animation[i["id"]]["center"]["x"]-=left
            animation[i["id"]]["center"]["y"]-=top
        else:
            i["xoffset"]=0
            i["yoffset"]=0;
        loaded_images.append(img)
    return loaded_images

def layout(images,objects,width,sort):
    x=0
    posx=0
    posy=0
    maxy=0
    maxx=0
    ids=[ (img["id"],img["heigth"]) for img in images]
    if sort:
        ids.sort(key=itemgetter(1))

    for (i,_) in ids:
        if (posx+images[i]["width"]+1) > width:
            if x==0:
                print("texture width too small")
                exit(1)
            x=0
            posx=0
            posy=maxy+1
        else:
            x=x+1

        images[i]["x"]=posx
        images[i]["y"]=posy
        if objects!= None:
            for part in objects[i]["parts"]:
                part["rect"]["x"]=part["rect"]["x"]+posx
                part["rect"]["y"]=part["rect"]["y"]+posy

        posx=posx+images[i]["width"]+1
        if posx>maxx:
            maxx=posx
        if (posy+images[i]["heigth"])>maxy:
            maxy=posy+images[i]["heigth"]

    return (maxx,maxy)

def createAtlas(width,height,atlas_file,loaded_images,images):
    atlas=Image.new("RGBA",(width,height),(0,0,0,0))
    for i in images: 
        atlas.alpha_composite(loaded_images[i["id"]],dest=(i["x"],i["y"]),source=(i["xoffset"],i["yoffset"]))
    atlas.save(atlas_file)

def writeImages(images,f):
    f.write("[images]\n")
    f.write("#id,posx,posy,width,height\n")
    for img in images:
        id=img["id"]
        posx=img["x"]
        posy=img["y"]
        width=img["width"]
        height=img["heigth"]
        f.write(f"{id},{posx},{posy},{width},{height}\n")

    return 

def writeTileObjects(data,f):
    f.write("\n[objects]\n")
    f.write("#id,tile_start,tiles\n")
    start_index=0
    for obj in data:
        id=obj["id"]
        num_tiles=obj["part_count"]
        f.write(f"{id},{start_index},{num_tiles}\n")
        start_index+=num_tiles

    f.write("\n[tiles]\n")
    f.write("#objid,x,y,posx,posy,width,height\n")
    for obj in data:
        id=obj["id"]
        for part in obj["parts"]:
            x=part["x"]
            y=part["y"]
            posx=part["rect"]["x"]
            posy=part["rect"]["y"]
            width=part["rect"]["width"]
            height=part["rect"]["height"]
            f.write(f"{id},{x},{y},{posx},{posy},{width},{height}\n")
    return
def writeAnimationData(data,f):
    f.write("\n[animation]\n")
    f.write("#frame,pivot_x,pivot_y\n")
    start_index=0
    for frame in data:
        id=frame["id"]
        x=frame["center"]["x"]
        y=frame["center"]["y"]
        f.write(f"{id},{x},{y}\n")

def main():
    input_file=sys.argv[1]
    output_file=sys.argv[2]
    atlas_file=sys.argv[3]
    sort=1
    width=1024
    if len(sys.argv)>4:
        if int(sys.argv[4])==0:
                sort=0
    if len(sys.argv)>5:
        width=int(sys.argv[5])

    with open(input_file,"r") as f:
        data=json.load(f)

        images=data["images"]

        animation=None
        objects=None
        t=data["type"]
        if t=="tile":
            objects=data["data"]

        elif t=="anim":
            animation=data["data"]
            sort=0

        loaded_images=loadImages(images,animation,os.path.dirname(input_file))

        (image_width,image_height)=layout(images,objects,width,sort)
        if image_width<=width//2:
            width//=2
        createAtlas(width,image_height,
                atlas_file,loaded_images,images)

        with open(output_file,"w") as f:
            writeImages(images,f)
            if t=="tile":
                writeTileObjects(objects,f)
            if t=="anim":
                writeAnimationData(animation,f)
            #json.dump(data,f,indent=4)

if __name__ == "__main__":
    main()

