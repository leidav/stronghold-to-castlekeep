stronghold to castlekeep
=======================

###Build instructions###

Create a build directory:
  
    mkdir build  
    cd build  

Configure:  

    cmake ../  

Build:  

    make 

###Usage### 

####Convert####

    ./convert.sh stronghold_dir asset_dir

####sh2ck####

    sh2ck [options] input_file output_dir
    options:
    	-h, --help	This help
    	-t, --tgx	Read a tgx file
