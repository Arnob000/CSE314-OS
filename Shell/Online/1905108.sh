#!/bin/bash

visit(){
    for v in $1/*
    do
    if [ -d "$v" ]
    then 
    visit $v
    elif [ -f $v ]
    then
    echo $v > demovir
    grep $v1 demovir > dem
    v=${v%.*} 
    a=${v: -$lv1}
    echo v1
    echo v2
    # echo $a
    if [ "$a" = "$v1" ]
    then
    echo $a
    elif [ "$a" = "$v2" ]
    then
    echo $a
    elif [ "$a" = "$v3" ]
    then
    echo $a
    fi

    cp $a out_dir
    
    
    
    # case i in $v
    # v1);;
    # v2);;
    # v3);;
    # esac

    
    fi

    done
}

//rm -r "$3"
mkdir "$3"
virus=$2
sed -n '1p' "$2" >> a
sed -n '2p' "$2" >> b
sed -n '3p' "$2" >>c
read v1 < a
read v2 < b
read v3 < c

lv1=${#v1}
#  while 
#     do
#     echo vir
#     done

#     echo ---------

visit $1