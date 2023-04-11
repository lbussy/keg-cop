#!/usr/bin/python3

# Script to gzip files for website
# from:
# https://community.platformio.org/t/question-esp32-compress-files-in-data-to-gzip-before-upload-possible-to-spiffs/6274/10

Import( 'env', 'projenv' )

import os
import gzip
import shutil
import glob

# Helper to gzip a file
def gzip_file( src_path, dst_path ):
    with open( src_path, 'rb' ) as src, gzip.open( dst_path, 'wb' ) as dst:
        for chunk in iter( lambda: src.read(4096), b"" ):
            dst.write( chunk )

# gzip compress files from 'data' dir to 'data/gzip/' dir
def gzip_webfiles( source, target, env ):
    
    # Filenames / suffixes which need to be compressed
    source_file_prefix = ''
    filetypes_to_gzip = [ 'css', 'html', 'htm', 'js' ]

    print( '\ngzip: Initiated gzip for SPIFFS\n' )
    GZIP_DIR_NAME = 'gzip'

    try:
        data_dir_path = env.get( 'PROJECT_DATA_DIR' )
        gzip_dir_path = os.path.join( env.get( 'PROJECT_DATA_DIR' ), GZIP_DIR_NAME )
    
    except:
        print( 'gzip: Unable to find PROJECT_DATA_DIR.' )
        return -1


    # Check data dir
    if not os.path.exists( data_dir_path ):
        print( 'gzip: Data directory missing at path: ' + data_dir_path )
        print( 'gzip: Please creste the directory first (aborting.)' )
        print( 'gzip: Failure / Aborted' )
        return -1
    
    # Check gzip dir
    if not os.path.exists( gzip_dir_path ):
        print( 'gzip: gzip directory missing at path: ' + gzip_dir_path )
        print( 'gzip: Attempting to create: ' + data_dir_path + "/" + gzip_dir_path )
        try:
            os.mkdir( gzip_dir_path )
        except Exception as e:
            print( 'gzip: Failed to create: ' + data_dir_path + "/" + gzip_dir_path)
            print( 'gzip: Exception: ' + str( e ) )
            print( 'gzip: Please create the directory first (aborting.)' )
            print( 'gzip: Failure / Aborted' )
            return -1

    # Find files to compress
    files_to_gzip = []
    for extension in filetypes_to_gzip:
        match_str = source_file_prefix + '*.'
        files_to_gzip.extend( glob.glob( os.path.join( data_dir_path, match_str + extension ) ) )
    
    print( 'gzip: gzipping files {}\n'.format( files_to_gzip ) )

    # Compress and move files
    was_error = False
    try:
        for source_file_path in files_to_gzip:
            print( 'gzip: Compressing: ' + source_file_path )
            base_file_path = source_file_path.replace( source_file_prefix, '' )
            target_file_path = os.path.join( gzip_dir_path, os.path.basename( base_file_path ) + '.gz' )
            # CHECK IF FILE ALREADY EXISTS
            if os.path.exists( target_file_path ):
                print( 'gzip: Removing ' + target_file_path )
                os.remove( target_file_path )

            print( 'gzip: gzipping FILE \n' + source_file_path + ' to \n' + target_file_path + "\n\n" )
            print( 'gzip: Compressed ' + target_file_path + "\n" )
            gzip_file( source_file_path, target_file_path )
    except IOError as e:
        was_error = True
        print( 'gzip: Failed to compress file: ' + source_file_path )
        print( 'gzip: Exception: {}'.format( e ) )
    if was_error:
        print( 'gzip: Failure / Incomplete.\n' )
    else:
        print( 'gzip: Success / Compressed.\n' )

# Do work
env.AddPreAction( '$BUILD_DIR/spiffs.bin', gzip_webfiles )
