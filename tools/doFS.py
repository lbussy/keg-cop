#!/usr/bin/python3

# Copyright (C) 2019-2023 Lee C. Bussy (@LBussy)

# This file is part of Lee Bussy's Keg Cop (keg-cop).

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

Import( 'env', 'projenv' )

import os
import gzip
import shutil
import glob

# Source subdirectory
DATA_SOURCE_NAME = 'data_source'
# Filenames / suffixes which need to be compressed
source_file_prefix = ''
extensions = [ 'css', 'html', 'htm', 'js' ]

# Copy files that are not to be compressed
def copy_other_files():
    try:
        if not os.path.exists( dest_path ):
            print( 'copyfiles: gzip directory missing at path: ' + dest_path )
            print( 'copyfiles: Attempting to create: ' + dest_path )
            try:
                os.mkdir( dest_path )
            except Exception as e:
                print( 'copyfiles: Failed to create: ' + dest_path )
                print( 'copyfiles: Exception: ' + str( e ) )
                print( 'copyfiles: Please create the directory first (aborting.)' )
                print( 'copyfiles: Failure / Aborted' )
                return -1

        # iterate through the files in the directory
        for file_name in os.listdir(source_dir_path):
            was_error = False
            try:
                # get the file path
                file_path = os.path.join(source_dir_path, file_name)

                # check if the file is a file (not a directory) and does not have an extension in the list
                if os.path.isfile(file_path) and not any(file_name.endswith(ext) for ext in extensions):
                    target_file_path = os.path.join( dest_path, os.path.basename(file_name) )
                    if os.path.exists( target_file_path ):
                        print( '\ncopyfiles: Removing ' + target_file_path )
                        os.remove( target_file_path )
                    # copy the file to a new location
                    print( 'copyfiles: Copying ' + file_path )
                    shutil.copy(file_path, dest_path)
            except IOError as e:
                was_error = True
                print( 'gzip: Failed to copy file: ' + file_path )
                print( 'gzip: Exception: {}'.format( e ) )
                return -1

    except IOError as e:
        was_error = True
        print( 'copyfiles: Unknwon failure copying files.' )
        print( 'copyfiles: Exception: {}'.format( e ) )
        return -1
    if was_error:
        print( '\ncopyfiles: Failure.\n' )
        return -1
    else:
        print( '\ncopyfiles: Success.\n' )

# Helper to gzip a file
def gzip_file( src_path, dst_path ):
    with open( src_path, 'rb' ) as src, gzip.open( dst_path, 'wb' ) as dst:
        for chunk in iter( lambda: src.read(4096), b"" ):
            dst.write( chunk )

# gzip compress files from 'data' dir to 'data/gzip/' dir
def gzip_webfiles( source, target, env ):
    print( '\ngzip: Initiated gzip for filesystem\n' )

    # Check data dir
    if not os.path.exists( source_dir_path ):
        print( 'gzip: Data directory missing at path: ' + source_dir_path )
        print( 'gzip: Please creste the directory first (aborting.)' )
        print( 'gzip: Failure / Aborted' )
        return -1
    
    # Check gzip dir
    if not os.path.exists( dest_path ):
        print( 'gzip: gzip directory missing at path: ' + dest_path )
        print( 'gzip: Attempting to create: ' + source_dir_path + "/" + dest_path )
        try:
            os.mkdir( dest_path )
        except Exception as e:
            print( 'gzip: Failed to create: ' + source_dir_path + "/" + dest_path)
            print( 'gzip: Exception: ' + str( e ) )
            print( 'gzip: Please create the directory first (aborting.)' )
            print( 'gzip: Failure / Aborted' )
            return -1

    # Find files to compress
    files_to_gzip = []
    for extension in extensions:
        match_str = source_file_prefix + '*.'
        files_to_gzip.extend( glob.glob( os.path.join( source_dir_path, match_str + extension ) ) )
    
    # print( 'gzip: gzipping files {}\n'.format( files_to_gzip ) )

    # Compress and move files
    was_error = False
    try:
        for source_file_path in files_to_gzip:
            print( 'gzip: Processing: ' + source_file_path )
            base_file_path = source_file_path.replace( source_file_prefix, '' )
            target_file_path = os.path.join( dest_path, os.path.basename( base_file_path ) + '.gz' )
            # CHECK IF FILE ALREADY EXISTS
            if os.path.exists( target_file_path ):
                print( 'gzip: Removing ' + target_file_path )
                os.remove( target_file_path )

            print( 'gzip: gzipping:\n\t' + source_file_path + '\n\tto:\n\t' + target_file_path )
            print( 'gzip: Compressed ' + target_file_path + "\n" )
            gzip_file( source_file_path, target_file_path )
    except IOError as e:
        was_error = True
        print( 'gzip: Failed to compress file: ' + source_file_path )
        print( 'gzip: Exception: {}'.format( e ) )
        return -1
    if was_error:
        print( 'gzip: Failure\n' )
        return -1
    else:
        print( 'gzip: Success.\n' )

    # Now copy the others:
    copy_other_files()

try:
    # Define destination directory
    source_dir_path = os.path.join( env.get( 'PROJECT_DIR' ), DATA_SOURCE_NAME )
    # Define source directory
    dest_path = env.get( 'PROJECT_DATA_DIR' )
    # dest_path = os.path.join( env.get( 'PROJECT_DATA_DIR' ), GZIP_DIR_NAME )

except:
    print( 'gzip: Unable to find PROJECT_DATA_DIR.' )
    exit(-1)

# Do work
env.AddPreAction( '$BUILD_DIR/spiffs.bin', gzip_webfiles )
