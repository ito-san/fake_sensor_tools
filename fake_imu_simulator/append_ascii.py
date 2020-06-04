# coding:UTF-8

import os
import sys
from array import array

argvs = sys.argv
argc = len(argvs)

if argc != 2:
  print("Usage: # python {0} logfile".format(argvs[0]))
  quit()

def main():
  path = argvs[1]
  frame = bytearray()
  frame.extend('$TSC,BIN,')
  data = array('B')

  with open(path + ".out", 'wb') as w:
    with open(path, "rb") as r:
      data.fromfile(r, 44)
      frame.extend(data)
      sum = 0
      for i in range(1,52):
        sum = sum ^ frame[i]
      frame.extend("*{:02X}\r\n".format(sum))
      w.write(frame)

      while True:
        frame = bytearray()
        frame.extend('$TSC,BIN,')
        data = array('B')
        try:
          data.fromfile(r, 44)
        except EOFError:
          break

        frame.extend(data)
        sum = 0
        for i in range(1,52):
          sum = sum ^ frame[i]
        frame.extend("*{:02X}\r\n".format(sum))
        w.write(frame)

if __name__ == "__main__":
  main()
