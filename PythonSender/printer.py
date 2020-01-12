import socket
import requests

from PIL import Image
from io import BytesIO

import time
import shutil

UDP_IP = "192.168.1.108"
UDP_PORT = 8525

sock = socket.socket(socket.AF_INET,
                  socket.SOCK_DGRAM)

def show(url):
    buf = []
    buf = get_imagedata(url)
    data = bytes(buf)
    print(data)
    sock.sendto(data, (UDP_IP, UDP_PORT))
    

def get_imagedata(url):
    print(url)
    buf = []

    img = Image.open(url)
    width, height = img.size

    i = 0
    byte = 0
    sum_color = 0.0
    l = 0
    for x in range(width):
       for y in range(height):
           l+=1
           current_color = img.getpixel( (x,y) )
           if isinstance(current_color, int):
               sum_color += current_color
           else:
               for c in current_color:
                   sum_color += c


    avg = 1.0 * sum_color / (width * height)

    for y in range(height):
       for x in range(width):
           current_color = img.getpixel( (x,y) )
           if isinstance(current_color, int):
               current_color= (current_color, current_color, current_color)

           new_color = (0, 0, 0)
           sum_color =  current_color[0] + current_color[1]  + current_color[2]
           b = 0
           if sum_color > 500:
               b = 1
               new_color = (255, 255, 255)

           byte = (byte << 1)|b
           i += 1
           if i >= 8:

               buf.append(byte)
               byte = 0
               i = 0
    return buf

show("test" + str(19) + ".png" )

