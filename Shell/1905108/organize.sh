#!/bin/bash
message(){
    if [ "$ver" = "1" ]
    then
    echo "$1" files of "$2"
    fi
}

visit(){
    if [ -d "$1" ]  
    then
        for v in "$1"/*
        do
            visit "$v"
        done
    elif [ -f "$1" ]
    then
        case "$1" in
        *.c) #echo "$1"; 
        path=$targets/C/"$id";
        mkdir $path; cp "$1" "$path/main.c"
        msg="Organizing"
        message $msg $id

        if [ "$noexecute" = "0" ]
        then
            gcc "$path/main.c" -o "$path/main.out";
            match  "$path/main" "C";
            msg="Executng";
            message $msg $id;
        fi
        ;;

        *.java) #echo "$1"; 
        path=$targets/Java/"$id";
        mkdir $path; cp "$1" "$path/Main.java"
        msg="Organizing"
        message $msg $id
 
        if [ "$noexecute" = "0" ]
        then
            javac "$path/Main.java";
            match "$path/Main" "Java";
            msg="Executng";
            message $msg $id;
        fi
        ;;

        *.py) #echo "$1";
        path=$targets/Python/"$id";
        mkdir $path; cp "$1" "$path/main.py"
        msg="Organizing"
        message $msg $id

        if [ "$noexecute" = "0" ]
        then            
            match  "$path/main" "Python";
            msg="Executng";
            message $msg $id
        fi
        ;;
        # *) echo "File not familiar";;
        esac
    fi
}

# $1 means submissions folder
begin(){
    for i in "$1"/*
    do
    id="${i: -11}"
    id=${id%.zip}
   # echo $id

    unzip -qq "$i"
    for node in `ls`
    do
            case $node in
            $answers|$submissions|$targets|$tests) ;;
            answers|submissions|$targets|tests) ;;
            organize.sh) ;;
            *) visit "$node" $targets; rm -r "$node";;
            esac
            
    done

    done
}

match(){
    path=$1;
    fileType=$2;
    path2="${path%/*}";
    #echo "$path2";

    # csv
    num=1;
    matched=0;
    unmatched=0;
    ids=${path2: -7};

    for t in "$tests"/*
    do
        if [ $fileType = "C" ]
        then
            "./$path.out" < "$t" > "$path2""/out$num.txt";
        elif [ $fileType = "Java" ]
        then
            java -cp "$path2" "Main" < "$t" > "$path2""/out$num.txt";
        elif [ $fileType = "Python" ]
        then
            python3 "$path.py" < "$t" > "$path2""/out$num.txt";
        fi
        
        diff "$answers/ans$num.txt" "$path2""/out$num.txt" > "$targets/demo.txt";
        if [ -s "$targets/demo.txt" ]
        then 
        unmatched=$(($unmatched+1));
        else
        matched=$(($matched+1));
        fi
        num=$(($num+1));
    done
    echo "$ids,$fileType,$matched,$unmatched" >> "$targets/result.csv";     
}

usage(){
    echo "Usage:"
    echo "./organize.sh <submission folder> <target folder> <test folder> <answer folder> [-v] [-noexecute]"
    echo ""
    echo "-v: verbose"
    echo "-noexecute: do not execute code files"
    echo ""
}

# Start
check=4
noexecute=0
ver=0
err=0
if [ ! -d "$1" ]
then 
err=1
fi

if [ ! -d "$3" ]
then
err=1
fi

if [ ! -d "$4" ]
then
err=1
fi

if [ "$5" = "-noexecute" ]
then
noexecute=1
elif [ "$6" = "-noexecute" ]
then
noexecute=1
fi

if [ "$5" = "-v" ]
then
ver=1
elif [ "$6" = "-v" ]
then
ver=1
fi



if [ $# -lt $check ]
then
usage
elif [ $err = "1" ]
then
usage
elif [ $# -ge $check ]
then
submissions="$1"
targets="$2"
tests="$3"
answers="$4"

for tar in `ls`
do
if [ "$tar" = "$targets" ]
then
rm -r $targets #fixed
fi
done

mkdir $targets
mkdir $targets/C
mkdir $targets/Python
mkdir $targets/Java

if [ "$ver" = "1" ]
then
testfiles=$(ls ${tests} | wc -l)
echo "Found ${testfiles} test files"
fi


if [ "$noexecute" = "0" ]
then
touch "$targets/result.csv"
touch "$targets/demo.txt"
echo "student_id,type,matched,not_matched" >> "$targets/result.csv"
# execute $targets $tests
fi


begin "$submissions"

if [ "$noexecute" = "0" ]
then
rm "$targets/demo.txt"
fi

fi
