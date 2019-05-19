#!/usr/bin/env python3
# coding=utf-8

filename = 'data.txt'
file_object = open(filename,'r')

str_bin = []

str_bin = file_object.readlines()

for i in range(0,len(str_bin)):
    str_bin[i] = str_bin[i].replace('\n','')
file_object.close()

out_filename = 'mailbox_samples.txt'
file_object = open(out_filename,'w')
for i in range(0,len(str_bin)):
    # file_object.write(bin(str_bin[i]).replace('0b',''))
    file_object.write(bin(int(str_bin[i],16)).replace('0b',''))
    file_object.write('\n')

file_object.close()

