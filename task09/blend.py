import numpy
from PIL import Image
from cppmodule import *

def main():
  dst = numpy.array(Image.open("../assets/autumn.png"))
  src = numpy.array(Image.open("../assets/squirrel.png"))
  src_msk = numpy.array(Image.open("../assets/squirrel_mask.png")).astype(numpy.uint8)
  offset = (50,200)
  num_iteration = 1000
  ret_r = poisson_blending(dst[:,:,0],src[:,:,0],src_msk,offset=offset,num_iteration=num_iteration)
  ret_g = poisson_blending(dst[:,:,1],src[:,:,1],src_msk,offset=offset,num_iteration=num_iteration)
  ret_b = poisson_blending(dst[:,:,2],src[:,:,2],src_msk,offset=offset,num_iteration=num_iteration)
  im_out = Image.fromarray(numpy.dstack([ret_r,ret_g,ret_b]).astype(numpy.uint8))
  im_out.show()

if __name__ == "__main__":
  main()