stronghold to castlekeep
=======================
A tool to convert strongholds gm1 and tgx files to png and json, as needed by castlekeep. 

## Build instructions

Create a build directory:
  
    mkdir build  
    cd build  

Configure:  

    cmake ../  

Build:  

    make 

## Usage

### Convert

    ./convert.sh stronghold_dir asset_dir

### sh2ck

    sh2ck [options] input_file output_dir
    options:
    	-h, --help	This help
    	-t, --tgx	Read a tgx file
