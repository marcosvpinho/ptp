#!/usr/bin/python

import sys,math
import getopt

def ajuda():
  print 'Uso: %s -f arq [-S|-s start][-g gap][-p pos] | -h' % sys.argv[0]
  sys.exit(1)

opcoes,extra = getopt.getopt(sys.argv[1:], 'Ss:g:f:p:h')
f = None
gap = 1
start = 0
minStart = 0
pos = -1

for op,valor in opcoes:
  if op == '-s':
    start = float(valor)
  elif op == '-p':
    pos = int(valor)
  elif op == '-S':
    minStart = 1
  elif op == '-g':
    gap = float(valor)
  elif op == '-f':
    if valor == '-': f = sys.stdin
    else: f = open(valor)
  else: ajuda()

if not f: ajuda()

h = {}
l = f.readlines()
l = map(lambda x: x.split()[pos], l)
l = map(float, l)

if minStart:
  start = int(min(l))

mp = 0
for x in l:
  p = int((x-start)/gap)
  #print '...',p, x
  mp = max(p, mp)
  try:
    h[p] += 1
  except:
    h[p] = 1
 
n = 0
while n <= mp:
  try:
    c = h[n]
  except:
    c = 0

  print n*gap+start, c
  n += 1

