#!/usr/bin/env bash

# Copyright (C) 2019-2022 Lee C. Bussy (@LBussy)

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

declare CWD GITNAME GITROOT GITTAG ENVIRONMENTS BINLOC PIO
BINLOC="firmware"

get_pio() {
    echo -e "\nChecking PlatformIO environment."
    if [[ $OSTYPE == 'darwin'* ]]; then
        # On a Mac
        PIO="$HOME/.platformio/penv/bin/platformio"
    elif [[ $(grep -q "WSL" /proc/version 2>/dev/null) -gt 0 ]]; then
        # Running WSL
        PIO="/mnt/c/Users/$LOGNAME/.platformio/penv/Scripts/platformio.exe"
    elif [ "$(expr substr $(uname -s) 1 5)" == "MINGW" ]; then
        # Running GitBash on Windows
        PIO="$HOME/.platformio/penv/Scripts/platformio.exe"
    elif [ "$(expr substr $(uname -s) 1 7)" == "MSYS_NT" ]; then
        # Running some weird bash on Windows
        PIO="$HOME/.platformio/penv/Scripts/platformio.exe"
    elif [[ $(grep -q "@WIN" /proc/version 2>/dev/null) -gt 0 ]]; then
        # Running Git Bash
        PIO="$HOME/.platformio/penv/Scripts/platformio.exe"
    else
        # Running some form of Linux
        PIO="$HOME/.platformio/penv/bin/platformio"
    fi
    if [[ ! -f "$PIO" ]]; then
        # PIO does not exist
        echo -e "\nERROR: Unable to find PlatformIO."
        exit
    fi
}

get_git() {
    if [[ ! -f $(which git 2>/dev/null) ]]; then
        echo -e "\nERROR: Git not found."
        exit
    fi
    echo -e "\nDetermining git root."
    if [[ ! $(git status 2>/dev/null) ]]; then
        echo -e "\nERROR: Git repository not found."
        exit
    fi
    GITROOT=$(git rev-parse --show-toplevel)
    GITNAME=$(git rev-parse --show-toplevel)
    GITNAME=${GITROOT##*/}
    GITTAG=$(git describe --tags --abbrev=0)
}

check_root() {
    CWD=$(pwd)
    if [[ ! -d "$GITROOT" ]]; then
        echo -e "\nERROR: Repository not found."
        exit
    fi
    cd "$GITROOT" || exit
}

get_envs() {
    echo -e "\nGathering build environments for $GITNAME."
    cd "$GITROOT" || exit
    while IFS= read var value; do
        if [ ! -z "$var" ] ; then
            ENVIRONMENTS+=($var)
            values+=($value)
        fi
    done <<<  $($PIO project config | grep "env:" | cut -d':' -f2)
}

list_envs() {
    echo -e "\nProcessing the following environments for $GITNAME:"
    for env in "${ENVIRONMENTS[@]}"
    do
        echo -e "\t$env"
    done
    sleep 3
}

create_version() {
    echo -e "\nCreating version JSON."
    sleep 1
    cat << EOF | tee "$GITROOT/data/version.json" "$GITROOT/$BINLOC/version.json" > /dev/null || exit
{
    "fw_version": "$GITTAG",
    "fs_version": "$GITTAG"
}
EOF
}

build_binaries() {
    cd "$GITROOT" || (echo -e "Environment not found." && exit)
    for env in "${ENVIRONMENTS[@]}"
    do
        if [ ! -z "$env" ] ; then
            echo -e "\nBuilding binaries for $env."
            sleep 3
            eval "$PIO" run -e "$env"
            echo -e "\nBuilding filesysyem for $env."
            sleep 3
            eval "$PIO" run --target buildfs -e "$env"
        fi
    done
}

copy_binaries() {
    echo
    if [ -d "$GITROOT"/"$BINLOC"/ ]; then
        for env in "${ENVIRONMENTS[@]}"
        do
            if [ ! -z "$env" ] ; then
                echo -e "Copying binaries for $env."
                cp "$GITROOT"/.pio/build/"$env"/firmware.bin "$GITROOT"/"$BINLOC"/"$env"_firmware.bin
                cp "$GITROOT"/.pio/build/"$env"/partitions.bin "$GITROOT"/"$BINLOC"/"$env"_partitions.bin
                cp "$GITROOT"/.pio/build/"$env"/spiffs.bin "$GITROOT"/"$BINLOC"/"$env"_spiffs.bin
            fi
        done
    else
        echo -e "\nERROR: Unable to copy files to $GITROOT/$BINLOC"
    fi
}

main() {
    get_pio "$@"
    get_git "$@"
    check_root "$@"
    get_envs "$@"
    list_envs "$@"
    create_version "$@"
    build_binaries "$@"
    copy_binaries "$@"
    cd "$CWD" || exit
    echo -e "\nBuild and prep for $GITNAME complete.\n"
}

main "$@" && exit 0
