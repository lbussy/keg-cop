#!/usr/bin/env bash

# Copyright (C) 2019-2020 Lee C. Bussy (@LBussy)

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

declare CWD GITNAME GITROOT ENVIRONMENTS BINLOC PIO
BINLOC="firmware"
# TODO:
    # Check for PIO
    # Check for git
    # Check for WSL
PIO="/c/Users/lee/.platformio/penv/Scripts/platformio.exe"

get_git() {
    echo -e "\nDetermining git root."
    GITROOT=$(git rev-parse --show-toplevel) || (echo -e "Environment not found." && exit)
    GITNAME=$(git rev-parse --show-toplevel); GITNAME=${GITNAME##*/}
}

check_root() {
    CWD=$(pwd)
    cd "$GITROOT" || (echo -e "Environment not found." && exit)
}

get_envs() {
    echo -e "\nGathering build environments for $GITNAME."
    cd "$GITROOT" || exit
    readarray -t ENVIRONMENTS < <("$PIO" project data | grep "env_name" | cut -d'"' -f2)
}

list_envs() {
    echo -e "\nProcessing the following environments for $GITNAME:"
    for env in "${ENVIRONMENTS[@]}"
    do
        echo -e "\t$env"
    done
    sleep 3
}

build_binaries() {
    cd "$GITROOT" || (echo -e "Environment not found." && exit)
    for env in "${ENVIRONMENTS[@]}"
    do
        echo -e "\nBuilding binaries for $env."
        sleep 3
        eval "$PIO" run -e "$env"
        echo -e "\nBuilding filesysyem for $env."
        sleep 3
        eval "$PIO" run --target buildfs -e "$env"
    done
}

copy_binaries() {
    echo
    if [ -d "$GITROOT"/"$BINLOC"/ ]; then
        for env in "${ENVIRONMENTS[@]}"
        do
            echo -e "Copying binaries for $env."
            cp "$GITROOT"/.pio/build/"$env"/firmware.bin "$GITROOT"/"$BINLOC"/"$env"_firmware.bin
            cp "$GITROOT"/.pio/build/"$env"/partitions.bin "$GITROOT"/"$BINLOC"/"$env"_partitions.bin
            cp "$GITROOT"/.pio/build/"$env"/spiffs.bin "$GITROOT"/"$BINLOC"/"$env"_spiffs.bin
        done
    else
        echo -e "Unable to copy files to $GITROOT/$BINLOC"
    fi
}

main() {
    get_git "$@"
    check_root "$@"
    get_envs "$@"
    list_envs "$@"
    build_binaries "$@"
    copy_binaries "$@"
    cd "$CWD" || exit
    echo -e "\nBuild and prep for $GITNAME complete."
}

main "$@" && exit 0
