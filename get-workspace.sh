#!/bin/bash

link='https://gist.githubusercontent.com/sorbatdev/ee63f928a22dbac9e3996cb2b80e5381/raw/c816fc274abde84bb9a879b3128d1f792aef74f4/Polos.code-workspace'
output='Polos.code-workspace'

if command -v curl > /dev/null 2>&1
then
    curl -o $output $link 
    exit 0
fi

if command -v wget > /dev/null 2>&1
then
    wget -O $output $link
    exit 0
fi

echo "wget or curl not found, aborting"
exit 1
