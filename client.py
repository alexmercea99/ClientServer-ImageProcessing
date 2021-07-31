import socket	
import cv2
import sys
import numpy as np
from random import randint

def add_noise(img, percentage):
    noisy_img = np.zeros((img.shape[0], img.shape[1]), np.uint8)
    percent = percentage * 10

    for i in range(0, img.shape[0]):
        for j in range(0, img.shape[1]):
            value = randint(1, 10)
            noisy_img[i, j] = img[i, j]
            if (value <= percent):
                value2 = randint(1, 10)
                if (value2 <= 5):
                    noisy_img[i, j] = 0
                elif (value2 >= 6):
                    noisy_img[i, j] = 255

    return noisy_img

if __name__ == "__main__":
    host=sys.argv[1]

    port=int(sys.argv[2])


    if len(sys.argv) != 3:
        print ('Usage: python %s <HostName> <PortNumber>' % (sys.argv[0]))
        sys.exit()
    
    s = socket.socket()		

    
    s.connect((host, port))
    k = 0
    
    print("Image Processing file connected successfully\n")
    while True:
        choose = s.recv(1024).decode('UTF-8')
        print (choose)
        if choose == "1":
            strng = '/home/alex/Music/ProiectPCD/server_folder/capture2'
            strng = strng + str(k + 1)
            strng = strng + '.jpeg'
            k = k + 1
            image = cv2.imread(strng)
            print(image)
            print("Enter in Grayscale\n")
            grayscale = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
            img_str = '/home/alex/Music/ProiectPCD/server_folder/new_grayScale'
            img_str = img_str + str(k)
            img_str = img_str + '.jpeg'
            cv2.imwrite(img_str, grayscale)
            s.send(bytes('new_grayScale.jpeg', 'utf-8'))
        elif choose == "2":
            str1 = '/home/alex/Music/ProiectPCD/server_folder/capture2'
            str1 = str1 + str(k + 1)
            str1 = str1 + '.jpeg'
            k = k + 1
            img = cv2.imread(str1)
            #img = img[:,:,0]*0.3 + img[:,:,1]*0.59 + img[:,:,2]*0.11
            print(img)
            print("Enter in Salt and paper\n")
            noisy_img = add_noise(img, 0.4) #20% noise
            img_str1 = '/home/alex/Music/ProiectPCD/server_folder/new_grayScale'
            img_str1 = img_str1 + str(k)
            img_str1 = img_str1 + '.jpeg'
            cv2.imwrite(img_str1, noisy_img)
            #cv2.imwrite('/home/alex/Music/ProiectPCD/server_folder/new_grayScale.jpeg', noisy_img)
        elif choose == "3":
            str1 = '/home/alex/Music/ProiectPCD/server_folder/capture2'
            str1 = str1 + str(k + 1)
            str1 = str1 + '.jpeg'
            k = k + 1
            img = cv2.imread(str1)
            #img = img[:,:,0]*0.3 + img[:,:,1]*0.59 + img[:,:,2]*0.11
            print(img)
            print("Enter in Canny Edge Detection\n")
            noisy_img = cv2.Canny(img, 100,200)
            img_str1 = '/home/alex/Music/ProiectPCD/server_folder/new_grayScale'
            img_str1 = img_str1 + str(k)
            img_str1 = img_str1 + '.jpeg'
            cv2.imwrite(img_str1, noisy_img)
            #cv2.imwrite('/home/alex/Music/ProiectPCD/server_folder/new_grayScale.jpeg', noisy_img)
            s.send(bytes('new_grayScale.jpeg', 'utf-8'))
        elif choose == "4":
            str1 = '/home/alex/Music/ProiectPCD/server_folder/capture2'
            str1 = str1 + str(k + 1)
            str1 = str1 + '.jpeg'
            k = k + 1
            img = cv2.imread(str1)
            #img = img[:,:,0]*0.3 + img[:,:,1]*0.59 + img[:,:,2]*0.11
            print(img)
            print("Enter in HSV\n")
            noisy_img = cv2.cvtColor(img,cv2.COLOR_BGR2HSV)
            img_str1 = '/home/alex/Music/ProiectPCD/server_folder/new_grayScale'
            img_str1 = img_str1 + str(k)
            img_str1 = img_str1 + '.jpeg'
            cv2.imwrite(img_str1, noisy_img)
            #cv2.imwrite('/home/alex/Music/ProiectPCD/server_folder/new_grayScale.jpeg', noisy_img)
            s.send(bytes('new_grayScale.jpeg', 'utf-8'))
        elif choose == "5":
            str1 = '/home/alex/Music/ProiectPCD/server_folder/capture2'
            str1 = str1 + str(k + 1)
            str1 = str1 + '.jpeg'
            k = k + 1
            img = cv2.imread(str1)
            #img = img[:,:,0]*0.3 + img[:,:,1]*0.59 + img[:,:,2]*0.11
            print(img)
            print("Enter in Gaussian Blur\n")
            noisy_img = cv2.GaussianBlur(img, (3, 3), 0)
            img_str1 = '/home/alex/Music/ProiectPCD/server_folder/new_grayScale'
            img_str1 = img_str1 + str(k)
            img_str1 = img_str1 + '.jpeg'
            cv2.imwrite(img_str1, noisy_img)
            #cv2.imwrite('/home/alex/Music/ProiectPCD/server_folder/new_grayScale.jpeg', noisy_img)
            s.send(bytes('new_grayScale.jpeg', 'utf-8'))
        elif choose == "6":
            str1 = '/home/alex/Music/ProiectPCD/server_folder/capture2'
            str1 = str1 + str(k + 1)
            str1 = str1 + '.jpeg'
            k = k + 1
            img = cv2.imread(str1)
            #img = img[:,:,0]*0.3 + img[:,:,1]*0.59 + img[:,:,2]*0.11
            print(img)
            print("Enter in Image Sharpening\n")
            kernel_sharpening = np.array([[-1,-1,-1], 
                              [-1,9,-1], 
                              [-1,-1,-1]])
            noisy_img = cv2.filter2D(img, -1, kernel_sharpening)
            img_str1 = '/home/alex/Music/ProiectPCD/server_folder/new_grayScale'
            img_str1 = img_str1 + str(k)
            img_str1 = img_str1 + '.jpeg'
            cv2.imwrite(img_str1, noisy_img)
            #cv2.imwrite('/home/alex/Music/ProiectPCD/server_folder/new_grayScale.jpeg', noisy_img)
            s.send(bytes('new_grayScale.jpeg', 'utf-8'))

    s.close()
