## Observaciones
STB_Image Installation on Ubuntu 20.04 LTS
- git clone https://github.com/nothings/stb.git  
  
Define only once the implementation before include libraries:  
  
 #define STB_IMAGE_IMPLEMENTATION  
 #include "stb_image/stb_image.h"  
 #define STB_IMAGE_WRITE_IMPLEMENTATION  
 #include "stb_image/stb_image_write.h"  
  
--  

