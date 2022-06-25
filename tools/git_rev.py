#!/usr/bin/python

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

import subprocess

# Get Git project name
projcmd = "git rev-parse --show-toplevel"
project = subprocess.check_output(projcmd, shell=True).decode().strip()
project = project.split("/")
project = project[len(project)-1]

# Get 0.0.0 version from latest Git tag
tagcmd = "git describe --tags --abbrev=0"
version = subprocess.check_output(tagcmd, shell=True).decode().strip()

# Get latest commit short from Git
revcmd = "git log --pretty=format:'%h' -n 1"
commit = subprocess.check_output(revcmd, shell=True).decode().strip()

# Get branch name from Git
branchcmd = "git rev-parse --abbrev-ref HEAD"
branch = subprocess.check_output(branchcmd, shell=True).decode().strip()

# Make all available for use in the macros
print("-DPIO_SRC_NAM={0}".format(project))
print("-DPIO_SRC_TAG={0}".format(version))
print("-DPIO_SRC_REV={0}".format(commit))
print("-DPIO_SRC_BRH={0}".format(branch))
