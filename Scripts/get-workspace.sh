#!/bin/bash

link='https://gist.github.com/sorbatdev/83973b1d409ce5270200c6f6398b0770'
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
